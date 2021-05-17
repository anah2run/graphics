#define STB_IMAGE_IMPLEMENTATION
#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "item.h"
#include "map.h"
#include "character.h"
#include "gun.h"
#include "sprite.h"
#include "skybox.h"
#include "sound.h"
#include "particle.h"
//*************************************
// global constants
static const char*	window_name = "bbangya";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 5, 5, 20 );
	vec3	at = vec3( 5, 5, 0 );
	vec3	up = vec3( 0, 1, 0 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	fovy = PI/4; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;

	mat4	projection_matrix;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	sprite_vertex_array = 0;
GLuint	cube_vertex_array = 0;
GLuint	circle_vertex_array = 0;
GLuint	skybox_vertex_array = 0;
// Texture & sprite
GLuint	SPRITE_CRT = 0;
GLuint	TEX_SKYBOX = 0;
//*************************************
// global variables
int		frame = 0;				// index of rendering frames
// holder of vertices and indices of a unit circle
Map map(new_map);
Character	crt(&map,vec2(4,3));
Gun			gun(&crt, 0);
std::list<Enemy> enemy_list;

bool	b_key_fire = false;
bool	b_key_right = false;
bool	b_key_left = false;
//*************************************
// scene objects
camera		cam;

//*************************************
void update(float t)
{
	// update projection matrix
	cam.aspect = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);

	if (b_key_right) {
		crt.move_right();
	}
	else if (b_key_left) {
		crt.move_left();
	}
	
	crt.update(float(t), b_key_left || b_key_right);
	gun.trigger(&b_key_fire, t);

	// build bullets
	for (std::list<Bullet>::iterator it = bullet_instances.begin(); it != bullet_instances.end(); it++) {
		bool destroy = false;
		it->update(t);
		Block* bp = map.block(it->position);
		if (bp != 0) // bp is not null;
		{
			destroy = bp->prop->destroy_bullet;
			if (bp->prop->max_hp > 0) {
				engine->play2D(mp3_src_blockhit); //sfx
				bp->hit(it->prop->block_dmg);
			}
		}
		for (std::list<Enemy>::iterator eit = enemy_list.begin(); eit != enemy_list.end(); eit++) {
			if (eit->hitbox.collid(it->position - eit->position)) {
				if (eit->hit(it->prop->dmg, it->direction))
				{
					destroy = true; //적들이 총알에 맞았을 때
					engine->play2D(mp3_src_enmhit); //sfx
					break;
				}
			}
		}
		if (it->life <= 0 || destroy) {
			it = bullet_instances.erase(it);
		}
	}
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		if (it->hitbox.collid(it->position - crt.position)) {
			crt.hit(it->damage, it->position - crt.position);
		}
		it->update(t, true);
		if (!(it->alive)) {
			engine->play2D(mp3_src_enmboom); //sfx
			particles_list.push_back(Particle(1, it->position));
			it=enemy_list.erase(it);
		}
	}
	for (std::list<Item>::iterator it = item_list.begin(); it != item_list.end(); it++) {
		it->theta += t * 2;
		if (crt.hitbox.collid(it->position - crt.position)) {
			switch (it->item_code) {
			case 1:
				crt.heal(2);
				break;
			case 2:
				gun.swap_gun(1);
				break;
			case 3:
				gun.swap_gun(2);
				break;
			case 4:
				gun.swap_gun(3);
				break;
			default:
				break;
			}
			it = item_list.erase(it);
		}
	}

	//particles
	for (std::list<Particle>::iterator it = particles_list.begin(); it != particles_list.end(); it++) {
		it->updates(t);
		if(it->particles.size()== 0) it = particles_list.erase(it);
	}

	// camera & skybox
	float temp_x = crt.position.x;
	temp_x = std::min(std::max(temp_x, 10.0f), 60.0f);
	cam.eye = vec3(temp_x, 7, 13);
	cam.at = vec3(temp_x,7,0);
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	// setup texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX_SKYBOX);
	glUniform1i(glGetUniformLocation(program, "TEX_SKYBOX"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, SPRITE_CRT);
	glUniform1i(glGetUniformLocation(program, "SPRITE_CRT"), 1);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
}
void render()
{
	glEnable(GL_DEPTH_TEST);
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	GLint uloc;
	mat4 model_matrix;
	
	// SKYBOX
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(skybox_vertex_array);
	glUniform1i(glGetUniformLocation(program, "mode"), 9);
	mat4 model_matrix_sky = mat4::translate(cam.eye.x, 5, 0) *
		mat4::rotate(vec3(1, 0, 0), -PI / 2) *
		mat4::rotate(vec3(0, 0, 1), cam.eye.x / 20) *
		mat4::scale(vec3(20.0f));
	uloc = glGetUniformLocation(program, "model_matrix");			
	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix_sky);
	glDrawElements(GL_TRIANGLES, NUM_TESS * (NUM_TESS + 1) * 6, GL_UNSIGNED_INT, nullptr);

	// 맵
	glBindVertexArray(cube_vertex_array);
	uloc = glGetUniformLocation(program, "mode");		
	if (uloc > -1) glUniform1i(uloc, 1);
	
	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_HEIGHT; j++) {
			int block_id = map.map[i][j].prop->block_id;
			if (block_id > 0) {
				Block* bp = &map.map[i][j];
				mat4 translate_matrix = mat4::translate(float(i), float(j), 0);
				mat4 scale_matrix = mat4::scale(1);
				switch (block_id) {
				case 2:
				case 5://wood & spike
					scale_matrix = mat4::scale(1, 1, float(bp->hp) / bp->prop->max_hp);
					break;
				default://wall
					break;
				}
				model_matrix = translate_matrix * scale_matrix;
				uloc = glGetUniformLocation(program, "model_matrix");
				if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
				glDrawElements(GL_TRIANGLES, 3 * 4 * 6, GL_UNSIGNED_INT, nullptr);

			}
		}
	}

	// 그림자 
	glBindVertexArray(circle_vertex_array);
	uloc = glGetUniformLocation(program, "mode");		
	if (uloc > -1) glUniform1i(uloc, -1);

	int temp_y = map.shadow_pos(crt.position);
	if (temp_y >= 0) {

		model_matrix = mat4::translate(crt.position.x, temp_y + 0.01f, 0) * mat4::scale(crt.hitbox.width() / 2, 0, crt.hitbox.width() / 2);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 6 * 32, GL_UNSIGNED_INT, nullptr);
	}
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		temp_y = map.shadow_pos(it->position);
		model_matrix = mat4::translate(it->position.x, temp_y + 0.01f, 0) * mat4::scale(it->hitbox.width() / 2, 0, it->hitbox.width() / 2);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 6 * 32, GL_UNSIGNED_INT, nullptr);
	}

	// render sprites
	glBindVertexArray(sprite_vertex_array);

	// render items
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 0);
	for (std::list<Item>::iterator it = item_list.begin(); it != item_list.end(); it++) {
		model_matrix = mat4::translate(it->position.x, it->position.y, 0) * mat4::scale(0.9f, 0.9f, 1) * mat4::rotate(vec3(0, 1, 0),it->theta);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	
	// render enemies
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 0);
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		model_matrix = mat4::translate(it->position.x, it->position.y, -.01f) * mat4::scale(1, 1, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}

	// build character model
	glActiveTexture(GL_TEXTURE1);
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "animation");		if (uloc > -1) glUniform4i(uloc, 0, 0, crt.max_frame, crt.frame); // sprite_id, status, max_frame, frame;	
	
	if (crt.invinc_t <= 0 || int(crt.invinc_t * 10) % 2 == 0) {
		model_matrix = mat4::translate(crt.position.x, crt.position.y, 0) * mat4::scale(crt.direction.x, 1, 1);
		uloc = glGetUniformLocation(program, "direction");		if (uloc > -1) glUniform2i(uloc, int(crt.direction.x), int(crt.direction.y));
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}

	// build bullets
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 0);
	std::list<Bullet>::iterator it;
	for (it = bullet_instances.begin(); it != bullet_instances.end(); it++) {
		model_matrix = mat4::translate(it->position.x, it->position.y, 0) * mat4::scale(0.2f, 0.2f, 1);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	glDisable(GL_DEPTH_TEST);
	// render particles
	uloc = glGetUniformLocation(program, "mode");		if (uloc > -1) glUniform1i(uloc, 4);
	for (std::list<Particle>::iterator it = particles_list.begin(); it != particles_list.end(); it++) {
		for (std::list<particle_t>::iterator pit = it->particles.begin(); pit != it->particles.end(); pit++) {
			mat4 translate_matrix = mat4::translate(vec3(pit->position.x, pit->position.y, -1));
			mat4 scale_matrix = mat4::scale(pit->scale);
			model_matrix = translate_matrix * scale_matrix;

			uloc = glGetUniformLocation(program, "particle_color");			if (uloc > -1) glUniform4fv(uloc, 1, pit->color);
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)		cam = camera();
		else if (key == GLFW_KEY_K)		crt.hit(1,vec2(5,5));
		else if (key == GLFW_KEY_A)
		{
			b_key_left = true;
		}
		else if (key == GLFW_KEY_D)
		{
			b_key_right = true;
		}
		else if (key == GLFW_KEY_W || key == GLFW_KEY_SPACE) 
		{
			if(crt.jump()) 	engine->play2D(mp3_src_jump);
		}
		else if (key == GLFW_KEY_J)							b_key_fire = true;
		else if (key == GLFW_KEY_1)	gun.swap_gun(0);
		else if (key == GLFW_KEY_2)	gun.swap_gun(1);
		else if (key == GLFW_KEY_3)	gun.swap_gun(2);
		else if (key == GLFW_KEY_4)	gun.swap_gun(3);
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) b_key_left = false;
		if (key == GLFW_KEY_D) b_key_right = false;
		if (key == GLFW_KEY_J) b_key_fire = false;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		//if (action == GLFW_PRESS);
		//else if (action == GLFW_RELEASE);
	}
}

