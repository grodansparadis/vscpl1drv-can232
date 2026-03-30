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
// init - Initialize serial port with specified parameters
//

bool CComm::init(unsigned int port, unsigned long baudrate, 
                 int dataBits, int parity, int stopBits, int handshake)
{
    char portname[32];

    // Close existing port if open
    if (INVALID_HANDLE_VALUE != m_hComPort) {
        close();
    }

    // Format port name (e.g., "COM1", "COM2", etc.)
    sprintf_s(portname, sizeof(portname), "COM%u", port);

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
    m_dcb.ByteSize = (BYTE)dataBits;
    m_dcb.Parity = (BYTE)parity;
    m_dcb.StopBits = (BYTE)stopBits;
    m_dcb.fDtrControl = DTR_CONTROL_ENABLE;
    m_dcb.fRtsControl = RTS_CONTROL_ENABLE;
    
    // Configure handshake
    if (HANDSHAKE_XONXOFF == handshake) {
        m_dcb.fOutX = TRUE;
        m_dcb.fInX = TRUE;
    } else {
        m_dcb.fOutX = FALSE;
        m_dcb.fInX = FALSE;
    }

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
// write - Send string with optional echo and CR
//

void CComm::write(const char *str, bool bEchoed, bool bAddCR)
{
    DWORD dwBytesWritten;
    char szBuf[256];

    if (INVALID_HANDLE_VALUE == m_hComPort || NULL == str) {
        return;
    }

    // Build the string to send
    strncpy_s(szBuf, sizeof(szBuf), str, _TRUNCATE);
    
    if (bAddCR) {
        strncat_s(szBuf, sizeof(szBuf), "\r", _TRUNCATE);
    }

    // Send the string
    WriteFile(m_hComPort, szBuf, (DWORD)strlen(szBuf), &dwBytesWritten, NULL);

    // If echoed, wait for and read the echo
    if (bEchoed) {
        char buf[256];
        readBuf(buf, sizeof(buf), 100);
    }
}

///////////////////////////////////////////////////////////////////////////////
// readBuf - Read buffer with timeout
//

int CComm::readBuf(char *buf, size_t size, int timeout)
{
    DWORD dwBytesRead = 0;
    COMMTIMEOUTS oldTimeouts, newTimeouts;

    if (INVALID_HANDLE_VALUE == m_hComPort || NULL == buf || 0 == size) {
        return -1;
    }

    // Save old timeouts
    GetCommTimeouts(m_hComPort, &oldTimeouts);

    // Set new timeout
    newTimeouts = oldTimeouts;
    if (timeout >= 0) {
        newTimeouts.ReadTotalTimeoutConstant = timeout;
    } else {
        // No timeout
        newTimeouts.ReadTotalTimeoutConstant = 0;
    }
    SetCommTimeouts(m_hComPort, &newTimeouts);

    // Read data
    if (!ReadFile(m_hComPort, buf, (DWORD)size - 1, &dwBytesRead, NULL)) {
        // Error reading
        SetCommTimeouts(m_hComPort, &oldTimeouts);
        return -1;
    }

    // Null terminate the string
    if (dwBytesRead < size) {
        buf[dwBytesRead] = '\0';
    }

    // Restore old timeouts
    SetCommTimeouts(m_hComPort, &oldTimeouts);

    return (int)dwBytesRead;
}

///////////////////////////////////////////////////////////////////////////////
// readChar - Read single character with timeout
//

int CComm::readChar(int timeout)
{
    char c;
    int nRead = readBuf(&c, 1, timeout);
    
    if (nRead != 1) {
        return -1;
    }
    
    return (unsigned char)c;
}

///////////////////////////////////////////////////////////////////////////////
// drainInput - Clear input buffer
//

void CComm::drainInput(void)
{
    if (INVALID_HANDLE_VALUE != m_hComPort) {
        PurgeComm(m_hComPort, PURGE_RXCLEAR);
    }
}

///////////////////////////////////////////////////////////////////////////////
// sendCommand - Send command and wait for response
//

bool CComm::sendCommand(const char *cmd, const char *term, int wait)
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
        Sleep(wait);
    }

    // Read response to verify
    int nRead = readBuf(response, sizeof(response), 100);
    
    // Return success if we got a response
    return (nRead > 0);
}
