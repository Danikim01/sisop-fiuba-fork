# Desafios realizados:

## ps (5pts): el comando process status muestra información básica de los procesos que están corriendo en el sistema. Se pide como mínimo una implementación que muestre el pid y comando (i.e. argv) de cada proceso (esto es equivalente a hacer ps -eo pid,comm, se recomienda compararlo con tal comando). Para más información, leer la sección ps0, de uno de los labs anteriores. Ayuda: leer proc(5) para información sobre el directorio /proc.

### Forma de ejecutar: ./ps (muestra un output similar a ps -eo pid,comm)

## ls (2pts): el comando list permite listar los contenidos de un directorio, brindando información extra de cada una de sus entradas. Se pide implementar una versión simplificada de ls -al donde cada entrada del directorio se imprima en su propia línea, indicando nombre, tipo (con la misma simbología que usa ls), permisos (se pueden mostrar numéricamente) y usuario (aquí nuevamente alcanza con mostrar el uid). Si se trata de un enlace simbólico, se debe mostrar a qué archivo/directorio apunta el mismo. Ayuda: es similar a find, pero deben incorporar stat(2) y readlink(2).

### Forma de ejecutar: ./ls (muestra un output similar a ls -al)

#### Aclaraciones: En la carpeta desafio_ls se han agregado algunos archivos de prueba para ver si funciona bien el programa.

## cp (3pts): implementar el comando copy de forma eficiente, haciendo uso de mmap tal y como se describe en la sección cp1 de uno de los labs anteriores. Solo se pide soportar el caso básico de copiar un archivo regular a otro (i.e. cp src dst donde src es un archivo regular y dst no existe). Si el archivo destino existe, debe fallar; y si el archivo fuente no existe también. Ayuda: mmap(2) y memcpy(3). Se recomienda implementar primero una versión simplificada con write y read; y luego optimizarlo con mmap.

### Forma de ejecutar: ./cp fuente.txt destino.txt (si el archivo destino existe falla)
