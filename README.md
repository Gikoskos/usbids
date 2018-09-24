# The USB.IDS list as a C array

The usb.ids list from [this link](http://www.linux-usb.org/usb.ids) as an array that can be used and indexed on the run-time in C programs. Binary search included.

```c
#include "usbids.h"
#include <stdio.h>

...

UsbDevStruct *usbdev;

usbdev = UsbListFind(0x041e, 0x4095);
if (usbdev) {
  printf('Vendor: %s, Device: %s\n', usbdev->Vendor, usbdev->Device);
}
```

Note that this is just an array of structs that contain the USB vendor and device IDs in numeric format (the default is `unsigned short`), and the USB vendor and device names as a null-terminated `char*` array.

Anything related to device classes, subclasses, HIDs etc, on the ubs.ids list, is ignored.

## How to use

In the repo you'll find a python script that downloads the current usb.ids list and generates the source files on-the-fly.

To run it you'll need Python 3:

`python3 make_usbids.py`

The files `usbids.h` and `usbids.c` will be created which you can include in your build system.

Only dependency is a C99 (at least) compatible standard library.

## Documentation

```c
//represents a USB device
typedef struct {
  	//the device's vendor ID
	unsigned short VendorID;

  	//the device's numerical ID
	unsigned short DeviceID;

  	//the name of the vendor as a null-terminated character array
	char* Vendor;

  	//the name of the device as a null-terminated character array
	char* Device;
} UsbDevStruct;
```

So, for example, for this entry in the usb.ids list:

```
03e8  EndPoints, Inc.
	0004  SE401 Webcam
```

the struct would look like this:

```c
VendorID = 0x03e8;
DeviceID = 0x0004;
Vendor = "EndPoints, Inc.";
Device = "SE401 Webcam";
```

------

```c
//each item of this array is an entry in the usb.ids list
UsbDevStruct UsbList[];

//the length of the UsbList array
unsigned int UsbListLength;
```

Note that in the usb.ids list there are a lot of Vendors which have no corresponding device entries, eg

```
03e9  Thesys Microelectronics
03ea  Data Broadcasting Corp.
```

The entries for these vendors simply have their `DeviceID` and `Device` fields set to 0:

```c
VendorID = 0x03e9;
DeviceID = 0;
Vendor = "Thesys Microelectronics";
Device = NULL;

VendorID = 0x03ea;
DeviceID = 0;
Vendor = "Data Broadcasting Corp.";
Device = NULL;
```

To check if an entry is a standalone vendor entry **_don't_** compare the entry's `DeviceID` with 0:

```c
UsbDevStruct *dev = UsbListFind(0x03ee, 0x0000);

if (dev->DeviceID == 0) { //Bad
  printf('This is a standalone vendor entry\n');
}
```

because there are devices that actually have 0 as their `DeviceID`, eg

```
03ee  Mitsumi
	0000  CD-R/RW Drive
```

Instead, check if that entry has a `Device` name:

```c
if (dev->Device == NULL) { //Good
  printf('This is a standalone vendor entry\n');
}
```

------

```c
//searches the UsbList array for a USB device that has `vendor` and `device` IDs
//returns a pointer to the UsbList array item if the search was successful, NULL otherwise
UsbDevStruct *UsbListFind(unsigned short vendor, unsigned short device);
```

`UsbListFind` uses the standard library's `bsearch` under the hood.

------

```c
//returns 1 if the UsbList array is sorted, 0 otherwise
int UsbListIsSorted(void);

//runs various tests that check the integrity of the array
//these tests should finish without any failed assertions
void UsbListRunTests(void);
```

These are utility functions that I used to test the integrity of the array. Feel free to delete them if you don't want to use them.

## Custom symbol names/types

Since the source files are generated on-the-fly, it's possible to set your own symbol names/types by changing the values of the dictionary `usbid_struct` on the `make_usbids.py` script:

```python
usbid_struct = {
    'name': 'UsbDevStruct',
    'vendorid': 'VendorID',
    'deviceid': 'DeviceID',
    'vendorname': 'Vendor',
    'devicename': 'Device',
    'nametype': 'char*',
    'idtype': 'unsigned short',
    'arrayname': 'UsbList'
}
```

For example, you can set the values of the dictionary to resemble names that are more C-like:

```python
usbid_struct = {
    'name': 'usbdev_s',
    'vendorid': 'vid',
    'deviceid': 'did',
    'vendorname': 'vname',
    'devicename': 'dname',
    'nametype': 'char*',
    'idtype': 'unsigned short',
    'arrayname': 'usblist'
}
```

The generated symbols will be

```c
typedef struct {
	unsigned short vid;
	unsigned short did;
	char* vname;
	char* dname;
} usbdev_s;

extern usbdev_s usblist[];
extern unsigned int usblistLength;

usbdev_s *usblistFind(unsigned short vendor, unsigned short device);
int usblistIsSorted(void);
void usblistRunTests(void);
```

Note that, even though it's possible to change the type of both the vendor/device ID and name members, of the `UsbDevStruct`, through the `nametype` and `idtype` keys of the dictionary above, it's not recommended to do so, unless you know what you're doing.

## License

This code is in the public domain.