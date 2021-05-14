#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used


varying vec3 varyPosition;
varying vec3 varyNormal;
varying vec2 texture;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform  float ka = 0.20;
uniform  float kd = 0.50;
uniform  float ks = 0.40;
uniform  float alpha = 20;

uniform vec3 color;

uniform vec3 lightcolor;
uniform vec3 lightposition;

uniform vec3 cameraposition;



void main()
{
    

    vec3 L = normalize(lightposition - varyPosition);
	vec3 N = varyNormal;
	vec3 R = normalize(reflect(-L,N));
	vec3 V = normalize(cameraposition-varyPosition);

	vec3 Ambiant = ka * color *lightcolor;

	vec3 Diffuse = kd*max(0.0,dot(N,L))*color*lightcolor;

	vec3 Specular = ks*pow(max(0.0,dot(R,V)),alpha)*lightcolor;


    gl_FragColor = (texture2D(texture1,texture)+texture2D(texture2,texture))*vec4(Ambiant+Diffuse+Specular,1.0);

}
