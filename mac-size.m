#include <sys/stat.h>
#include <Foundation/Foundation.h>

// Use diskutil to find device size
off_t diskfile_device_size(const char *path, int fd) {
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
