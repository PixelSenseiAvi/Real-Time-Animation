#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

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
glm::mat4 mat_cast, mat_cast1, mat_cast2;
GLfloat roll =0., pitch =0., yaw =0.;

GLboolean ifroll, ifyaw, ifpitch;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

GLboolean rot;

/*
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, 
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}
*/


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void RenderScene()
{
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 2.0f, -2.5f));
	model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));
	
	model = model * mat_cast;
		//* mat_cast1 * mat_cast2;

	objectCoord = glm::mat4();
	objectCoord = glm::rotate(objectCoord, glm::radians(180.f), glm::vec3(0, 1, 0));
	objectCoord = glm::translate(objectCoord, glm::vec3(0.,-3.,0.));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//get the uniform location from frag shader - samplercube
	planeTexture.UseTexture();
	//set 0 to the above
	sphere.RenderModel();

}



void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	if (camera.firstPerson) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(0, -3, 0));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(180.f), glm::vec3(0., 1., 0.));

	}*/

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformModel = shaderList[0].GetModelLocation();
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
			mat_cast1 = glm::rotate(mat_cast, angle, glm::vec3(1, 0, 0));
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
			mat_cast1 = glm::rotate(mat_cast, angle, glm::vec3(1, 0, 0));
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
			mat_cast2 = glm::rotate(mat_cast, pitchAngle, glm::vec3(0, 0, 1));
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
			mat_cast2 = glm::rotate(mat_cast, pitchAngle, glm::vec3(0, 0, 1));
		}
	}
	//quaternion
	else {
		//try negating other axis w.r.t. to the axis of rotation

		if (keys[GLFW_KEY_UP])
		{
			if (rollAngle < 3.14f) {
				rollAngle += .1f;
			}else {
				rollAngle = .0f;
				//Quat *= glm::quat(0, 0, cos(3.14f), sin(3.14f));
			}
			Quat *= glm::quat(0, cos(rollAngle), 0, sin(rollAngle));

			mat_cast *= glm::mat4_cast(Quat);
		}

		if (keys[GLFW_KEY_DOWN])
		{
			if (rollAngle > -3.14f) {
				rollAngle -= 0.1f;
			}
			else {
				rollAngle = .0f;
				Quat *= glm::quat(0, 0, cos(-3.14), sin(-3.14));
			}
			Quat *= glm::quat(0, cos(rollAngle), 0, sin(rollAngle));

			mat_cast *= glm::mat4_cast(Quat);
		}

		if (keys[GLFW_KEY_LEFT])
		{

			if (angle > -3.14f) {
				angle -= .1f;
			}
			else {
				angle = -3.14f;
			}
			rollQuat *= glm::quat(cos(angle), 0, 0, sin(angle));

			mat_cast *= glm::mat4_cast(rollQuat);
		}

		if (keys[GLFW_KEY_RIGHT])
		{
			if (angle < 3.14f) {
				angle += .1f;
			}
			else {
				angle = 0.0f;
			}
			rollQuat *= glm::quat(cos(angle), 0, 0, sin(angle));

			mat_cast *= glm::mat4_cast(rollQuat);
		}
		if (keys[GLFW_KEY_K])
		{
			if (pitchAngle > -3.14f) {
				pitchAngle -= .1f;
			}
			else {
				pitchAngle = 0.f;
			}

			pitchQuat *= glm::quat(0, 0, cos(pitchAngle), sin(pitchAngle));

			mat_cast *= glm::mat4_cast(pitchQuat);
		}
		if (keys[GLFW_KEY_L])
		{
			if (pitchAngle < 3.14f) {
				pitchAngle += .1f;
			}
			else {
				pitchAngle = 0.f;
			}
			pitchQuat *= glm::quat(0, 0, cos(pitchAngle), sin(pitchAngle));

			mat_cast *= glm::mat4_cast(pitchQuat);
		}

		/*
		mat_cast = glm::mat4_cast(Quat);
		mat_cast1 = glm::mat4_cast(rollQuat);
		mat_cast2 = glm::mat4_cast(pitchQuat);
		*/
	}

}


int main() 
{
	rot = false;
	mat_cast = glm::mat4();
	mat_cast1 = glm::mat4();
	mat_cast2 = glm::mat4();

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

		//RenderPass(camera.calculateViewMatrix(), projection);

		if (camera.firstPerson) {
			RenderPass(glm::inverse(model)*objectCoord, projection);
		}else if (camera.thirdPerson) {
			//change here for the third person
			RenderPass(camera.calculateViewMatrix(), projection);
		}else {
			RenderPass(camera.calculateViewMatrix(), projection);
		}

		mainWindow.swapBuffers();
	}

	return 0;
}