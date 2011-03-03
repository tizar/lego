#include "legolinux.h"
#include "string.h"
struct usb_device * usb_devinit(void){
    struct usb_bus *usb_bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next)
	{
		for (dev = usb_bus->devices; dev; dev = dev->next)
		{
			if ((dev->descriptor.idVendor == NXT_VENDOR_ID) && (dev->descriptor.idProduct == NXT_PRODUCT_ID))
				return dev;
		}
	}
	return 0;
}

int usb_devopen(){
    struct usb_device *usb_dev;
	int nEp = 0;

	usb_dev = usb_devinit();

	if (usb_dev == 0)
	{
		return 0;
	}

	pUSBHandle = usb_open(usb_dev);

	if (pUSBHandle == 0)
	{
		return 0;
	}
	else
	{
		if (usb_dev->config)
		{
			if (usb_dev->config->interface)
			{
				if (usb_dev->config->interface->altsetting)
				{
					usb_claim_interface(pUSBHandle, usb_dev->config->interface->altsetting->bInterfaceNumber);
					if ((nEp = usb_dev->config->interface->altsetting->bNumEndpoints))
					{
						if (usb_dev->config->interface->altsetting->endpoint)
						{
							ucEpIn = (unsigned char) (usb_dev->config->interface->altsetting->endpoint[0].bEndpointAddress);
							if (nEp > 1)
								ucEpOut = (unsigned char) (usb_dev->config->interface->altsetting->endpoint[1].bEndpointAddress);

						}
					}
				}
			}
		}
	}
	return 1;
}
int usb_devreset(){
    return usb_reset(pUSBHandle);
}

void usb_devwrite(ViByte * buf, int buf_size)
{
	if (pUSBHandle)
		usb_bulk_write(pUSBHandle, ucEpIn,(char*)buf, buf_size, TIMEOUT);
}

void usb_devread(ViByte * buf, int buf_size)
{
	if (pUSBHandle)
		usb_bulk_read(pUSBHandle, ucEpOut, (char *)buf, buf_size, TIMEOUT);
}

/* communication */

void comm_open(int *result){
    *result = 0;

	if (usb_devopen())
	{
		if (usb_devreset() == 0)
			*result = 1;
	}
}


void comm_senddirectcommand(ViBoolean response, ViByte * dc_buf,
	int dc_buf_size, ViByte * re_buf, int re_buf_size)
{
	ViByte buf[dc_buf_size + 1];
	int i;

	memcpy(buf + 1, dc_buf, dc_buf_size * sizeof(ViByte));
	buf[0] = response ? 0x00 : 0x80;

	usb_devwrite(buf, dc_buf_size + 1);

	if (response)
	{
		ViByte tempreadbuf[re_buf_size + 1];
		usb_devread(tempreadbuf, re_buf_size + 1);

		for (i = 1; i < re_buf_size + 1; i++)
			re_buf[i - 1] = tempreadbuf[i];
	}
}


/* NXT functions */
void nxt_playtone(int * frequency, int * duration)
{
	ViUInt8 directCommandBuffer[] = { 0x03, *frequency%256, (*frequency-(*frequency%256))/256, *duration%256, (*duration-(*duration%256))/256 };

	// Send the direct command to the NXT.
	 comm_senddirectcommand( 0 /* a response is not required for this direct command */,
		(ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		NULL /* no response buffer */, 0 /* no response buffer, specify 0 for size */);
}

void nxt_batterylevel(int * batterylevel){
    ViUInt8 directCommandBuffer[] = { 0x0B };
	ViUInt8 responseBuffer[] = { 0x01, 0x01, 0x01, 0x01 };
    *batterylevel=-1;
	// Send the direct command to the NXT.
	 comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));
    
	*batterylevel= responseBuffer[3]*256+responseBuffer[2];
}


void nxt_KeepAlive()
{
	ViUInt8 directCommandBuffer[] = { 0x0D };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}



/* NXT motor */
void nxt_motor_getrotation(int * port,int * tacho)
{
	ViUInt8 directCommandBuffer[] = { 0x06, *port };
	ViInt8 responseBuffer[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

	// Send the direct command to the NXT.
	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));

	int i = responseBuffer[20];
	if(i < 0)
		i = 256 + i;
	if(responseBuffer[22] == -1)
		responseBuffer[22] = 0;
	if(responseBuffer[23] == -1)
		responseBuffer[23] = 0;

	*tacho = responseBuffer[23]*16777216+responseBuffer[22]*65536+responseBuffer[21]*256+i;
}

