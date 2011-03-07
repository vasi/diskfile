#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fs.h>

off_t diskfile_device_size(const char *path, int fd) {	
    unsigned long long size;
    ioctl(fd, BLKGETSIZE64, &size);
    return size;
}
