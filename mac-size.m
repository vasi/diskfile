#include <sys/stat.h>
#include <Foundation/Foundation.h>

off_t diskfile_device_size(const char *path, int fd) {	
	// Regular files are easy
	struct stat st;
	int err = fstat(fd, &st);
	if (err == 0 && S_ISREG(st.st_mode)) {
		return st.st_size;
	}
	
	// See what diskutil has to say
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSTask *task = [[NSTask alloc] init];
	[task setLaunchPath: @"/usr/sbin/diskutil"];
	[task setArguments: [NSArray arrayWithObjects: @"info", @"-plist",
											 [NSString stringWithUTF8String: path], nil]];
	NSPipe *pipe = [NSPipe pipe];
	[task setStandardOutput: pipe];
	
	[task launch];
	[task waitUntilExit];
	
	NSData *data = [[pipe fileHandleForReading] readDataToEndOfFile];
	NSDictionary *plist = [NSPropertyListSerialization
												 propertyListFromData: data
												 mutabilityOption: 0
												 format: NULL
												 errorDescription: NULL];
	off_t size = [[plist objectForKey: @"TotalSize"] longLongValue];
	
	[pool release];
	return size;

}
