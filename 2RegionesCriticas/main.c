#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"

// Colores
#define tblack "\E[30m" // Texto color negro
#define tred "\E[31m" // Texto color rojo
#define tgreen "\E[32m" // Texto color verde
#define tyellow "\E[33m" // Texto color amarillo
#define tblue "\E[34m" // Texto color azul
#define tpurple "\E[35m" // Texto color morado
#define tcyan "\E[36m" // Texto color cyan
#define reset "\E[m" // Texto color blanco
#define fpurple "\E[45m" // Fondo color morado

// Tamaño que ocupa el string de la hora
#define TAM_HORA 9

// Tamaño del Buffer
#define TAM_BUFFER 10

// Estructura utilizada para guardar la información de los Hilos Productores.
typedef struct ST_HILOPROD{
  // TID del hilo
  pthread_t tid;

  // Número de hilo, autoincremental
  unsigned int id;

  // Tiempo que el hilo va a tardar en realizar la producción
  int tiempo;

  // Tiempo que esperará el hilo al salir de la región crítica. En caso de ser
  // negativo se escogerá un aleatorio entre 0 y 4
  int postProduccion;

  // Número de producciones que va a realizar el hilo
  unsigned int numProducciones;
} HiloProductor;

// Estructura utilizada para guardar la información de los Hilos Consumidores.
typedef struct ST_HILOCONS{
  // TID del hilo
  pthread_t tid;

  // Número de hilo, autoincremental
  unsigned int id;

  // Tiempo que el hilo va a tardar en realizar la consumición
  int tiempo;

  // Tiempo que esperará el hilo al salir de la región crítica. En caso de ser
  // negativo se escogerá un aleatorio entre 0 y 4
  int postConsumicion;
} HiloConsumidor;

// Variable Buffer que hará la labor de cola, donde los productores añadirán sus
// producciones y donde los consumidores obtendrán sus consumiciones.
Buffer buffer;

// Mutex para el acceso a la región crítica de los consumidores
pthread_mutex_t mutexConsum;

// Mutex para el acceso a la región crítica de los productores
pthread_mutex_t mutexProd;

// Mutex para las operaciones pthread_cond_wait y pthread_cond_signal, común a
// los dos tipos de hilos (productores y consumidores)
pthread_mutex_t mutexDespertar;

// Variable de condición asociada al mutexDespertar, utilizada para dormir o
// despertar a los productores y consumidores
pthread_cond_t condDespertar;

/*
* Función que crea los hilos productores correspondientes a partir de la
* información pasada por parámetro.
*
* La variable numProductores indica el número de productores que componen el
* array 'hilos'
*/
void crearProductores(HiloProductor* hilos, unsigned int numProductores);

/*
* Función que crea los hilos consumidores correspondientes a partir de la
* información pasada por parámetro.
*
* La variable numConsumidores indica el número de consumidores que componen el
* array 'hilos'
*/
void crearConsumidores(HiloConsumidor* hilos, unsigned int numConsumidores);

/*
* Función que realiza la espera pthread_join de todos los hilos productores
*
* La variable numProductores indica el número de productores que componen el
* array 'hilos
*/
void joinProductores(HiloProductor* hilos, unsigned int numProductores);

/*
* Función que realiza la espera pthread_join de todos los hilos consumidores
*
* La variable numConsumidores indica el número de consumidores que componen el
* array 'hilos'
*/
void joinConsumidores(HiloConsumidor* hilos, unsigned int numConsumidores);

/*
* Función asociada a los hilos de tipo productor
*/
void productor(HiloProductor* hilo);

/*
* Función asociada a los hilos de tipo consumidores
*/
void consumidor(HiloConsumidor* hilo);

/*
* Función de producción para los hilos productores. Devuelve un entero aleatorio
* entre 0 y 9.
*/
int producir();

/*
* Función que modifica la cadena de caracteres pasada por argumento añadiéndole
* la hora actual. La cadena de caracteres tiene que tener como mínimo 'TAM_HORA'
* caracteres.
*/
void calcularHora(char* hora);

