#include "diskfile.h"

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#define FOREACH_ENTRY(_var) \
	for (diskfile_entry *_var = diskfile_entries; \
		_var < diskfile_entries + diskfile_entries_count; \
		++_var)

diskfile_entry diskfile_entries[DISKFILE_MAX_ENTRIES];
size_t diskfile_entries_count = 0;

static off_t
diskfile_source_size(const char *path) {	
	// Regular files are easy
	struct stat st;
	int err = lstat(path, &st);
	if (err == 0 && S_ISREG(st.st_mode)) {
		return st.st_size;
	}
	
	return diskfile_device_size(path);
}

static int
diskfile_getattr(const char *path, struct stat *stbuf) {
  memset(stbuf, 0, sizeof(struct stat));
  
  if (strcmp(path, "/") == 0) { /* The root directory of our file system. */
    stbuf->st_mode = S_IFDIR | 0555;
    stbuf->st_nlink = 2 + diskfile_entries_count;
    return 0;
  }
	FOREACH_ENTRY(entry) {
		if (strcmp(path, entry->dest) == 0) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			
			if (entry->size == -1)
				entry->size = diskfile_source_size(entry->source);
			stbuf->st_size = entry->size;
			return 0;
		}
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
	FOREACH_ENTRY(entry) {
		filler(buf, entry->dest + 1, NULL, 0);
	}
	
	return 0;
}

static int
diskfile_open(const char *path, struct fuse_file_info *fi) {
	FOREACH_ENTRY(entry) {
		if (strcmp(path, entry->dest) == 0) {
			if ((fi->flags & O_ACCMODE) != O_RDONLY)
				return -EACCES;
			
			fi->fh = open(entry->source, O_RDONLY);
			return 0;
		}
	}
	return -ENOENT;
}

static int
diskfile_release(const char *path, struct fuse_file_info *fi) {
	close(fi->fh);
	return 0;
}

static int
diskfile_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
	return pread(fi->fh, buf, size, offset);
}

struct fuse_operations diskfile_operations = {
  .getattr     = diskfile_getattr,
  .readdir     = diskfile_readdir,
  .open        = diskfile_open,
	.release     = diskfile_release,
  .read        = diskfile_read,
};
