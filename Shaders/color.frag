#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used

// varying vec4 varyColor;
varying vec2 texture;

uniform sampler2D texture1;
uniform sampler2D texture2;



void main()
{
    // gl_FragColor = varyColor;
    gl_FragColor = texture2D(texture1,texture)+texture2D(texture2,texture);

}
