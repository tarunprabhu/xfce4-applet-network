#ifndef XFCE_APPLET_SPEED_TYPES_H
#define XFCE_APPLET_SPEED_TYPES_H

#include "Array.h"
#include "Enum.h"

// Useful typedefs and enums are collected here

ENUM(XferDirection,
     Rx, // Incoming data from the network or device
     Tx  // Data sent on the network or written to the device
);

// Indicates whether the value of the statistic is over the sampling interval
// or since the device came up
ENUM(StatsRange, // <
     Interval,   // <
     Total);

// The purpose of the icon. This is used to determine the size that will be
// used. By not having the size hard-coded here, we can (comparatively) easily
// change the icon size as desired
ENUM(IconKind, // <
     Dialog,
     Menu,
     Toolbar,
     Tooltip);

ENUM(LabelPosition, // <
     Left,          // <
     Top,           // <
     Right,         // <
     Bottom);

ENUM(DialKind,
     SingleArc360,      // A single (almost) circular dial
     SingleArc180,      // A semi-circular dial
     MultipleArc360,    // Separate (almost) circular dials
     MultipleArc180,    // Separate semi-circular dials
     CombinedVertical,  // Two semi-circular vertical dials
     CombinedHorizontal // Two semi-circular horizontal dials
);

ENUM(Verbosity, // <
     Limited,
     Moderate,
     Verbose);

ENUM(DeviceStatus,
     // <begin common values>
     Unavailable, // The device is not present
     Error,       // The device stats cannot be obtained even
     // <end common values>

     // <begin disk-specific values>
     Mounted,
     Unmounted,
     // <end disk-specific values>

     // <begin network-specific values>
     Connected,   // The interface is up
     Disconnected // The interface is down
                  // <end network-specific values>
);

ENUM(DiskKind,
     Internal,   // Internal hard disk (SSD, SATA etc.)
     Multimedia, // Multimedia card (SD, MMC etc.)
     Optical,    // Optical device
     Other,      // Unknown device kind
     Removable   // Removable disk (USB, Firewire etc.)
);

ENUM(NetworkKind,
     Cellular, // Cellular networks
     Other,    // Other network kinds that we may not yet know about
     PPP,      // Old-school internet over a modem
     USB,      // USB Ethernet
     Virtual,  // Virtual networks include loopbacks and VM networks
     Wired,    // Ethernet and other wired networks
     Wireless  // WLAN networks
);

ENUM(DeviceClass,
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

ENUM(Unit,
     Bytes,   // <
     Rate,    // <
     Seconds, // <
     None);

ENUM(UnitPrefixKind, // <
     Binary,     // <
     Metric      // <
);

template <typename Type, typename Dim1> using Array1 = Array<Type, Dim1>;

template <typename Type, typename Dim1, typename Dim2>
using Array2 = Array<Array<Type, Dim2>, Dim1>;

template <typename Type, typename Dim1, typename Dim2, typename Dim3>
using Array3 = Array<Array<Array<Type, Dim3>, Dim2>, Dim1>;

#endif // XFCE_APPLET_SPEED_TYPES_H
