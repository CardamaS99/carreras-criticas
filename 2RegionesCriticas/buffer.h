#ifndef BUFFER_H
#define BUFFER_H

/*
* -----------------------------DESCRIPCIÓN DEL TAD-----------------------------
* El TAD Buffer tiene a su disposición tantos elementos de tipo 'int' como se
* indiquen en su constructor. Se devolverá una variable de tipo Buffer que
* cuando deje de ser necesaria, esta deberá de ser destruida con la función
* 'destruirBuffer'
*/

/*
* ------------------------------ESTRUCTURA DEL TAD------------------------------
* Tipo de dato exportado: una estructura tipo ST_BUFFER
* Campos:
*		- valores: variable que apunta al primer elemento del Buffer
*		- tam: número de elementos que puede almacenar el buffer
*		- inicio: posición anterior del primer elemento de la cola (donde empieza)
*		- final: posición del último elemento de la cola
*		- numElementos: número de elementos que hay actualmente en la cola
*		- producciones: número de producciones que van a ser realizadas por los
*										productores y que quedan por consumir
*/
typedef struct ST_BUFFER{
	int* valores;
	int tam;
	int inicio;
	int final;
	int numElementos;
	int producciones;
} Buffer;

/*
* ---------------------------MODIFICACIÓN DE VARIABLES--------------------------
*	- Variable  inicio: el entero apuntado por 'inicio' puede verse modificado en
*											la función 'sacarBuffer'
*
*	- Variable   final: el entero apuntado por 'final' puede verse modificado en
*											la función 'insertarBuffer'
*
*	- Array de valores: los enteros del array pueden ser modificados mediante
*											las funciones 'insertarBuffer' y 'sacarBuffer', pudiendo
*											modificar solamente el último valor insertado.
*
*	- Producciones    : el número de producciones que quedan por consumir se
*											puede modificar con la función 'incrementarProducciones'
*											indicando un incremento en concreto.
*
* - numElementos    : el número de elementos del buffer se modifica en las
*											funciones 'sacarBuffer' e 'insertarBuffer'
*/


/*
* Nombre: crearBuffer
* Tipo: constructor
* Constructor del buffer a partir del tamaño de este.
*
* Precondición : el tamaño indicado debe ser mayor a 0
* Postcondición: el usuario recibe una variable tipo Buffer del tamaño indicado
*								 cuyos valores están vacíos.
*/
Buffer crearBuffer(unsigned int tam);

/*
* Nombre: destruirBuffer
* Tipo: destructor
* Destructor del buffer, liberando los recursos correspondientes
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'
* Postcondición: la memoria reservada para los valores del Buffer es liberada.
*								 La variable 'valores' se pone a NULL, el resto de variables del
*								 buffer quedan establecidas a -1.
*/
void destruirBuffer(Buffer* buf);

/*
* Nombre: insertarBuffer
* Tipo: modificador
* Función que inserta el valor indicado por parámetro en la primera posición
* libre del buffer, en caso de que el buffer esté lleno se descarta la inserción
*
* Tiempo añadido de inserción: 0
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
*	Postcondición: se pueden dar los siguientes escenarios principales:
*					- Se inserta el valor en la primera posición libre del Buffer y la
*						variable 'final' se ve incrementada.
*					- El buffer se encuentra lleno por lo tanto el valor es descartado y
*						la variable 'final' no se ve incrementada.
*/
void insertarBuffer(Buffer* buffer, int valor);

/*
* Nombre: insertarBufferTime
* Tipo: modificador
* Función que inserta el valor indicado por parámetro en la primera posición
* libre del buffer, en caso de que el buffer esté lleno se descarta la inserción
*
* Se permite especificar el tiempo que se tardará en producir el elemento en el
* buffer.
*
* Tiempo añadido de inserción: MAX(0, tiempo)
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'
*	Postcondición: se pueden dar los siguientes escenarios principales:
*					- Se inserta el valor en la primera posición libre del Buffer y la
*						variable 'final' se ve incrementada.
*					- El buffer se encuentra lleno por lo tanto el valor es descartado y
*						la variable 'final' no se ve incrementada.
*/
void insertarBufferTime(Buffer* buffer, int valor, int tiempo);

