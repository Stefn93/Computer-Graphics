attribute vec4 vertices2, vertices3;
uniform float time;

void main()
{
    float s =  0.5*(1.0 + sin(0.001 * time));
    vec4 t = mix(gl_Vertex, vertices2, s);
    gl_Position = gl_ModelViewProjectionMatrix*t;

	/* ----------- ADD ------------------ 
		maintain color change with movement of the triangle
	*/
	gl_FrontColor = gl_Color + vec4(0, s/2, 0, 1);

	/* ----------- ADD ------------------ 
		morph shape
	*/
}