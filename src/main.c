#include <nmea/nmea.h>

#include <string.h>
#include <stdio.h>

const char DeviceName[] = "/dev/ttyUSB0";

static unsigned char rdesc[] = {
	0x05, 0x20, 	/* USAGE_PAGE (Sensors) */
	0x09, 0xA0,		/* USAGE (Time) */
	0xa1, 0x00,		/* COLLECTION (Physical) */
	0x09, 0x01,			/* USAGE (Sensor) */
	0x0A, 0x21, 0x05,	/* USAGE (Time Year) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0xff,			/* LOGICAL_MAXIMUM (255) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x0A, 0x22, 0x05,	/* USAGE (Time Month) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x0B,			/* LOGICAL_MAXIMUM (11) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x0A, 0x23, 0x05,	/* USAGE (Time Day) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x1f,			/* LOGICAL_MAXIMUM (31) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x0A, 0x25, 0x05,	/* USAGE (Time Hour) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x17,			/* LOGICAL_MAXIMUM (23) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x0A, 0x26, 0x05,	/* USAGE (Time Minute) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x3b,			/* LOGICAL_MAXIMUM (59) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0x0A, 0x27, 0x05,	/* USAGE (Time Seconde) */
	0x15, 0x00,			/* LOGICAL_MINIMUM (0) */
	0x25, 0x3b,			/* LOGICAL_MAXIMUM (59) */
	0x75, 0x08,			/* REPORT_SIZE (8) */
	0x95, 0x03,			/* REPORT_COUNT (1) */
	0x81, 0x02,			/* INPUT (Data,Var,Abs) */
	0xc0,			/* END_COLLECTION */
};

static int uhid_write(int fd, const struct uhid_event *ev)
{
	ssize_t ret;

	ret = write(fd, ev, sizeof(*ev));
	if (ret < 0) {
		fprintf(stderr, "Cannot write to uhid: %m\n");
		return -errno;
	} else if (ret != sizeof(*ev)) {
		fprintf(stderr, "Wrong size written to uhid: %ld != %lu\n",
			ret, sizeof(ev));
		return -EFAULT;
	} else {
		return 0;
	}
}

static int uhid_create(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_CREATE;
	strcpy((char*)ev.u.create.name, "rtc-uhid-device");
	ev.u.create.rd_data = rdesc;
	ev.u.create.rd_size = sizeof(rdesc);
	ev.u.create.bus = BUS_USB;
	ev.u.create.vendor = 0x15d9;
	ev.u.create.product = 0x0a37;
	ev.u.create.version = 0;
	ev.u.create.country = 0;

	return uhid_write(fd, &ev);
}

int main(int argc, char **argv)
{
    nmeaINFO info;
    nmeaPARSER parser;
    char *devicename = DeviceName;
    int gps_fd;
    int uhid_fd;
    char buff[101];
    int size, it = 0;
    int i;
    int stop = 0;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				case 'D':
					devicename = argv[i+1];
					i++;
				break;
			}
		}
	}

    gps_fd = open(devicename, IO_RDONLY);

    if(gps_fd < 3)
        return -1;

    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(!stop)
    {
		int maxfd;
		fd_set rfds;
		
		FD_ZERO(&rfds);
		FD_SET(gps_fd, &rfds);
		maxfd = gps_fds;
		
		ret = select(maxfd + 1, &rfds, NULL, NULL, NULL);
		if (ret < 0 && errno != EINTR)
		{
			stop = 1;
			continue;
		}
		else if (ret == 0 || (ret < 0 && errno == EINTR ))
		{
			continue;
		}
		else if (ret > 0 && FD_ISSET(gps_fd, &rfds))
		{
			
			size = (int)read(gps_fd, &buff[0], 100);

			nmea_zero_INFO(&info);
			nmea_parse(&parser, &buff[0], size, &info);

			printf(
				"%03d, %02d/%02d/%04d %02d:%02d:%02d",
				it++, info.utc.day, info.utc.mon + 1, info.utc.year + 1900, info.utc.hour, info.utc.min, info.utc.sec
				);
			printf(
				"\tsatellites: %d/%d quality: %d",
				info.satinfo.inuse, info.satinfo.inview, info.sig
				);
			printf("\n");
			if (info.satinfo.inuse && info.sig && hid_start)
			{
				
			}
		}
    }

    nmea_parser_destroy(&parser);
    fclose(file);

    return 0;
}
