#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

//texture
uniform sampler2D	SPRITE_CRT;
uniform int			texture_id;
uniform vec2			direction;

// input from vertex shader
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

void main()
{
	if(texture_id == 1){

		fragColor = texture( SPRITE_CRT, tc );
	}
	else{
		fragColor = vec4(normalize(norm), 1.0);
	}
}
