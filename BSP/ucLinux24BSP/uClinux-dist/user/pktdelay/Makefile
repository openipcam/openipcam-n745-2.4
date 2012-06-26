#CC = gcc

TESTDIR = test

CPPFLAGS += -g -DASSERT_CONDITIONS=1 $(INCSG) $(INCSGPP)

IPQLIBS = ../iptables/libipq/libipq.a

OBJS=delay.o 

EXEC=pktdelay

$(EXEC): $(OBJS)
	echo $$CXXLIBS
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBSNAPGEAR) $(IPQLIBS) \
	$(LIBSNAPGEARPP) $(LDLIBS) $(CXXLIBS)
#/usr/local/lib/gcc-lib/i586-linux/3.0.4/./libstdc++.a

clean:
	rm -f *.o
	rm -f $(EXEC)

romfs:
	$(ROMFSINST) /bin/pktdelay
