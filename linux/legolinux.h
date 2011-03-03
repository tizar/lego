#ifndef _LEGOLINUX_
#define _LEGOLINUX_

#include <usb.h>
#include "visatype.h"

#define NXT_VENDOR_ID 0x0694
#define NXT_PRODUCT_ID 0x0002


usb_dev_handle *pUSBHandle = 0;
char ucEpOut = 0;
char ucEpIn = 0;
int TIMEOUT = 0x0BB8;

/* usb functions */
struct usb_device * usb_devinit(void);
int usb_devopen();
int usb_devreset();
void usb_devwrite(ViByte * buf, int buf_size);
void usb_devread(ViByte * buf, int buf_size);

/* comm functions */
void comm_open(int *result);
void comm_senddirectcommand(ViBoolean response, ViByte * dc_buf,
    int dc_buf_size, ViByte * re_buf, int re_buf_size);

/* nxt functions */
void nxt_playtone(int * frequency, int * duration);
void nxt_batterylevel(int * batterylevel);
void nxt_KeepAlive();

/* nxt motor */	
void nxt_motor_getrotation(int * port,int * tacho);
void nxt_motor_resetrotation(int * port, int * relative);
void nxt_motor_setforward(int *port, int *power);
void nxt_motor_setreverse(int *port, int *power);
void nxt_motor_stop(int *port, int *brake);
void nxt_motor_brakeon(int *port);
void nxt_motor_brakeoff(int *port);
void nxt_motor_goto(int * port, int *power, int * tacho, int *brake);
#endif
