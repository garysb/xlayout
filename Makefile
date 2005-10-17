# If you don't use CC 
CC       = gcc

# Path to copy the binary to
BINARY_PATH = /usr/bin
MAN_PATH = /usr/man/man1
XROOT    = /usr/X11R6
CFLAGS  += -I$(XROOT)/include -Wall
LDFLAGS += -L$(XROOT)/lib -lX11 -lXmu 
SRCS     = xlayout.c
OBJS     = $(SRCS:.c=.o)

all: xlayout

xlayout: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

#%.o: %.c xlayoutlib.h
#	$(CC) $(CFLAGS) -c $<

install:
	install -s xlayout $(BINARY_PATH)
	install xlayout.1 $(MAN_PATH)
	ls -lh $(BINARY_PATH)/xlayout

uninstall:
	rm -f $(BINARY_PATH)/xlayout

clean:
	rm -f xlayout $(OBJS)
