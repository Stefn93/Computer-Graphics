varying vec3 N;
varying vec3 L;
varying vec3 E;
varying vec3 H;

void main()
{
    vec3 Normal = normalize(N);
    vec3 Light  = normalize(L);
    vec3 Eye    = normalize(E);
    vec3 Half   = normalize(H);
	
	vec3 R = normalize(-reflect(L,N));

    float Kd = max(dot(Normal, Light), 0.0);
    //float Ks = pow(max(dot(Half, Normal), 0.0), gl_FrontMaterial.shininess);
	float Ks = pow(max(dot(R, E), 0.0), 0.1* gl_FrontMaterial.shininess);
    float Ka = 0.0;

    vec4 diffuse  = Kd * gl_FrontLightProduct[0].diffuse;
    vec4 specular = Ks * gl_FrontLightProduct[0].specular;
    vec4 ambient  = Ka * gl_FrontLightProduct[0].ambient;
	
    gl_FragColor = ambient + diffuse + specular;

}