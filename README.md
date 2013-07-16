diskfile
========

**Expose a disk block device as a regular file (FUSE)**

Rationale
---------

Many utilities, such as mksquashfs, rsync and tar, will treat a disk device as a small block-special file, but sometimes you want them to see the actual contents of the disk. 

Example: `diskfile /dev/sdb1 /dev/sdb4 /dev/sdb6 /path/to/mountpount`

Now files mountpoint/sdb1 and friends exist as regular files with the size and content of the actual disk. You can run `mksquashfs mountpoint/sdb1 disk.squashfs` to create a Fedora-style nested squashfs filesystem. Or `tar czf backup.tgz mountpoint` is a convenient, if inefficient, way to backup several partitions.

Caveats
-------

* Code is not well tested, with little error-checking
* Only disk block-devices are tested, I've no idea what will happen with other such devices
* Only Mac OS X and Linux supported
* Only read access is allowed
* There's no support for fancy parameters like offset and file mode. For those, check out [fusefile](https://github.com/vi/fusefile).

All the above should be fixable, patches welcome!
