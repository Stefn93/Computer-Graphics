uniform float time;
attribute float vx, vy, vz; //add velocity in z-direction
const float a = -0.000001;

void main()
{

    vec4 t = gl_Vertex;
    t.x = gl_Vertex.x + vx*time;
    t.y = gl_Vertex.y + vy*time + 0.5 * a * time * time;
	t.z = gl_Vertex.z + vz*time + 0.5 * a * time;

	//utilize t.z and gl_PointSize

    gl_Position = gl_ModelViewProjectionMatrix*t;
	gl_PointSize = gl_Position.y * 1.5;
    gl_FrontColor =  gl_Color;
}