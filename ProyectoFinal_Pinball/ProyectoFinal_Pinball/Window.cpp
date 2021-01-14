#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	mover = 0.0f;
	muevex = 2.0f;
	OnOff = 0.0f; //Off
	OnOff_lpaletas = 1.0f; //On
	OnOff_ltablero = 0.0f; //Off
	OnOff_lobjeto = 1.0f; //On
	activar = 0.0f;
	camaraCanica = GL_FALSE; //Camara de la canica desactivada
	PaletaL = 25.0f; //Desactivada
	PaletaR = 25.0f; //Desactivada
	PaletaC = 25.0f; //Desactivada
	clic = 0.0f; //Desactivado
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
	glfwSetMouseButtonCallback(mainWindow, BotonesMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	//Para mover molino
	if (key == GLFW_KEY_G)
	{
		if (theWindow->mover < 360.0f)
			theWindow->mover += 1.0f;
		else
			theWindow->mover = 0.0f;
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}

	// Encender/apagar luces
	///Luz linterna
	if (action == GLFW_PRESS && key == GLFW_KEY_P && theWindow->OnOff == 1.0)
	{
		theWindow->OnOff = 0.0;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_P && theWindow->OnOff == 0.0) {
		theWindow->OnOff = 1.0;
	}

	/// Luz paletas
	if (action == GLFW_PRESS && key == GLFW_KEY_O && theWindow->OnOff_lpaletas == 1.0)
	{
		theWindow->OnOff_lpaletas = 0.0;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_O && theWindow->OnOff_lpaletas == 0.0) {
		theWindow->OnOff_lpaletas = 1.0;
	}

	/// Luz tablero
	if (action == GLFW_PRESS && key == GLFW_KEY_I && theWindow->OnOff_ltablero == 1.0)
	{
		theWindow->OnOff_ltablero = 0.0;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_I && theWindow->OnOff_ltablero == 0.0) {
		theWindow->OnOff_ltablero = 1.0;
	}

	/// Luz objeto
	if (action == GLFW_PRESS && key == GLFW_KEY_U && theWindow->OnOff_lobjeto == 1.0)
	{
		theWindow->OnOff_lobjeto = 0.0;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_U && theWindow->OnOff_lobjeto == 0.0) {
		theWindow->OnOff_lobjeto = 1.0;
	}

	// Movimiento de las paletas
	if (action == GLFW_PRESS && key == GLFW_KEY_Z)
	{
		theWindow->PaletaL = -15.0f;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_Z) {
		theWindow->PaletaL = 25.0f;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_X)
	{
		theWindow->PaletaR = -15.0f;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_X) {
		theWindow->PaletaR = 25.0f;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_C)
	{
		theWindow->PaletaC = -15.0f;
	}
	else if (action == GLFW_RELEASE && key == GLFW_KEY_C) {
		theWindow->PaletaC = 25.0f;
	}
	

	//Cambio de camara de la canica
	if (action == GLFW_PRESS && key == GLFW_KEY_Q && theWindow->camaraCanica == GL_TRUE) {
		theWindow->camaraCanica = GL_FALSE; //Desactivamos la camara  de la canica
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_Q && theWindow->camaraCanica == GL_FALSE) {
		theWindow->camaraCanica = GL_TRUE;  //Activamos la camara de la canica
	}

	//Reporte P11
	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && theWindow->activar == 1.0)
	{
		theWindow->activar = 0.0;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_SPACE && theWindow->activar == 0.0) {
		theWindow->activar = 1.0;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::BotonesMouse(GLFWwindow* window, int button, int action, int mods)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{	
		theWindow->clic = 1.0f;
		//printf("se presiono clic \n");
	}
	else 
	{
		theWindow->clic = 0.0f;
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