/*
* Función para imprimir la cabecera del hilo productor indicado en un color
* determinado.
*/
void imprimirCabeceraProduc(HiloProductor hilo, char* color);

/*
* Función para imprimir la cabecera del hilo consumidor indicado en un color
* determinado.
*/
void imprimirCabeceraConsum(HiloConsumidor hilo, char* color);

int main(int argc, char *argv[]){

  // Array de información de hilos productores y consumidores que se usarán en
  // el programa
  HiloProductor* productores;
  HiloConsumidor* consumidores;

  // Variables que indican el número de productores y consumidores que se
  // crearán, que por defecto será de 1
  int numProductores = 1, numConsumidores = 1;

  srand(time(NULL));

  // En caso de que el número de argumentos sea dos, se comprueba si lo que se
  // está indicando es la opción de ayuda
  if(argc == 2){
    switch(argv[1][0]){
      case '-':
      // En caso de que lleve la terminación '-' se comprueba si está ejecutando
      // el modo de ayuda
      // Se comprueba que haya un segundo carácter en la cadena de caracteres
      if(strlen(argv[1]) >= 2){
        switch(argv[1][1]){
          case 'h':

          // Se imprime la ayuda al usuario y se sale de forma exitosa
          // Se imprime la ayuda al usuario y se sale de forma exitosa
          printf("Modo de uso: %s <numProductores> <numConsumidores> "
                 "<defecto>\n"
                 "\t-> defecto: se utilizan los parámetros por defecto para los"
                      " hilos:\n"
                      "\t\t-> Tiempo de producción: 2\n"
                      "\t\t-> Tiempo de consumición: 1\n"
                      "\t\t-> Tiempo de postProducción: aleatorio entre 0 y 4\n"
                      "\t\t-> Tiempo de postConsumición: aleatorio entre 0 y 4"
                      "\n\t\t-> Número de producciones: 10 por hilo\n"
                      , argv[0]);

          exit(EXIT_SUCCESS);
          break;
        }
      }
      break;
    }
  }

  // Se comprueba que haya más de 2 argumentos (nombre del ejecutable, número de
  // de productores y número de consumidores)
  if(argc > 2){
    // Se realiza un switch en función del primer carácter de la segunda cadena
    numProductores = atoi(argv[1]);
    numConsumidores = atoi(argv[2]);

  }

  // Se reserva memoria para los productores y consumidores
  productores = (HiloProductor*)  malloc(sizeof(HiloProductor)*numProductores);
  consumidores = (HiloConsumidor*) malloc(sizeof(HiloConsumidor)*
                                          numConsumidores);

  // En caso de que el número argumentos solo sean 3, se pide al usuario los
  // parámetros para los hilos
  if(argc <= 3){
    printf("[?] ¿Tiempo de producción? ");
    scanf("%d", &(productores[0].tiempo));
    printf("[?] ¿Tiempo de consumición? ");
    scanf("%d", &(consumidores[0].tiempo));
    printf("[?] ¿Tiempo de post producción? ");
    scanf("%d", &(productores[0].postProduccion));
    printf("[?] ¿Tiempo de post consumición? ");
    scanf("%d", &(consumidores[0].postConsumicion));
    printf("[?] ¿Producciones a realizar por hilo? ");
    scanf("%d", &(productores[0].numProducciones));
  } else {
    // En caso contrario se establecen los valores por defecto.
    // Al establecer los tiempos a -1 se utilizarán tiempos aleatorios entre 0 y
    // 4 segundos.
    productores[0].tiempo = 2;
    productores[0].postProduccion = -1;
    consumidores[0].tiempo = 1;
    consumidores[0].postConsumicion = -1;
    productores[0].numProducciones = 10;
  }

  // Se inicializan los mutexes a usar explicados en la cabecera del programa
  pthread_mutex_init(&mutexConsum, NULL);
  pthread_mutex_init(&mutexProd, NULL);
  pthread_mutex_init(&mutexDespertar, NULL);

  // Inicialización de la variable de condición utilizada para despertar a los
  // hilos
  pthread_cond_init(&condDespertar, NULL);

  // Se llama a la función de crearBuffer para obtener un buffer del tamaño
  // indicado
  buffer = crearBuffer(TAM_BUFFER);

  // Se crean los productores y consumidores, pasándole a estas funciones los
  // arrays con la información de los hilos correspondientes.
  //
  // El primer elemento de cada array contiene la información que deberá ser
  // duplicada para el resto de hilos
  crearProductores(productores, numProductores);
  crearConsumidores(consumidores, numConsumidores);

  // Las funciones join realizan un pthread_join sobre todos los Hilos
  // La función joinProductores realiza un join sobre los productores, que serán
  // en gran parte de los casos los primeros en acabar.
  joinProductores(productores, numProductores);

  // La función joinConsumidores realiza un join sobre los consumidores, que
  // serán, en la gran parte de los casos, los últimos en finalizar.
  joinConsumidores(consumidores, numConsumidores);

  // Se destruyen los mutexes una vez finalizada su función
  pthread_mutex_destroy(&mutexConsum);
  pthread_mutex_destroy(&mutexProd);
  pthread_mutex_destroy(&mutexDespertar);

  // Se destruye la variable de condición
  pthread_cond_destroy(&condDespertar);

  // Se destruye el buffer
  destruirBuffer(&buffer);

  // El proceso finaliza
  exit(EXIT_SUCCESS);
}

