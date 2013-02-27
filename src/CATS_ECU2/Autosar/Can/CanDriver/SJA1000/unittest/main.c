#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// #include "cats_api.h"
#include "kernel_driver/cats_ioctl.h"
#include "kernel_driver/cats_sja1000.h"

#define CATSDRV_PATH "/dev/catsdrv"

void read_register(int device, int adr, int *data)
{
	int fd, ret;
	CAN_REGISTER reg;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		printf("Can't open device file: '%s'\n", CATSDRV_PATH);
		return;
	}

	reg.device = device;
	reg.address = adr;
	reg.data = -1;
	ret = ioctl(fd, IOCTL_CATS_CAN_read_register, &reg);
	if(ret < 0) {
	   printf("Can't io-controle device file: '%s'\n", CATSDRV_PATH);
	}
	*data = reg.data;

	close(fd);
}


void write_register(int device, int adr, int data)
{
	int fd, ret;
	CAN_REGISTER reg;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		printf("Can't open device file: '%s'\n", CATSDRV_PATH);
		return;
	}

	reg.device = device;
	reg.address = adr;
	reg.data = data;
	ret = ioctl(fd, IOCTL_CATS_CAN_write_register, &reg);
	if(ret < 0) {
	   printf("Can't io-controle device file: '%s'\n", CATSDRV_PATH);
	}

	close(fd);
}


void can_init(int device)
{
	int fd, ret;
	CAN_RESET cr;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		printf("Can't open device file: '%s'\n", CATSDRV_PATH);
		return;
	}

	ret = ioctl(fd, IOCTL_CATS_CAN_init, &device);

	close(fd);
}


void can_stop(int device)
{
	int fd, ret;
	CAN_RESET cr;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		printf("Can't open device file: '%s'\n", CATSDRV_PATH);
		return;
	}

	ret = ioctl(fd, IOCTL_CATS_CAN_stop, &device);

	close(fd);
}



void test_reset(int device, int baud)
{
	int fd, ret;
	CAN_RESET cr;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		printf("Can't open device file: '%s'\n", CATSDRV_PATH);
		return;
	}

	cr.device = device;
	cr.baud = baud;
	ret = ioctl(fd, IOCTL_CATS_CAN_reset, &cr);
	if(ret < 0) {
	   printf("Can't io-controle device file: '%s'\n", CATSDRV_PATH);
	}

	close(fd);
}

int test_scan(int device)
{
	int fd, ret;
	CAN_SCAN cs;

	fd = open(CATSDRV_PATH, O_RDWR);
	if(fd < 0) {
		printf("Can't open device file: '%s'\n", CATSDRV_PATH);
		return -1;
	}

	cs.device = device;
	cs.status = 0;
	ret = ioctl(fd, IOCTL_CATS_CAN_send_scan, &cs);
	if(ret < 0) {
	   printf("Can't io-controle device file: '%s'\n", CATSDRV_PATH);
	}

	close(fd);

	return cs.status;
}


