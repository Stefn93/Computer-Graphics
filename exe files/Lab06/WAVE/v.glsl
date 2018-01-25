//#version 110

uniform float time;
uniform float A;
uniform float omega;

void main()
{

	vec4 v = vec4(gl_Vertex); //costruttore che prende gl_Vertex
	v.y = A * sin(omega*time + 5.0*gl_Vertex.x) * sin(omega*time + 5.0*gl_Vertex.z);
	
	gl_Position = gl_ModelViewProjectionMatrix * v;
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_FrontColor = gl_Color;
}