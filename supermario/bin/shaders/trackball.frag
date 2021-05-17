#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

//texture
uniform sampler2D	SPRITE_CRT;
uniform sampler2D	TEX_SKYBOX;
uniform sampler2D	TEX_BLOCKS;
uniform sampler2D	TEX_BLOCKS_OP;
//
uniform int			mode;
uniform int			block_id;
uniform vec2		direction;
uniform ivec4		animation; // sprite_id, status, max_frame, frame;
uniform vec4		particle_color;

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

//light
uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	//sunlight
uniform int		NUM_LIGHT;
uniform vec4	light_position2[10], Ia2[10], Id2[10], Is2[10];	//lights
uniform vec4	Ka, Kd, Ks;

vec4 phong(vec3 l, vec3 n, vec3 h, vec4 Kd, vec4 ila, vec4 ild, vec4 ils)
{
	
	vec4 Ira,Ird,Irs;
	if (block_id == 4) {
		Ira = 0.5f*Ka * ila;											// ambient reflection
		Ird = 0.7f*max(Kd * dot(l, n) * ild, 0.0);					// diffuse reflection
		Irs = max(Ks * pow(dot(h, n), shininess*5.f) * ils, 0.0);	// specular reflection
	}
	else {
		Ira = Ka * ila;
		Ird = max(Kd * dot(l, n) * ild, 0.0);
		Irs = max(Ks * pow(dot(h, n), shininess) * ils, 0.0);
	}
		
	return Ira + Ird + Irs;
}


void main()
{
	vec4 lpos = view_matrix * light_position;
	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l + v);	// the halfway vector
	//sunlight effect
	fragColor = phong(l, n, h, Kd,Ia,Id,Is) * 1.7f;
		
	//lights effect
	float len = 10.f;
	for (int i = 0; i < NUM_LIGHT; i++) {
		lpos = view_matrix * light_position2[i];
		len = 0.4f*abs(length(lpos - epos));
		len = len < 0.3f ? 0.3f : len;
		l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));
		h = normalize(l + v);
		fragColor = min(fragColor + phong(l, n, h, Kd, Ia2[i], Id2[i], Is2[i]) / (len *len),1.5f);
	}
	switch (mode) {
	case 9: // skybox
		fragColor = texture( TEX_SKYBOX, tc);
		break;
	case 1: // block texture
		fragColor *= texture(TEX_BLOCKS, tc+vec2(0,0.1f* (block_id-1)));
		//if(block_id==4)	fragColor.a *= 0.15f;
		fragColor.a = texture(TEX_BLOCKS_OP, tc + vec2(0, 0.1f * (block_id - 1))).x;
		break;
	case 2: // sprite animation
		fragColor *= texture( SPRITE_CRT, vec2((animation.a + (tc.x*0.8f))/ animation.z ,tc.y * 0.8f));
		break;
	case 3: // sprite
		fragColor *= texture( SPRITE_CRT, tc);
		break;
	case 4: // particle
		fragColor *= particle_color;
		break;
	case -1: //shadow
		fragColor = vec4(0,0,0,fragColor.x/3 );
		break;
	default:
		fragColor = vec4(normalize(norm), 1.0f);
		break;
	}
}