void can_send_recv(int baud, int loopcnt)
{
  int fd, ret, data, loop;
  CAN_RESET cr;
  CAN_SCAN scan;
  CAN_SEND send;
  CAN_RECV recv;
  int i, j;
  CAN_REGISTER reg;
  int senddev = 0;
  int recvdev = 1;

  fd = open(CATSDRV_PATH, O_RDWR);
  if(fd < 0) {
    printf("Can't open device file: '%s'\n", CATSDRV_PATH);
    return;
  }


  ret = ioctl(fd, IOCTL_CATS_CAN_init, &senddev);
  ret = ioctl(fd, IOCTL_CATS_CAN_init, &recvdev);

  cr.device = senddev;
  cr.baud = baud;
  ret = ioctl(fd, IOCTL_CATS_CAN_reset, &cr);
  cr.device = recvdev;
  ret = ioctl(fd, IOCTL_CATS_CAN_reset, &cr);
  
  
  reg.device = senddev;
  reg.address = 2;
  reg.data = -1;
  ioctl(fd, IOCTL_CATS_CAN_read_register, &reg);
  printf("Device 0, Status: %02X\n", reg.data);
  reg.device = recvdev;
  ioctl(fd, IOCTL_CATS_CAN_read_register, &reg);
  printf("Device 1, Status: %02X\n", reg.data);
  

  ret = ioctl(fd, IOCTL_CATS_CAN_start, &senddev);
  printf("Send device start:%d\n", ret);
  ret = ioctl(fd, IOCTL_CATS_CAN_start, &recvdev);
  printf("Recv device start:%d\n", ret);

  usleep(100000);
  for (loop = 0; loop < loopcnt; ++loop) {
    

// 送信
    send.device = senddev;
    send.frame.id = 0;
    send.frame.dlc = 8;
    send.frame.length = 8;
    send.frame.type = 1;
    send.frame.data[0] = 0x01;
    send.frame.data[1] = 0x23;
    send.frame.data[2] = 0x45;
    send.frame.data[3] = 0x67;
    send.frame.data[4] = 0x89;
    send.frame.data[5] = 0xab;
    send.frame.data[6] = 0xcd;
    send.frame.data[7] = 0xef;
    
    usleep(20000);
    ret = ioctl(fd, IOCTL_CATS_CAN_send, &send);
    printf("send result:%x\n", ret);
    
    // 受信
#if 1
    {
      int recvcount;
      scan.device = recvdev;
      scan.status = 0;
      for(recvcount = 0; recvcount < 300; ++recvcount) {
	ret = ioctl(fd, IOCTL_CATS_CAN_recv_scan, &scan);
	if(scan.status) break;
	usleep(1000); // 1msec
      }
      if (scan.status == 0) {
	printf("No data received device #%d\n", 1);
      }
    }
    
    // for(j=0; j<99999; ++j) { read_register(1, 0, &data); }
    
    recv.device = 1;
    recv.frame.id = 0;
    recv.frame.length = 0;
    recv.frame.data[0] = 0x00;
    recv.frame.data[1] = 0x00;
    recv.frame.data[2] = 0x00;
    recv.frame.data[3] = 0x00;
    recv.frame.data[4] = 0x00;
    recv.frame.data[5] = 0x00;
    recv.frame.data[6] = 0x00;
    recv.frame.data[7] = 0x00;
    ret = ioctl(fd, IOCTL_CATS_CAN_recv, &recv);
    
    printf("Frame ID: %d\n", recv.frame.id);
    printf("Frame length: %d\n", recv.frame.length);
    for(i = 0; i < 8; ++i) {
      printf("Frame data%d: %02X\n", i, recv.frame.data[i]);
    }	
#endif
  } // loop
  
  usleep(1000000); // 20msec
  
  ret = ioctl(fd, IOCTL_CATS_CAN_stop, &senddev);
  ret = ioctl(fd, IOCTL_CATS_CAN_stop, &recvdev);
  
  close(fd);

// read status...
  read_register(0, 2, &data);
  printf("Status 0: %02X\n", data);
  read_register(1, 2, &data);
  printf("Status 1: %02X\n", data);

// read error counter.
   read_register(0, 14, &data);
   printf("Device 0, RX Error: %d\n", data);

   read_register(0, 15, &data);
   printf("Device 0, TX Error: %d\n", data);

   read_register(1, 14, &data);
   printf("Device 1, RX Error: %d\n", data);

   read_register(1, 15, &data);
   printf("Device 1, TX Error: %d\n", data);
}


void dump_register(int device, int start, int count)
{
	int i;
	int data;
	for(i = 0; i < count; ++i) {
		  read_register(device, start + i, &data);
		  if((i & 3)==0) {
		  		 printf("(%d)Adr: 0x%02X:-   ", device, i + start);
		  }
		  printf("0x%02X", data);
		  if((i & 3)==3) {
		  		printf("\n");
		  } else {
		  	printf(" . ");
		  }
	}
}


int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  int i;
  int data;
  int loopcnt = 1;
  if (argc > 1) {
    loopcnt = atoi(argv[1]);
  }

#if 0
		write_register(0, 14, 0);
		write_register(0, 15, 0);
		write_register(1, 14, 0);
		write_register(1, 15, 0);
return 0;
#endif

//	can_init(0);
//	can_init(1);


//    test_reset(1, 1234);

//	  i = test_scan(0);
//	  printf("Scan status: %d\n", i);
//      can_start(0, 100);

 can_send_recv(10, loopcnt);


//		can_stop(0);
//		can_stop(1);


#if 0
// Device Read test...
for(;;) {
		read_register(0, 0, &data);
}
#endif

#if 0
	write_register(0, 0, 1);	// RESET
	dump_register(0, 0, 32);
	write_register(1, 0, 1);	// RESET
	dump_register(1, 0, 32);
#endif

#if 0
// TJA1041 の /stb 制御
for(i = 0; i < 9999999; ++i) {
   write_register(0, 0x100, i & 1);
   write_register(1, 0x100, ~i & 1);
   write_register(0, 0x101, i & 1);
   write_register(1, 0x101, ~i & 1);
}
#endif

	return 0;
}

/* end of file "main.c" */

