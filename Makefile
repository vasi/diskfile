CFLAGS = -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26

UNAME = $(shell uname)
ifeq ($(UNAME),Darwin)
	CFLAGS += -D__DARWIN_64_BIT_INO_T=1 -D__FreeBSD__=10
	LIBS = -lfuse_ino64 -framework Foundation
else
	LIBS = -lfuse
endif

all: diskfile

diskfile: diskfile.c main.c mac-size.m
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f diskfile

.PHONY: clean all
