#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GLFW/glfw3.h>
#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include "Material.h"
#include "Model.h"

#include "Skybox.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0, uniformFarPlane = 0;

glm::mat4 objectCoord;

Window mainWindow;

std::vector<Shader> shaderList;

Camera camera;

//Texture dirtTexture;

Material shinyMaterial;
Material dullMaterial;

Model sphere, propeller;

glm::mat4 model;

Texture planeTexture;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLuint skyboxLocation;

GLfloat angle =0.f, rollAngle = 0.f, pitchAngle = 0.f;
glm::quat Quat, rollQuat, pitchQuat;
glm::mat4 mat_cast;


// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

bool eulerCheck, quaterCheck;
glm::mat4 thirdPerson;
glm::vec3 offset;

bool fPerson = false, tPerson = false, freelook = true;


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void RenderScene()
{
	model = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 2.5f));
	model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));
	//model = glm::rotate(model, 3.14f, glm::vec3(0, 1, 0));
	
	model = model * mat_cast;
		//* mat_cast1 * mat_cast2;

	//if 3rd person is enabled -> assign camera to matrix
	thirdPerson = glm::translate(model, glm::vec3(camera.offset.x, camera.offset.y, camera.offset.z));

	objectCoord = glm::mat4();
	objectCoord = glm::rotate(objectCoord, glm::radians(180.f), glm::vec3(0, 1, 0));
	objectCoord = glm::translate(objectCoord, glm::vec3(0.f,-3.f,0.f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//get the uniform location from frag shader - samplercube
	planeTexture.UseTexture();
	//set 0 to the above
	sphere.RenderModel();
	
	glm::mat4 propModel = glm::mat4();
	propModel *= model;
	//propModel = glm::translate(propModel, glm::vec3())
	//propeller.RenderModel();
}



void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	//uniformModel = shaderList[0].GetModelLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	skyboxLocation = shaderList[0].GetSkyboxLocation();
	glUniform1i(skyboxLocation, 0);

	shaderList[0].SetTexture(1);
	shaderList[0].Validate();

	RenderScene();
}


