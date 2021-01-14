#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getMove() { return mover; }
	GLfloat getmuevex() { return muevex; }
	GLfloat getOnOff() { return OnOff; }
	GLfloat getOnOff_lpaletas() { return OnOff_lpaletas; }
	GLfloat getOnOff_ltablero() { return OnOff_ltablero; }
	GLfloat getOnOff_lobjeto() { return OnOff_lobjeto; }
	GLfloat getPaletaL() { return PaletaL; }
	GLfloat getPaletaR() { return PaletaR; }
	GLfloat getPaletaC() { return PaletaC; }
	GLboolean getCamaraCanica() { return camaraCanica; }
	GLfloat getClic() { return clic; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	//Reporte P11
	GLfloat getActivar() { return activar; }

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat mover;
	GLfloat OnOff;
	GLfloat OnOff_lpaletas;
	GLfloat OnOff_ltablero;
	GLfloat OnOff_lobjeto;
	GLfloat PaletaL;	//Paleta Left
	GLfloat PaletaR;	//Paleta Right
	GLfloat PaletaC;	//Paleta Central
	GLboolean camaraCanica;
	GLfloat clic;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	static void BotonesMouse(GLFWwindow* window, int button, int action, int mods);

	//Reporte P11
	GLfloat activar;
};