void crearProductores(HiloProductor* hilos, unsigned int numProductores){
  // Contador
  int i;

  for(i = 0; i < numProductores; i++){
    // Se asigna el id correspondiente al hilo, en función del orden
    hilos[i].id = i;

    // El número de producciones de cualquier hilo se establece como el mismo
    // del primer hilo.
    // Lo mismo ocurre para las variables de postProduccion y tiempo
    hilos[i].numProducciones = hilos[0].numProducciones;
    hilos[i].postProduccion = hilos[0].postProduccion;
    hilos[i].tiempo = hilos[0].tiempo;

    // Se incrementan el número de producciones en función de las que vaya a
    // hacer el hilo correspondiente
    incrementarProducciones(&buffer, hilos[i].numProducciones);

    // Se crea el hilo, almacenando la información en su variable concreta.
    // El hilo ejecutará la función 'productor' que recibe como parámetro el
    // puntero a la información del hilo correspondiente
    pthread_create(&(hilos[i].tid), NULL, (void*)productor, hilos+i);
  }

}

void crearConsumidores(HiloConsumidor* hilos, unsigned int numConsumidores){
  int i;

  for(i = 0; i < numConsumidores; i++){
    // Se asigna el id correspondiente al hilo, en función del orden
    hilos[i].id = i;

    // Los tiempos de cualquier hilo se establecen como los mismos del primer
    // hilo
    hilos[i].tiempo = hilos[0].tiempo;
    hilos[i].postConsumicion = hilos[0].postConsumicion;

    // Se crea el hilo, almacenando la información en su variable concreta.
    // El hilo ejecutará la función 'consumidor' que recibe como parámetro el
    // puntero a la información del hilo correspondiente
    pthread_create(&(hilos[i].tid), NULL, (void*)consumidor, hilos+i);
  }
}

void joinProductores(HiloProductor* hilos, unsigned int numProductores){
  int i;
  for(i = 0; i < numProductores; i++){
    // Se hace un join sobre todos los hilos productores
    pthread_join(hilos[i].tid, NULL);
  }
}

void joinConsumidores(HiloConsumidor* hilos, unsigned int numConsumidores){
  int i;
  for(i = 0; i < numConsumidores; i++){
    // Se hace un join sobre todos los hilos consumidores
    pthread_join(hilos[i].tid, NULL);
  }
}


