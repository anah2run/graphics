#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "map.h"
#include "character.h"
#include "gun.h"

//*************************************
// global constants
static const char*	window_name = "cgbase - trackball";
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

	float	fovy = PI/6.0f; // must be in radian
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
GLuint	vertex_array = 0;	// ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_block_vertices;	// host-side vertices
Map map(new_map);
Character	crt(&map,vec2(1,3));
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
				bp->hit(it->prop->block_dmg);
			}
		}
		for (std::list<Enemy>::iterator eit = enemy_list.begin(); eit != enemy_list.end(); eit++) {
			if (eit->hitbox.collid(it->position - eit->position)) {
				if (eit->hit(it->prop->dmg, it->direction))
				{
					destroy = true; //적들이 총알에 맞았을 때
					break;
				}
			}
		}
		if (it->life <= 0 || destroy) {
			bullet_instances.erase(it);
		}
	}
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		if (it->hitbox.collid(it->position - crt.position)) {
			crt.hit(it->damage, it->position - crt.position);
		}
		it->update(t, true);
		if (!(it->alive)) {
			enemy_list.erase(it);
		}
	}
	
	cam.eye = vec3(crt.position.x, 8, 20);
	cam.at = vec3((crt.position.x + cam.at.x) / 2,7,0);
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
}

void render()
{
	//printf("player: %f %f\n", crt.position.x, crt.position.y);

	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// build bullets
	std::list<Bullet>::iterator it;
	for (it = bullet_instances.begin(); it != bullet_instances.end(); it++) {
		mat4 model_matrix = mat4::translate(it->position.x, it->position.y, 0) * mat4::scale(0.2f,0.2f,0.2f);

		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 3*4*6, GL_UNSIGNED_INT, nullptr);
	}

	GLint uloc;
	mat4 model_matrix;
	// build the model matrix for map
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
					scale_matrix = mat4::scale(1,1,float(bp->hp) / bp->prop->max_hp);
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

	// build character model
	if (crt.invinc_t <= 0 || int(crt.invinc_t*10) % 2 == 0) {
		model_matrix = mat4::translate(crt.position.x - crt.hitbox.width() / 2, crt.position.y - crt.hitbox.height() / 2, 0) * mat4::scale(crt.hitbox.width(), crt.hitbox.height(),0.1f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 3 * 4 * 6, GL_UNSIGNED_INT, nullptr);
	}
	for (std::list<Enemy>::iterator it = enemy_list.begin(); it != enemy_list.end(); it++) {
		model_matrix = mat4::translate(it->position.x - it->hitbox.width() / 2, it->position.y - it->hitbox.height() / 2, 0) * mat4::scale(it->hitbox.width(), it->hitbox.height(), 0.1f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
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
		else if (key == GLFW_KEY_W || key == GLFW_KEY_SPACE) crt.jump();
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

void update_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	std::vector<uint> indices = block_indices;
	
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// generation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests	

	// define the position of four corner vertices
	unit_block_vertices = std::move(create_block_vertices());

	// create vertex buffer
	update_vertex_buffer(unit_block_vertices);

	glBindVertexArray(vertex_array);

	enemy_list.push_back(Enemy(&map, &crt, vec2(18, 3)));
	enemy_list.push_back(Enemy(&map, &crt, vec2(34, 6)));
	enemy_list.push_back(Enemy(&map, &crt, vec2(29, 6)));
	enemy_list.push_back(Enemy(&map, &crt, vec2(8, 3)));
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
