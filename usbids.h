#ifndef USB_IDS_H
#define USB_IDS_H

typedef struct {
	unsigned short VendorID;
	unsigned short DeviceID;
	char *Vendor;
	char *Device;
} UsbDevStruct;

extern UsbDevStruct UsbList[];
extern size_t UsbListLength;

UsbDevStruct *UsbListFind(long vendor, long device);
int UsbListIsSorted(void);

#endif