void nxt_motor_resetrotation(int * port, int * relative)
{
	ViUInt8 directCommandBuffer[] = { 0x0A, *port, *relative };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_motor_setforward(int *port, int *power)
{
	ViUInt8 directCommandBuffer[] = { 0x04, *port, *power, 0x01 | 0x04, 0x01, 0, 0x20, 0, 0, 0, 0 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_motor_setreverse(int *port, int *power)
{
	ViUInt8 directCommandBuffer[] = { 0x04, *port, - *power, 0x01 | 0x04, 0x01, 0, 0x20, 0, 0, 0, 0 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_motor_stop(int *port, int *brake)
{
	if(*brake)
	{
		ViUInt8 directCommandBuffer[] = { 0x04, *port, 0, 0x01 | 0x02 | 0x04, 0x01, 0, 0x20, 0, 0, 0, 0 };
		comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
	}
	else
	{
		ViUInt8 directCommandBuffer[] = { 0x04, *port, 0, 0x00, 0x00, 0, 0x00, 0, 0, 0, 0};
		comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
	}
}

void nxt_motor_brakeon(int *port)
{
	ViUInt8 directCommandBuffer[] = { 0x04, *port, 0, 0x01 | 0x02 | 0x04, 0x01, 0, 0x20, 0, 0, 0, 0 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_motor_brakeoff(int *port)
{
	ViUInt8 directCommandBuffer[] = { 0x04, *port, 0, 0x00, 0x00, 0, 0x00, 0, 0, 0, 0};
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_motor_goto(int * port, int *power, int * tacho, int *brake)
{
    int curr_tacho;
    nxt_motor_getrotation(port,&curr_tacho);
	if(*tacho >= curr_tacho)
	{
		nxt_motor_setforward(port, power);
		while(1) {
		    nxt_motor_getrotation(port,&curr_tacho);
            if (curr_tacho < *tacho) break;
		}
		nxt_motor_stop(port, brake);
	}
	else
	{
		nxt_motor_setreverse(port, power);
		while(1) {
		    nxt_motor_getrotation(port,&curr_tacho);
            if (curr_tacho < *tacho) break;
        }
		nxt_motor_stop(port, brake);
	}
}



/* Sensor functions */
void nxt_sensor_settouch(int *port)
{
	ViUInt8 directCommandBuffer[] = { 0x05, *port, 0x01, 0x20 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_sensor_setsound(int *port)
{
	ViUInt8 directCommandBuffer[] = { 0x05, *port, 0x07, 0x80 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_sensor_setlight(int *port, int *active)
{
	if(*active)
	{
		ViUInt8 directCommandBuffer[] = { 0x05, *port, 0x05, 0x80 };
		comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
	}
	else
	{
		ViUInt8 directCommandBuffer[] = { 0x05, *port, 0x06, 0x80 };
		comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
	}
}

void nxt_sensor_setsonar(int *port)
{
	ViUInt8 directCommandBuffer[] = { 0x05, *port, 0x0B, 0x00 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

void nxt_sensor_setraw(int *port)
{
	ViUInt8 directCommandBuffer[] = { 0x05, *port, 0x07, 0x00 };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}

/*
void nxt_sensor_set(int *port, SensorType type)
{
	int msgtype, mode;
	switch (type)
	{
	case Touch:
		msgtype = 0x01;
		mode = 0x20;
		break;
	case Sound:
		msgtype = 0x07;
		mode = 0x80;
		break;
	case Light:
		msgtype = 0x06;
		mode = 0x80;
		break;
	case ActiveLight:
		msgtype = 0x05;
		mode = 0x80;
		break;
	case Sonar:
		msgtype = 0x0B;
		mode = 0x00;
		break;
	}
	ViUInt8 directCommandBuffer[] = { 0x05, *port, msgtype, mode };
	comm_senddirectcommand( 0, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ), NULL, 0);
}
*/
void nxt_sensor_getvalue(int *port, int * value)
{
	ViUInt8 directCommandBuffer[] = { 0x07, *port };
	ViUInt8 responseBuffer[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));

	*value=responseBuffer[12]*256+responseBuffer[11];
}

void nxt_sensor_lsgetstatus(int *port,int * status)
{
	ViUInt8 directCommandBuffer[] = {0x0E, *port};
	ViUInt8 responseBuffer[] = {1,1,1};


	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer),
		   (ViByte*)responseBuffer, sizeof( responseBuffer ));

	*status= (int)responseBuffer[2];
}

void nxt_sensor_getsonarval(int *port, int *value)
{

	ViUInt8 directCommandBuffer[] = {0x0F, *port, 0x03, 0x00, 0x02, 0x41, 0x02};
	ViUInt8 responseBuffer[] = { 1,1};

	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));

	int bytesRead = 0;

	do {

	    ViUInt8 directCommandBuffer[] = {0x0F, *port, 0x02, 0x01, 0x02, 0x42};
	    ViUInt8 responseBuffer[] = {1, 1};


	    comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		        (ViByte*)responseBuffer, sizeof( responseBuffer));

        nxt_sensor_lsgetstatus(port,&bytesRead);

	} while (bytesRead < 1);
	
	ViUInt8 directCommandBuffer2[] = {0x10, *port};
	ViUInt8 responseBuffer2[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


	comm_senddirectcommand( 1, (ViByte*) directCommandBuffer2, sizeof( directCommandBuffer2 ),
		(ViByte*)responseBuffer2, sizeof( responseBuffer2 ));

	*value= (int)responseBuffer2[3];
	  
}

void nxt_sensor_setsonaroff(int *port)
{
	ViUInt8 directCommandBuffer[] = {0x0F, *port, 0x03, 0x00, 0x02, 0x41, 0x00};
	ViUInt8 responseBuffer[] = { 1,1};

	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));
}

void nxt_sensor_setsonarsingleshot(int *port)
{
	ViUInt8 directCommandBuffer[] = {0x0F, *port, 0x03, 0x00, 0x02, 0x41, 0x01};
	ViUInt8 responseBuffer[] = { 1,1};

	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));
}

void nxt_sensor_setsonarcontinuous(int *port)
{
	ViUInt8 directCommandBuffer[] = {0x0F, *port, 0x03, 0x00, 0x02, 0x41, 0x02};
	ViUInt8 responseBuffer[] = { 1,1};

	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));
}

void nxt_sensor_setSonarContinuousInterval(int *port, int interval)
{
	ViUInt8 directCommandBuffer[] = {0x0F, *port, 0x03, 0x00, 0x02, 0x40, interval};
	ViUInt8 responseBuffer[] = { 1,1};

	comm_senddirectcommand( 1, (ViByte*)directCommandBuffer, sizeof( directCommandBuffer ),
		(ViByte*)responseBuffer, sizeof( responseBuffer ));
}

