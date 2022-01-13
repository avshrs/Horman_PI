#include "USB_serial.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <termios.h>
#include <iomanip>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/ioctl.h>
#ifdef __linux__
#include <linux/serial.h>
#endif





void USB_serial::serial_open(char *serial_name, int baud)
{ 
if(baud){

}
  struct termios newtermios;
  fd = open(serial_name,  O_RDWR | O_NOCTTY | O_NONBLOCK);
  
  if (fd < 0) 
  {
    std::cout << "Error from open serial port" << fd << std::endl;
  }   
  if(tcgetattr(fd, &newtermios) != 0) 
  {
    std::cout << "Error from tcgetattr " << std::endl;
  }
	newtermios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR
						| IGNCR | ICRNL | IXON);
	newtermios.c_oflag &= OPOST;
	newtermios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

	newtermios.c_cflag &= ~CSIZE;
	// newtermios.c_cflag |= csize_flag;
	// set parity
	newtermios.c_cflag &= ~(PARENB | PARODD);
	// newtermios.c_cflag |= parity_flag;
	// set stopbits
	newtermios.c_cflag &= ~CSTOPB;
	// newtermios.c_cflag |= stopbits_flag;

	// enable reading; ignore control lines
	newtermios.c_cflag |= CREAD | CLOCAL;
	// disable flow contro
//   newtermios.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
//   // newtermios.c_cflag |= PARENB;  // Set parity bit, enabling parity
//   newtermios.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
//   // newtermios.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
//   newtermios.c_cflag &= ~CSIZE | CRTSCTS; // Clear all the size bits, then use one of the statements below
//   newtermios.c_cflag |= CS8; // 8 bits per byte (most common)
//   newtermios.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)

//   newtermios.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
//   newtermios.c_lflag &= ~ICANON;
//   newtermios.c_lflag &= ~ECHO; // Disable echo
//   newtermios.c_lflag &= ~ECHOE; // Disable erasure
//   newtermios.c_lflag &= ~ECHONL; // Disable new-line echo
//   newtermios.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
//   newtermios.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
//   newtermios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
  
//   newtermios.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
//   newtermios.c_oflag &= ~(ONLCR |OPOST); // Prevent conversion of newline to carriage return/line feed
// //   newtermios.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
// //   newtermios.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

  newtermios.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  newtermios.c_cc[VMIN] = 1;
  newtermios.c_cc[VSTART] = 0x55;

  
  cfsetispeed(&newtermios,B19200);
  cfsetospeed(&newtermios, B19200);
    
}   


void USB_serial::serial_open2(const char *device, int baudrate, bool rtscts, struct termios *old)
{
	struct termios new_ter;
	int b;

	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	
	if (fd == -1)
		std::cout << "open port error" << std::endl;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);

	if (old != NULL)
		tcgetattr(fd, old);

	b = convert_baudrate(baudrate);

	if (b == -1) {

		struct serial_struct ss;

		if (ioctl(fd, TIOCGSERIAL, &ss) == -1) {
			close(fd);
			std::cout << "ioctl error" << std::endl;
		}

		// ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
		ss.custom_divisor = (ss.baud_base + (baudrate / 2)) / baudrate;

		if (ioctl(fd, TIOCSSERIAL, &ss) == -1) {
			close(fd);
			std::cout << "ioctl 2 error" << std::endl;
		}

		if (ss.baud_base / ss.custom_divisor != baudrate)
			fprintf(stderr, "Baud rate set to %d\n", ss.baud_base / ss.custom_divisor);

		b = B19200;

	}

	new_ter.c_cflag = b | CS8 | CREAD;

	if (rtscts)
		new_ter.c_cflag |= CRTSCTS;
	else
		new_ter.c_cflag |= (CLOCAL | CSTOPB| PARENB);

	new_ter.c_iflag = IGNPAR;
	new_ter.c_oflag |= ~(ONLCR |OPOST);
	new_ter.c_lflag |= ~(ICANON | ECHO | ECHOE | ECHONL| ISIG);
	new_ter.c_cc[VMIN] = 1;
	new_ter.c_cc[VTIME] = 0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &new_ter);

	
}

//   newtermios.c_lflag &= ~ICANON;
//   newtermios.c_lflag &= ~ECHO; // Disable echo
//   newtermios.c_lflag &= ~ECHOE; // Disable erasure
//   newtermios.c_lflag &= ~ECHONL; // Disable new-line echo
//   newtermios.c_lflag &= ~ISIG; 

void USB_serial::serial_send(uint8_t *data, int size)
{ 	char buf[15+3] = {0};
	for(int i = 0; i< size; i++)
	{
		buf[i] = static_cast<char>(data[i]);
	}
	write(fd, buf, size);
}

void USB_serial::serial_read(uint8_t *data, int size)
{	
	char * buf = new char[size]; 

    read(fd, buf, size);
	for(int i = 0; i< size; i++)
	{
		data[i] = static_cast<uint8_t>(buf[i]);
	}
	delete[] buf;
}

void USB_serial::serial_close()
{
   close(fd);
}

int USB_serial::convert_baudrate(unsigned int baudrate)
{
	switch (baudrate) {
		case 50: return B50;
		case 75: return B75;
		case 110: return B110;
		case 134: return B134;
		case 150: return B150;
		case 200: return B200;
		case 300: return B300;
		case 600: return B600;
		case 1200: return B1200;
		case 1800: return B1800;
		case 2400: return B2400;
		case 4800: return B4800;
		case 9600: return B9600;
		case 19200: return B19200;
		case 38400: return B38400;
		case 57600: return B57600;
		case 115200: return B115200;
#ifdef B230400
		case 230400: return B230400;
#endif
#ifdef B460800
		case 460800: return B460800;
#endif
#ifdef B500000
		case 500000: return B500000;
#endif
#ifdef B576000
		case 576000: return B576000;
#endif
#ifdef B921600
		case 921600: return B921600;
#endif
#ifdef B1000000
		case 1000000: return B1000000;
#endif
#ifdef __linux__
		default: return -1;
#else
		default:
			fprintf(stderr, "Unknown baud rate %d\n", baudrate);
			exit(1);
#endif
	}
}