void bind_keys(bool* keys, GLfloat deltaTime)
{
	//GLfloat velocity = deltaTime *;
	if (keys[GLFW_KEY_U]) {
		glfwSetInputMode(mainWindow.mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	//Euler
	if (eulerCheck) {
		if (keys[GLFW_KEY_UP])
		{
			if (rollAngle < 90.f) {
				rollAngle += 2.f;
			}
			else {
				rollAngle = 90.f;
			}
			//check for radians later
			mat_cast = glm::rotate(mat_cast, rollAngle, glm::vec3(0, 1, 0));
		}
		if (keys[GLFW_KEY_DOWN])
		{
			if (rollAngle < 90.f) {
				rollAngle -= 2.f;
			}
			else {
				rollAngle = 90.f;
			}
			//check for radians later
			mat_cast = glm::rotate(mat_cast, rollAngle, glm::vec3(0, 1, 0));
		}
		if (keys[GLFW_KEY_LEFT])
		{
			if (angle > -90.f) {
				angle -= 2.f;
			}
			else {
				angle = 90.f;
			}
			//check for radians later
			mat_cast = glm::rotate(mat_cast, angle, glm::vec3(1, 0, 0));
		}
		if (keys[GLFW_KEY_RIGHT])
		{
			if (angle > -90.f) {
				angle += 2.f;
			}
			else {
				angle = 90.f;
			}
			//check for radians later
			mat_cast = glm::rotate(mat_cast, angle, glm::vec3(1, 0, 0));
		}
		if (keys[GLFW_KEY_K])
		{
			if (pitchAngle > -90.f) {
				pitchAngle -= 2.f;
			}
			else {
				pitchAngle = 90.f;
			}
			//check for radians later
			mat_cast = glm::rotate(mat_cast, pitchAngle, glm::vec3(0, 0, 1));
		}
		if (keys[GLFW_KEY_L])
		{
			if (pitchAngle > -90.f) {
				pitchAngle += 2.f;
			}
			else {
				pitchAngle = 90.f;
			}
			//check for radians later
			mat_cast = glm::rotate(mat_cast, pitchAngle, glm::vec3(0, 0, 1));
		}
	}
	//quaternion
	else if(quaterCheck){
		//reference: https://stackoverflow.com/questions/5782658/extracting-yaw-from-a-quaternion

		GLfloat q0, q1, q2, q3, mag;

		//yaw
		if (keys[GLFW_KEY_UP])
		{
			if (rollAngle < 3.14f) {
				rollAngle += .001f;
			}
			else {
				rollAngle = 3.14f;
			}
			q0 = cos(rollAngle / 2); q1 = 0;  q2 = sin(rollAngle / 2); q3 = 0;
			//magnitude for normalization
			mag = sqrt(q0*q0 + q2 * q2);
			q0 /= mag;
			q2 /= mag;

			Quat *= glm::quat(q0, q1, q2, q3);
			mat_cast *= glm::mat4_cast(Quat);
		}

		if (keys[GLFW_KEY_DOWN])
		{
			if (rollAngle > -3.14f) {
				rollAngle -= 0.001f;
			}
			else {
				rollAngle = -3.14f;
			}
			q0 = cos(rollAngle / 2); q1 = 0;  q2 = sin(rollAngle / 2); q3 = 0;
			//calculating magnitude for normalizing
			mag = sqrt(q0*q0 + q2 * q2);
			q0 /= mag;
			q2 /= mag;

			Quat *= glm::quat(q0, q1, q2, q3);
			mat_cast *= glm::mat4_cast(Quat);
		}

		if (keys[GLFW_KEY_LEFT])
		{
			if (angle > -3.14f) {
				angle -= .001f;
			}
			else {
				angle = -3.14f;
			}
			q0 = cos(angle / 2), q1 = sin(angle / 2), q2 = 0, q3 = 0;
			//normalizing
			mag = sqrt(q0*q0 + q1 * q1);
			q0 /= mag;
			q1 /= mag;
			rollQuat *= glm::quat(q0, q1, q2, q3);
			mat_cast *= glm::mat4_cast(rollQuat);
		}

		if (keys[GLFW_KEY_RIGHT])
		{
			if (angle < 3.14f) {
				angle += .001f;
			}
			else {
				angle = 3.14f;
			}
			q0 = cos(angle / 2), q1 = sin(angle / 2), q2 = 0, q3 = 0;
			//normalizing
			mag = sqrt(q0*q0 + q1 * q1);
			q0 /= mag;
			q1 /= mag;
			rollQuat *= glm::quat(q0, q1, q2, q3);
			mat_cast *= glm::mat4_cast(rollQuat);
		}
		if (keys[GLFW_KEY_K])
		{
			if (pitchAngle > -3.14f) {
				pitchAngle -= .001f;
			}
			else {
				pitchAngle = -3.14f;
			}
			q0 = cos(pitchAngle / 2), q1 = 0, q2 = 0, q3 = sin(pitchAngle / 2);
			mag = sqrt(q0*q0 + q3 * q3);
			q0 /= mag;
			q3 /= mag;
			pitchQuat *= glm::quat(q0, q1, q2, q3);

			mat_cast *= glm::mat4_cast(pitchQuat);
		}
		if (keys[GLFW_KEY_L])
		{
			if (pitchAngle < 3.14f) {
				pitchAngle += .001f;
			}
			else {
				pitchAngle = 3.14f;
			}
			q0 = cos(pitchAngle / 2), q1 = 0, q2 = 0, q3 = sin(pitchAngle / 2);
			mag = sqrt(q0*q0 + q3 * q3);
			q0 /= mag;
			q3 /= mag;
			pitchQuat *= glm::quat(q0, q1, q2, q3);

			mat_cast *= glm::mat4_cast(pitchQuat);
		}
	}
}


/*
void EulerAngles(GLfloat rotAngle, GLfloat yawAngle, GLfloat pitchAngle) {

	mat_cast *= glm::rotate(mat_cast, glm::degrees(rotAngle), glm::vec3(1, 0, 0));
	mat_cast *= glm::rotate(mat_cast, glm::degrees(yawAngle), glm::vec3(0, 1, 0));
	mat_cast *= glm::rotate(mat_cast, glm::radians(pitchAngle), glm::vec3(0, 0, 1));
}


void Quaternions(GLfloat rotAngle, GLfloat yawAngle, GLfloat pitchAngle) {

	if (rotAngle != .0f) {
		GLfloat q0, q1, q2, q3, mag;
		q0 = cos(rotAngle / 2); q1 = sin(rotAngle / 2);  q2 = 0; q3 = 0;
		//magnitude for normalization
		mag = sqrt(q0*q0 + q1 * q1);
		q0 /= mag;
		q1 /= mag;

		Quat *= glm::quat(q0, q1, q2, q3);
	}
	if (yawAngle != .0f) {
		GLfloat q0, q1, q2, q3, mag;
		q0 = cos(yawAngle / 2); q1 = 0;  q2 = sin(yawAngle / 2); q3 = 0;
		//magnitude for normalization
		mag = sqrt(q0*q0 + q2 * q2);
		q0 /= mag;
		q2 /= mag;

		Quat *= glm::quat(q0, q1, q2, q3);
	}
	if (pitchAngle != .0f) {
		GLfloat q0, q1, q2, q3, mag;
		q0 = cos(pitchAngle / 2); q1 = 0;  q2 = 0; q3 = sin(pitchAngle / 2);
		//magnitude for normalization
		mag = sqrt(q0*q0 + q3 * q3);
		q0 /= mag;
		q3 /= mag;

		Quat *= glm::quat(q0, q1, q2, q3);
	}
	mat_cast *= glm::mat4_cast(Quat);
}
*/

int main() 
{
	mat_cast = glm::mat4();
	thirdPerson = glm::mat4();

	Quat = glm::quat();
	rollQuat = glm::quat();
	pitchQuat = glm::quat();

	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	sphere = Model();
	sphere.LoadModel("Models/piper_pa18.obj");

	//Load Textures
	planeTexture = Texture("Textures/plane_diffuse.jpg");
	planeTexture.LoadTextureA();

	propeller = Model();
	propeller.LoadModel("Plane/propeller.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/mp_cloud9/desertsky_rt.tga");
	skyboxFaces.push_back("Textures/mp_cloud9/desertsky_lf.tga");
	skyboxFaces.push_back("Textures/mp_cloud9/desertsky_up.tga");
	skyboxFaces.push_back("Textures/mp_cloud9/desertsky_dn.tga");
	skyboxFaces.push_back("Textures/mp_cloud9/desertsky_bk.tga");
	skyboxFaces.push_back("Textures/mp_cloud9/desertsky_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 330";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(mainWindow.mainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	//bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//main loop
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); 
		deltaTime = now - lastTime; 
		lastTime = now;

		glfwPollEvents();

		bind_keys(mainWindow.getsKeys(), deltaTime);

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			static int counter = 0;

			ImGui::Begin("Flight Deck");                          

			ImGui::Text("Controls");  
			ImGui::Checkbox("Euler", &eulerCheck);
			ImGui::Checkbox("Quaternions", &quaterCheck);

			/*
			ImGui::SliderFloat("rotate", &rollAngle, -3.14f, 3.14f);    
			ImGui::SliderFloat("pitch", &pitchAngle, -3.14f, 3.14f);
			ImGui::SliderFloat("yaw", &angle, -3.14f, 3.14f);
			*/

			ImGui::Text("Camera");
			ImGui::Checkbox("First Person", &fPerson);
			ImGui::Checkbox("Third Person", &tPerson);
			ImGui::Checkbox("Free Look", &freelook);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::End();
		}

		// Rendering
		if (fPerson) {
			RenderPass(glm::inverse(model), projection);
		}else if (tPerson) {
			//change here for third person
			RenderPass(thirdPerson, projection);
		}else if(freelook){
			RenderPass(camera.calculateViewMatrix(), projection);
		}

		/*
		if (eulerCheck) {
			EulerAngles(rollAngle, angle, pitchAngle);
		}
		else if (quaterCheck) {
			Quaternions(rollAngle, angle, pitchAngle);
		}
		*/

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		mainWindow.swapBuffers();
	}

	return 0;
}