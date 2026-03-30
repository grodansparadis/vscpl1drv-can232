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
// close - Close the serial port
//

void Comm::close(void)
{
    pthread_mutex_lock(&m_mutex);

    if (m_fd >= 0) {
        tcflush(m_fd, TCIOFLUSH);
        tcsetattr(m_fd, TCSANOW, &m_oldTios);
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
    strncpy(szBuf, str, sizeof(szBuf) - 1);
    szBuf[sizeof(szBuf) - 1] = '\0';
    
    if (bAddCR) {
        strncat(szBuf, "\r", sizeof(szBuf) - 1);
    }

    // Send the string
    nResult = ::write(m_fd, szBuf, strlen(szBuf));

    pthread_mutex_unlock(&m_mutex);

    // If echoed, wait for and read the echo
    if (bEchoed) {
        char buf[256];
        readBuf(buf, sizeof(buf), 100);
    }
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

int Comm::readChar(int timeout)
{
    char c;
    int nRead = readBuf(&c, 2, timeout);  // 2 for space + null terminator
    
    if (nRead != 1) {
        return -1;
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
