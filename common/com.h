///////////////////////////////////////////////////////////////////////////////
// com.h: Platform-specific serial communication wrapper
//
// This file provides the correct com.h for the build platform:
// - Windows: com_win32.h
// - macOS: com_macos.h
// - Linux: VSCP's original com.h
//

#if defined(_WIN32) || defined(WIN32)
    #include "com_win32.h"
#elif defined(__APPLE__) || defined(__MACH__)
    #include "com_macos.h"
#else
    // Fall back to system/VSCP version for other platforms
    #include <com.h>
#endif
