void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    vec4 eyePosition = gl_ModelViewMatrix * gl_Vertex;
    vec4 eyeLightPos = gl_LightSource[0].position;

    vec3 N = normalize(gl_NormalMatrix * gl_Normal);
    //vec3 N = normalize(gl_ModelViewMatrix * vec4(gl_Normal, 0.0));  // this is the same as above, when only using uniform scaling
    vec3 L = normalize(eyeLightPos.xyz - eyePosition.xyz);
    vec3 E = -normalize(eyePosition.xyz);
	vec3 R = -reflect(N, L);

    float Kd = max(dot(N, L), 0.0);
    float Ks = pow(max(dot(R, N), 0.0), gl_FrontMaterial.shininess);
    float Ka = 0.0;

    vec4 ambient  = Ka * gl_FrontLightProduct[0].ambient;
    vec4 diffuse  = Kd * gl_FrontLightProduct[0].diffuse;
    vec4 specular = Ks * gl_FrontLightProduct[0].specular;

    gl_FrontColor = ambient + diffuse + specular;
}
