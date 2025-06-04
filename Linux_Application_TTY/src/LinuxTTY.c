/*
 ============================================================================
 Name        : LinuxTTY.c
 Author      : 
 Version     :
 Copyright   : 
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

int set_interface_attribs (int fd, int speed, int parity)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		printf ("Error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	tty.c_iflag &= ~IGNBRK;         				// disable break processing
	tty.c_lflag = 0;                				// no signaling chars, no echo, no canonical processing
	tty.c_oflag = 0;                				// no remapping, no delays
	tty.c_cc[VMIN]  = 0;            				// read doesn't block
	tty.c_cc[VTIME] = 5;            				// 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); 		// shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);				// ignore modem controls,
													// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      		// shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		printf ("Error %d from tcsetattr", errno);
		return -1;
	}
	return 0;
}


int main(int argc, char *argv[])
{
    char *portname = "/dev/ttyRPMSG0";

    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
            printf ("Error %d opening %s: %s", errno, portname, strerror (errno));
            return -1;
    }

    set_interface_attribs (fd, B115200, 0);

    write (fd, "Hello from A7!", 14);

    usleep (10000);

    char buf[22];
    read (fd, buf, sizeof buf);

    printf ("Get Message From Remote Side: %s\n", buf);

    return EXIT_SUCCESS;
}

