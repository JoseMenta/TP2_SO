Para ejecutar el TP de SO:
LINUX:
    Requerimientos:
        Contar con una versión de Docker en el dispositivo.
        Contar con permisos de super user.

    Instrucciones:
        Desempaquetar el .zip
        Abrir una terminal de Comandos dentro de la carpeta y correr:
            $ ./dockerScript.sh*
            $ ./run.sh

MAC:
    Requerimientos:
        Contar con una versión de Docker en el dispositivo.
        Contar con multipass, un sistema similar a WSL para mac (ver más en https://multipass.run). El mismo debe configurarse para poder conectarse con ssh y transmitir un entorno gráfico mediante XQuartz (https://www.xquartz.org)
        Contar con permisos de superuser en la instancia de multipass.

    Instrucciones:
        Desempaquetar el .zip
        Abrir una terminal de Comandos dentro de la instancia de multipass, en la carpeta con el proyecto y correr:
            $ ./dockerScript.sh*
            $ ./run.sh


WINDOWS:
    Requerimientos:
        Tener configurado WSL 2 e instalado una distribución de Linux
        Contar con una versión de Docker de Windows.

    Instrucciones:
        Desempaquetar el .zip
        Abrir la distribución de Linux
        Moverse al directorio donde se ubica el archivo desempaquetado
        Compilar el proyecto y correrlo con acceso de superusuario:
        $ sudo ./dockerScript.sh*
        En caso de fallo, intentar con chmod, o copiar las líneas del archivo, pegarlas en un archivo .sh nuevo y ejecutar ese nuevo archivo
        $ sudo ./run.sh

*: En todos los casos, para que dockerScript.sh funcione correctamente, se deberan seguir los siguientes pasos (utilizando una consola de Linux o UNIX):
    Moverse al directorio donde se encuentra el proyecto (la carpeta con los archivos fuente)
    Escribir en consola los siguientes comandos:
    $ docker pull agodio/itba-so:1.0
    $ docker run -d -v "${PWD}:/root" --security-opt seccomp:unconfined -ti --name tpe_arqui_compile agodio/itba-so:1.0

En el caso donde no funcione correctamente el archivo dockerScript.sh (se presentaron algunos problemas de compatibilidad), correr los siguientes comandos
	$ docker start tpe_arqui_compile
	$ docker exec -it tpe_arqui_compile make clean -C/root/Toolchain
	$ docker exec -it tpe_arqui_compile make clean -C/root/
	$ docker exec -it tpe_arqui_compile make -C/root/Toolchain
	$ docker exec -it tpe_arqui_compile make -C/root/
	$ docker stop tpe_arqui_compile


Soporte:

	En caso de necesitar información adicional o querer conocer detalles de la implementación se sugiere acceder al repositorio de GitHub público del proyecto O contactarse con cualquiera de los desarrolladores

    Link al repositorio público:
        https://github.com/JoseMenta/TPE

    Contacto de desarrolladores:

        Axel Facundo Preiti Tasat: https://github.com/AxelPreitiT
        Gaston Ariel Francois: https://github.com/francoisgaston
        Jose Rodolfo Mentasti: https://github.com/JoseMenta
