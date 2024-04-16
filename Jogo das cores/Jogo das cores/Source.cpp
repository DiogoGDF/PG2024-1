/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 14/08/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

//Classe para manipula��o dos shaders
#include "Shader.h"

// Prot�tipo da fun��o de callback da GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


// Prot�tipos das fun��es
int setup();
void pickColor(GLdouble xpos, GLdouble ypos);
//initRandomColors();

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1024, HEIGHT = 768;
const int COLUMNS = 5, LINES = 11;

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo das cores", nullptr, nullptr);
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o shader
	Shader shader("../shaders/retangulo.vs", "../shaders/retangulo.fs");
	Shader shaderPalete("../shaders/palete.vs", "../shaders/palete.fs");

	// Gerando um buffer simples
	GLuint VAO = setup();
	
	glm::mat4 model = glm::mat4(1);

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO); //Conectando ao buffer de geometria
		
		glm::vec3 color(0.0f, 0.0f, 1.0f); // sugest�o

		shader.Use();
		GLfloat xc = -0.77f,
			    xl = 0.90f;
		for (int c = 0; c < COLUMNS; c++) 
			for (int l = 0; l < LINES; l++) {

			shader.setVec3("cor", color.r, color.g, color.b); // sugest�o colorMatrix[COLUMNS][LINES] 

			model = glm::mat4(1);

			model = glm::translate(model, glm::vec3(xc + c * 0.385, xl - l * 0.125, 0)); // sugest�o
			model = glm::scale(model, glm::vec3(0.38, 0.38, 1)); // sugest�o

			shader.setMat4("model", glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		
		shaderPalete.Use();

		model = glm::mat4(1);

		model = glm::translate(model, glm::vec3(0.70, -0.70, 0)); 
		model = glm::scale(model, glm::vec3(0.5, 0.5, 1)); 

		shaderPalete.setMat4("model", glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glBindVertexArray(0); //Desconectando o buffer de geometria

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Fun��o de callback de clique do bot�o
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	GLdouble xpos, ypos;
	int w, h;
	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xpos, &ypos);
		cout << "X: " << xpos << " Y: " << ypos << endl; // posic�o do pixel
		glfwGetWindowSize(window, &w, &h);
		pickColor(xpos, h - ypos); // cor do pixel
	}
}

// Esta fun��o est� bastante harcoded 
int setup()
{
	GLfloat vertices[] = {
		-0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,  // V�rtice ret�ngulo
		-0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // V�rtice ret�ngulo
		0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // V�rtice ret�ngulo
		0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,    // V�rtice ret�ngulo 
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // V�rtice cores  (R)
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // V�rtice cores  (G)
		0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // V�rtice cores  (B)
	   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f   // V�rtice cores  (RGB)
	};

	GLuint VBO, VAO;
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	//Atributo 0 - posi��o
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo 1 - cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}

void pickColor(GLdouble xpos, GLdouble ypos) {
	unsigned char pixel[4];
	glReadPixels(xpos, ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if(pixel[0] || pixel[1] || pixel[2]) // se a cor for diferente de preto
		cout << "R: " << (int)pixel[0] << " G: " << (int)pixel[1] << " B: " << (int)pixel[2] << endl;
}
/*
??? initRandomColors()
{
	// sortear e armazenar as cores em uma colorMatrix[COLUMNS][LINES] de glm::vec3
}
*/