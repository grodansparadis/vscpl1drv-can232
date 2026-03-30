///////////////////////////////////////////////////////////////////////////////
// com_macos.cpp: macOS-specific serial communication implementation
//
// This file is part of vscpl1drv-can232
// Copyright (C) 2000-2026 Ake Hedman, Grodans Paradis AB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//

#include "com_macos.h"
#include <sys/select.h>
#include <errno.h>

static int lockSerialFd(int fd)
{
    struct flock lock;

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    return fcntl(fd, F_SETLK, &lock);
}

static int unlockSerialFd(int fd)
{
    struct flock lock;

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;

    return fcntl(fd, F_SETLK, &lock);
}

// Baud rate mapping
static struct {
    unsigned long baudrate;
    speed_t speed;
} baudrate_map[] = {
    {9600, B9600},
    {19200, B19200},
    {38400, B38400},
    {57600, B57600},
    {115200, B115200},
    {0, 0}
};

///////////////////////////////////////////////////////////////////////////////
// Constructor
//

Comm::Comm(void)
{
    m_fd = -1;
    m_readTimeout = 1000;  // Default 1 second timeout
    pthread_mutex_init(&m_mutex, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
//

Comm::~Comm(void)
{
    close();
    pthread_mutex_destroy(&m_mutex);
}

///////////////////////////////////////////////////////////////////////////////
// getPortName - Convert port number to device path
//

void Comm::getPortName(unsigned int port, char *devpath, size_t maxlen)
{
    // Try typical macOS device names
    // For port 1: /dev/tty.usbserial-*, etc.
    // For simplicity, use /dev/ttyUSB0, /dev/ttyUSB1, etc. as fallback
    snprintf(devpath, maxlen, "/dev/ttyUSB%u", port - 1);
}

///////////////////////////////////////////////////////////////////////////////
// init - Initialize serial port with specified parameters
//

bool Comm::init(unsigned int port, unsigned long baudrate, 
                int dataBits, int parity, int stopBits, int handshake)
{
    char devpath[256];
    speed_t speed = B9600;
    int i;

    // Find matching baud rate
    for (i = 0; baudrate_map[i].baudrate != 0; i++) {
        if (baudrate_map[i].baudrate == baudrate) {
            speed = baudrate_map[i].speed;
            break;
        }
    }

    // Close existing port if open
    if (m_fd >= 0) {
        close();
    }

    pthread_mutex_lock(&m_mutex);

    // Get device path for port
    getPortName(port, devpath, sizeof(devpath));

    // Open the serial port
    m_fd = ::open(devpath, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (m_fd < 0) {
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Get current port settings
    if (tcgetattr(m_fd, &m_tios) != 0) {
        ::close(m_fd);
        m_fd = -1;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Save original settings for restoration
    m_oldTios = m_tios;

    // Configure port settings
    // Clear all flags and set defaults
    m_tios.c_iflag = IGNBRK | IGNPAR;
    m_tios.c_oflag = 0;
    m_tios.c_cflag = CREAD | CLOCAL;
    m_tios.c_lflag = 0;

    // Set data bits
    m_tios.c_cflag &= ~CSIZE;
    switch (dataBits) {
        case 5: m_tios.c_cflag |= CS5; break;
        case 6: m_tios.c_cflag |= CS6; break;
        case 7: m_tios.c_cflag |= CS7; break;
        default:
        case 8: m_tios.c_cflag |= CS8; break;
    }

    // Set parity
    m_tios.c_cflag &= ~(PARENB | PARODD);
    if (parity == EVENPARITY) {
        m_tios.c_cflag |= PARENB;
    } else if (parity == ODDPARITY) {
        m_tios.c_cflag |= (PARENB | PARODD);
    }

    // Set stop bits
    if (stopBits == TWOSTOPBITS) {
        m_tios.c_cflag |= CSTOPB;
    } else {
        m_tios.c_cflag &= ~CSTOPB;
    }

    // No flow control
    m_tios.c_iflag &= ~(IXON | IXOFF | IXANY);
    m_tios.c_cflag &= ~CRTSCTS;

    // Set timeouts: 0 means non-blocking read
    m_tios.c_cc[VMIN] = 0;   // Non-blocking read
    m_tios.c_cc[VTIME] = 0;  // No timer

    // Set baud rate
    if (cfsetispeed(&m_tios, speed) != 0 ||
        cfsetospeed(&m_tios, speed) != 0) {
        tcsetattr(m_fd, TCSANOW, &m_oldTios);
        ::close(m_fd);
        m_fd = -1;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Apply new settings
    if (tcsetattr(m_fd, TCSANOW, &m_tios) != 0) {
        tcsetattr(m_fd, TCSANOW, &m_oldTios);
        ::close(m_fd);
        m_fd = -1;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Clear buffers
    tcflush(m_fd, TCIOFLUSH);

    pthread_mutex_unlock(&m_mutex);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// open - Open serial port by device path (VSCP com.h compatibility)
//

bool Comm::open(const char *szDevice)
{
    const char *device = (NULL != szDevice) ? szDevice : "/dev/ttyS0";

    if (isOpen()) {
        return false;
    }

    pthread_mutex_lock(&m_mutex);

    m_fd = ::open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (m_fd < 0) {
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    if (-1 == lockSerialFd(m_fd)) {
        ::close(m_fd);
        m_fd = -1;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    if (tcgetattr(m_fd, &m_tios) != 0) {
        unlockSerialFd(m_fd);
        ::close(m_fd);
        m_fd = -1;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    m_oldTios = m_tios;

    pthread_mutex_unlock(&m_mutex);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// close - Close the serial port
//

void Comm::close(void)
{
    pthread_mutex_lock(&m_mutex);

    if (m_fd >= 0) {
        tcflush(m_fd, TCIOFLUSH);
        tcsetattr(m_fd, TCSANOW, &m_oldTios);
        unlockSerialFd(m_fd);
        ::close(m_fd);
        m_fd = -1;
    }

    pthread_mutex_unlock(&m_mutex);
}

///////////////////////////////////////////////////////////////////////////////
// write - Send string with optional echo and CR
//

void Comm::write(const char *str, bool bEchoed, bool bAddCR)
{
    ssize_t nResult;
    char szBuf[256];

    if (m_fd < 0 || NULL == str) {
        return;
    }

    pthread_mutex_lock(&m_mutex);

    // Build the string to send
    snprintf(szBuf, sizeof(szBuf), "%s%s", str, bAddCR ? "\r" : "");

    // Send the string
    nResult = ::write(m_fd, szBuf, strlen(szBuf));
    (void)nResult;

    pthread_mutex_unlock(&m_mutex);

    // If echoed, wait for and read the echo
    if (bEchoed) {
        char buf[256];
        readBuf(buf, sizeof(buf), 100);
    }
}

///////////////////////////////////////////////////////////////////////////////
// setParam - Set serial parameters (VSCP com.h compatibility)
//

void Comm::setParam(const char *baud, const char *parity, const char *bits, int HWFlow, int SWFlow)
{
    int spd = -1;
    int newbaud;
    int bit;

    if (!isOpen() || NULL == baud || NULL == parity || NULL == bits) {
        return;
    }

    bit = bits[0];

    if ((newbaud = (atol(baud) / 100)) == 0 && baud[0] != '0') {
        newbaud = -1;
    }

    switch (newbaud) {
        case 0: spd = B0; break;
        case 3: spd = B300; break;
        case 6: spd = B600; break;
        case 12: spd = B1200; break;
        case 24: spd = B2400; break;
        case 48: spd = B4800; break;
        case 96: spd = B9600; break;
        case 192: spd = B19200; break;
        case 384: spd = B38400; break;
        case 576: spd = B57600; break;
        case 1152: spd = B115200; break;
        default: break;
    }

    pthread_mutex_lock(&m_mutex);

    if (tcgetattr(m_fd, &m_tios) != 0) {
        pthread_mutex_unlock(&m_mutex);
        return;
    }

    if (spd != -1) {
        cfsetospeed(&m_tios, (speed_t)spd);
        cfsetispeed(&m_tios, (speed_t)spd);
    }

    m_tios.c_cflag &= ~CSIZE;
    switch (bit) {
        case '5': m_tios.c_cflag |= CS5; break;
        case '6': m_tios.c_cflag |= CS6; break;
        case '7': m_tios.c_cflag |= CS7; break;
        case '8':
        default:  m_tios.c_cflag |= CS8; break;
    }

    m_tios.c_iflag = IGNBRK;
    m_tios.c_lflag = 0;
    m_tios.c_oflag = 0;
    m_tios.c_cflag |= CLOCAL | CREAD;

    if (SWFlow) {
        m_tios.c_iflag |= IXON | IXOFF;
    } else {
        m_tios.c_iflag &= ~(IXON | IXOFF | IXANY);
    }

    if (HWFlow) {
        m_tios.c_cflag |= CRTSCTS;
    } else {
        m_tios.c_cflag &= ~CRTSCTS;
    }

    m_tios.c_cflag &= ~(PARENB | PARODD);
    if (parity[0] == 'E') {
        m_tios.c_cflag |= PARENB;
    } else if (parity[0] == 'O') {
        m_tios.c_cflag |= PARODD;
    }

    m_tios.c_cc[VMIN] = 1;
    m_tios.c_cc[VTIME] = 5;

    tcsetattr(m_fd, TCSANOW, &m_tios);

    pthread_mutex_unlock(&m_mutex);
}

///////////////////////////////////////////////////////////////////////////////
// comm_puts - Send string (VSCP com.h compatibility)
//

int Comm::comm_puts(char *Buffer, bool bDrain)
{
    if (NULL == Buffer) {
        return 0;
    }

    return comm_puts(Buffer, (int)strlen(Buffer), bDrain);
}

///////////////////////////////////////////////////////////////////////////////
// comm_puts - Send byte array (VSCP com.h compatibility)
//

int Comm::comm_puts(char *Buffer, int len, bool bDrain)
{
    ssize_t rv;

    if (!isOpen() || NULL == Buffer || len <= 0) {
        return 0;
    }

    pthread_mutex_lock(&m_mutex);
    rv = ::write(m_fd, Buffer, (size_t)len);
    if (bDrain && rv > 0) {
        tcdrain(m_fd);
    }
    pthread_mutex_unlock(&m_mutex);

    if (rv <= 0) {
        return 0;
    }

    return (int)rv;
}

///////////////////////////////////////////////////////////////////////////////
// comm_gets - Read available data (VSCP com.h compatibility)
//

int Comm::comm_gets(char *Buffer, int max)
{
    if (!isOpen() || NULL == Buffer || max <= 0) {
        return 0;
    }

    return readBuf(Buffer, (size_t)max, 0);
}

///////////////////////////////////////////////////////////////////////////////
// comm_gets - Read data with timeout in microseconds (VSCP compatibility)
//

int Comm::comm_gets(char *Buffer, int nChars, long timeout)
{
    int cnt;
    int capacity;
    int s_to = 0;
    int us_to = 0;

    if (!isOpen() || NULL == Buffer || nChars <= 0) {
        return 0;
    }

    if (1 == nChars) {
        Buffer[0] = '\0';
        return 0;
    }

    capacity = nChars - 1;

    if (timeout > 1000000) {
        s_to = (int)(timeout / 1000000);
        if (s_to > 60) {
            s_to = 60;
        }
        us_to = (int)(timeout - (long)s_to * 1000000L);
    } else {
        us_to = (int)timeout;
    }

    pthread_mutex_lock(&m_mutex);

    for (cnt = 0; cnt < capacity; cnt++) {
        fd_set readfds;
        struct timeval tval;

        FD_ZERO(&readfds);
        FD_SET(m_fd, &readfds);

        tval.tv_sec = s_to;
        tval.tv_usec = us_to;

        if (select(m_fd + 1, &readfds, NULL, NULL, &tval) > 0) {
            if (::read(m_fd, &Buffer[cnt], 1) != 1) {
                Buffer[cnt] = '\0';
                pthread_mutex_unlock(&m_mutex);
                return cnt;
            }
        } else {
            Buffer[cnt] = '\0';
            pthread_mutex_unlock(&m_mutex);
            return cnt;
        }
    }

    Buffer[capacity] = '\0';
    pthread_mutex_unlock(&m_mutex);
    return capacity;
}

///////////////////////////////////////////////////////////////////////////////
// readBuf - Read buffer with timeout
//

int Comm::readBuf(char *buf, size_t size, int timeout)
{
    ssize_t nResult;
    fd_set readfds;
    struct timeval tv;
    int selectResult;

    if (m_fd < 0 || NULL == buf || 0 == size) {
        return -1;
    }

    pthread_mutex_lock(&m_mutex);

    // Set up select with timeout
    FD_ZERO(&readfds);
    FD_SET(m_fd, &readfds);

    if (timeout >= 0) {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        selectResult = select(m_fd + 1, &readfds, NULL, NULL, &tv);
    } else {
        // No timeout
        selectResult = select(m_fd + 1, &readfds, NULL, NULL, NULL);
    }

    if (selectResult <= 0) {
        pthread_mutex_unlock(&m_mutex);
        return selectResult;  // 0 for timeout, -1 for error
    }

    // Data is available, read it
    nResult = ::read(m_fd, buf, size - 1);

    // Null terminate if successful
    if (nResult > 0 && nResult < (ssize_t)size) {
        buf[nResult] = '\0';
    }

    pthread_mutex_unlock(&m_mutex);

    if (nResult < 0) {
        return -1;
    }

    return (int)nResult;
}


///////////////////////////////////////////////////////////////////////////////
// readChar - Read single character with timeout
//

unsigned char Comm::readChar(int *pCnt)
{
    char c = 0;
    ssize_t nRead;
    fd_set readfds;
    struct timeval tv;
    int selectResult;
    int localCnt = 0;

    if (NULL == pCnt) {
        pCnt = &localCnt;
    }

    if (m_fd < 0) {
        *pCnt = 0;
        return 0;
    }

    pthread_mutex_lock(&m_mutex);

    // Set up select with short timeout (100ms)
    FD_ZERO(&readfds);
    FD_SET(m_fd, &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;  // 100ms timeout

    selectResult = select(m_fd + 1, &readfds, NULL, NULL, &tv);

    if (selectResult <= 0) {
        pthread_mutex_unlock(&m_mutex);
        *pCnt = 0;
        return 0;
    }

    // Data is available, read one character
    nRead = ::read(m_fd, &c, 1);

    // Try to check how many bytes are available
    // macOS doesn't have a direct equivalent to Windows' GetFileSize on serial ports,
    // so we use a non-blocking read to check or just return 0 for simplicity
    *pCnt = 0;  // Conservative: don't indicate more bytes available

    pthread_mutex_unlock(&m_mutex);

    if (nRead != 1) {
        return 0;
    }

    return (unsigned char)c;
}

///////////////////////////////////////////////////////////////////////////////
// drainInput - Clear input buffer
//

void Comm::drainInput(void)
{
    if (m_fd >= 0) {
        tcflush(m_fd, TCIFLUSH);
    }
}

///////////////////////////////////////////////////////////////////////////////
// sendCommand - Send command and wait for response
//

bool Comm::sendCommand(const char *cmd, const char *term, int wait)
{
    char response[256];

    if (NULL == cmd || NULL == term) {
        return false;
    }

    // Send the command with terminator
    write(cmd, false, false);
    write(term, false, false);

    // Wait for response
    if (wait > 0) {
        usleep(wait * 1000);  // Convert ms to us
    }

    // Read response to verify
    int nRead = readBuf(response, sizeof(response), 100);
    
    // Return success if we got a response
    return (nRead > 0);
}
