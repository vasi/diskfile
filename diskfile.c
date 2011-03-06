#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

off_t diskfile_device_size(const char *path, int fd);

char *device_path = NULL;
static const char *file_path = "/device";

static off_t device_size;
static int device_fd;

static int
diskfile_getattr(const char *path, struct stat *stbuf) {
  memset(stbuf, 0, sizeof(struct stat));
  
  if (strcmp(path, "/") == 0) { /* The root directory of our file system. */
    stbuf->st_mode = S_IFDIR | 0555;
    stbuf->st_nlink = 3;
    return 0;
  } else if (strcmp(path, file_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = device_size;
		return 0;
	}
  return -ENOENT;
}

static int
diskfile_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
  if (strcmp(path, "/") != 0) /* We only recognize the root directory. */
    return -ENOENT;
  
  filler(buf, ".", NULL, 0);           /* Current directory (.)  */
  filler(buf, "..", NULL, 0);          /* Parent directory (..)  */
	filler(buf, file_path + 1, NULL, 0);
  
  return 0;
}

static int
diskfile_open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, file_path) == 0) {
		if ((fi->flags & O_ACCMODE) != O_RDONLY)
			return -EACCES;
		return 0;
	}
	return -ENOENT;
}

static int
diskfile_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
	if (strcmp(path, file_path) == 0) {
		ssize_t ret = pread(device_fd, buf, size, offset);
		return ret;
	}
	return -ENOENT;
}

static void *
diskfile_init(struct fuse_conn_info *conn) {
	device_fd = open(device_path, O_RDONLY);
	device_size = diskfile_device_size(device_path, device_fd);	
	return NULL;
}

struct fuse_operations diskfile_operations = {
  .getattr     = diskfile_getattr,
  .readdir     = diskfile_readdir,
  .open        = diskfile_open,
  .read        = diskfile_read,
	.init				 = diskfile_init,
};
