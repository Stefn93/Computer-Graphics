#ifndef V3D_H
#define V3D_H


void v3dSum( const float v1[], const float v2[], float res[] );

void v3dSub( const float v1[], const float v2[], float res[] );

void v3dMul( const float v[], const float k, float res[] );

float v3dLength( const float vec[] );

void v3dNormalize( float vec[] );

float v3dDot( const float v1[], const float v2[] );

void v3dCross( const float v1[], const float v2[], float res[]);

void v3dSet( float dst[], const float src[] );

void v3dSet2( float dst[], float vx, float vy, float vz );

void v3dSetZero( float v[] );

float v3dDist( const float v1[], const float v2[] );

int v3dEqual( const float v1[], const float v2[] );

int v3dEqualThreshold( const float v1[], const float v2[], float threshold );

#endif
