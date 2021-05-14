#version 130
precision mediump float;

attribute vec3 vPosition; //Depending who compiles, these variables are not "attribute" but "in". In this version (130) both are accepted. in should be used later
attribute vec3 vNormal;
attribute vec2 vUV;

uniform mat4 uMVP;

// varying vec4 varyColor; 
varying vec2 texture; 



void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0);
    // varyColor = vec4((vNormal+vec3(1,1,1))/2.0, 1.0);
    texture = vUV;
}
