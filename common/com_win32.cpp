///////////////////////////////////////////////////////////////////////////////
// com_win32.cpp: Windows-specific serial communication implementation
//
// This file is part of vscpl1drv-can232
// Copyright (C) 2000-2026 Ake Hedman, Grodans Paradis AB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//

#include "com_win32.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor
//

CComm::CComm(void)
{
    m_hComPort = INVALID_HANDLE_VALUE;
    m_readTimeout = 1000;  // Default 1 second timeout
    
    ZeroMemory(&m_dcb, sizeof(DCB));
    ZeroMemory(&m_timeouts, sizeof(COMMTIMEOUTS));
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
//

CComm::~CComm(void)
{
    close();
}

///////////////////////////////////////////////////////////////////////////////
// open - Open serial port with specified parameters
//

bool CComm::open(const char *portname, long baudrate)
{
    if (NULL == portname || !*portname) {
        return false;
    }

    // Close existing port if open
    if (INVALID_HANDLE_VALUE != m_hComPort) {
        close();
    }

    // Open the serial port
    m_hComPort = CreateFileA(
        portname,
        GENERIC_READ | GENERIC_WRITE,
        0,                      // Exclusive access
        NULL,                   // No security
        OPEN_EXISTING,          // Must exist
        0,                      // No overlapped I/O
        NULL                    // No template
    );

    if (INVALID_HANDLE_VALUE == m_hComPort) {
        return false;
    }

    // Get current port settings
    if (!GetCommState(m_hComPort, &m_dcb)) {
        CloseHandle(m_hComPort);
        m_hComPort = INVALID_HANDLE_VALUE;
        return false;
    }

    // Configure port settings
    m_dcb.DCBlength = sizeof(DCB);
    m_dcb.BaudRate = baudrate;
    m_dcb.ByteSize = 8;
    m_dcb.StopBits = ONESTOPBIT;
    m_dcb.Parity = NOPARITY;
    m_dcb.fDtrControl = DTR_CONTROL_ENABLE;
    m_dcb.fRtsControl = RTS_CONTROL_ENABLE;
    m_dcb.fOutX = FALSE;
    m_dcb.fInX = FALSE;
    m_dcb.fErrorChar = FALSE;
    m_dcb.fNull = FALSE;
    m_dcb.fAbortOnError = FALSE;

    // Set new port settings
    if (!SetCommState(m_hComPort, &m_dcb)) {
        CloseHandle(m_hComPort);
        m_hComPort = INVALID_HANDLE_VALUE;
        return false;
    }

    // Set timeouts
    m_timeouts.ReadIntervalTimeout = 0;
    m_timeouts.ReadTotalTimeoutMultiplier = 0;
    m_timeouts.ReadTotalTimeoutConstant = m_readTimeout;
    m_timeouts.WriteTotalTimeoutMultiplier = 0;
    m_timeouts.WriteTotalTimeoutConstant = 5000;  // 5 second write timeout

    if (!SetCommTimeouts(m_hComPort, &m_timeouts)) {
        CloseHandle(m_hComPort);
        m_hComPort = INVALID_HANDLE_VALUE;
        return false;
    }

    // Purge any outstanding I/O
    PurgeComm(m_hComPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// close - Close the serial port
//

void CComm::close(void)
{
    if (INVALID_HANDLE_VALUE != m_hComPort) {
        PurgeComm(m_hComPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
        CloseHandle(m_hComPort);
        m_hComPort = INVALID_HANDLE_VALUE;
    }
}

///////////////////////////////////////////////////////////////////////////////
// send - Send data to the serial port
//

int CComm::send(const void *buf, size_t len)
{
    DWORD dwBytesWritten = 0;

    if (INVALID_HANDLE_VALUE == m_hComPort || NULL == buf || 0 == len) {
        return -1;
    }

    if (!WriteFile(m_hComPort, buf, (DWORD)len, &dwBytesWritten, NULL)) {
        return -1;
    }

    return (int)dwBytesWritten;
}

///////////////////////////////////////////////////////////////////////////////
// receive - Receive data from the serial port
//

int CComm::receive(void *buf, size_t len)
{
    DWORD dwBytesRead = 0;

    if (INVALID_HANDLE_VALUE == m_hComPort || NULL == buf || 0 == len) {
        return -1;
    }

    if (!ReadFile(m_hComPort, buf, (DWORD)len, &dwBytesRead, NULL)) {
        return -1;
    }

    return (int)dwBytesRead;
}

///////////////////////////////////////////////////////////////////////////////
// flush - Flush serial port buffers
//

void CComm::flush(void)
{
    if (INVALID_HANDLE_VALUE != m_hComPort) {
        FlushFileBuffers(m_hComPort);
        PurgeComm(m_hComPort, PURGE_TXCLEAR | PURGE_RXCLEAR);
    }
}
