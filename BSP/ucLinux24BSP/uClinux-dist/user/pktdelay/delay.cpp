#include <stdio.h>
#include <syslog.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4/ip_queue.h>
#include "../iptables/include/libipq/libipq.h"

#include <sg_llist.h>
#include <sg_select.h>
#include <sg_timeout.h>
#include <sg_select_timeout.h>
#include <sg_error.h>
#include <sg_debug.h>

struct timeval to = {tv_sec:2,tv_usec:0};



class CPacket : public CTimeoutTarget {
public:
	/*
	 * desc:	create a new packet structure and populate it with
	 * 			interesting stuff.
	 */
	CPacket(ipq_packet_msg_t *msg, unsigned char *data, int datalen, 
			struct timeval to, struct ipq_handle *q);

	/*
	 * desc:	We accept the packet here, and let it continue its journey.
	 */
	int Expired();

	/*
	 * desc:	Set the verdict on this packet.
	 */
	virtual int SetVerdict(int verdict);
protected:
	ipq_packet_msg_t *msg;
	ssize_t len;
	unsigned char buf[BUFSIZ];
	int datalen;
	struct ipq_handle *hipq;
};

int CPacket::SetVerdict(int verdict)
{
	return ipq_set_verdict(hipq, msg->packet_id, verdict, 0, NULL);
}


CPacket::CPacket(ipq_packet_msg_t *msg, unsigned char *data, int datalen, 
		struct timeval to, struct ipq_handle *q)
{
	memcpy(buf, data, datalen);
	this->msg = ipq_get_packet(buf);
	this->datalen = datalen;
	SetTimeout(to);
	hipq = q;
}

int CPacket::Expired()
{
	/*
	 * desc:	Set the accept verdict.
	 */
	if (ipq_set_verdict(hipq, msg->packet_id, NF_ACCEPT, 0, NULL) < 0) {
		syslog(LOG_ERR, "Cannot set verdict - %s\n", ipq_errstr());
	}

//	printf("Packet timeout\n");

	/*
	 * always get removed from the list.
	 */
	return 0;
}


class CSnarfAndQueue : public CSelectTarget{
	public:
	/*
	 * desc:	We need to specify the maximum number of packets to queue.
	 *
	 * input:	maxQueueLen - the max number of packets to queue up.
	 * 				Should this be a max number of bytes??
	 */
	CSnarfAndQueue(CTimeoutSelect *tothing);

	/*
	 * desc:	Cause all of the queued packets to be flushed to the system.	
	 * 			Thakfully we can let the system now worry about routing,
	 * 			and ensuring the packets are appropriate on leaving the
	 * 			system.
	 */
	virtual void ReleasePackets();

	/*
	 * desc:	Dump all of the packets we have on the queue.
	 *
	 * input:	disposition - The disposition to set on all the packets.
	 * 				valid values are:
	 * 				NF_DROP - drop the packet.
	 * 				NF_ACCEPT - let the packet continue traversal.
	 *
	 * 				There are other values, but I don't know what to do with
	 * 				these.
	 *
	 */
	virtual void Flush(int disposition);

	/*
	 * desc:	Dump some useful information out to the screen.
	 */
	virtual void Dump();

protected:
	int maxQueueLen;
	int release;
	virtual int Read();
//	virtual int Write();
	virtual int Exception();
	struct ipq_handle *hipq;
	CTimeoutSelect *selector;   /*needed to add timeouts to*/
};



/*
 * CSnarfAndQueue implementation.
 */
int CSnarfAndQueue::Read()
{
	CIterator it;
	CPacket *pkt;
	static unsigned char data[BUFSIZ];
	int len;
	ipq_packet_msg_t *msg;

	if ((len = ipq_read(hipq, data, sizeof(data), 0)) < 0) {
		/*
		 * Couldn't read it - log an error
		 */
		fprintf(stderr, "Cannot read packet - %s\n", ipq_errstr());
//		syslog(LOG_ERR, "Cannot read packet - %s", ipq_errstr());
		return 0;
	}

	if (ipq_message_type(data) == NLMSG_ERROR) {
		syslog(LOG_ERR, "Received Error message - %d,%s\n",
				ipq_get_msgerr(data), 
				strerror(ipq_get_msgerr(data))
				);
		exit(1);
		return 1;
	}

	msg = ipq_get_packet(data);

	/*
	 * Create the new packet and add it too the list.
	 */
	pkt = new CPacket(msg, data, len, to, hipq);

	if (!pkt) {
		/*
		 * couldn't get it going.
		 */
		return 0;
	}

	/*
	 * Inser the thing into the list of packets.
	 */
	selector->AddTimeoutTarget(pkt);

	return 0;
}


int CSnarfAndQueue::Exception()
{
	printf("/");
	return 0;
}


CSnarfAndQueue::CSnarfAndQueue(CTimeoutSelect *tothing) {
	this->selector = tothing;
	release = 0;

	hipq = ipq_create_handle(0, PF_INET);
	assert_post(hipq, "hipq created - %s", ipq_errstr());

	if (ipq_set_mode(hipq, IPQ_COPY_META, BUFSIZ) < 0) {
		assert_post(0, "hipq mode set - %s", ipq_errstr());
	}

	SetFD(hipq->fd);

	SetType(CSelectTarget::SelectType_Read);
}


void CSnarfAndQueue::ReleasePackets() {
	/*
	 * Cause packets to be released when they arrive.
	 */
	release = 1;
}


void CSnarfAndQueue::Flush(int disposition) {
	CIterator it;
	CPacket *pkt;

	assert_pre(0, "NOT SUPPORTED YET\n");
	assert_pre((disposition == NF_ACCEPT) || (disposition == NF_DROP),
			"disposition is NF_ACCEPT or NF_DROP");

}


void CSnarfAndQueue::Dump()
{
}


int main(int argc, char *argv[]) {
	CTimeoutSelect *to;
	CSelect *sel;
	CSnarfAndQueue *kew;
	struct timeval timer = {1,0};
	int err;

	to = new CTimeoutSelect;

	if (!to) {
		printf("Unable to create CSelectTimeout");
		return 1;
	}

	/*
	 * Queue no more than 5 packets
	 */
	kew = new CSnarfAndQueue(to);

	if (!kew) {
		printf("Unable to create CSnarfAndQueue\n");
		return 1;
	}

	openlog("snarf and queue test", LOG_PID|LOG_CONS, LOG_USER);

	if (err = to->AddSelectTarget(kew)) {
		printf("Unable to add select target - %d\n", err);
		return 1;
	}

	while ((err = to->SelectAndTimeoutLoop() == SUCCESS));

#if 0
	while ((err = sel->SelectLoop()) == SUCCESS);
#endif /*0*/

	printf("We were told to exit = %x", err);

	return 0;
}
