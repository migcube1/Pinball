/*
	UNIVERSIDAD NACIONAL AUTÓNOMA DE MÉXICO
	FACULTAD DE INGENIERÍA
	LABORATORIO DE COMPUTACIÓN GRÁFICA E INTERACCIÓN HUMANO-COMPUTADORA
	PROYECTO FINAL "PINBALL"
	SEMESTRE 2021-1
	ALUMNOS: 
	 - LEYVA BEJARANO MIGUEL ANGEL
	 - VELASCO ARCINIEGA ERNESTO
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"
#include "Sphere.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Declaración de la cámara
Camera camera;

// Delaración de las Texturas
Texture plainTexture;
Texture plainDark;
Texture Portada;
Texture Isaac;
Texture Necromorph;

//Declaración de materiales
Material Material_brillante;
Material Material_opaco;

//Declaración de luz direccional
DirectionalLight mainLight;

//Declaración de las luces tipo spotlight y pointlights
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Declaración de modelos
Model MuroEsquinas;
Model Cono;
Model Cilindro;
Model Luces;
Model Barra_Lateral;
Model Barra_Inferior;


// Declaración de la canica
Sphere canica = Sphere(1, 20, 20);

//Declaración del skybox
Skybox skybox;

//Declaración de la variables de tiempo
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//cálculo del promedio de las normales para sombreado de Phong
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

void CreateObjects() 
{
	unsigned int floorIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	GLfloat floorVertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, 0.0f,  1.0f,		0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		-1.0f, 0.0f, -1.0f,		0.0f,  1.0f,	0.0f, -1.0f, 0.0f,
		 1.0f, 0.0f, -1.0f,		1.0f,  1.0f,	0.0f, -1.0f, 0.0f,
		 1.0f, 0.0f,  1.0f,		1.0f,  0.0f,	0.0f, -1.0f, 0.0f
	};

	///calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj1);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f,   0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,   0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,   0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,   0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,   0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};
	
	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CrearPrisma()
{
	unsigned int prismaT_indices[] = {
		// front
		0, 1, 2,
		0, 2, 3,
		// left
		4, 5, 6,
		4, 6, 7,
		// back
		8, 9, 10,
		8, 10, 11,
		// bottom
		12, 13, 14,
		// top
		15, 16, 17,
	};
	// average normals
	GLfloat prismaT_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f,  0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f,   0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,
		// left
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,   0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		// back
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		-1.0f,	0.0f,	1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f,	0.0f,		-1.0f,	0.0f,	1.0f,
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		-1.0f,	0.0f,	1.0f,
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		-1.0f,	0.0f,	1.0f,
		// bottom
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *prismaT = new Mesh();
	prismaT->CreateMesh(prismaT_vertices, prismaT_indices, 144, 24);
	meshList.push_back(prismaT);

}

void CrearPersonajes()
{
	unsigned int cubos_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	// average normals
	GLfloat head_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.7226f,0.7558f,	0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.5937f,0.7558f,	0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.5937f,1.0f,		0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.7226f,1.0f,		0.0f,	0.0f,	-1.0f,
		// right
		0.5f, -0.5f,  0.5f,	    0.8632f,0.7558f,	-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.9941f,0.7558f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.9941f,1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.8632f,1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0546f,0.7558f,	0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.1836f,0.7558f,	0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.1836f,1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0546f,1.0f,		0.0f,	0.0f,	1.0f,
		// left
		-0.5f, -0.5f,  -0.5f,	0.3242f,0.7558f,	1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.4531f,0.7558f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.4531f,1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.3242f,1.0f,		1.0f,	0.0f,	0.0f,
		// bottom
		-0.5f, -0.5f,  0.5f,	0.0546f,0.7558f,	0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.1836f,0.7558f,	0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.1836f,1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0546f,1.0f,		0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  0.5f,	0.0546f,1.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.1836f,1.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.1836f,0.7558f,	0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0546f,0.7558f,	0.0f,	-1.0f,	0.0f,
	};
	GLfloat body_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.5917f,0.3789f,	0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.7246f,0.3789f,	0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.7246f,0.7480f,	0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.5917f,0.7480f,	0.0f,	0.0f,	-1.0f,
		// right
		0.5f, -0.5f,  0.5f,	    0.8808f,0.7558f,	-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.9941f,0.7558f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.9941f,1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.8808f,1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0507f,0.3808f,	0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.1836f,0.3808f,	0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.1836f,0.7480f,	0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0507f,0.7480f,	0.0f,	0.0f,	1.0f,
		// left
		-0.5f, -0.5f,  -0.5f,	0.3242f,0.7558f,	1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.4355f,0.7558f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.4355f,1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.3242f,1.0f,		1.0f,	0.0f,	0.0f,
		// bottom
		-0.5f, -0.5f,  0.5f,	0.0546f,0.7558f,	0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.1836f,0.7558f,	0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.1836f,1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0546f,1.0f,		0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  0.5f,	0.0546f,1.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.1836f,1.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.1836f,0.7558f,	0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0546f,0.7558f,	0.0f,	-1.0f,	0.0f,
	};

	GLfloat armL_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.5429f,0.3808f,	0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.5898f,0.3808f,	0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.5898f,0.7480f,	0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.5429f,0.7480f,	0.0f,	0.0f,	-1.0f,
		// right
		0.5f, -0.5f,  0.5f,	    0.8984f,0.3808f,	-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.9589f,0.3808f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.9589f,0.7480f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.8984f,0.7480f,	-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.1894f,0.3808f,	0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.2343f,0.3808f,	0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.2343f,0.7441f,	0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.1894f,0.7441f,	0.0f,	0.0f,	1.0f,
		// left
		-0.5f, -0.5f,  -0.5f,	0.3242f,0.7558f,	1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.4355f,0.7558f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.4355f,1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.3242f,1.0f,		1.0f,	0.0f,	0.0f,
		// bottom
		-0.5f, -0.5f,  0.5f,	0.5429f,0.3808f,	0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.5898f,0.3808f,	0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.5898f,0.4335f,	0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.5429f,0.4335f,	0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  0.5f,	0.0546f,1.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.1836f,1.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.1836f,0.7558f,	0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0546f,0.7558f,	0.0f,	-1.0f,	0.0f,
	};

	GLfloat armR_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.7285f,0.3808f,	0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.7734f,0.3808f,	0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.7734f,0.7460f,	0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.7285f,0.7460f,	0.0f,	0.0f,	-1.0f,
		// right
		0.5f, -0.5f,  0.5f,	    0.3593f,0.3808f,	-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.4179f,0.3808f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.4179f,0.7480f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.3593f,0.7480f,	-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.1894f,0.3808f,	0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.2343f,0.3808f,	0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.2343f,0.7441f,	0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.1894f,0.7441f,	0.0f,	0.0f,	1.0f,
		// left
		-0.5f, -0.5f,  -0.5f,	0.3572f,0.3808f,	1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.4179f,0.3808f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.4179f,0.7480f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.3572f,0.7480f,	1.0f,	0.0f,	0.0f,
		// bottom
		-0.5f, -0.5f,  0.5f,	0.5898f,0.3808f,	0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.5429f,0.3808f,	0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.5429f,0.4335f,	0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.5898f,0.4335f,	0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  0.5f,	0.0546f,1.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.1836f,1.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.1836f,0.7558f,	0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0546f,0.7558f,	0.0f,	-1.0f,	0.0f,
	};

	GLfloat legL_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.5937f,0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.6562f,0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.6562f,0.3730f,	0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.5917f,0.3730f,	0.0f,	0.0f,	-1.0f,
		// right
		0.5f, -0.5f,  0.5f,	    0.8984f,0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.9589f,0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.9589f,0.3730f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.8984f,0.3730f,	-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0546f,0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.1152f,0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.1152f,0.3730f,	0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0546f,0.3730f,	0.0f,	0.0f,	1.0f,
		// left
		-0.5f, -0.5f,  -0.5f,	0.3572f,0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.4179f,0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.4179f,0.3730f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.3572f,0.3730f,	1.0f,	0.0f,	0.0f,
		// bottom
		-0.5f, -0.5f,  0.5f,	0.3572f,0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.4179f,0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.4179f,0.0293f,	0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.3572f,0.0293f,	0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  0.5f,	0.3572f,0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.4179f,0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.4179f,0.0293f,	0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.3572f,0.0293f,	0.0f,	-1.0f,	0.0f,
	};

	GLfloat legR_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.6562f,0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.7207f,0.0f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.7207f,0.3730f,	0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.6562f,0.3730f,	0.0f,	0.0f,	-1.0f,
		// right
		0.5f, -0.5f,  0.5f,	    0.8984f,0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.9589f,0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.9589f,0.3730f,	-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.8984f,0.3730f,	-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.1152f,0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.1816f,0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.1816f,0.3730f,	0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.1152f,0.3730f,	0.0f,	0.0f,	1.0f,
		// left
		-0.5f, -0.5f,  -0.5f,	0.3572f,0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.4179f,0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.4179f,0.3730f,	1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.3572f,0.3730f,	1.0f,	0.0f,	0.0f,
		// bottom
		-0.5f, -0.5f,  0.5f,	0.3572f,0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.4179f,0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.4179f,0.0293f,	0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.3572f,0.0293f,	0.0f,	1.0f,	0.0f,
		//UP
		 -0.5f, 0.5f,  0.5f,	0.3572f,0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.4179f,0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.4179f,0.0293f,	0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.3572f,0.0293f,	0.0f,	-1.0f,	0.0f,
	};

	Mesh *head = new Mesh();
	head->CreateMesh(head_vertices, cubos_indices, 192, 36);
	meshList.push_back(head);

	Mesh *body = new Mesh();
	body->CreateMesh(body_vertices, cubos_indices, 192, 36);
	meshList.push_back(body);

	Mesh *armL = new Mesh();
	armL->CreateMesh(armL_vertices, cubos_indices, 192, 36);
	meshList.push_back(armL);

	Mesh *armR = new Mesh();
	armR->CreateMesh(armR_vertices, cubos_indices, 192, 36);
	meshList.push_back(armR); 
	
	Mesh *legL = new Mesh();
	legL->CreateMesh(legL_vertices, cubos_indices, 192, 36);
	meshList.push_back(legL);

	Mesh *legR = new Mesh();
	legR->CreateMesh(legR_vertices, cubos_indices, 192, 36);
	meshList.push_back(legR);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() 
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CrearPersonajes();
	CrearPrisma();
	CreateShaders();

	//Contador para las luces
	int iCount = 2;
	
	canica.init();
	canica.load();

/*----------------------------POS Y CONFIG DE LA CAMARA----------------------------------*/
							//pos					Up							Yaw		Pitch  Mspeed Tspeed
	camera = Camera(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 7.0f, 0.7f);

