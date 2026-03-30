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
// open - Open serial port with specified parameters
//

bool Comm::open(const char *portname, speed_t baudrate)
{
    if (NULL == portname || !*portname) {
        return false;
    }

    // Close existing port if open
    if (m_fd >= 0) {
        close();
    }

    pthread_mutex_lock(&m_mutex);

    // Open the serial port
    m_fd = ::open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
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
    m_tios.c_cflag = CREAD | CLOCAL | CS8;
    m_tios.c_lflag = 0;

    // No flow control
    m_tios.c_iflag &= ~(IXON | IXOFF | IXANY);
    m_tios.c_cflag &= ~(CRTSCTS);

    // Set parity: none
    m_tios.c_cflag &= ~PARENB;

    // Set stop bits: 1
    m_tios.c_cflag &= ~CSTOPB;

    // Set baud rate
    if (cfsetispeed(&m_tios, baudrate) != 0 ||
        cfsetospeed(&m_tios, baudrate) != 0) {
        tcsetattr(m_fd, TCSANOW, &m_oldTios);
        ::close(m_fd);
        m_fd = -1;
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Set timeouts: 0 means non-blocking
    m_tios.c_cc[VMIN] = 0;   // Non-blocking read
    m_tios.c_cc[VTIME] = 0;  // No timer

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
// send - Send data to the serial port
//

int Comm::send(const void *buf, size_t len)
{
    ssize_t nResult;

    if (m_fd < 0 || NULL == buf || 0 == len) {
        return -1;
    }

    pthread_mutex_lock(&m_mutex);

    nResult = write(m_fd, buf, len);

    pthread_mutex_unlock(&m_mutex);

    if (nResult < 0) {
        return -1;
    }

    return (int)nResult;
}

///////////////////////////////////////////////////////////////////////////////
// receive - Receive data from the serial port with timeout
//

int Comm::receive(void *buf, size_t len)
{
    ssize_t nResult;
    fd_set readfds;
    struct timeval tv;

    if (m_fd < 0 || NULL == buf || 0 == len) {
        return -1;
    }

    pthread_mutex_lock(&m_mutex);

    // Set up select with timeout
    FD_ZERO(&readfds);
    FD_SET(m_fd, &readfds);

    tv.tv_sec = m_readTimeout / 1000;
    tv.tv_usec = (m_readTimeout % 1000) * 1000;

    // Wait for data with timeout
    int selectResult = select(m_fd + 1, &readfds, NULL, NULL, &tv);

    if (selectResult <= 0) {
        pthread_mutex_unlock(&m_mutex);
        return selectResult;  // 0 for timeout, -1 for error
    }

    // Data is available, read it
    nResult = read(m_fd, buf, len);

    pthread_mutex_unlock(&m_mutex);

    if (nResult < 0) {
        return -1;
    }

    return (int)nResult;
}

///////////////////////////////////////////////////////////////////////////////
// flush - Flush serial port buffers
//

void Comm::flush(void)
{
    if (m_fd >= 0) {
        pthread_mutex_lock(&m_mutex);
        tcflush(m_fd, TCIOFLUSH);
        pthread_mutex_unlock(&m_mutex);
    }
}
