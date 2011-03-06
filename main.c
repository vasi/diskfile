/*
 * Compile on the command line as follows:
 * gcc -o "diskfile" diskfile.c main.c -lfuse
 *     -D_FILE_OFFSET_BITS=64 -D__FreeBSD__=10 -DFUSE_USE_VERSION=26
 */
#include "fuse.h"

#include <stdio.h>
#include <stdlib.h>

extern struct fuse_operations diskfile_operations;
extern char *device_path;

static int have_mountpoint = 0;

static void usage(struct fuse_args *args) {
	fprintf(stderr, "Create a regular file with the contents of a device\n\n"
					"Usage: %s [options] DEVICE MOUNTPOINT\n\n", args->argv[0]);
	fuse_main(args->argc, args->argv, NULL, NULL);
	exit(-1);
}

static int diskfile_opt_proc(void *data, const char *arg, int key,
														 struct fuse_args *outargs) {
	if (key == FUSE_OPT_KEY_NONOPT) {
		if (device_path == NULL) {
			device_path = realpath(arg, NULL);
			return 0;
		} else if (!have_mountpoint) {
			have_mountpoint = 1;
		} else {
			usage(outargs);
		}
	}
	return 1;
}

int main(int argc, char* argv[], char* envp[], char** exec_path) {
  umask(0);
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	fuse_opt_parse(&args, NULL, NULL, &diskfile_opt_proc);
	
	if (device_path == NULL || !have_mountpoint)
		usage(&args);
	
  return fuse_main(args.argc, args.argv, &diskfile_operations, NULL);
}