void productor(HiloProductor* hilo){
  int i;
  int item;

  // Se informa al usuario del número del productor
  imprimirCabeceraProduc(*hilo, reset);
  printf("[i] Soy el productor número %d\n", hilo->id);

  // Se crean las producciones indicadas en la información del hilo
  for(i = 0; i < hilo->numProducciones; i++){
    // Se produce el item
    item = producir();

    imprimirCabeceraProduc(*hilo, tcyan);
    printf("[*] Intentando acceder a la región crítica de productores\n%s",
            reset);

    // Se intenta acceder a la región crítica del productor
    pthread_mutex_lock(&mutexProd);

    // Se bloquea la región crítica utilizada para los pthread_cond_wait
    // y pthread_cond_signal y para las comprobaciones de colaLlena y colaVacia
    pthread_mutex_lock(&mutexDespertar);

    // Se comprueba si la cola está llena, ya que en caso de que lo esté, será
    // necesario dormir al productor esperando a que un consumidor lo despierte
    while(colaLlena(buffer)){

      imprimirCabeceraProduc(*hilo, fpurple);
      printf("[!] La cola está llena. Durmiendo...%s\n", reset);

      // Se duerme el productor, dejando libre la región crítica asociada al
      // mutexDespertar para que otro consumidor lo pueda despertar, pero no la
      // región crítica asociada al productor, ya que no aporta nada que otro
      // productor pueda entrar, debido a que se va a quedar bloqueado.
      pthread_cond_wait(&condDespertar, &mutexDespertar);

    }
    // Se libera el mutex
    pthread_mutex_unlock(&mutexDespertar);

    // Se inserta en el buffer, indicando el tiempo de producción que se tardará
    insertarBufferTime(&buffer, item, hilo->tiempo);
    imprimirCabeceraProduc(*hilo, tgreen);
    printf("[%d / %d] He fabricado el valor: %d\n%s", i+1,
            hilo->numProducciones, item, reset);
    imprimirBuffer(buffer);

    // Se bloquea el mutex utilizado para la comunicación entre consumidores y
    // productores
    pthread_mutex_lock(&mutexDespertar);

    // En caso de que el número de elementos del buffer ahora sea 1, es porque
    // la cola estaba vacía, por lo tanto se despierta al consumidor
    if(numElementos(buffer) == 1){
      imprimirCabeceraProduc(*hilo, tpurple);
      printf("[!] Despertando al consumidor.\n%s", reset);

      // Se despierta al consumidor
      pthread_cond_signal(&condDespertar);
    }

    // Se libera el mutex común a productores y consumidores
    pthread_mutex_unlock(&mutexDespertar);

    // Se libera la región crítica de los productores
    pthread_mutex_unlock(&mutexProd);

    imprimirCabeceraProduc(*hilo, tcyan);
    printf("[i] Región crítica de productores liberada\n%s", reset);

    // Se realiza la post producción, en caso de que el tiempo indicado sea
    // negativo, se escoge un tiempo aleatorio entre 0 y 4
    if(hilo->postProduccion < 0){
      hilo->postProduccion = rand()%5;
    }

    imprimirCabeceraProduc(*hilo, tpurple);
    printf("[*] Realizando espera post producción de %d segundos\n%s",
            hilo->postProduccion, reset);

    if(hilo->postProduccion > 0)
      sleep(hilo->postProduccion);
  }

  imprimirCabeceraProduc(*hilo, tred);
  printf("[!] He acabado de producir. Finalizando...\n%s", reset);

  // El hilo finaliza correctamente
  pthread_exit(EXIT_SUCCESS);
}

