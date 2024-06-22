#include <iostream>
#include <string>
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>

using namespace std;

//Classe para manipulação dos shaders
#include "Shader.h"

// Protótipo da função de callback da GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


// Protótipos das funções
int setup();
void pickColor(GLdouble xpos, GLdouble ypos);
void initRandomColors();
void removeColor(int r, int g, int b);

// Variáveis globais
int tentativas = 0;
int retangulosRemovidos = 0;

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Retangulos e matriz de cores
const int COLUMNS = 5, LINES = 11;
glm::vec3 colorMatrix[COLUMNS][LINES];

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Inicializa o gerador de números aleatórios (para sortear as cores)
	std::srand(std::time(0));

	// Versão do OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo das cores", nullptr, nullptr);
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o shader
	Shader shader("../shaders/retangulo.vs", "../shaders/retangulo.fs");
	Shader shaderPalete("../shaders/palete.vs", "../shaders/palete.fs");

	// Gerando um buffer simples
	GLuint VAO = setup();

	glm::mat4 model = glm::mat4(1);

	initRandomColors(); // Inicializa a matriz de cores

	// Loop principal
	while (!glfwWindowShouldClose(window))
	{

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO); //Conectando ao buffer de geometria

		shader.Use();

		GLfloat xInitialPos = -0.77f,
			yInitialPos = 0.90f;

		// Desenha os retângulos
		for (int c = 0; c < COLUMNS; c++)
			for (int l = 0; l < LINES; l++) {
				// Define a cor do retângulo
				shader.setVec3("cor", colorMatrix[c][l].r, colorMatrix[c][l].g, colorMatrix[c][l].b);

				model = glm::mat4(1);

				// Posiciona o retângulo
				model = glm::translate(model, glm::vec3(xInitialPos + c * 0.385, yInitialPos - l * 0.125, 0));
				model = glm::scale(model, glm::vec3(0.38, 0.38, 1));

				shader.setMat4("model", glm::value_ptr(model));

				// Desenha o retângulo
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}

		shaderPalete.Use();

		model = glm::mat4(1);

		// Posiciona a paleta de cores
		model = glm::translate(model, glm::vec3(0.70, -0.70, 0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 1));

		shaderPalete.setMat4("model", glm::value_ptr(model));

		// Desenha a paleta de cores
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glBindVertexArray(0); //Desconectando o buffer de geometria

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Fecha a aplicação se a tecla ESC for pressionada
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Função de callback de clique do botão
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	GLdouble xpos, ypos;
	int w, h;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Obtém a posição do mouse
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwGetWindowSize(window, &w, &h);

		// Pega a cor do pixel clicado
		pickColor(xpos, h - ypos);
	}
}

int setup()
{
	// Definindo um gradiente de cores para a paleta
	GLfloat vertices[] = {
		-0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,  // Vértice retângulo
		-0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // Vértice retângulo
		0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // Vértice retângulo
		0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,    // Vértice retângulo 
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Vértice cores  (R)
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Vértice cores  (G)
		0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // Vértice cores  (B)
	   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f   // Vértice cores  (RGB)	
	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Atributo 0 - posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo 1 - cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO
	glBindVertexArray(0);

	return VAO;
}


void pickColor(GLdouble xpos, GLdouble ypos) {
	unsigned char pixel[4];
	glReadPixels(xpos, ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
	if (pixel[0] || pixel[1] || pixel[2]) // se a cor for diferente de preto
	{
		// Incrementa o número de tentativas
		tentativas += 1;
		if (retangulosRemovidos < COLUMNS * LINES)
			cout << "Tentativas: " << tentativas << endl;

		removeColor((int)pixel[0], (int)pixel[1], (int)pixel[2]);
	}
}
void initRandomColors()
{
	// Função para sortear e armazenar as cores em uma colorMatrix[COLUMNS][LINES] de glm::vec3
	for (int c = 0; c < COLUMNS; c++)
	{
		for (int l = 0; l < LINES; l++)
		{
			// Sorteia valores de cor aleatórios
			float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			// Armazena a cor na matriz
			colorMatrix[c][l] = glm::vec3(r, g, b);
		}
	}
}

void removeColor(int r, int g, int b) {
	// Tolerância
	float tolerance = 0.40f;

	// Converte os valores de RGB para a faixa de 0 a 1
	float red = static_cast<float>(r) / 255.0f;
	float green = static_cast<float>(g) / 255.0f;
	float blue = static_cast<float>(b) / 255.0f;

	// Calcula os limites superior e inferior para a tolerância
	float lowerRed = max(0.0f, red - tolerance);
	float upperRed = min(1.0f, red + tolerance);
	float lowerGreen = max(0.0f, green - tolerance);
	float upperGreen = min(1.0f, green + tolerance);
	float lowerBlue = max(0.0f, blue - tolerance);
	float upperBlue = min(1.0f, blue + tolerance);

	// Percorre todos os pixels da matriz de cores
	for (int c = 0; c < COLUMNS; c++) {
		for (int l = 0; l < LINES; l++) {
			// Verifica se a cor do pixel está dentro da tolerância
			if (colorMatrix[c][l].r >= lowerRed && colorMatrix[c][l].r <= upperRed &&
				colorMatrix[c][l].g >= lowerGreen && colorMatrix[c][l].g <= upperGreen &&
				colorMatrix[c][l].b >= lowerBlue && colorMatrix[c][l].b <= upperBlue)
			{
				// 
				colorMatrix[c][l] = glm::vec3(-1.0f, -1.0f, -1.0f);
				retangulosRemovidos += 1;
				cout << "Retangulos removidos: " << retangulosRemovidos << endl;
				if (retangulosRemovidos == COLUMNS * LINES)
					cout << "\nParabens! Voce venceu em " << tentativas << " tentativas!" << endl;
			}
		}
	}
}