
LOGIN = login
LOGINOBJS = login.o

PASSWD = passwd
PASSWDOBJS = passwd.o

ifdef CONFIG_USER_OLD_PASSWORDS
CFLAGS += -DOLD_CONFIG_PASSWORDS
endif

all: $(LOGIN) $(PASSWD)

$(LOGIN): $(LOGINOBJS)
	$(CC) $(LDFLAGS) -o $@ $(LOGINOBJS) $(LIBCRYPT) $(LDLIBS$(LDLIBS-$(@)))

$(PASSWD): $(PASSWDOBJS)
	$(CC) $(LDFLAGS) -o $@ $(PASSWDOBJS) $(LIBCRYPT) $(LDLIBS)

romfs:
	$(ROMFSINST) -e CONFIG_USER_LOGIN_LOGIN /bin/login
	$(ROMFSINST) -e CONFIG_USER_LOGIN_PASSWD /bin/passwd

clean:
	-rm -f $(LOGIN) $(PASSWD) *.elf *.gdb *.o

