% VSCPL1DRV-CAN232(1) VSCP Level I CAN232 (slcan) Driver
% Åke Hedman, Grodans Paradis AB
% October 7, 2019

# NAME

vscpl1drv-can232 - Lawicel CAN232 (slcan) Level I Driver

# SYNOPSIS

vscpl1drv-can232

# DESCRIPTION

This driver interface is for the can232 adapter from Lawicel (or other slcan drivers). This is a low cost CAN adapter that connects to one of the serial communication ports on a computer. The driver can handle the adapter in both polled and non polled mode, which handled transparently to the user. It is recommended however that the following settings are made before real life use.

* Set the baud rate for the device to 115200. You do this with the U1 command. This is the default baud rate used by this driver.
* Set auto poll mode by issuing the X1 command.
* Enable the time stamp by issuing the Z1 command.

## Configuration string

The driver string has the following format (note that all values can be entered in either decimal or hexadecimal form (for hex precede with 0x).

> comport;baudrate;mask;filter;bus-speed[;btr0;btr1]

####  comport
The serial communication port to use. For windows use 1,2,3... for Linux use /dev/ttyS0, /dev/ttyUSB1 etc.

#### baudrate
A valid baud rate for the **serial interface** ( for example. 9600 ).

#### mask
The mask for the adapter. Read the Lawicel CAN232 manual on how to set this. It is not the same as for CANAL/VSCP.

#### filter
The filter for the adapter. Read the Lawicel CAN232 manual on how to set this. It is not the same as for CANAL.

#### bus-speed
is the speed or the **CAN interface**. Valid values are

| Setting | Bus-speed |
| :-----: | :---------: |
| 10 | 10Kbps |
| 20 | 20Kbps |
| 50 | 50Kbps |
| 100 |  100Kbps |
| 125 | 125Kbps |
| 250 | 250Kbps |
| 500 | 500Kbps |
| 800 | 800Kbps |
| 1000 | 1Mbps |

#### btr0/btr1 (Optional.)
Instead of setting a bus-speed you can set the SJA1000 BTR0/BTR1 values directly. If both are set the bus_speed parameter is ignored.

This link can be a help for data https://www.port.de/engl/canprod/sv_req_form.html

If no device string is given COM1/ttyS0 will be used. Baud rate will be set to 115200 baud and the filter/mask to fully open. The CAN bit rate will be 500Kbps.

## Flags

 Not used. Set to zero.

## Status return

The CanalGetStatus call returns the status structure with the channel_status member having the following meaning:

 | Bit      | Description |
 | ---      | ----------- |
 | Bit 0-7  | TX Error Counter. |
 | Bit 8-15 | RX Error Counter. |
 | Bit 16   | Overflow. Cleard by status read. |
 | Bit 17   | RX Warning. |
 | Bit 18   | TX Warning. |
 | Bit 19   | TX bus passive. |
 | Bit 20   | RX bus passive. |
 | Bit 21   | Reserved. |
 | Bit 22   | Reserved. |
 | Bit 23   | Reserved. |
 | Bit 24   | Reserved. |
 | Bit 25   | Reserved. |
 | Bit 26   | Reserved. |
 | Bit 27   | Reserved. |
 | Bit 28   | Reserved. |
 | Bit 29   | Bus Passive. |
 | Bit 30   | Bus Warning status. |
 | Bit 31   | Bus off status |

## Example configurations

> 5;115200;0;0;1000

Uses COM5 at 115200 with filters/masks open to receive all messages and with 1Mbps CAN bit rate.

> /dev/ttyUSB1;57600;0;0;0;0x09;0x1C

Uses serial USB adapter 1 at 57600 baud with filters/masks open to receive all messages and with a CAN bit-rate set to 50Kbps using btr0/btr1

### Typical settings for VSCP daemon config

```xml
    <driver enable="true" >
        <name>can232</name>
        <config>/dev/ttyUSB1;57600;0;0;0;0x09;0x1C</config>
        <path>/usr/lib/vscpl1drv_can232.so</path>
        <flags>0</flags>
        <guid>00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00</guid>
    </driver>
```

---

There are many Level I drivers (CANAL drivers) available in VSCP & Friends framework that can be used with both VSCP Works and the VSCP Daemon (vscpd) and other tools that interface the drivers using the CANAL standard interface. Added to that many Level II and Level III drivers are available that can be used with the VSCP Daemon.

Level I drivers is documented [here](https://grodansparadis.gitbooks.io/the-vscp-daemon/level_i_drivers.html).

Level II drivers is documented [here](https://grodansparadis.gitbooks.io/the-vscp-daemon/level_ii_drivers.html)

Level III drivers is documented [here](https://grodansparadis.gitbooks.io/the-vscp-daemon/level_iii_drivers.html)

# SEE ALSO

`vscpd` (8).
`uvscpd` (8).
`vscpworks` (1).
`vscpcmd` (1).
`vscp-makepassword` (1).
`vscphelperlib` (1).

The VSCP project homepage is here <https://www.vscp.org>.

The [manual](https://grodansparadis.gitbooks.io/the-vscp-daemon) for vscpd contains full documentation. Other documentation can be found here <https://grodansparadis.gitbooks.io>.

The vscpd source code may be downloaded from <https://github.com/grodansparadis/vscp>. Source code for other system components of VSCP & Friends are here <https://github.com/grodansparadis>

# COPYRIGHT
Copyright 2000-2019 Åke Hedman, Grodans Paradis AB - MIT license.
