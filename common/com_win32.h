///////////////////////////////////////////////////////////////////////////////
// com_win32.h: Windows-specific serial communication interface
//
// This file is part of vscpl1drv-can232
// Copyright (C) 2000-2026 Ake Hedman, Grodans Paradis AB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//

#ifndef COM_WIN32_H
#define COM_WIN32_H

#include <windows.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// CComm - Windows serial communication class
//

class CComm {
public:
    // Constructor
    CComm(void);
    
    // Destructor
    ~CComm(void);

    // Open communication port
    // portname: COM port name (e.g., "COM1", "COM2")
    // baudrate: Baud rate (9600, 19200, 38400, 57600, 115200, etc.)
    // Returns: true on success, false on failure
    bool open(const char *portname, long baudrate = 9600);

    // Close communication port
    void close(void);

    // Check if port is open
    bool isOpen(void) const { return m_hComPort != INVALID_HANDLE_VALUE; }

    // Send data
    // Returns: number of bytes sent, -1 on error
    int send(const void *buf, size_t len);

    // Receive data
    // Returns: number of bytes received, -1 on error
    int receive(void *buf, size_t len);

    // Flush serial buffers
    void flush(void);

    // Set receive timeout (milliseconds)
    void setReadTimeout(DWORD ms) { m_readTimeout = ms; }

    // Get receive timeout
    DWORD getReadTimeout(void) const { return m_readTimeout; }

private:
    HANDLE m_hComPort;          ///< Handle to serial port
    DCB m_dcb;                  ///< Device control block
    COMMTIMEOUTS m_timeouts;    ///< Serial port timeouts
    DWORD m_readTimeout;        ///< Read timeout in milliseconds
};

#endif // COM_WIN32_H
