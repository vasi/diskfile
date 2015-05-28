#include "diskfile.h"

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>

off_t diskfile_device_size(const char *path) {	
    unsigned long long size;
    int fd = open(path, O_RDONLY);
		if (fd == -1)
			return 0;
    if (ioctl(fd, BLKGETSIZE64, &size) == -1)
			size = 0;
    close(fd);
    return size;
}