/*
* Nombre: sacarBuffer
* Tipo: modificador
*	Función que permite sacar del buffer el primer elemento insertado, en caso de
* que el buffer esté vacío no se asegura un valor correcto de retorno, por lo
* que deberá ser controlado por el usuario.
*
* Tiempo añadido de eliminación: 0
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
*								 El buffer no se encuentra vacío.
* Postcondición: se pueden dar los siguientes escenarios princiales:
*					- Se saca el valor de la primera posición ocupada del Buffer y la
*						variable 'inicio' se ve incrementada.
*					- El buffer se encuentra vacío por lo tanto el valor devuelto es -1 y
*						la variable 'inicio' no se ve incrementada.
*/
int sacarBuffer(Buffer* buffer);

/*
* Nombre: sacarBufferTime
* Tipo: modificador
*	Función que permite sacar del buffer el primer elemento insertado, en caso de
* que el buffer esté vacío no se asegura un valor correcto de retorno, por lo
* que deberá ser controlado por el usuario.
*
* Se permite especificar el tiempo que se tardará en consumir el elemento del
* buffer
*
* Tiempo añadido de eliminación: MAX(0, tiempo)
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
*								 El buffer no se encuentra vacío.
* Postcondición: se pueden dar los siguientes escenarios princiales:
*					- Se saca el valor de la primera posición ocupada del Buffer y la
*						variable 'inicio' se ve incrementada.
*					- El buffer se encuentra vacío por lo tanto el valor devuelto es -1 y
*						la variable 'inicio' no se ve incrementada.
*/
int sacarBufferTime(Buffer* buffer, int tiempo);

/*
* Nombre: tamano
* Tipo: consulta
* Función que devuelve el tamaño del buffer pasado por parámetro.
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'
* Postcondición: le es devuelto al usuario el tamaño del buffer.
*/
int tamano(Buffer buffer);

/*
* Nombre: valores
* Tipo: consulta
* Función que devuelve el array de valores del buffer pasado por parámetro.
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'
* Postcondición: le es devuelto al usuario el array de valores del buffer.
*/
int* valores(Buffer buffer);

/*
* Nombre: obtenerProducciones
* Tipo: consulta
* Función que devuelve el número de producciones que quedan por consumir de las
* posibles producciones
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
* Postcondición: le es devuelto al usuario el número de producciones que quedan
*								 por consumir
*/
int obtenerProducciones(Buffer buffer);

/*
* Nombre: incrementarProducciones
* Tipo: modificador
* Función para realizar un incremento sobre la variable producciones del buffer
* pasado por parámetro.
*
* En caso de que se de la situación de que las producciones den un número
* negativo después de la operación, se establecerá el valor de estas a 0
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'
* Postcondición: el número de producciones se ve incrementado en el incremento
*								 indicado. En caso de que el número de producciones resulte en
*								 un número negativo, este se establece a 0.
*/
void incrementarProducciones(Buffer* buffer, int incremento);

/*
* Nombre: imprimirBuffer
* Tipo: consulta
* Función que devuelve el array de valores del buffer pasado por parámetro.
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
*								 El buffer no podrá contar con el -1 como un valor de inserción
*								 normal, debido a que ese valor está reservado para mostrar la
*								 ocurrencia de carreras críticas en esta función.
* Postcondición: se imprime por pantalla los valores insertados en el buffer.
*/
void imprimirBuffer(Buffer buffer);

/*
* Nombre: colaVacia
* Tipo: consulta
* Función que devuelve un 1 en caso de que el buffer pasado por parámetro se
* encuentre vacío y un 0 en caso contrario.
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
* Postcondición: el valor devuelto es un 1 si el buffer está vacío, en caso
*								 contrario, el valor de retorno es 0.
*/
int colaVacia(Buffer buffer);

/*
* Nombre: colaLlena
* Tipo: consulta
* Función que devuelve un 1 en caso de que el buffer pasado por parámetro se
* encuentre lleno y un 0 en caso contrario.
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
* Postcondición: el valor devuelto es un 1 si el buffer está lleno, en caso
*								 contrario, el valor de retorno es 0.
*/
int colaLlena(Buffer buffer);

/*
* Nombre: numElementos
* Tipo: consulta
* Función que devuelve el número de elementos que actualmente están en el buffer
*
* Precondición : el buffer debe haber sido creado con la función 'crearBuffer'.
* Postcondición: se devuelve el número de elementos del buffer
*/
int numElementos(Buffer buffer);

#endif
