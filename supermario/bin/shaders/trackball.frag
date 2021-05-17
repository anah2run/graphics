#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

//texture
uniform sampler2D	SPRITE_CRT;
uniform sampler2D	TEX_SKYBOX;
//
uniform int			mode;
uniform vec2		direction;
uniform ivec4		animation; // sprite_id, status, max_frame, frame;
uniform vec4		particle_color;
// input from vertex shader
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

void main()
{
	if(mode == 9){ // skybox
		fragColor = texture( TEX_SKYBOX, tc);
	}
	else if(mode == 1){ // block texture
		fragColor = vec4(normalize(norm), 1.0f);
	}
	else if(mode == 2){ // sprite animation
		fragColor = texture( SPRITE_CRT, vec2((animation.a + (tc.x*0.8f))/ animation.z ,tc.y * 0.8f));
	}
	else if(mode == 3){ // sprite
		fragColor = texture( SPRITE_CRT, tc);
	}
	else if(mode == 4){ // particle
		fragColor = particle_color;
	}
	else if(mode == -1){ //shadow
		fragColor = vec4(0,0,0,0.5f);
	}
	else{ //default
		fragColor = vec4(normalize(norm), 1.0f);
	}
}
