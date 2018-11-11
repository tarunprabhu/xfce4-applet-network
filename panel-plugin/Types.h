#ifndef XFCE_APPLET_SPEED_TYPES_H
#define XFCE_APPLET_SPEED_TYPES_H

#include "Array.h"
#include "Enum.h"

// Useful typedefs and enums are collected here

ENUM_CREATE(XferDirection,
            Rx, // Incoming data from the network or device
            Tx  // Data sent on the network or written to the device
);

// Indicates whether the value of the statistic is over the sampling interval
// or since the device came up
ENUM_CREATE(StatsRange, Interval, Total);

ENUM_CREATE(IconKind, Dialog, Menu, Toolbar, Tooltip);

ENUM_CREATE(LabelPosition, Left, Top, Right, Bottom);

ENUM_CREATE(TooltipVerbosity, Limited, Moderate, Verbose);

ENUM_CREATE(DeviceStatus,
            Connected,    // The device is connected and operational
            Disabled,     // The device is not connected
            Disconnected, // In the case of networks, the network is down
            Error // This occurs when the device stats cannot be obtained even
                  // though the device is determined to be available
);

ENUM_CREATE(DiskKind,
            Internal,   // Internal hard disk (SSD, SATA etc.)
            Multimedia, // Multimedia card (SD, MMC etc.)
            Optical,    // Optical device
            Other,      // Unknown device kind
            Removable   // Removable disk (USB, Firewire etc.)
);

ENUM_CREATE(NetworkKind,
            Cellular, // Cellular networks
            Other,    // Other network kinds that we may not yet know about
            PPP,      // Old-school internet over a modem
            USB,      // USB Ethernet
            Virtual,  // Virtual networks include loopbacks and VM networks
            Wired,    // Ethernet and other wired networks
            Wireless  // WLAN networks
);

ENUM_CREATE(DeviceClass,
            Disk, // The device is a block device (hard disk, optical device,
            // multimedia card etc.)
            Network // The device is a network interface
);

// Since the disk kind and the network kind enums are different
// Given the device type, we need to find a way to find the right enum for
// its kind. We also want this to be indepdendent of the value itself so
// clients do not need to be changed if the order of values in enum DeviceClass
// change
template <DeviceClass clss> struct DeviceClassKind;

template <> struct DeviceClassKind<DeviceClass::Disk> {
  using type = DiskKind;
};

template <> struct DeviceClassKind<DeviceClass::Network> {
  using type = NetworkKind;
};

template <typename Type, typename Dim1> using Array1 = Array<Type, Dim1>;

template <typename Type, typename Dim1, typename Dim2>
using Array2 = Array<Array<Type, Dim2>, Dim1>;

template <typename Type, typename Dim1, typename Dim2, typename Dim3>
using Array3 = Array<Array<Array<Type, Dim3>, Dim2>, Dim1>;

#endif // XFCE_APPLET_SPEED_TYPES_H
