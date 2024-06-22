#version 400

// variavel de entrada
in vec3 vertexColor;

// variavel de saida
out vec4 color;

void main()
{
	// atribui a cor do vertice a variavel de saida
	color = vec4(vertexColor,1.0);

}