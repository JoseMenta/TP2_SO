# ITBA - 72.11 - Sistemas Operativos - TP02
## jOSe 2.0
### Grupo 07

El repositorio es una extensión del sistema operativo Pure64, aplicando los conocimientos y conceptos aprendidos durante las clases teóricas y prácticas en la materia. Además, permite ejecutar programas que se detallarán más adelante.
<br /> El desarrollo es una continuación del sistema implementado el cuatrimestre anterior, en la materia 72.08 - Arquitectura de computadoras. Si desea consultarlo, el proyecto se encuentra disponible en el siguiente enlace: https://github.com/JoseMenta/TPE.


## Instrucciones

### Requerimientos
#### General
* Descargar la imagen Docker agodio/itba-so:1.0
```shell 
docker pull agodio/itba-so:1.0
```

* Contar con el emulador QEMU: https://www.qemu.org/

#### Linux GUI
* Contar con una versión de Docker en el dispositivo.
* Contar con permisos de superusuario.

#### MacOS
* Contar con una versión de Docker en el dispositivo.
* Contar con una versión de Qemu. 
* Contar con permisos de superusuario.

#### Windows
* Tener configurado WSL 2.
* Contar con una distribución de Linux.
* Contar con una versión de Docker de Windows.
* Contar con el entorno gráfico XServer (ver más en: https://sourceforge.net/projects/vcxsrv/).


### Instalación

1. Descargar el repositorio del proyecto, el cual se encuentra en el siguiente enlace: https://github.com/JoseMenta/TP2_SO.
2. Desempaquetar el archivo .zip.

### Compilación

1. Abrir una terminal en el directorio donde se descomprimió el archivo. 
* En el caso de MacOS, la terminal debe ser dentro de _multipass_.
* En el caso de Windows, la terminal debe ser dentro de _WSL_.
2.   Abrir el contenedor de Docker.
````shell
docker run -v "${PWD}:/root" --privileged --rm -ti agodio/itba-so:1.0
````
3.   Moverse al directorio _root_.
````shell 
cd ./root
````
4.   Compilar el proyecto.
````shell
make all
````
*   Alternativamente, en el caso de querer usar el manejador de memoria Buddy, compilar con _mm=buddy_.
````shell 
make all mm=buddy
````
5.   Cerrar el contenedor.
````shell
exit
````

### Ejecución
1. Ejecutar el proyecto.
````shell
make run
````
2. Escribir la contraseña de superusuario.


## Programas del SO
### sh
````shell
sh
````
Es el intérprete de comandos del sistema operativo. Cada vez que se inicie la máquina, este será el programa que se ejecutará inicialmente por defecto.
<br />![Mensaje de bienvenida al ejecutar el intérprete de comandos](images/shell_1.png?raw=true)

Mediante este programa podrá ejecutar el resto de programas que ofrece el sistema operativo. Incluso, puede ejecutar una nueva instancia de la shell ingresando el comando _sh_.
<br />![Ejecución de una nueva instancia de la shell](images/shell_2.png?raw=true)



#### Background & Pipes
````shell
<prog_a> <| <prog_b>> <&>
````
El intérprete de comandos permite ejecutar programas en background si se agrega el carácter ‘&’ al final del comando. 
<br />De igual manera, es posible definir un pipe entre dos procesos escribiendo _<prog_a> | <prog_b>_, siendo _prog_a_ el programa, junto a sus argumentos, que redirigirá su salida estándar y _prog_b_ el programa, junto a sus argumentos, que redirigirá su entrada estándar. 
<br />Esto permite, por ejemplo, redirigir la salida de wc para que su resultado se imprima sin vocales escribiendo el comando _wc | filter_. Cabe aclarar que si se utiliza ‘&’ y ‘|’ en el mismo comando, ambos programas se ejecutarán en background.
<br />![Redirigiendo la salida estándar de wc a la entrada estándar de filter](images/pipes_1.png?raw=true)
<br />![Redirigiendo la salida estándar de ps a la entrada estándar de filter, y en background](images/back&pipes_1.png?raw=true)



***

### help
````shell
help
````
Este programa permite conocer todos los programas con los que cuenta el sistema operativo. Al ejecutar el comando _help_ en la shell, el programa imprimirá un listado de los programas disponibles, el cual indica el comando a ejecutar y una descripción de su funcionamiento.
<br />![Listado de los programas disponibles; a la izquierda, se detalla el comando a ejecutar y, a la derecha, una descripción del programa](images/help_1.png?raw=true)


***

### mem
````shell
mem
````
Otorga un detalle del estado de la memoria dinámica. El mismo indica el algoritmo utilizado, el espacio total de la memoria en bytes, la cantidad de bytes ocupados, la cantidad de bloques/nodos ocupados y la cantidad de bytes libres.
<br />![Ejecución de mem](images/mem_1.png?raw=true)


***
### ps
````shell
ps
````
Otorga un detalle de los procesos en ejecución (EXECUTE), en la cola de listos (READY), detenidos (BLOCKED) y finalizados (FINISHED) al momento de ejecutar el programa. Al ejecutar el comando _ps_, se imprime un listado de los procesos, indicando su pid, su nombre, su estado, su nivel de prioridad, su base pointer, su stack pointer y un booleano que indica si está en foreground o no.
<br />![Ejecución de ps en dos instantes distintos](images/ps_1.png?raw=true)


***
### loop
````shell
loop
````
Es un programa simple, el cual solamente imprime un saludo e indica su pid infinitamente. Para frenar su ejecución, se debe usar el comando que mata procesos, la tecla **ESC**.
<br />![Ejecución de loop imprimiendo su mensaje cada 2 segundos](images/loop_1.png?raw=true)


***
### kill
````shell
kill <pid>
````
Este programa permite finalizar la ejecución de procesos indicando el pid del proceso.
<br />![Utilizando kill para “matar” el proceso test-processes](images/kill_1.png?raw=true)


***
### nice
````shell
nice <pid> <0-4>
````
Permite modificar el nivel de prioridad de un proceso, indicando su pid y el nuevo nivel de prioridad.
<br />![Modificando el nivel de prioridad de test-processes de 3 a 0, usando nice](images/nice_1.png?raw=true)


***

### block & unblock
````shell
block <pid>

unblock <pid>
````
Estos programas permiten modificar el estado de un proceso entre bloqueado y desbloqueado, respectivamente. Para ello, se debe indicar el pid del proceso que se quiere bloquear o desbloquear.
<br />![Bloqueando el proceso test-processes](images/block_1.png?raw=true)
<br />![Desbloqueando el proceso test-processes](images/unblock_1.png?raw=true)


***
### sem
````shell
sem
````
Imprime el estado de los semáforos activos, indicando su nombre, su valor, el pid de los procesos bloqueados por el semáforo y el pid de los procesos conectados al semáforo. Si el semáforo no tiene nombre (unnamed semaphore), aparecerá la frase “sin nombre”.
<br />![Listado de los semáforos activos](images/sem_1.png?raw=true)


***
### pipe
````shell
info.pipe <amount>
````
Similar a _sem_ pero, en este caso, imprime el estado de los pipes activos en el sistema operativo. Indica el nombre del pipe, el índice de lectura y de escritura, el pid de los procesos bloqueados (por lectura y escritura) y el pid de los procesos conectados al pipe. Si el pipe no tiene nombre (unnamed fifo), aparecerá la frase “sin nombre”. El comando debe recibir un argumento, el cual indica la cantidad de pipes que se desea consultar su estado.
<br />![Estado del pipe de teclado](images/pipe_1.png?raw=true)


***
### cat
````shell
cat
````
Es un programa que recibe caracteres por entrada estándar (STDIN), e imprime lo recibido por salida estándar (STDOUT) cuando lee el carácter ‘\n’. Este programa se detiene al leer el carácter especial EOF (CTRL+D).
<br />![Ejecución de cat](images/cat_1.png?raw=true)


***

### filter
````shell
filter
````
Es un programa similar a _cat_, pero con la diferencia de que no imprime por salida estándar las vocales que recibe por entrada estándar. De igual manera, finaliza al leer EOF.
<br />![Ejecución de filter](images/filter_1.png?raw=true)


***
### wc
````shell
wc
````
Este programa recibe los caracteres por entrada estándar, pero no los imprime por salida estándar. En cambio, al finalizar su ejecución por EOF, imprime un análisis de los caracteres ingresados. Indica la cantidad de caracteres leídos (bytes), la cantidad de palabras leídas (words) y la cantidad de líneas leídas (lines).
<br />![Ejecución de wc](images/wc_1.png?raw=true)


***
### phylo
````shell
phylo
````
Es una extensión del problema _The Dining Philosophers_, ya que permite aumentar o reducir la cantidad de filósofos de manera dinámica, es decir mientras se ejecuta el programa. 
<br />Los filósofos podrán estar en tres estados: comiendo (C), pensando (P) o con hambre (H). Pero nunca podrán haber dos filósofos contiguos comiendo.
<br />Siempre empieza su ejecución siendo 5 filósofos.
<br />![Estado inicial de phylo](images/phylo_1.png?raw=true)
<br />Para aumentar la cantidad de filósofos, se debe presionar la tecla A, y para reducir se debe presionar la tecla R. 
<br />Para finalizar el programa se debe presionar la tecla Q.
<br />![Aviso del programa de que se está agregando un filósofo](images/phylo_2.png?raw=true)
<br />![Aviso del programa de que se está removiendo un filósofo](images/phylo_3.png?raw=true)
<br />Cabe aclarar que, como mínimo, debe haber dos filósofos y, como máximo, debe haber 15 filósofos.
<br />![Ejecución de phylo con 15 filósofos](images/phylo_4.png?raw=true)
<br />![Ejecución de phylo con 2 filósofos](images/phylo_5.png?raw=true)



## Soporte

En caso de necesitar información adicional o querer conocer detalles de la implementación, se sugiere acceder al repositorio de GitHub público del proyecto. Alternativamente, puede contactarse con cualquiera de los desarrolladores.

### Link al repositorio público
https://github.com/JoseMenta/TP2_SO

### Contacto de desarrolladores:

* Axel Facundo Preiti Tasat: https://github.com/AxelPreitiT
* Gastón Ariel Francois: https://github.com/francoisgaston
* José Rodolfo Mentasti: https://github.com/JoseMenta

