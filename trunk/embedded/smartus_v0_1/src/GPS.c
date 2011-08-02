#include "def.h"

char DISTANCE1[6] = "0";
char DISTANCE2[6] = "0";
char DISTANCE3[6] = "0";
float LaA = 0;
float LoA = 0;
float LatAdd1 = 45.3868;
float LonAdd1 = -71.9253;
float LatAdd2 = 45.4003;
float LonAdd2 = -71.8899;
float LatAdd3 = 45.3863;
float LonAdd3 = -71.9110;
extern char gps[];
int D1,D2,D3;

const int EARTH_RADIUS = 6738;
const float FACT = 3.14159/180;

#ifdef GPS_FEEDTHROUGH
int getDist(float LatA,float LonA, float LatB, float LonB)
{
	float dLat,dLon,a1,a2,a3,a,c,d;

	dLat = (LatB-LatA)*(FACT);		//OK
	dLon = (LonB-LonA)*(FACT);		//OK
	LatA = LatA*FACT;						//OK
	LatB = LatB*FACT;						//OK
	
	a1 = sinf((float)((float)dLat/2)) * sinf((float)((float)dLat/2));		//OK
	a2 = sinf((float)((float)dLon/2)) * sinf((float)((float)dLon/2));		//OK
	a3 = cosf((float)LatA)* cosf((float)LatB);			//OK
	
	a = a1+ (a2*a3);						//OK
	
	c = 2*atan2f((float)sqrtf(a),(float)sqrtf(1-a));			//OK
	
	d = (float)(EARTH_RADIUS * c);
	
	d = d*1000;
	
	return (int)d;			
}

void assignDist(float LatA, float LonA)
{
	D1 = getDist(LatA,LonA,LatAdd1,LonAdd1);
	
	D1 = D1-50;
	
	if(D1<0)
		D1 = 0;
	
	sprintf (DISTANCE1, "%i\n", D1);
	
	D2 = getDist(LatA,LonA,LatAdd2,LonAdd2);
	
	D2 = D2-50;
	if(D2<0)
		D2 = 0;
	
	sprintf (DISTANCE2, "%i\n", D2);
	
	D3 = getDist(LatA,LonA,LatAdd3,LonAdd3);
	
	D3 = D3-50;
	if(D3<0)
		D3 = 0;
	
	sprintf (DISTANCE3, "%i\n", D3);
	
	puts_usart1(DISTANCE1);
	//puts_usart1(DISTANCE2);
	//puts_usart1(DISTANCE3);
	
}

void convStr(void)
{
	int sign = 1;
	float minLaA,minLoA;
	float temp1;
	float temp2;
	

	
	if(gps[31]=='S') //If lat is negative
	{
		sign = -1;
	}
	
	LaA = 0;
	LoA = 0;

	LaA = (10*(float)(gps[20]-48)) + (float)(gps[21]-48);
	temp1 =(float)(gps[22]-48);
	temp2 = (float)(gps[23]-48);
	minLaA = (10*temp1) + (temp2);
	
	temp1 =(float)(gps[25]-48);
	temp2 = (float)(gps[26]-48);
	minLaA += (temp1/10) + (temp2/100);
	temp1 = (float)(gps[27]-48);
	minLaA += (temp1/1000);
	minLaA = minLaA/60;
	LaA += minLaA;
	LaA = sign * LaA;
	
	sign =1;
	
	if(gps[45]=='W') //If lon is negative
	{
		sign = -1;
	}
	
	LoA = (100*((float)(gps[33]-48)))+(10*(float)(gps[34]-48)) + (float)(gps[35]-48);
	temp1 =(float)(gps[36]-48);
	temp2 = (float)(gps[37]-48);
	minLoA = (10*temp1) + (temp2);
	
	temp1 =(float)(gps[39]-48);
	temp2 = (float)(gps[40]-48);
	minLoA += (temp1/10) + (temp2/100);
	
	temp1 = (float)(gps[41]-48);
	minLoA += (temp1/1000);
	
	minLoA = minLoA/60;
	
	LoA += minLoA;
	LoA = sign * LoA;
	
}
#endif

