CFLAGS = -Wall -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
SOURCES = diskfile.c main.c

UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	CFLAGS += -D__DARWIN_64_BIT_INO_T=1 -D__FreeBSD__=10
	LIBS = -lfuse_ino64 -framework Foundation
	SOURCES += mac-size.m
else
	LIBS = -lfuse
	SOURCES += linux-size.c
endif

all: diskfile

diskfile: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f diskfile

.PHONY: clean all
