#include "diskfile.h"

#include <libgeom.h>

off_t diskfile_device_size(const char *path) {
	int fd = g_open(path, 0);
	off_t size = g_mediasize(fd);
	g_close(fd);
    return size;
}
