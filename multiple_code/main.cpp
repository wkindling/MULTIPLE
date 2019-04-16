#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL.h"
#pragma comment(lib,"SOIL.lib")
#include "gtc\type_ptr.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "object.h"
#include "AABB.h"

/*----------------------------���������б�--------------------------------------------------------*/
// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// ����ƶ��ص�����ԭ������
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// �����ֻص�����ԭ������
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// �������ƶ�
void do_movement();
//������
bool ray_aabb(glm::vec3 position, glm::vec3 forward, AABB aabbBox);
//������ص�����ԭ������
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


/*------------------------- ���������б�---------------------------------------------------------*/
const int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(-4.76269f, 45.0f, -241.855f));

bool shoot; //������
bool kill=false;  //���м��
bool destroy = false;
int score = -1;
float red_transfer = 0.0f;
int enemy_explode_ctrl = 0;
int table_explode_ctrl = 0;
int scene_explode_ctrl = 0;
bool enemy_explode_end = false;
bool table_explode_end = false;
bool scene_explode_end = false;
double enemy_speed = 0.01;
bool alive = false;
bool jump = false;
int jump_count=0;
bool game_over = false;

int main(int argc, char** argv)
{
/*------------------------- ����GLFW��������---------------------------------------------------------*/
	glfwInit();
	std::cout << "Thank you for playing my game:)" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,"MULTIPLE", NULL, NULL);
	
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
/*------------------------- ������պк����׼��---------------------------------------------------------*/
	//׼����Ϣ
	GLfloat haircross[] ={
		0.015f,0.0f,0.0f,
	   -0.015f,0.0f,0.0f,
		0.0f,-0.025f,0.0f,
		0.0f, 0.025f,0.0f
	};  
	//��պ���Ϣ
	GLfloat skyboxVertices[] = {
		// ����
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, -1.0f, -1.0f,	// B
		1.0f, -1.0f, -1.0f,		// C
		1.0f, -1.0f, -1.0f,		// C
		1.0f, 1.0f, -1.0f,		// D
		-1.0f, 1.0f, -1.0f,		// A
		// �����
		-1.0f, -1.0f, 1.0f,		// E
		-1.0f, -1.0f, -1.0f,	// B
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, 1.0f, -1.0f,		// A
		-1.0f, 1.0f, 1.0f,		// F
		-1.0f, -1.0f, 1.0f,		// E
		// �Ҳ���
		1.0f, -1.0f, -1.0f,		// C
		1.0f, -1.0f, 1.0f,		// G
		1.0f, 1.0f, 1.0f,		// H
		1.0f, 1.0f, 1.0f,		// H
		1.0f, 1.0f, -1.0f,		// D
		1.0f, -1.0f, -1.0f,		// C
		// ����
		-1.0f, -1.0f, 1.0f,  // E
		-1.0f, 1.0f, 1.0f,  // F
		1.0f, 1.0f, 1.0f,  // H
		1.0f, 1.0f, 1.0f,  // H
		1.0f, -1.0f, 1.0f,  // G
		-1.0f, -1.0f, 1.0f,  // E
		// ����
		-1.0f, 1.0f, -1.0f,  // A
		1.0f, 1.0f, -1.0f,  // D
		1.0f, 1.0f, 1.0f,  // H
		1.0f, 1.0f, 1.0f,  // H
		-1.0f, 1.0f, 1.0f,  // F
		-1.0f, 1.0f, -1.0f,  // A
		// ����
		-1.0f, -1.0f, -1.0f,  // B
		-1.0f, -1.0f, 1.0f,   // E
		1.0f, -1.0f, 1.0f,    // G
		1.0f, -1.0f, 1.0f,    // G
		1.0f, -1.0f, -1.0f,   // C
		-1.0f, -1.0f, -1.0f,  // B
	};
	//Ϊ׼�ǻ�������VAO��VBO
	GLuint crossVAOId, crossVBOId;
	glGenVertexArrays(1, &crossVAOId);
	glGenBuffers(1, &crossVBOId);
	glBindVertexArray(crossVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, crossVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(haircross), haircross, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	// Ϊ��պл�������VAO��VBO
	GLuint skyBoxVAOId, skyBoxVBOId;
	glGenVertexArrays(1, &skyBoxVAOId);
	glGenBuffers(1, &skyBoxVBOId);
	glBindVertexArray(skyBoxVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	// ������պ�����
	std::vector<const char*> faces;
	faces.push_back("skybox/sky_rt.jpg");
	faces.push_back("skybox/sky_lf.jpg");
	faces.push_back("skybox/sky_up.jpg");
	faces.push_back("skybox/sky_dn.jpg");
	faces.push_back("skybox/sky_bk.jpg");
	faces.push_back("skybox/sky_ft.jpg");
	GLuint skyBoxTextId = Model::loadSkyBoxTexture(faces);	
/*------------------------- ��ϷԪ�ص�ģ�������shader����---------------------------------------------------------*/
	Shader skyBoxShader("shader/skybox.vertex", "shader/skybox.frag");
	Shader crossShader("shader/haircross.vertex", "shader/haircross.frag");
	Shader scene_shader("shader/light_model.vertex", "shader/light_model.frag"); 
	Shader red_shader("shader/red.vertex", "shader/red.frag");
	//������ģ�͵ĵ���
	Model Main_scene;
	Main_scene.loadModel("scene/megaforce.obj", 1.0f);
	Model monster;
	monster.loadModel("monster/monster.obj", 1.5f);
	std::vector<Vertex>::iterator i;
	for (i = monster.meshes[0].vertData.begin(); i != monster.meshes[0].vertData.end(); i++)
	{
		(*i).position += glm::vec3(0, 6, 7);
	}
	
	Model buffer; //����ģ�ͣ�����ˢ�µ���
	glm::vec3 scene_scale = glm::vec3(30.0f, 23.0f, 30.0f);
	Scene table(Main_scene.meshes[12], scene_scale);
	camera.detected_list.push_back(table.aabbBox);
	Enemy enemy;
/*------------------------- ��ʼ��Ϸ��ѭ��---------------------------------------------------------*/
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//clearColor��ɫ��ͬʱҲ��׼����ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//���Ȼ���׼�ǣ���Ϊ׼��ʼ���ڻ���������
		crossShader.use();
		glBindVertexArray(crossVAOId);
		glDrawArrays(GL_LINES, 0, 4); 
		
		//������պ�
		glDepthMask(GL_FALSE); // ��ֹд����Ȼ�����
		skyBoxShader.use();
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 1000.0f); // ͶӰ����
		glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix())); // �ӱ任���� �Ƴ�translate����
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.programId, "projection"),1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.programId, "view"),1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(skyBoxVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextId); // ע��󶨵�CUBE_MAP
		glUniform1i(glGetUniformLocation(skyBoxShader.programId, "skybox"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (!alive)
		{
			buffer = monster;
			enemy = Enemy(&buffer);
			alive = true;
			enemy_explode_end = false;
			enemy_explode_ctrl = 0;
			kill = false;
			enemy.speed = enemy_speed;
			enemy_speed += 0.002;
			score++;
		}

		if (shoot)
		{ 
			if (!kill) kill = ray_aabb(camera.position, camera.forward, enemy.aabbBox);
			if (!destroy) destroy = ray_aabb(camera.position, camera.forward, table.aabbBox);
		}

		if (destroy)
		{
			table.hp=table.hp-10;
			if (table.hp < 0) table.hp = 0;
			destroy = false;
		}

		//���е��˺���˵�������Ч�����˻�������������ը��
		if (kill& enemy_explode_ctrl <400)
		{
			if (enemy_explode_ctrl < 100)
			{
				for (std::vector<Vertex>::iterator i = enemy.pModel->meshes[0].vertData.begin(); i != enemy.pModel->meshes[0].vertData.end(); i += 3)
				{
					(*i).position -= (*i).normal / 230.0f;
					(*i).position.y -= 0.015;
					(*(i + 1)).position -= (*(i)).normal / 230.0f;
					(*(i + 1)).position.y -= 0.015;
					(*(i + 2)).position -= (*(i)).normal / 230.0f;
					(*(i + 2)).position.y -= 0.015;
				}
			}
			else
			{
				for (std::vector<Vertex>::iterator i = enemy.pModel->meshes[0].vertData.begin(); i != enemy.pModel->meshes[0].vertData.end(); i += 3)
				{
					(*i).position += (*i).normal / 250.0f;
					(*i).position.y -= 0.015;
					(*(i + 1)).position += (*(i)).normal / 250.0f;
					(*(i + 1)).position.y -= 0.015;
					(*(i + 2)).position += (*(i)).normal / 250.0f;
					(*(i + 2)).position.y -= 0.015;
				}
			}
			enemy_explode_ctrl++;
		}
		else if (enemy_explode_ctrl >= 400) { enemy_explode_end = true; alive = false; }
		
		//�ƻ�����
		if (table.hp==0 &&table_explode_ctrl < 400)
		{
			for (std::vector<Vertex>::iterator i = table.pMesh.vertData.begin(); i != table.pMesh.vertData.end(); i += 3)
			{
				(*i).position += (*i).normal / 30.0f;
				(*i).position.y -= 0.015;
				(*(i + 1)).position += (*(i)).normal /30.0f;
				(*(i + 1)).position.y -= 0.015;
				(*(i + 2)).position += (*(i)).normal / 30.0f;
				(*(i + 2)).position.y -= 0.015;
			}
			table_explode_ctrl++;
		}
		else if (table_explode_ctrl >= 400) 
		{ 
			table_explode_end = true; 
			camera.detected_list.clear();
		}

		//���������� �����޵У�
		if (table_explode_end&& scene_explode_ctrl<800)
		{
			for (unsigned int j = 0; j < 18; j++)
			{
				if (j != 12)
				{
					for (std::vector<Vertex>::iterator i = Main_scene.meshes[j].vertData.begin(); i != Main_scene.meshes[j].vertData.end(); i += 3)
					{
						(*i).position += (*i).normal / 40.0f;
						(*i).position.y -= 0.06;
						(*(i + 1)).position += (*(i)).normal / 40.0f;
						(*(i + 1)).position.y -= 0.06;
						(*(i + 2)).position += (*(i)).normal / 40.0f;
						(*(i + 2)).position.y -= 0.06;
					}
				}
			}
			scene_explode_ctrl++;
		}
		else if (scene_explode_ctrl == 800) { scene_explode_end = true; game_over = true; scene_explode_ctrl = 801; }

		//ʵ���������Ծ
		if (jump&&jump_count!=482)
		{
			if (jump_count<=240) camera.position.y += (0.30);
			if (jump_count>240) camera.position.y -= (0.30);
			camera.aabbBox.max_position = camera.position + glm::vec3(10, 0, 10);
			camera.aabbBox.min_position = camera.position - glm::vec3(10, 45, 10);
			jump_count++;
		}
		else if (jump_count ==482) { jump = false; jump_count = 0; }
	 
		//����δ������ʱ���������
		if (!kill)
		{
			table.hp = table.hp - 0.02;
			if (table.hp < 0) table.hp = 0;
			enemy.random_move();
		}

		// ���Ƴ���
		scene_shader.use();
		glDepthMask(GL_TRUE);	
		//���ù���
		glm::vec3 lampPos(0.0f, 10.0f, 0.0f);
		GLint lightAmbientLoc = glGetUniformLocation(scene_shader.programId, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(scene_shader.programId, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(scene_shader.programId, "light.specular");
		GLint lightPosLoc = glGetUniformLocation(scene_shader.programId, "light.position");
		glUniform3f(lightAmbientLoc, 0.8f,0.8f,0.8f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
		glUniformMatrix4fv(glGetUniformLocation(scene_shader.programId, "projection"),1, GL_FALSE, glm::value_ptr(projection));
		view = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(scene_shader.programId, "view"),1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(30.0f, 23.0f, 30.0f)); // �ʵ���Сģ��
		glUniformMatrix4fv(glGetUniformLocation(scene_shader.programId, "model"),1, GL_FALSE, glm::value_ptr(model));
		
		if (!scene_explode_end)
		{
			for (unsigned int i = 0; i < Main_scene.meshes.size(); i++)
			{
				if (i != 12) Main_scene.meshes[i].draw(scene_shader);
			}
		}
		//�ڵ���������Ч�������֮ǰʼ�ջ��Ƶ���
		if (!enemy_explode_end && table.hp>0) enemy.pModel->draw(scene_shader);
         
		red_shader.use();
		glDepthMask(GL_TRUE);
		GLint lightAmbientLoc_red = glGetUniformLocation(red_shader.programId, "light.ambient");
		GLint lightDiffuseLoc_red = glGetUniformLocation(red_shader.programId, "light.diffuse");
		GLint lightSpecularLoc_red = glGetUniformLocation(red_shader.programId, "light.specular");
		GLint lightPosLoc_red = glGetUniformLocation(red_shader.programId, "light.position");
		GLfloat red = glGetUniformLocation(red_shader.programId, "light.red");
		glUniform3f(lightAmbientLoc_red, 0.8, 0.8f, 0.8f);
		glUniform3f(lightDiffuseLoc_red, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightSpecularLoc_red, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc_red, lampPos.x, lampPos.y, lampPos.z);
		red_transfer = (100.0f - table.hp)*0.01;
		glUniform1f(red, red_transfer);
		glUniformMatrix4fv(glGetUniformLocation(red_shader.programId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(red_shader.programId, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(red_shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		if (!table_explode_end) table.pMesh.draw(red_shader);

		glBindVertexArray(0);
		glUseProgram(0);

		shoot = false;//ÿ��ѭ�������������״̬

		if (game_over)
		{
			std::cout << "Game Over!"<<std::endl<< "Your score is " << score << "!!!" << std::endl;
			std::cout << "Have a nice day!" << std::endl;
			game_over = false;
		}

		glfwSwapBuffers(window); // ��������
	}
	glDeleteVertexArrays(1, &skyBoxVAOId);
	glDeleteBuffers(1, &skyBoxVBOId);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true;
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // �״�����ƶ�
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false; 
	}

	GLfloat xoffset = lastX-xpos;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}
// ����������ദ�������ֿ���
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}
// ����������ദ����̿���
void do_movement()
{
	if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_SPACE])
	{
		//if (!jump)camera.handleKeyPress(JUMP, deltaTime);
		 jump = true;
	}
}

bool ray_aabb(glm::vec3 position, glm::vec3 forward, AABB aabbBox)
{
	double tmin = 0;
	double tmax = 10000000;
	double t1, t2;
	forward = glm::normalize(forward);

	if (abs(forward.x) < 0.00001)
	{
		if (position.x<aabbBox.min_position.x || position.x>aabbBox.max_position.x) return false;
	}
	else
	{
		double ood = 1.0 / forward.x;
		t1 = (aabbBox.min_position.x - position.x)*ood;
		t2 = (aabbBox.max_position.x - position.x)*ood;
		if (t1 > t2)
		{
			double temp = t1;
			t1 = t2;
			t2 = temp;
		}
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;
		if (tmin > tmax) return false;
	}

	if (abs(forward.y) < 0.00001)
	{
		if (position.y<aabbBox.min_position.y || position.y>aabbBox.max_position.y) return false;
	}
	else
	{
		double ood = 1.0 / forward.y;
		t1 = (aabbBox.min_position.y - position.y)*ood;
		t2 = (aabbBox.max_position.y - position.y)*ood;
		if (t1 > t2)
		{
			double temp = t1;
			t1 = t2;
			t2 = temp;
		}
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;
		if (tmin > tmax) return false;
	}

	if (abs(forward.z) < 0.00001)
	{
		if (position.z<aabbBox.min_position.z || position.z>aabbBox.max_position.z) return false;
	}
	else
	{
		double ood = 1.0 / forward.z;
		t1 = (aabbBox.min_position.z - position.z)*ood;
		t2 = (aabbBox.max_position.z - position.z)*ood;
		if (t1 > t2)
		{
			double temp = t1;
			t1 = t2;
			t2 = temp;
		}
		if (t1 > tmin) tmin = t1;
		if (t2 < tmax) tmax = t2;
		if (tmin > tmax) return false;
	}

	return true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			shoot = true;
			break;
		default:
			return;
		}
	return;
}