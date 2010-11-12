#include <stdio.h>
#include <errno.h>

#include "osdep/osdep.h"
#include "osdep.h"


static struct wif *_wi_in, *_wi_out;

struct devices
{
    int fd_in,  arptype_in;
    int fd_out, arptype_out;
    int fd_rtc;
} dev;

int current_channel = 0;


int osdep_start(char *interface)
{
    /* open the replay interface */
    _wi_out = wi_open(interface);
    if (!_wi_out)
    	return 1;
    dev.fd_out = wi_fd(_wi_out);

    /* open the packet source */
    _wi_in = _wi_out;
    dev.fd_in = dev.fd_out;

    /* XXX */
    dev.arptype_in = dev.arptype_out;
    
    return 0;
}


int osdep_send_packet(unsigned char *buf, size_t count)
{
	struct wif *wi = _wi_out; /* XXX globals suck */
	if (wi_write(wi, buf, count, NULL) == -1) {
		switch (errno) {
		case EAGAIN:
		case ENOBUFS:
			usleep(10000);
			return 0; /* XXX not sure I like this... -sorbo */
		}

		perror("wi_write()");
		return -1;
	}

	return 0;
}


int osdep_read_packet(unsigned char *buf, size_t count)
{
	struct wif *wi = _wi_in; /* XXX */
	int rc;

	rc = wi_read(wi, buf, count, NULL);
	if (rc == -1) {
		switch (errno) {
		case EAGAIN:
			return 0;
		}

		perror("wi_read()");
		return -1;
	}

	return rc;
}


void osdep_set_channel(int channel)
{
    wi_set_channel(_wi_out, channel);
    current_channel = channel;
}


int osdep_get_channel()
{
    return current_channel;
}