void motion( GLFWwindow* window, double x, double y )
{
}

void update_vertex_buffer(const std::vector<vertex>& vertices, std::vector<uint>& indices, GLuint* v_array)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }
	
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// generation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (*v_array) glDeleteVertexArrays(1, v_array);
	*v_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!*v_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	//glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests	
	glEnable(GL_TEXTURE_2D);			// enable texturing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);		// notify GL the current texture slot is 0

	// create vertex buffer
	update_vertex_buffer(create_block_vertices(), block_indices, &cube_vertex_array);
	update_vertex_buffer(sprite_vertices, sprite_indices, &sprite_vertex_array);
	update_vertex_buffer(create_circle_vertices(32), circle_indices, &circle_vertex_array);
	update_vertex_buffer(create_skybox_vertices(NUM_TESS), skybox_indices, &skybox_vertex_array);
	
	// sound
	init_sound();

	//
	enemy_list = {
		Enemy(&map, &crt, vec2(22, 3)),
		Enemy(&map, &crt, vec2(38, 6)),
		Enemy(&map, &crt, vec2(33, 6)),
		Enemy(&map, &crt, vec2(12, 3))
	};

	// texture
	TEX_SKYBOX = cg_create_texture(skybox_image_path, true);  if (!TEX_SKYBOX) return false;
	SPRITE_CRT = cg_create_texture(sprite_crt_run_image_path, true); if (!SPRITE_CRT) return false;
	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	engine->play2D(mp3_src_bgm, true);

	float tp, t = 0;
	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		tp = t;
		t = float(glfwGetTime());
		glfwPollEvents();	// polling and processing of events
		update(t-tp);			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
