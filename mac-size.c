#include "diskfile.h"

#include <string.h>
#include <libgen.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOMedia.h>
#include <CoreFoundation/CoreFoundation.h>

#define DEV_PREFIX "/dev/"

off_t diskfile_device_size(const char *path) {
  // Get the path within /dev
  if (strncmp(path, DEV_PREFIX, strlen(DEV_PREFIX)) != 0)
    return 0;
  const char *bsdname = path + strlen(DEV_PREFIX);

  // Find the matching IOService
  CFMutableDictionaryRef matching = IOBSDNameMatching(kIOMasterPortDefault, 0,
    bsdname);
  // Consumes the matching dictionary
  io_service_t match = IOServiceGetMatchingService(kIOMasterPortDefault,
    matching);
  if (!match)
    return 0;

  // Get the size property
  CFNumberRef cfsize = IORegistryEntryCreateCFProperty(match,
    CFSTR(kIOMediaSizeKey), kCFAllocatorDefault, 0);
  IOObjectRelease(match);
  if (!cfsize)
    return 0;

  long long size;
  if (!CFNumberGetValue(cfsize, kCFNumberLongLongType, &size))
    return 0;

  return size;
}
