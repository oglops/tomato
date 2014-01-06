CC = $(CROSS_COMPILE)gcc
LDFLAGS = 
CFLAGS = -Os -s

NS_SRC = nullserv.c

nullserv: $(SRC)
		$(CC) $(CFLAGS) -o nullserv $(NS_SRC)

all: nullserv

clean:
	rm nullserv
