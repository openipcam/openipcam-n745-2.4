.SUFFIXES : .x .c .o .s

all:
#	make -C pppd/pptp
	make -C pppd/plugins/rp-pppoe
	make -C pppd

clean:
#	make -C pppd/pptp clean
	make -C pppd/plugins/rp-pppoe clean
	make clean -C pppd
	rm bin/* -f

