#version 130
precision mediump float;

attribute vec3 vPosition; //Depending who compiles, these variables are not "attribute" but "in". In this version (130) both are accepted. in should be used later
attribute vec3 vNormal;
attribute vec2 vUV;

uniform mat4 uMVP;
uniform mat4 modelmatrix;
uniform mat3 inv_modelmatrix;

varying vec3 varyPosition; 
varying vec3 varyNormal;
varying vec2 texture; 



void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0);
    
	vec4 tmp = modelmatrix * vec4(vPosition, 1.0);
	tmp = tmp / tmp.w;
	varyPosition=tmp.xyz;
    
    varyNormal = normalize(transpose(inv_modelmatrix)*vNormal);
    
    texture = vUV;
}
