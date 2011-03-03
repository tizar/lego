#ifndef _LEGODLL_H_
#define _LEGODLL_H_

#include "visatype.h"

extern "C"
{
  
	__declspec(dllexport) void nxtinit( int * status);
	__declspec(dllexport) void nxtstop();

	__declspec(dllexport) void getbatterylevel(int * level);
	__declspec(dllexport) void startprogram(char * filename);
	__declspec(dllexport) void stopprogram();
	//sound
	__declspec(dllexport) void playtone( int * frequency, int * duration);
	__declspec(dllexport) void playsoundfile(char * name,int * loop);
	__declspec(dllexport) void stopsound();
	//motors
	__declspec(dllexport) void motor_getrotation(int * port,int * rotation);
	__declspec(dllexport) void motor_resetrotation(int * port, int * relative);
	__declspec(dllexport) void motor_setforward(int * port, int * power);
	__declspec(dllexport) void motor_setreverse(int * port, int * power);
	__declspec(dllexport) void motor_stop(int * port, int * brake);   
	//sensor
	__declspec(dllexport) void sensor_settouch(int * port);
	__declspec(dllexport) void sensor_setsound(int * port);
	__declspec(dllexport) void sensor_setlight(int * port, int * active);
	__declspec(dllexport) void sensor_setsonar(int * port);
	__declspec(dllexport) void sensor_getvalue(int * port, int * value);

	//__declspec(dllexport) void (int * status);
	//__declspec(dllexport) void senddirectcommand(int * response, int * dc_buf, int * dc_buf_size, int *re_buf, int * re_buf_size);

}

#endif
