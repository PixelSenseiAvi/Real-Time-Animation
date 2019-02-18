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
GLfloat roll =0.f, pitch =0.f, yaw =0.f;

GLboolean ifroll, ifyaw, ifpitch;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

bool rot;
glm::mat4 thirdPerson;
glm::vec3 offset;

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
	if (keys[GLFW_KEY_E]) {
		if (rot == false) { rot = true; }
		else { rot = false; }
	}

	//Euler
	if (rot == true) {
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
	else {
		//try negating other axis w.r.t. to the axis of rotation
		//reference: https://stackoverflow.com/questions/5782658/extracting-yaw-from-a-quaternion
		
		GLfloat q0, q1, q2, q3, mag;
		
		if (keys[GLFW_KEY_D]) {
			glfwSetInputMode(mainWindow.mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		//yaw
		if (keys[GLFW_KEY_UP])
		{
			if (rollAngle < 3.14f) {
				rollAngle += .001f;
			}else {
				rollAngle = 3.14f;
			}
			q0 = cos(rollAngle / 2); q1 = 0;  q2 = sin(rollAngle / 2); q3 = 0;
			//magnitude for normalization
			mag = sqrt(q0*q0 + q2*q2);
			q0 /= mag;
			q2 /= mag;
			
			Quat *= glm::quat(q0, q1,q2,q3);
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
			rollQuat *= glm::quat(cos(angle/2), 0, 0, sin(angle/2));
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
			rollQuat *= glm::quat(cos(angle/2), 0, 0, sin(angle/2));

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

			pitchQuat *= glm::quat(0, 0, cos(pitchAngle/2), sin(pitchAngle/2));

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
			pitchQuat *= glm::quat(0, 0, cos(pitchAngle/2), sin(pitchAngle/2));

			mat_cast *= glm::mat4_cast(pitchQuat);
		}
	}

}


int main() 
{
	rot = false;
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
	bool show_another_window = false;
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

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		

		//RenderPass(camera.calculateViewMatrix(), projection);

		if (camera.firstPerson) {
			RenderPass(glm::inverse(model), projection);
		}else if (camera.thirdPerson) {
			//change here for third person
			RenderPass(thirdPerson, projection);
		}else {
			RenderPass(camera.calculateViewMatrix(), projection);
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		mainWindow.swapBuffers();
	}

	return 0;
}