import datetime
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
UTC = datetime.timezone.utc


class VersioningTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source = self.root / "source"
        self.source.mkdir()
        for name in (
            "CMakeLists.txt",
            "CMAKE_BUILD.md",
            "cmake/dependencies.cmake",
            "linux/CMakeLists.txt",
            "linux/vscpl1drv-can232.cpp",
            "linux/vscpl1drv-can232.h",
            "linux/can232obj.cpp",
            "linux/can232obj.h",
        ):
            target = self.source / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / name, target)

        self.vscp = self.root / "vscp"
        common = self.vscp / "src/common"
        common.mkdir(parents=True)
        (self.vscp / "src/vscp/common").mkdir(parents=True)
        (common / "dllist.c").write_text("", encoding="utf-8")
        (common / "com.cpp").write_text("", encoding="utf-8")
        self.env = os.environ.copy()
        self.env.update({
            "TZ": "UTC",
            "GIT_AUTHOR_NAME": "Version test",
            "GIT_AUTHOR_EMAIL": "test@example.invalid",
            "GIT_COMMITTER_NAME": "Version test",
            "GIT_COMMITTER_EMAIL": "test@example.invalid",
        })

    def git(self, *args, env=None):
        subprocess.run(
            ["git", *args], cwd=self.source, env=env or self.env,
            check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        )

    def commit(self, date):
        env = self.env.copy()
        env.update({"GIT_AUTHOR_DATE": date, "GIT_COMMITTER_DATE": date})
        self.git("commit", "--allow-empty", "-m", "version test", env=env)

    def version(self, date, timezone="UTC"):
        env = self.env.copy()
        env["SOURCE_DATE_EPOCH"] = str(int(date.timestamp()))
        env["TZ"] = timezone
        build = self.root / "build"
        subprocess.run(
            ["cmake", "-S", str(self.source), "-B", str(build),
             f"-DFETCHCONTENT_SOURCE_DIR_TP_VSCP={self.vscp}"],
            env=env, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        )
        for line in (build / "CMakeCache.txt").read_text(encoding="utf-8").splitlines():
            if line.startswith("CMAKE_PROJECT_VERSION:STATIC="):
                return line.split("=", 1)[1]
        self.fail("CMake did not record the project version")

    def test_patch_resets_and_increments_at_month_boundary(self):
        self.git("init", "-q")
        self.commit("2026-02-28T23:59:59+00:00")
        march = datetime.datetime(2026, 3, 15, tzinfo=UTC)
        self.assertEqual(self.version(march), "2026.03.0")

        self.commit("2026-03-01T00:00:00+00:00")
        self.assertEqual(self.version(march), "2026.03.0")

        self.commit("2026-03-01T00:01:00+00:00")
        self.assertEqual(self.version(march), "2026.03.1")
        self.assertEqual(
            self.version(datetime.datetime(2026, 4, 1, tzinfo=UTC)),
            "2026.04.0",
        )

    def test_source_without_git_uses_zero_patch(self):
        self.assertEqual(
            self.version(datetime.datetime(2027, 1, 1, tzinfo=UTC)),
            "2027.01.0",
        )

    def test_calendar_month_is_utc(self):
        date = datetime.datetime(2026, 3, 1, 0, 30, tzinfo=UTC)
        self.assertEqual(self.version(date, "UTC+08"), "2026.03.0")


if __name__ == "__main__":
    unittest.main()
