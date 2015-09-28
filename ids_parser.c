/************************************************
* Really broken parser for the usb.ids file.     *
* Doesn't check for "" or ??? or other sequences *
* of characters that break C strings.            *
* George Koskeridis for the Public Domain 2015   * 
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *filename = "usb.ids";

/*getline taken from Will Hartung's implementation and made a few changes to work with NULL strings; see:
* http://stackoverflow.com/questions/735126/are-there-alternate-implementations-of-gnu-getline-interface 
* cause I was bored to make my own :P*/
size_t getline(char **lineptr, size_t *n, FILE *stream)
{
	char *bufptr = NULL;
	char *p = bufptr;
	size_t size;
	int c;

	if (lineptr == NULL) {
		return -1;
	}
	if (stream == NULL) {
		return -1;
	}
	bufptr = *lineptr;

	c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (bufptr == NULL) {
		bufptr = malloc(128);
		if (bufptr == NULL) {
			return -1;
		}
		size = 128;
	}
	p = bufptr;
	while(c != EOF) {
		if ((p - bufptr) > (size - 1)) {
			size = size + 128;
			bufptr = realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		*p++ = c;
		if (c == '\n') {
			break;
		}
		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;

	return p - bufptr - 1;
}

int main (int argc, char *argv[])
{
	FILE *ids_fd;
	char *curr_line = NULL, *next_line = NULL;
	char vID[5], dID[5], tempID[5];
	char vendor[256], device[256];
	
	unsigned long dev_count = 0, vendor_count = 0;

	ids_fd = fopen(filename, "r");
	if (!ids_fd) {
		fprintf(stderr, "Couldn't open %s", filename);
	}
	vID[4] = dID[4] = '\0';
	getline(&curr_line, NULL, ids_fd);
	getline(&next_line, NULL, ids_fd);
	while (!feof(ids_fd)) {
		if (curr_line[0] == '#' || curr_line[0] == '\n' || curr_line[2] == '\t') {
			free(curr_line);
			curr_line = malloc(strlen(next_line)+1);
			sprintf(curr_line, next_line);
			free(next_line);
			next_line = NULL;
			getline(&next_line, NULL, ids_fd);
			continue;
		}
		if (curr_line[0] != '\t') {
			for (int i = 0; i < 4; i++) {
				vID[i] = curr_line[i];
			}
			for (int i = 6; i < (strlen(curr_line)+1); i++) {
				vendor[i-6] = curr_line[i];
			}
			vendor[strlen(curr_line) - 7] = '\0';
			if (next_line[0] == '\t')
				strncpy(tempID, vID, 5);
			else
				printf("{ 0x%s, 0xabcd, \"%s\", NULL },\n", vID, vendor);
			vendor_count++;
		} else {
			for (int i = 1; i < 5; i++) {
				dID[i-1] = curr_line[i];
			}
			for (int i = 7; i < (strlen(curr_line)+1); i++) {
				device[i-7] = curr_line[i];
			}
			device[strlen(curr_line) - 8] = '\0';
			printf("{ 0x%s, 0x%s, \"%s\", \"%s\"}, \n", tempID, dID, vendor, device);
			dev_count++;
		}
		free(curr_line);
		curr_line = malloc(strlen(next_line)+1);
		sprintf(curr_line, next_line);
		free(next_line);
		next_line = NULL;
		getline(&next_line, NULL, ids_fd);
	}
	if (next_line) free(next_line);
	if (curr_line) free(curr_line);
	fclose(ids_fd);
}

