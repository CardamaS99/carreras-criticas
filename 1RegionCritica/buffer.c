#include "buffer.h"

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

Buffer crearBuffer(unsigned int tam){

	// Buffer a devolver al usuario
	Buffer buf;

	// Se asigna al buffer el tamaño correspondiente
	buf.tam = tam;

	// Se reserva memoria para los componentes del buffer
	buf.valores = (int*) malloc(sizeof(int) * tam);

	// Se inicializa el número de elementos a 0
	buf.numElementos = 0;

	// El número de producciones inicial será 0
	buf.producciones = 0;

	// Se asignan los punteros final y inicio a la última posición del buffer,
	// ya que "inicio" es la posición anterior al primer elemento de la cola, por
	// lo tanto, cuando se añada un elemento está posición será el 0
	buf.final = buf.tam-1;
	buf.inicio = buf.tam-1;

	// Se retorna el buffer al usuario
	return buf;

}

void destruirBuffer(Buffer* buf){
	if(buf != NULL && buf->valores != NULL){

			free(buf->valores);
			buf->valores = NULL;

			// Se ponen el resto de variables a -1
			buf->inicio = -1;
			buf->final = -1;
			buf->producciones = -1;
			buf->numElementos = -1;
			buf->tam = -1;
	}
}

/*
* Función que retorna la posición posterior de la cola circular del parámetro
* indicado
*/
int siguiente(Buffer buffer, int posicion){

	// La posición se obtiene como el módulo entre la siguiente posición y el
	// tamaño del buffer (cola circular)
	return ((posicion + 1) % (buffer.tam));
}

/*
* Función que devuelve 1 si la Cola está llena y un 0 en caso contrario
*/
int colaLlena(Buffer buffer){

	// La condición de ColaLlena es que el número de elementos del buffer sea
	// igual al tamaño del buffer
	return(buffer.numElementos == buffer.tam);
}

/*
* Función que devuelve 1 si la Cola está vacía y un 0 en caso contrario
*/
int colaVacia(Buffer buffer){
	// La condición de ColaVacía es que el número de elementos del buffer sea 0
	return(buffer.numElementos == 0);
}

void insertarBuffer(Buffer* buffer, int valor){
	// Se llama a la función insertarBuffer con un tiempo de producción de 0
	insertarBufferTime(buffer, valor, 0);
}

void insertarBufferTime(Buffer* buffer, int valor, int tiempo){
	int posicionInsercion;

	if(buffer != NULL && buffer->valores != NULL){
		if(!colaLlena(*buffer)){
			// Se obtiene la posición de la siguiente inserción
			posicionInsercion = siguiente(*buffer, buffer->final);

			// Se añade el valor en la posición correspondiente
			buffer->valores[posicionInsercion] = valor;

			// Se actualiza la variable final a la nueva posición
			buffer->final = posicionInsercion;

			// Se incrementa el número de elementos
			buffer->numElementos += 1;

			if(tiempo > 0)
				sleep(tiempo);
		}
	}
}

int sacarBuffer(Buffer* buffer){

	// Se llama a sacarBuffer y que el tiempo de consumición sea 0
	return sacarBufferTime(buffer, 0);
}

int sacarBufferTime(Buffer* buffer, int tiempo){
	int valor = -1;

	if(buffer != NULL && buffer->valores != NULL){
		if(!colaVacia(*buffer)){
			// Se actualiza la posición del principio (inicio apunta a la posición
			// anterior del primer elemento)

			buffer->inicio = siguiente(*buffer, buffer->inicio);

			// Se obtiene el valor de la posición nueva
			valor = buffer->valores[buffer->inicio];

			// Se actualiza el valor a -1
			buffer->valores[buffer->inicio] = -1;

			// Se decrementa el número de elementos
			buffer->numElementos -= 1;

			if(tiempo > 1)
				sleep(tiempo);
		}
	}

	// Se retorna el valor
	return valor;
}

int tamano(Buffer buffer){
	return buffer.tam;
}

int* valores(Buffer buffer){
	return buffer.valores;
}

int inicio(Buffer buffer){
	int count = -1;

	// Se comprueba que el Buffer este inicializado
	if(buffer.valores != NULL){
		count = buffer.inicio;
	}
	return count;
}

int final(Buffer buffer){
	int count = -1;

	// Se comprueba que el Buffer este inicializado
	if(buffer.valores != NULL){
		count = buffer.final;
	}
	return count;
}

int obtenerProducciones(Buffer buffer){
	int producciones = -1;

	// Se comprueba que el Buffer esté inicializado
	if(buffer.valores != NULL){
		producciones = buffer.producciones;
	}
	return producciones;
}

void incrementarProducciones(Buffer* buffer, int incremento){

	// Se realiza un incremento sobre las producciones
	buffer->producciones += incremento;

	// Si el valor de producciones resulta en un número negativo, se establece a 0
	if(buffer->producciones < 0){
		buffer->producciones = 0;
	}
}

int numElementos(Buffer buffer){
	return buffer.numElementos;
}

void imprimirBuffer(Buffer buffer){
	int inicio, final;
	int condicion;
	int i;

	inicio = buffer.inicio;
	final = buffer.final;

	for(i = 0; i < buffer.tam; i++){
		if(i == 0){
			printf("┌─");
		} else if(i == buffer.tam - 1){
			printf("┬─┐\n");
		} else {
			printf("┬─");
		}
	}
	for(i = 0; i < buffer.tam; i++){
		if(inicio < final){
			condicion = i > inicio && i <= final;
		} else if (inicio > final){
			condicion = i > inicio || i <= final;
		} else {
			if(buffer.numElementos == buffer.tam){
				 condicion = 1;
			} else {
				 condicion = 0;
			}
		}
		if(i == buffer.tam - 1){
			if(condicion){
				if(buffer.valores[i] == -1){
					printf("│▓│\n");
				} else {
					printf("│%d│\n", buffer.valores[i]);
				}
			} else {
				printf("│ │\n");
			}
		} else {
			if(condicion){
				if(buffer.valores[i] == -1){
					printf("│▓");
				} else {
					printf("│%d", buffer.valores[i]);
				}
			} else{
				printf("│ ");
			}
		}
	}

	for(i = 0; i < buffer.tam; i++){
		if(i == 0){
			printf("├─");
		} else if(i == buffer.tam - 1){
			printf("┴─┘\n");
		} else {
			printf("┴─");
		}
	}

	printf("└─> Tam: %d | Inicio: %d | Final: %d \n", buffer.tam, inicio, final);
}
