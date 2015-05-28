#include "diskfile.h"

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

static void usage(struct fuse_args *args) {
	fprintf(stderr, "Create a regular file with the contents of a device\n\n"
					"Usage: %s [options] DEVICE1 [DEVICE2 ...] MOUNTPOINT\n\n", args->argv[0]);
	fuse_main(args->argc, args->argv, NULL, NULL);
	exit(-1);
}

static int diskfile_opt_proc(void *data, const char *arg, int key,
														 struct fuse_args *outargs) {
	if (key == FUSE_OPT_KEY_NONOPT) {
		if (diskfile_entries_count >= DISKFILE_MAX_ENTRIES) {
			fprintf(stderr, "Too many entries\n");
			exit(-1);
		}

		diskfile_entry *entry = diskfile_entries + diskfile_entries_count++;
		entry->source = realpath(arg, NULL);

    // Generate the destination name.
    // Check if a dest name is duplicated?
    size_t size = strlen(arg) + 1;
    char *argmut = malloc(size);
    strncpy(argmut, arg, size);
		char *base = basename(argmut);

		entry->dest = malloc(size + 1);
		entry->dest[0] = '/';
		strncpy(entry->dest + 1, base, size);
		free(argmut);

    entry->size = -1;
    return 0;
	}
	return 1;
}

int main(int argc, char* argv[]) {
  umask(0);
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	fuse_opt_parse(&args, NULL, NULL, &diskfile_opt_proc);

	// Need at least a
	if (diskfile_entries_count < 2)
		usage(&args);

	// put the mountpoint back
	fuse_opt_add_arg(&args, diskfile_entries[--diskfile_entries_count].source);

	diskfile_time = time(NULL);

	return fuse_main(args.argc, args.argv, &diskfile_operations, NULL);
}
