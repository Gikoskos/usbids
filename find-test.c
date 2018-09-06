#include <stdio.h>
#include <stdlib.h>
#include "usbids.h"

int main(void)
{
	unsigned long vendor, device;
	UsbDevStruct *p;

	if (!UsbListIsSorted()) {
		fprintf(stderr, "error: UsbList is unsorted\n");
		return EXIT_FAILURE;
	}
	while (scanf("%9lx %9lx", &vendor, &device) == 2)
		if (vendor > 0xffff || device > 0xffff
		 || (p = UsbListFind(vendor, device)) == NULL)
			fprintf(stderr, "(%ld, %ld): unknown device\n", vendor,
			        device);
		else
			printf("%s %s\n", p->Vendor, p->Device);
	return 0;
}
