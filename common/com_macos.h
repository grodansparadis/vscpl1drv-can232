///////////////////////////////////////////////////////////////////////////////
// com_macos.h: macOS-specific serial communication interface
//
// This file is part of vscpl1drv-can232
// Copyright (C) 2000-2026 Ake Hedman, Grodans Paradis AB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//

#ifndef COM_MACOS_H
#define COM_MACOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

// Parity constants
#define NOPARITY        0x00
#define EVENPARITY      0x01
#define ODDPARITY       0x02

// Stop bits constants
#define ONESTOPBIT      0x00
#define TWOSTOPBITS     0x02

// Handshake constants
#define HANDSHAKE_NONE          0x00
#define HANDSHAKE_XONXOFF      0x01

///////////////////////////////////////////////////////////////////////////////
// CComm - POSIX/macOS serial communication class
//

class Comm {
public:
    // Constructor
    Comm(void);
    
    // Destructor
    ~Comm(void);

    // Initialize serial port with configuration
    bool init(unsigned int port, unsigned long baudrate, 
              int dataBits, int parity, int stopBits, int handshake);

    // Close communication port
    void close(void);

    // Check if port is open
    int getHandle(void) const { return m_fd; }

    // Send string with optional echo and CR
    void write(const char *str, bool bEchoed = false, bool bAddCR = false);

    // Read buffer with timeout
    int readBuf(char *buf, size_t size, int timeout = -1);

    // Read single character with timeout
    // pCnt: Pointer to count variable (set to number of characters remaining, 0 on timeout)
    // Returns: Character read
    unsigned char readChar(int *pCnt);

    // Clear input buffer
    void drainInput(void);

    // Send command and wait for response
    bool sendCommand(const char *cmd, const char *term, int wait = 100);

private:
    int m_fd;                           ///< File descriptor for serial port
    struct termios m_tios;              ///< Terminal settings
    struct termios m_oldTios;           ///< Original terminal settings
    pthread_mutex_t m_mutex;            ///< Mutex for thread safety
    unsigned int m_readTimeout;         ///< Read timeout in milliseconds
    
    // Helper to convert port number to device path
    void getPortName(unsigned int port, char *devpath, size_t maxlen);
};

// Define CComm as alias for Comm for compatibility
typedef Comm CComm;

#endif // COM_MACOS_H
