#include "legodll.h"
#include "NXT++.h"
static Comm::NXTComm comm;
extern "C"
{
	__declspec(dllexport) void nxtinit(int * status){
		if(NXT::Open(&comm)){
			NXT::PlayTone(&comm,440,50);
			*status=1;
		} else {
			*status=0;
		}
	}

	__declspec(dllexport) void nxtstop(){
		NXT::Close(&comm);
	}

	__declspec(dllexport) void getbatterylevel(int *level){
//		NXT::PlayTone(&comm,440,1000);
		*level = NXT::BatteryLevel(&comm);
	}

	__declspec(dllexport) void startprogram(char * filename){
		std::string fname = std::string(filename);
		NXT::StartProgram(&comm, fname);

	}

	__declspec(dllexport) void stopprogram(){
		NXT::StopSound(&comm);
	}

	//sound
	__declspec(dllexport) void playtone( int * frequency, int * duration){
		NXT::PlayTone(&comm, *frequency, *duration);
	}
	__declspec(dllexport) void playsoundfile(char * filename,int * loop){
		std::string fname = std::string(filename);
		NXT::PlaySoundFile(&comm, fname, (bool)(*loop));
	}
	__declspec(dllexport) void stopsound(){
		NXT::StopSound(&comm);
	}
	
	//motor
	__declspec(dllexport) void motor_getrotation(int *  port,int * rotation){
		*rotation=NXT::Motor::GetRotationCount(&comm,*port);
	}
	__declspec(dllexport) void motor_resetrotation(int * port, int * relative){
		NXT::Motor::ResetRotationCount(&comm, *port, *relative);
	}
	__declspec(dllexport) void motor_setforward(int * port, int * power){
		NXT::Motor::SetForward(&comm, *port, *power);
	}
	__declspec(dllexport) void motor_setreverse(int * port, int * power){
		NXT::Motor::SetReverse(&comm, *port, *power);
	}

	__declspec(dllexport) void motor_stop(int * port, int * brake){
		 NXT::Motor::Stop(&comm, *port, (bool)(*brake));
	}
	//sensor
	__declspec(dllexport) void sensor_settouch(int * port);
	__declspec(dllexport) void sensor_setsound(int * port);
	__declspec(dllexport) void sensor_setlight(int * port, int * active);
	__declspec(dllexport) void sensor_setsonar(int * port);
	__declspec(dllexport) void sensor_getvalue(int * port, int * value);

	//__declspec(dllexport) void (int * status);
	//__declspec(dllexport) void senddirectcommand(int * response, int * dc_buf, int * dc_buf_size, int *re_buf, int * re_buf_size);
  
}
