# Resolución del problema productor-consumidor mediante mutexes y variables de condición

Se aportan dos implementaciones distintas para dar solución al famoso problema del __productor-consumidor__ mediante el uso de _variables de condición_ y _mutexes_.

Cada una de ellas tiene sus ventajas y desventajas las cuales se encuentran explicadas con detalle en el fichero [Informe.pdf](https://github.com/CardamaS99/carreras-criticas/blob/master/Informe.pdf).


## ¿Cómo compilar y ejecutar cada implementación?

Para la compilación se aporta un __Makefile__ para cada implementación
```bash
    cd <implementacion-especifica>
    make
```

La ejecución se realiza de la siguiente manera
```bash
    cd <implementacion-especifica>
    ./buffer <num-productores> <num-consumidores> <por-defecto>
```

En caso de que se seleccione la opción por defecto (indicando un 1 en la opción), los valores serán los siguientes.

* Tiempo de producción: 2 segundos
* Tiempo de consumición: 1 segundos
* Tiempo de postProducción: aleatorio entre 0 y 4 segundos
* Tiempo de postConsumición: aleatorio entre 0 y 4 segundos
* Número de producciones: 10 por hilo