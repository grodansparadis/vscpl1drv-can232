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
#include <stdio.h>
#include <stdlib.h>

// Handshake constants (parity and stop bits are defined in Windows headers)
#define HANDSHAKE_NONE          0x00
#define HANDSHAKE_XONXOFF      0x01
#define HANDSHAKE_RTS_CTS      0x02
#define HANDSHAKE_DTR_DSR      0x03

///////////////////////////////////////////////////////////////////////////////
// CComm - Windows serial communication class
//

class CComm {
public:
    // Constructor
    CComm(void);
    
    // Destructor
    ~CComm(void);

    // Initialize serial port
    // port: COM port number (1, 2, 3, etc.)
    // baudrate: Baud rate
    // dataBits: Data bits (5-8)
    // parity: Parity (NOPARITY, EVENPARITY, ODDPARITY, etc.)
    // stopBits: Stop bits (ONESTOPBIT, TWOSTOPBITS, etc.)
    // handshake: Handshake mode
    // Returns: true on success
    bool init(unsigned int port, unsigned long baudrate, 
              int dataBits, int parity, int stopBits, int handshake);

    // Close communication port
    void close(void);

    // Check if port is open
    HANDLE getHandle(void) const { return m_hComPort; }

    // Send string with optional echo and CR
    // str: String to send
    // bEchoed: Wait for echo
    // bAddCR: Add carriage return
    void write(const char *str, bool bEchoed = false, bool bAddCR = false);

    // Read buffer with timeout
    // buf: Buffer to read into
    // size: Buffer size
    // timeout: Timeout in milliseconds (-1 for no timeout)
    // Returns: Number of bytes read
    int readBuf(char *buf, size_t size, int timeout = -1);

    // Read single character with timeout
    // pCnt: Pointer to count variable (set to number of characters remaining, 0 on timeout)
    // Returns: Character read
    unsigned char readChar(int *pCnt);

    // Clear input buffer
    void drainInput(void);

    // Send command and wait for response
    // cmd: Command to send
    // term: Terminator string
    // wait: Wait time between send and read
    // Returns: true if successful
    bool sendCommand(const char *cmd, const char *term, int wait = 100);

private:
    HANDLE m_hComPort;          ///< Handle to serial port
    DCB m_dcb;                  ///< Device control block
    COMMTIMEOUTS m_timeouts;    ///< Serial port timeouts
    DWORD m_readTimeout;        ///< Read timeout in milliseconds
};

#endif // COM_WIN32_H
