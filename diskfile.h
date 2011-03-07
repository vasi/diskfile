#define _GNU_SOURCE // asprintf
#include <sys/types.h>

#define DISKFILE_MAX_ENTRIES 256

typedef struct {
	char *source;
	char *dest;
	off_t size;
} diskfile_entry;
extern diskfile_entry diskfile_entries[];
extern size_t diskfile_entries_count;

extern struct fuse_operations diskfile_operations;
off_t diskfile_device_size(const char *path);
