# Generated automatically from Makefile.in by configure.
#
# makefile for w3camd
#
prefix=/usr/local
sbindir=/usr/local/sbin

CFLAGS += -I.. -I. $(INCJPEG) -DDEBUG -DCAM_PORT=8999 -D_REENTRANT

default: w3camd

OBJ = w3camd.o w3socket.o w3v4l.o w3http.o w3jpeg.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

w3camd: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -lpthread -ljpeg $(LDLIBS$(LDLIBS-$(@)))

install:
	install w3camd $(sbindir)/

clean:
	rm -f *.o w3camd

README: index.html
	lynx -dump http://www/~rasca/w3cam/w3camd/index.html >README