void consumidor(HiloConsumidor* hilo){
  // Contador del número de consumiciones
  int i = 1;
  int item;

  // Bucle infinito hasta que el número de producciones llegue a 0
  while(1){

    imprimirCabeceraConsum(*hilo, tcyan);
    printf("[*] Intentando acceder a la región crítica de consumidores...\n%s",
            reset);

    // Se intenta acceder a la región crítica del consumidor
    pthread_mutex_lock(&mutexConsum);

    // Se comprueba el número de producciones que aún no han sido consumidas. En
    // caso de que sean 0 el consumidor finaliza su ejecución
    if(obtenerProducciones(buffer) == 0){
      imprimirCabeceraConsum(*hilo, tred);
      printf("[!] No quedan producciones. Finalizando...\n%s", reset);

      // Se libera la región crítica
      pthread_mutex_unlock(&mutexConsum);
      pthread_exit(EXIT_SUCCESS);
    }

    // Se accede a la región crítica común a consumidores y productores para
    // realizar la comprobación correspondiente a si la cola está vacía, debido
    // a que se de este último caso, no habrá nada para producir y el consumidor
    // deberá dormirse
    pthread_mutex_lock(&mutexDespertar);
    while(colaVacia(buffer)){
      imprimirCabeceraConsum(*hilo, fpurple);
      printf("[!] La cola está vacía. Durmiendo...%s\n", reset);

      // Se ejecuta el pthread_cond_wait para que el consumidor se bloquee
      pthread_cond_wait(&condDespertar, &mutexDespertar);
    }
    pthread_mutex_unlock(&mutexDespertar);

    // Se saca el item del buffer, indicando el tiempo que se desea que dure la
    // operación
    item = sacarBufferTime(&buffer, hilo->tiempo);

    // Se decrementa en 1 el número de producciones que quedan por consumir
    incrementarProducciones(&buffer, -1);

    imprimirCabeceraConsum(*hilo, tgreen);
    printf("[Nª: %d] He consumido el valor: %d\n%s", i, item, reset);

    imprimirCabeceraConsum(*hilo, tyellow);
    printf("[i] Quedan por consumidor %d elementos\n%s",
            obtenerProducciones(buffer), reset);

    imprimirBuffer(buffer);

    // Se vuelve a acceder a la región crítica común para comprobar que, en el
    // caso de que la cola estuviese llena antes de sacar el elemento, se
    // despierte al productor
    pthread_mutex_lock(&mutexDespertar);
    if(numElementos(buffer) == tamano(buffer) - 1){
      imprimirCabeceraConsum(*hilo, tpurple);
      printf("[!] Despertando al productor...\n%s", reset);

      // Se lanza la señal para despertar al productor
      pthread_cond_signal(&condDespertar);
    }

    // Se libera la región crítica común
    pthread_mutex_unlock(&mutexDespertar);

    // Se libera la región crítica de los consumidores
    pthread_mutex_unlock(&mutexConsum);

    imprimirCabeceraConsum(*hilo, tcyan);
    printf("[i] Región crítica de consumidores liberada\n%s", reset);

    // Se realiza una espera de post consumición antes de volver a pedir la
    // región crítica.
    if(hilo->postConsumicion < 0){
      hilo->postConsumicion = rand()%5;
    }

    imprimirCabeceraConsum(*hilo, tpurple);
    printf("[*] Realizando espera post consumición de %d segundos\n%s",
            hilo->postConsumicion, reset);

    if(hilo->postConsumicion > 0)
      sleep(hilo->postConsumicion);

    // Se incrementa el número de consumiciones
    i++;
  }
}

int producir(){
  return rand()%10;
}

void calcularHora(char* hora){
  time_t t;
  struct tm *tim;

  t = time(NULL);
  tim = localtime(&t);
  strftime(hora, TAM_HORA, "%H:%M:%S", tim);
}

void imprimirCabeceraProduc(HiloProductor hilo, char* color){
  char hora[TAM_HORA];
  calcularHora(hora);
  printf("%s{P: %d}(%s) │ ", color, hilo.id, hora);
}

void imprimirCabeceraConsum(HiloConsumidor hilo, char* color){
  char hora[TAM_HORA];
  calcularHora(hora);
  printf("%s{C: %d}(%s) │ ", color, hilo.id, hora);
}
