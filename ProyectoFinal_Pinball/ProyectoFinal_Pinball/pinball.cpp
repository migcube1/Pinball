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
float movResorte;
float movPosPalanca;
float movPosResorte;
float movCanicaR_x;
float movCanicaR_y;
float movCanicaR_z;
float countAnimCanica;
float movOffset;
bool avanza;
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Declaración de la cámara
Camera camera;
Camera camera2;

// Delaración de las Texturas
Texture plainTexture;
Texture plainDark;
Texture Portada;
Texture Isaac;
Texture Necromorph;
Texture plainPis;
Texture metal;

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
Model Barra_Lateral_Izq;
Model Barra_Lateral_Der;
Model Barra_Inferior_Izq;
Model Barra_Inferior_Der;
Model Barra_Superior;
Model Paleta;
Model Resorte;
Model Palanca;
Model SoporteResorte;
Model MuroInferior;
Model Caja;
Model Flecha_Detalle;


// Declaración de la canica
Sphere canica = Sphere(1, 20, 20);
glm::vec3 pos_canica = glm::vec3(-7.0f, 1.5, 18.0f);

//Declaración del skybox
Skybox skybox;

//Declaración de la variables de tiempo
GLfloat deltaTime = 0.0f;
GLfloat deltaTime2 = 90.0f;
GLfloat lastTime = 0.0f;


//void my_input(GLFWwindow *window);
void inputKeyframes(bool* keys);

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

//////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;

//NEW// Keyframes
//float posXCanica = -1.0f, posYCanica = 1.0, posZCanica = 2.0f;   //Posición inicial del helicoptero
float posXCanica = -1.0f, posYCanica = 1.0, posZCanica = 2.0f;   //Posición inicial del helicoptero
float	movCanica_x = 0.0f, movCanica_y = 0.0f, movCanica_z = 0.0f;;
float giroCanica = 0;

#define MAX_FRAMES 30  //Numero maximo de cuadros a guardar (min 30 FPS)
int i_max_steps = 90; //Valor maximo de interpolaciones entre KeyFrames
int i_curr_steps = 5; //Cuantos KeyFrame  guardados actualmente

typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movCanica_x;		//Variable para PosicionX
	float movCanica_y;		//Variable para PosicionY
	float movCanica_z;		//Variable para PosicionZ
	float movCanica_xInc;		//Variable para IncrementoX
	float movCanica_yInc;		//Variable para IncrementoY
	float movCanica_zInc;		//Variable para IncrementoY
	float giroCanica;
	float giroCanicaInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].movCanica_x = movCanica_x;
	KeyFrame[FrameIndex].movCanica_y = movCanica_y;
	KeyFrame[FrameIndex].movCanica_z = movCanica_z;
	KeyFrame[FrameIndex].giroCanica;

	FrameIndex++;
}

void resetElements(void)
{

	movCanica_x = KeyFrame[0].movCanica_x;
	movCanica_y = KeyFrame[0].movCanica_y;
	movCanica_z = KeyFrame[0].movCanica_z;
	giroCanica = KeyFrame[0].giroCanica;
}

void interpolation(void)
{
	KeyFrame[playIndex].movCanica_xInc = (KeyFrame[playIndex + 1].movCanica_x - KeyFrame[playIndex].movCanica_x) / i_max_steps;
	KeyFrame[playIndex].movCanica_yInc = (KeyFrame[playIndex + 1].movCanica_y - KeyFrame[playIndex].movCanica_y) / i_max_steps;
	KeyFrame[playIndex].movCanica_zInc = (KeyFrame[playIndex + 1].movCanica_z - KeyFrame[playIndex].movCanica_z) / i_max_steps;
	KeyFrame[playIndex].giroCanicaInc = (KeyFrame[playIndex + 1].giroCanica - KeyFrame[playIndex].giroCanica) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			printf("x %f\n", posXCanica + movCanica_x);
			printf("y %f\n", posYCanica + movCanica_y);
			printf("z %f\n", posZCanica + movCanica_z);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movCanica_x += KeyFrame[playIndex].movCanica_xInc;
			movCanica_y += KeyFrame[playIndex].movCanica_yInc;
			movCanica_z += KeyFrame[playIndex].movCanica_zInc;
			giroCanica += KeyFrame[playIndex].giroCanicaInc;
			i_curr_steps++;
		}

	}
}

