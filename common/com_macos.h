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

///////////////////////////////////////////////////////////////////////////////
// Comm - macOS/POSIX serial communication class
//

class Comm {
public:
    // Constructor
    Comm(void);
    
    // Destructor
    ~Comm(void);

    // Open communication port
    // portname: Serial port device (e.g., "/dev/ttyUSB0", "/dev/tty.usbserial")
    // baudrate: Baud rate (9600, 19200, 38400, 57600, 115200, etc.)
    // Returns: true on success, false on failure
    bool open(const char *portname, speed_t baudrate = B9600);

    // Close communication port
    void close(void);

    // Check if port is open
    bool isOpen(void) const { return m_fd >= 0; }

    // Send data
    // Returns: number of bytes sent, -1 on error
    int send(const void *buf, size_t len);

    // Receive data (non-blocking with timeout)
    // Returns: number of bytes received, 0 on timeout, -1 on error
    int receive(void *buf, size_t len);

    // Flush serial buffers
    void flush(void);

    // Set receive timeout (milliseconds)
    void setReadTimeout(unsigned int ms) { m_readTimeout = ms; }

    // Get receive timeout
    unsigned int getReadTimeout(void) const { return m_readTimeout; }

private:
    int m_fd;                           ///< File descriptor for serial port
    struct termios m_tios;              ///< Terminal settings
    struct termios m_oldTios;           ///< Original terminal settings
    pthread_mutex_t m_mutex;            ///< Mutex for thread safety
    unsigned int m_readTimeout;         ///< Read timeout in milliseconds
};

#endif // COM_MACOS_H
