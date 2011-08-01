#include "def.h"

volatile unsigned char DISTANCE1[] = "0";
volatile unsigned char DISTANCE2[] = "0";
volatile unsigned char DISTANCE3[] = "0";
float LaA = 0;
float LoA = 0;
float LatAdd1 = 45.3868;
float LonAdd1 = -71.9253;
float LatAdd2 = 45.4003;
float LonAdd2 = -71.8899;
float LatAdd3 = 45.3863;
float LonAdd3 = -71.9110;
extern char gps[];

const int EARTH_RADIUS = 6731;
const float FACT = 3.14159/180;

int getDist(float LatA,float LonA, float LatB, float LonB)
{

	float dLat = (LatB-LatA)*(FACT);		//OK
	float dLon = (LonB-LonA)*(FACT);		//OK
	LatA = LatA*FACT;						//OK
	LatB = LatB*FACT;						//OK
	
	float a1 = sin(dLat/2) * sin(dLat/2);		//OK
	float a2 = sin(dLon/2) * sin(dLon/2);		//OK
	float a3 = cos(LatA)* cos(LatB);			//OK
	
	float a = a1+ (a2*a3);						//OK
	
	float c = 2*atan2(sqrt(a),sqrt(1-a));			//OK
	
	float d = (float)(EARTH_RADIUS * c);
	
	d = d*1000;
	
	return (int)d;			
}

void assignDist(float LatA, float LonA)
{	
	int D1 = getDist(LatA,LonA,LatAdd1,LonAdd1);
	
	sprintf (DISTANCE1, "%d", D1);
	
	int D2 = getDist(LatA,LonA,LatAdd2,LonAdd2);
	
	sprintf (DISTANCE2, "%d", D2);
	
	int D3 = getDist(LatA,LonA,LatAdd3,LonAdd3);
	
	sprintf (DISTANCE3, "%d", D3);
	
}

void convStr(void)
{
	//LaA = (48-gpsstr[20]);							//+(48-gpsstr[21]);
	puts_usart1(gps);
	LaA = ((char)gps[21]-48);
}