/*------------------------------------TEXTURAS-------------------------------------------*/


	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	plainDark = Texture("Textures/plainDark.png");
	plainDark.LoadTextureA();

	Portada = Texture("Textures/portadaJuego.png");
	Portada.LoadTextureA();

	Isaac = Texture("Textures/Isaac.tga");
	Isaac.LoadTextureA();

	Necromorph = Texture("Textures/Necromorph.tga");
	Necromorph.LoadTextureA();

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

/*------------------------------------MODELOS--------------------------------------------*/
	MuroEsquinas = Model();
	MuroEsquinas.LoadModel("Models/muroEsquina.obj");

	Cono = Model();
	Cono.LoadModel("Models/cono.obj");

	Cilindro = Model();
	Cilindro.LoadModel("Models/cilindro.obj");

	Luces = Model();
	Luces.LoadModel("Models/luces.obj");

	Barra_Lateral = Model();
	Barra_Lateral.LoadModel("Models/barra_lateral.obj");

	Barra_Inferior = Model();
	Barra_Inferior.LoadModel("Models/barra_inferior.obj");



/*---------------------------------------LUCES--------------------------------------------*/

	//Posición de las verdes y rojas del lado izquierdo
	glm::vec3 posLucesGR_Left[] = {
		glm::vec3(-7.0f, 1.0f, 13.0f),		//R
		glm::vec3(-9.0f, 1.0f, 11.0f),		//V
		glm::vec3(-11.0f, 1.0f, 9.0f),		//R
		glm::vec3(-13.0f, 1.25f, 7.0f),		//V
		glm::vec3(-13.0f, 1.5f, 3.0f),		//R
		glm::vec3(-13.0f, 1.75f, -1.0f),	//V	
	};

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.5f, 0.5f,
								0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	//pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,   //Color
	//							0.0f, 1.0f,		    //Intensidad
	//							0.0f, 2.0f, 12.0f,//2.0f, 1.5f,1.5f,    //Posición
	//							0.3f, 0.2f, 0.1f);  // con, lin, exp
	//pointLightCount++;

	
	unsigned int spotLightCount = 0;

	//Luz de Paletas inferiores
	spotLights[0] = SpotLight(1.0f, 0.0f, 1.0f,	//Color ROSA
		0.0f, 2.0f,								//Intensity
		0.0f, 2.0f, 16.0f,						//Pos
		0.0f, 0.0f, -1.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;


	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,	//Color ROJO
		0.0f, 2.0f,								//Intensity
		-7.0f, 1.0f, 13.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,	//Color VERDE
		0.0f, 2.0f,								//Intensity
		-9.0f, 1.0f, 11.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	//linterna
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,	//Color
		0.0f, 2.0f,								//Intensity
		0.0f, 0.0f, 0.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;



/*---------------------------------------SKYBOX--------------------------------------------*/

	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	std::vector<std::string> skyboxFaces;
	/*skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");*/

	skyboxFaces.push_back("Textures/Skybox/Space_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/Space_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/Space_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/Space_up.tga");
	skyboxFaces.push_back("Textures/Skybox/Space_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/Space_ft.tga");


	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	movCoche = 0.0f;
	movOffset = 0.5f;

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		lastTime = now;
		if(movCoche < 5.0f)
		{
			movCoche += movOffset * deltaTime;
		}

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Cargamos el skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		//Asociamos la cámara con la luz de la linterna
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[3].SetFlash(lowerLight, camera.getCameraDirection());

		//Cargamos la luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//Control de las luces verdes y rojas lado izquierdo
		if (iCount > 5) { iCount = 0; }
		spotLights[1].SetPos(posLucesGR_Left[iCount]);
		iCount += 1;
		spotLights[2].SetPos(posLucesGR_Left[iCount]);
		iCount += 1;

		//Pender y apagar la linterna (P)
		if (mainWindow.getOnOff() == 1.0) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		/*-------------------------------------------------------------------------------------------*/
		/*---------------------------------------DIBUJADO--------------------------------------------*/
		/*-------------------------------------------------------------------------------------------*/
		glm::mat4 model(1.0);
		glm::mat4 modelRot(1.0);

		/*---------------------------------------Plano--------------------------------------------*/
		//Plano base
		model = glm::mat4(1.0);
		modelRot = model = glm::rotate(model, 5 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Portada.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();
		
		/*---------------------------------------MUROS--------------------------------------------*/
		//Perímetros

		//Right
		model = modelRot;
		model = glm::translate(model, glm::vec3(15.0f, 1.5f, 5.0f));
		model = glm::scale(model, glm::vec3(0.25f, 3.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		//Left
		model = modelRot;
		model = glm::translate(model, glm::vec3(-15.0f, 1.5f, 5.0f));
		model = glm::scale(model, glm::vec3(0.25f, 3.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		//Top
		model = modelRot;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, -20.0f));
		model = glm::scale(model, glm::vec3(10.0f, 3.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		//Down
		model = modelRot;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 20.0f));
		model = glm::scale(model, glm::vec3(30.0f, 3.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		//Muros internos
		model = modelRot;
		model = glm::translate(model, glm::vec3(12.0f, 1.5f, 4.5f));
		model = glm::scale(model, glm::vec3(0.25f, 3.0f, 24.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		//Triángulo izq
		model = modelRot;
		model = glm::translate(model, glm::vec3(-10.375f, 1.5f, 11.5f));
		model = glm::scale(model, glm::vec3(9.25f, 3.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[8]->RenderMesh();
		//Triángulo der
		model = modelRot;
		model = glm::translate(model, glm::vec3(7.375f, 1.5f, 11.5f));
		model = glm::scale(model, glm::vec3(-9.25f, 3.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[8]->RenderMesh();

		//MuroEsquinas superior
		model = modelRot;
		model = glm::translate(model, glm::vec3(10.0f, 1.5f, -15.0f));
		model = glm::scale(model, glm::vec3(10.0f, 3.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//plainDark.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		MuroEsquinas.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-10.0f, 1.5f, -15.0f));
		model = glm::scale(model, glm::vec3(-10.0f, 3.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//plainDark.UseTexture();
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		MuroEsquinas.RenderModel();

		/*---------------------------------------LUCES--------------------------------------------*/

		// Luces Izquierdas
		model = modelRot;
		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 13.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-9.0f, 0.0f, 11.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-11.0f, 0.0f, 9.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-13.0f, 0.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-13.0f, 0.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-13.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

	    // Luces Derechas

		model = modelRot;
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 13.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 11.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(8.0f, 0.0f, 9.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();



		/*---------------------------------------PERSONAJES--------------------------------------------*/

		//Head Isaac
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isaac.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		//Body Isaac
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 8.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isaac.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//ArmL Isaac
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.7f, 8.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isaac.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		//ArmR Isaac
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.7f, 8.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isaac.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		//LegL Isaac
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.25f, 6.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isaac.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();
		//LegR Isaac
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.25f, 6.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Isaac.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();

		//Head Necromorph
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Necromorph.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		//Body Necromorph
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, 8.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Necromorph.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//ArmL Necromorph
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.7f, 8.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Necromorph.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		//ArmR Necromorph
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.3f, 8.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Necromorph.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		//LegL Necromorph
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.75f, 6.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Necromorph.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMesh();
		//LegR Necromorph
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.25f, 6.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Necromorph.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[7]->RenderMesh();


		/*---------------------------------------CANICA--------------------------------------------*/

		model = modelRot;
		model = glm::translate(model, glm::vec3(13.5f, 1.0f, 15.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		canica.render(); //Renderiza esfera


		/*---------------------------------------BARRAS--------------------------------------------*/

		// Barra lateral izquierda
		model = modelRot;
		model = glm::translate(model, glm::vec3(-10.5f, 1.0f, 6.0f));
		model = glm::rotate(model, -50 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Lateral.RenderModel();

		// Barra lateral derecha
		model = modelRot;
		model = glm::translate(model, glm::vec3(8.0f, 1.0f, 6.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 130 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Lateral.RenderModel();

		// Barra inferior izquierda
		model = modelRot;
		model = glm::translate(model, glm::vec3(-7.0f, 1.0f, 4.0f));
		model = glm::rotate(model, -50 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Inferior.RenderModel();

		// Barra inferior derecha
		model = modelRot;
		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 4.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 130 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Inferior.RenderModel();

		



		////Cono
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-2.0f, -1.4f, -0.15f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Cono.RenderModel();

		////Cilindro
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-4.0f, -1.4f, -0.15f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		/////dirtTexture.UseTexture();
		//Cilindro.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}