/* FIN KEYFRAMES*/



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CrearPersonajes();
	CrearPrisma();
	CreateShaders();

	// Inializamos y cargamos la geometria de la canica
	canica.init();
	canica.load();

	/*----------------------------POS Y CONFIG DE LA CAMARA----------------------------------*/
								//pos					Up							Yaw		Pitch  Mspeed Tspeed
	camera = Camera(glm::vec3(0.0f, 40.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 7.0f, 0.7f);
	camera2 = Camera(pos_canica, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 7.0f, 0.7f);

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

	plainPis = Texture("Textures/plainPis.png");
	plainPis.LoadTextureA();

	metal = Texture("Textures/metal.png");
	metal.LoadTextureA();

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

	Barra_Lateral_Izq = Model();
	Barra_Lateral_Izq.LoadModel("Models/barra_lateral_izq.obj");

	Barra_Lateral_Der = Model();
	Barra_Lateral_Der.LoadModel("Models/barra_lateral_der.obj");

	Barra_Inferior_Izq = Model();
	Barra_Inferior_Izq.LoadModel("Models/barra_inferior_izq.obj");

	//Barra_Inferior_Der = Model();
	//Barra_Inferior_Der.LoadModel("Models/barra_inferior_der.obj");

	Barra_Superior = Model();
	Barra_Superior.LoadModel("Models/barra_superior.obj");

	MuroInferior = Model();
	MuroInferior.LoadModel("Models/muroInferior.obj");

	Palanca = Model();
	Palanca.LoadModel("Models/palanca.obj");

	Resorte = Model();
	Resorte.LoadModel("Models/resorte.obj");

	Paleta = Model();
	Paleta.LoadModel("Models/paletas.obj"); 

	SoporteResorte = Model();
	SoporteResorte.LoadModel("Models/soporteResorte.obj");

	Caja = Model();
	Caja.LoadModel("Models/caja.obj");

	Flecha_Detalle = Model();
	Flecha_Detalle.LoadModel("Models/Flecha_Detalle.obj");

	/*---------------------------------------POSICIÓN Y ROTACIÓN DE PERSONAJES--------------------------------------------*/
	glm::vec3 posNecro[] = {
		glm::vec3(11.5f, 2.7f, -6.825f),
		glm::vec3(11.5f, 2.7f, -5.375f),
		glm::vec3(11.5f, 2.7f, -3.925f),

		glm::vec3(1.45f, 2.7f, -19.5f),
		glm::vec3(0.0f, 2.7f, -19.5f),
		glm::vec3(-1.45f, 2.7f, -19.5f),

		glm::vec3(-14.5f, 2.7f, -6.825f),
		glm::vec3(-14.5f, 2.7f, -5.375f),
		glm::vec3(-14.5f, 2.7f, -3.925f),
	};

	int rotNecro[] = {
		-90, -90, -90,
		  0,   0,   0,
		 90,  90,  90,
	};

	glm::vec3 posIsaac[] = {
		glm::vec3(12.5f, 7.5f, -17.5f),
		glm::vec3(-12.5f, 7.5f, -17.5f),
	};

	int rotIsaac[] = {
		-45, 45,
	};

	/*---------------------------------------LUCES--------------------------------------------*/

	//Contador para las luces
	int iCount = 2;

	//Posición de las verdes y rojas del lado izquierdo
	glm::vec3 posLucesGR_Left[] = {
		glm::vec3(-7.0f, 1.0f, 13.0f),		//R
		glm::vec3(-9.0f, 1.0f, 11.0f),		//V
		glm::vec3(-11.0f, 1.0f, 9.0f),		//R
		glm::vec3(-13.0f, 1.25f, 7.0f),		//V
		glm::vec3(-13.0f, 1.5f, 3.0f),		//R
		glm::vec3(-13.0f, 1.75f, -1.0f),	//V	
	};

	//Posición de las verdes y rojas del lado derecho
	glm::vec3 posLucesGR_Right[] = {
		glm::vec3(4.0f, 1.0f, 13.0f),		//R
		glm::vec3(6.0f, 1.0f, 11.0f),		//V
		glm::vec3(8.0f, 1.0f, 9.0f),		//R
		glm::vec3(10.0f, 1.25f, 7.0f),		//V
		glm::vec3(10.0f, 1.5f, 3.0f),		//R
		glm::vec3(10.0f, 1.75f, -1.0f),		//V	
	};

	//Posición de las luces amarillas  de lado izquierdo
	glm::vec3 posLucesRY_Left[]{
		glm::vec3(-5.0f, 1.25f, 9.0f), //A
		glm::vec3(-7.0f, 1.25f, 7.0f), //R
		glm::vec3(-9.0f, 1.25f, 5.0f), //A
		glm::vec3(-3.0f, 1.25f, 5.0f), //R
		glm::vec3(-4.0f, 1.25f, 2.0f), //A
	};

	//Posición de las luces amarillas  de lado derecho
	glm::vec3 posLucesRY_Right[]{
		glm::vec3(2.0f, 1.25f, 9.0f), //A
		glm::vec3(4.0f, 1.25f, 7.0f), //R
		glm::vec3(6.0f, 1.25f, 5.0f), //A
		glm::vec3(0.0f, 1.25f, 5.0f), //R
		glm::vec3(1.0f, 1.25f, 2.0f), //A
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

	//LUZ DE PALETAS INFERIORES
	spotLights[0] = SpotLight(1.0f, 0.0f, 1.0f,	//Color ROSA
		0.0f, 2.0f,								//Intensity
		-1.5f, 0.0f, 20.0f,						//Pos
		0.0f, 0.0f, -1.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		20.0f);									//Edg
	spotLightCount++;

	//LUCES DEL TABLERO
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

	spotLights[3] = SpotLight(1.0f, 0.0f, 0.0f,	//Color ROJO
		0.0f, 2.0f,								//Intensity
		4.0f, 1.0f, 13.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	spotLights[4] = SpotLight(0.0f, 1.0f, 0.0f,	//Color VERDE
		0.0f, 2.0f,								//Intensity
		6.0f, 1.0f, 11.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	spotLights[5] = SpotLight(1.0f, 1.0f, 0.0f,	//Color AMARILLO
		0.0f, 2.0f,								//Intensity
		6.0f, 1.0f, 11.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	spotLights[6] = SpotLight(1.0f, 0.0f, 0.0f,	//Color ROJO
		0.0f, 2.0f,								//Intensity
		6.0f, 1.0f, 11.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	spotLights[7] = SpotLight(1.0f, 1.0f, 0.0f,	//Color AMARILLO
		0.0f, 2.0f,								//Intensity
		6.0f, 1.0f, 11.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	spotLights[8] = SpotLight(1.0f, 0.0f, 0.0f,	//Color ROJO
		0.0f, 2.0f,								//Intensity
		6.0f, 1.0f, 11.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		15.0f);									//Edg
	spotLightCount++;

	//LUZ DEL ILUMINA TABLERO
	spotLights[9] = SpotLight(1.0f, 1.0f, 1.0f,	//Color
		0.0f, 0.25f,								//Intensity
		0.0f, 200.0f, 0.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		20.0f);									//Edg
	spotLightCount++;

	//LUZ OBJETO
	spotLights[10] = SpotLight(0.0f, 1.0f, 1.0f,	//Color
		0.0f, 1.0f,								//Intensity
		0.0f, 2.0f, -5.0f,						//Pos
		-1.0f, 0.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		10.0f);									//Edg
	spotLightCount++;

	//LUZ OBJETO
	spotLights[11] = SpotLight(0.0f, 1.0f, 1.0f,	//Color
		0.0f, 1.0f,								//Intensity
		0.0f, 2.0f, -5.0f,						//Pos
		1.0f, 0.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		10.0f);									//Edg
	spotLightCount++;


	//LINTERNA
	spotLights[12] = SpotLight(1.0f, 1.0f, 1.0f,	//Color
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

	//KEYFRAMES DECLARADOS INICIALES

	KeyFrame[0].movCanica_x = 0.0f;
	KeyFrame[0].movCanica_y = 0.0f;
	KeyFrame[0].movCanica_z = 0.0f;
	KeyFrame[0].giroCanica = 0;


	KeyFrame[1].movCanica_x = -2.5f;
	KeyFrame[1].movCanica_y = 0.0f;
	KeyFrame[1].movCanica_z = 4.0f;
	KeyFrame[1].giroCanica = 0;

	KeyFrame[2].movCanica_x = 2.0f;
	KeyFrame[2].movCanica_y = 0.0f;
	KeyFrame[2].movCanica_z = 4.0f;
	KeyFrame[2].giroCanica = 0;

	KeyFrame[3].movCanica_x = 2.0f;
	KeyFrame[3].movCanica_y = 0.0f;
	KeyFrame[3].movCanica_z = -16.0f;
	KeyFrame[3].giroCanica = 0;

	KeyFrame[4].movCanica_x = 2.0f;
	KeyFrame[4].movCanica_y = 0.0f;
	KeyFrame[4].movCanica_z = 4.0f;
	KeyFrame[4].giroCanica = 0;

	KeyFrame[5].movCanica_x = 0.0f;
	KeyFrame[5].movCanica_y = 0.0f;
	KeyFrame[5].movCanica_z = 0.0f;
	KeyFrame[5].giroCanica = 0;


	/*
	KeyFrame[2].movCanica_x = .0f;
	KeyFrame[2].movCanica_y = 0.0f;
	KeyFrame[2].movCanica_z = 10.0f;
	KeyFrame[2].giroCanica = 0;*/



	//KeyFrame[2].movCanica_x = 2.0f;
	//KeyFrame[2].movCanica_y = 0.0f;
	//KeyFrame[2].movCanica_z = 2.0f;
	//KeyFrame[2].giroCanica = 0;

	//KeyFrame[3].movCanica_x = 4.0f;
	//KeyFrame[3].movCanica_y = 0.0f;
	//KeyFrame[3].movCanica_z = 4.0f;
	//KeyFrame[3].giroCanica = 0;

	//KeyFrame[4].movCanica_x = 4.0f;
	//KeyFrame[4].movCanica_y = 0.0f;
	//KeyFrame[4].movCanica_z = 4.0f;
	//KeyFrame[4].giroCanica = 0;




	//Agregar Kefyrame[5] para que el avión regrese al inicio
		
	// Variables para la animación 
	movResorte = 0.35f;
	movPosResorte = 22.5;
	movPosPalanca = 19.0;
	movCanicaR_x = 11.5f;
	movCanicaR_z = 18.8f;
	countAnimCanica = -2.0f;

	//Con el Offset se controlará la velocidad del resorte
	movOffset = 1.5f;

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		/*-------------------------------------------------------------------------------------------*/
		/*---------------------------------ANIMACIÓN SENCILLA----------------------------------------*/
		/*-------------------------------------------------------------------------------------------*/
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		//Animación sencilla del resorte
		if (mainWindow.getClic() == 1.0f)
		{
			//printf("\n%f", movPosPalanca);
			if (movResorte < 1.275f)
			{
				movResorte += movOffset * deltaTime;
				movPosResorte -= movOffset * deltaTime;
				movPosPalanca += (movOffset * 3.5)* deltaTime;
			}
			avanza = 1;
		}
		else
		{
			movResorte = 0.425f;
			movPosResorte = 20.075f;
			movPosPalanca = 19.5f;
		}
		//Animación sencilla de una canica
		if (avanza == 1)
		{
			if (movCanicaR_x < 13.5f  && countAnimCanica == -2.0f)
			{
				movCanicaR_x += movOffset * deltaTime * 0.5f;
			}
			else if (movCanicaR_x >= 13.5f && countAnimCanica == -2.0f)
			{
				countAnimCanica = -1.0f;
			}
			else if (movCanicaR_z > 15.5f  && countAnimCanica == -1.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 5.0f;
			}
			else if (movCanicaR_z <= 15.5f && countAnimCanica == -1.0f)
			{
				countAnimCanica = 0.0f;
				movCanicaR_x = 13.5;
			}
			else if (movCanicaR_z < 18.5f && movCanicaR_x == 13.5f && countAnimCanica == 0.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 3.0f;
			}
			else if (movCanicaR_z >= 18.5f && movCanicaR_x == 13.5f && countAnimCanica == 0.0f)
			{
				countAnimCanica = 1.0f;
			}
			else if (movCanicaR_z > -10.0f && movCanicaR_x == 13.5f && countAnimCanica == 1.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 7.0f;
			}
			else if (movCanicaR_z <= -10.0f && movCanicaR_x == 13.5f && countAnimCanica == 1.0f)
			{
				countAnimCanica = 2.0f;
			}
			else if (movCanicaR_z > -12.5f  && countAnimCanica == 2.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 7.0f;
				movCanicaR_x -= movOffset * deltaTime * 1.5f;
			}
			else if (movCanicaR_z <= -12.5f  && countAnimCanica == 2.0f)
			{
				countAnimCanica = 3.0f;
			}
			else if (movCanicaR_z > -16.0f  && countAnimCanica == 3.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 7.0f;
				movCanicaR_x -= movOffset * deltaTime * 3.0f;
			}
			else if (movCanicaR_z <= -16.0f  && countAnimCanica == 3.0f)
			{
				countAnimCanica = 4.0f;
			}
			else if (movCanicaR_z > -18.5f  && countAnimCanica == 4.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 3.0f;
				movCanicaR_x -= movOffset * deltaTime * 7.0f;
			}
			else if (movCanicaR_z <= -18.5f  && countAnimCanica == 4.0f)
			{
				countAnimCanica = 5.0f;
			}
			else if (movCanicaR_x > 3.0f  && countAnimCanica == 5.0f)
			{
				movCanicaR_x -= movOffset * deltaTime * 7.0f;
			}
			else if (movCanicaR_x <= 3.0f  && countAnimCanica == 5.0f)
			{
				countAnimCanica = 6.0f;
			}
			else if (movCanicaR_z < -14.25f  && countAnimCanica == 6.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 7.0f;
				movCanicaR_x += movOffset * deltaTime * 4.0f;
			}
			else if (movCanicaR_z >= -14.25f  && countAnimCanica == 6.0f)
			{
				countAnimCanica = 7.0f;
			}
			else if (movCanicaR_x > -5.75f  && countAnimCanica == 7.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 4.0f;
				movCanicaR_x -= movOffset * deltaTime * 7.0f;
			}
			else if (movCanicaR_x <= -5.75f  && countAnimCanica == 7.0f)
			{
				countAnimCanica = 8.0f;
			}
			else if (movCanicaR_z > -11.25f  && countAnimCanica == 8.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 7.0f;
				movCanicaR_x -= movOffset * deltaTime * 4.75f;
			}
			else if (movCanicaR_z <= -11.25f  && countAnimCanica == 8.0f)
			{
				countAnimCanica = 9.0f;
			}
			else if (movCanicaR_x > -13.00f  && countAnimCanica == 9.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 7.0f;
				movCanicaR_x -= movOffset * deltaTime * 6.0f;
			}
			else if (movCanicaR_x <= -13.0f  && countAnimCanica == 9.0f)
			{
				countAnimCanica = 10.0f;
			}
			else if (movCanicaR_x < -3.0f  && countAnimCanica == 10.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 4.0f;
				movCanicaR_x += movOffset * deltaTime * 7.0f;
			}
			else if (movCanicaR_x >= -3.0f  && countAnimCanica == 10.0f)
			{
				countAnimCanica = 11.0f;
			}
			else if (movCanicaR_x > -4.5f  && countAnimCanica == 11.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 7.0f;
				movCanicaR_x -= movOffset * deltaTime * 4.0f;
			}
			else if (movCanicaR_x <= -4.5f  && countAnimCanica == 11.0f)
			{
				countAnimCanica = 12.0f;
			}
			else if (movCanicaR_x < -2.55f  && countAnimCanica == 12.0f)
			{
				movCanicaR_z -= movOffset * deltaTime * 4.0f;
				movCanicaR_x += movOffset * deltaTime * 7.0f;
			}
			else if (movCanicaR_x >= -2.55f  && countAnimCanica == 12.0f)
			{
				countAnimCanica = 13.0f;
			}
			else if (movCanicaR_z < 18.8f  && countAnimCanica == 13.0f)
			{
				movCanicaR_z += movOffset * deltaTime * 7.0f;
				movCanicaR_x += movOffset * deltaTime * 0.85f;
			}
			else if (movCanicaR_x <= 18.8f   && countAnimCanica == 13.0f)
			{
				countAnimCanica = 14.0f;
			}
			else if (movCanicaR_x < 11.5f  && countAnimCanica == 14.0f)
			{
				movCanicaR_x += movOffset * deltaTime * 6.0f;
			}
			else if (movCanicaR_x < 13.5f   && countAnimCanica == 14.0f)
			{
				countAnimCanica = -2.0f;
				avanza = 0;							//Fin de la animación
			}
		}


		
		//Recibir eventos del usuario
		glfwPollEvents();
		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Control de teclado para el cambio de cámara
		if (mainWindow.getCamaraCanica() == GL_TRUE) {  //Camara de canica Activa
			camera2.keyControl(mainWindow.getsKeys(), deltaTime);
			camera2.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		if (mainWindow.getCamaraCanica() == GL_FALSE) { // Camara de tablero activado
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

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


		/*-------------------------------------------------------------------------------------------*/
		/*-------------------------------- CONTROL DE LUCES -----------------------------------------*/
		/*-------------------------------------------------------------------------------------------*/

		//Asociamos la cámara con la luz de la linterna
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[12].SetFlash(lowerLight, camera.getCameraDirection());

		//Cargamos la luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Control de las luces verdes, rojas y amarillas 
		if (iCount > 5) { iCount = 0; }

		// Rojas
		spotLights[1].SetPos(posLucesGR_Left[iCount]);
		spotLights[3].SetPos(posLucesGR_Right[iCount]);
		// Amarillas
		spotLights[5].SetPos(posLucesRY_Left[iCount]);
		spotLights[7].SetPos(posLucesRY_Right[iCount]);
		iCount += 1;

		//Verdes
		spotLights[2].SetPos(posLucesGR_Left[iCount]);
		spotLights[4].SetPos(posLucesGR_Right[iCount]);
		//Rojas
		spotLights[6].SetPos(posLucesRY_Left[iCount]);
		spotLights[8].SetPos(posLucesRY_Right[iCount]);
		iCount += 1;

		//Control para encender/apagar la linterna (P)
		if (mainWindow.getOnOff() == 1.0) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}

		//Control para encender/apagar la luz de las paletas (O)
		if (mainWindow.getOnOff_lpaletas() == 0.0) {  ///LUZ APAGADA
			spotLights[0].SetEdge(0.0f);
		}
		else {
			spotLights[0].SetEdge(20.0f); ///LUZ ENCENDIDA
		}

		//Control para encender/apagar la luz del tablero (I)
		if (mainWindow.getOnOff_ltablero() == 0.0) {  ///LUZ APAGADA
			spotLights[9].SetEdge(0.0f);
		}
		else {
			spotLights[9].SetEdge(20.0f); ///LUZ ENCENDIDA
		}

		//Control para encender/apagar la luz del objeto (U)
		if (mainWindow.getOnOff_lobjeto() == 0.0) {  ///LUZ APAGADA
			spotLights[10].SetEdge(0.0f);
			spotLights[11].SetEdge(0.0f);
		}
		else {
			spotLights[10].SetEdge(20.0f); ///LUZ ENCENDIDA
			spotLights[11].SetEdge(20.0f);
		}


		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// Control para el cambio de cámara
		if (mainWindow.getCamaraCanica() == GL_TRUE) {  //Camara de canica Activa
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera2.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera2.getCameraPosition().x, camera2.getCameraPosition().y, camera2.getCameraPosition().z);
		}
		if (mainWindow.getCamaraCanica() == GL_FALSE) { // Camara de tablero activa
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}

		/*-------------------------------------------------------------------------------------------*/
		/*---------------------------------------DIBUJADO--------------------------------------------*/
		/*-------------------------------------------------------------------------------------------*/
		glm::mat4 model(1.0);
		glm::mat4 modelRot(1.0);
		glm::mat4 modelTemp(1.0);

		/*---------------------------------------Plano--------------------------------------------*/
		//Plano base
		model = glm::mat4(1.0);
		modelRot = model = glm::rotate(model, 5 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 1.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Portada.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		/*---------------------------------Flecha decorativa--------------------------------------*/
		model = modelRot;
		model = glm::translate(model, glm::vec3(-6.0f, 0.0f, -1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha_Detalle.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, -1.5f));
		model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Flecha_Detalle.RenderModel();

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
		/*model = modelRot;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 20.0f));
		model = glm::scale(model, glm::vec3(30.0f, 3.0f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainDark.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();*/
		model = modelRot;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 20.0f));
		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MuroInferior.RenderModel();

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

		// Luces rojas y verdes Izquierdas
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

		// Luces rojas y verdes Derechas
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

		// Luces amarillas izquierdas
		model = modelRot;
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 9.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();
		
		model = modelRot;
		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-9.0f, 0.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();


		// Luces amarillas derechas
		model = modelRot;
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 9.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();

		model = modelRot;
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luces.RenderModel();


		/*---------------------------------------PERSONAJES--------------------------------------------*/
		for (unsigned int i = 0; i < 2; i++)
		{

			//Head Isaac
			model = modelRot;
			model = glm::translate(model, posIsaac[i]);
			model = glm::rotate(model, rotIsaac[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Isaac.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[2]->RenderMesh();
			//Body Isaac
			modelTemp = model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Isaac.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[3]->RenderMesh();
			//ArmL Isaac
			model = modelTemp;
			model = glm::translate(model, glm::vec3(0.7f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Isaac.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[4]->RenderMesh();
			//ArmR Isaac
			model = modelTemp;
			model = glm::translate(model, glm::vec3(-0.7f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Isaac.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[5]->RenderMesh();
			//LegL Isaac
			model = modelTemp;
			model = glm::translate(model, glm::vec3(-0.25f, -2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Isaac.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
			//LegR Isaac
			model = modelTemp;
			model = glm::translate(model, glm::vec3(0.25f, -2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Isaac.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[7]->RenderMesh();
		}

		for (unsigned int i = 0; i < 9; i++)
		{
			//Head Necromorph
			model = modelRot;
			model = glm::translate(model, posNecro[i]);
			//model = glm::translate(model, glm::vec3(11.5f, 2.7f, -6.825f));
			model = glm::rotate(model, rotNecro[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.75f, 0.6f, 0.75f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Necromorph.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[2]->RenderMesh();
			//Body Necromorph
			modelTemp = model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Necromorph.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[3]->RenderMesh();
			//ArmL Necromorph
			model = modelTemp;
			model = glm::translate(model, glm::vec3(0.7f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Necromorph.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[4]->RenderMesh();
			//ArmR Necromorph
			model = modelTemp;
			model = glm::translate(model, glm::vec3(-0.7f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Necromorph.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[5]->RenderMesh();
			//LegL Necromorph
			model = modelTemp;
			model = glm::translate(model, glm::vec3(-0.25f, -2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Necromorph.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMesh();
			//LegR Necromorph
			model = modelTemp;
			model = glm::translate(model, glm::vec3(0.25f, -2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Necromorph.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[7]->RenderMesh();
		}

		/*---------------------------------------CANICA--------------------------------------------*/

	
		//Canica (1) con resorte asociado
		model = modelRot;
		
		model = glm::translate(model, glm::vec3(movCanicaR_x, 1.0f, movCanicaR_z));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		metal.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		canica.render(); //Renderiza esfera

		//Canica (2) con animación asociada
		model = modelRot;
		model = glm::translate(model, glm::vec3(posXCanica + movCanica_x, posYCanica + movCanica_y, posZCanica + movCanica_z));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		metal.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		canica.render(); //Renderiza esfera

		//Canica (3) con cámara asociada
		model = modelRot;
		pos_canica = camera2.getCameraPosition();
		model = glm::translate(model, pos_canica);
		//model = glm::rotate(model, 50 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		metal.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Este if servirá para renderizar o no la canica si estamos usando su vista
		if (mainWindow.getCamaraCanica() == GL_FALSE)  //Camara de canica Activa
			canica.render(); //Renderiza esfera



		/*---------------------------------------BARRAS--------------------------------------------*/

		// Barra lateral izquierda
		model = modelRot;
		model = glm::translate(model, glm::vec3(-10.5f, 1.0f, 6.0f));
		model = glm::rotate(model, -50 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Lateral_Izq.RenderModel();

		// Barra lateral derecha
		model = modelRot;
		model = glm::translate(model, glm::vec3(8.0f, 1.0f, 6.0f));
		//model = glm::rotate(model, -135 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 50.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Lateral_Izq.RenderModel();
		//Barra_Lateral_Der.RenderModel();


		// Barra inferior izquierda
		model = modelRot;
		model = glm::translate(model, glm::vec3(-6.5f, 1.0f, 3.5f));
		model = glm::rotate(model, -50 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Inferior_Izq.RenderModel();

		// Barra inferior derecha
		model = modelRot;
		model = glm::translate(model, glm::vec3(4.0f, 1.0f, 3.5f));
		//model = glm::rotate(model, -130 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -50 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Barra_Inferior_Izq.RenderModel();
		//Barra_Inferior_Der.RenderModel();

		// Barras superiores derechas
		///1
		model = modelRot;
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, -12.5f));
		model = glm::rotate(model, 30 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Barra_Superior.RenderModel();
		///2
		model = modelRot;
		model = glm::translate(model, glm::vec3(6.0f, 0.0f, -6.5f));
		model = glm::rotate(model, 30 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Barra_Superior.RenderModel();

		// Barras superiores izquierda
		///1
		model = modelRot;
		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, -12.5f));
		model = glm::rotate(model, -30 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Barra_Superior.RenderModel();
		///2
		model = modelRot;
		model = glm::translate(model, glm::vec3(-7.0f, 0.0f, -6.5f));
		model = glm::rotate(model, -30 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Barra_Superior.RenderModel();

		/*---------------------------------------PALETAS--------------------------------------------*/
		
		///Izquierda
		model = modelTemp = modelRot;
		model = glm::translate(model, glm::vec3(-5.75f, 0.0f, 11.5f));
		modelTemp = model = glm::rotate(model, -mainWindow.getPaletaL() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		model = modelTemp;
		model = glm::translate(model, glm::vec3(2.25f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Paleta.RenderModel();

		///Derecha
		model = modelTemp = modelRot;
		model = glm::translate(model, glm::vec3(3.25f, 0.0f, 11.5f));
		modelTemp = model = glm::rotate(model, mainWindow.getPaletaR() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		model = modelTemp;
		model = glm::translate(model, glm::vec3(-2.25f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(-1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Paleta.RenderModel();

		///Central
		model = modelTemp = modelRot;
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, -4.0f));
		modelTemp = model = glm::rotate(model, -mainWindow.getPaletaC() * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		model = modelTemp;
		model = glm::translate(model, glm::vec3(2.25f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Paleta.RenderModel();

		//Soporte del resorte
		model = modelRot;
		model = glm::translate(model, glm::vec3(13.5f, 1.0f, 22.5f));
		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SoporteResorte.RenderModel();
		
		/*---------------------------------------CAJAS--------------------------------------------*/

		// CAJA CENTRAL 
		model = modelRot;
		model = glm::translate(model, glm::vec3(-1.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Caja.RenderModel();

		//CAJA DERECHA
		model = modelRot;
		model = glm::translate(model, glm::vec3(2.0f, 1.0f, -16.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Caja.RenderModel();

		//CAJA IZQUIERDA
		model = modelRot;
		model = glm::translate(model, glm::vec3(-3.0f, 1.0f, -16.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Caja.RenderModel();

		/*---------------------------------------RESORTE--------------------------------------------*/

		
		
		model = modelRot;
		model = glm::translate(model, glm::vec3(13.5f, 1.0f, movPosResorte));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, movResorte));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Resorte.RenderModel();

		//Palanca
		model = modelRot;
		model = glm::translate(model, glm::vec3(13.5f, 1.0f, movPosPalanca));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Palanca.RenderModel();




		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movCanica_x);
			//printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movCanica_x += 1.0f;
			printf("movAvion_x es: %f\n", movCanica_x);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
		}
	}

}