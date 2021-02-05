# sistope-lab-2
Laboratorio 2 sistope

Trabajo realizado n para el ramo de sistope 2_2020

Autores: Catalina Araya y Ekaterina Araya

Enunciado:
El objetivo de este laboratorio es conocer, usar y apreciar las caracter ́ısticas de paralelismo de un
programa multi hebreado. Aunque no es necesario disponer de un multiprocesador para desarrollar y
ejecutar este lab, ser ́ıa provechoso que la ejecucion lo realizaran en un computador con al menos dos
n ́ucleos.
En este laboratorio usted implementar ́a un programa multi-hebreado que calcula el histograma de
una imagen.
2 El Histograma
Sea A una imagen de N × N p ́ıxeles, tal que 0 ≤ Aij ≤ 255. El histograma de A es una funcion
discreta unidimensional H, tal que
Hk =
N
X−1
i=0
N
X−1
j=0
I(Aij − k), k = 0, 1, . . . , 255 (1)

donde I es la funcion indicador definida de la siguiente manera

I(x) = 

1 if x = 0
0 otherwise (2)
Note que la funcion indicador en Hk retorna el valor 1 cuando el pixel en la posici ́on (i, j) tiene
valor k. Es decir, Hk es el n ́umero de veces que el valor k aparece en la imagen.
Cada k representa un bin del histograma y en la equacion (3) un bin corresponde a solo un valor
de pixel. Pero esto no siempre es el caso. Es posible construir histogramas donde cada bin represente
un subconjunto de valores. Por ejemplo, podr ́ıamos construir un histograma de las edades de los
estudiantes del Diinf, usando los siguientes bins: [1-18], [19-21], [22-25], [26-31], [32,-]. Asi, H0 ser ́ıa el
n ́umero de estudiantes con edades menores o igual a 18, H1 el n ́umero de estudiantes con edades entre
19 y 21 a ̃nos, etc.
El histograma puede ser normalizado tal que Hk represente la frecuencia (porcentaje) que un valor
aparece en un conjunto de datos, es decir

Hk =
1
N × N
N
X−1
i=0
N
X−1
j=0
I(Aij − k), k = 0, 1, . . . , 255 (3)
La siguiente figura muestra una imagen en niveles de gris y su respectivo histograma con 256 bins.
En cambio, la siguiente figura muestra el histograma para la misma imagen, pero con solo 16 bins.
Los l ́ımites de los bins son: [0,15], [16,31], [32,47], [48,63], [64,79], [80,95], [96,11], [112,127], [128,143],


Figure 1: Imagen con valores de p ́ıxeles entre 0 y 255, y su correspondiente histograma con 256 bins.

Figure 2: Histograma para la imagen anterior con 16 bins.

Figure 3: Descomposici ́on recursiva de un domino 2D con un quadtree de 3 niveles.
[144,159], [160,175], [176,191], [192,207], [208,223], [224,239], [240,256]. Es decir, los valores de los
p ́ıxeles son acumulados en bins de igual tama ̃no, que en este caso son de 16 valores.
Note que para un histograma discreto, se satisface la siguiente propiedad

X
k
Hk = N
2

que es independiente del n ́umero de bins. Esta propiedad puede ser usada para verificar correctitud
de c ́alculo del histograma.
3 Quadtree

Un quadtree o  ́arbol cuaternario es una estructura de datos que almacena eficientemente datos bidi-
mensionales. Un quadtree tambi ́en se usa para particionar un espacio de datos bidimensionales, recur-
sivamente subdividi ́endolo en 4 cuadrantes. La subdivisi ́on de alg ́un cuadrante puede condicionarse
segun alguna propiedad de los p ́ıxeles del cuadrante, pero en nuestro caso siempre un cuadrante se
subdividir ́a en cuatro subcuadrantes de igual tama ̃no. El n ́umero de niveles de descomposicion estara
dado por un argumento de l ́ınea de comando.
En este lab usaremos la l ́ogica de descomposici ́on del quadtree para dividir el trabajo de las hebras
por cuadrantes.
La siguiente figura ejemplifica el particionamiento de una imagen con un quadtree de 3 niveles.
Si la imagen del nivel 0 (imagen inicial) es una image cuadrada de N × N, entonces, los cuadrantes
del nivel 1, son de N/2 × N/2, y los del nivel 2 sonde N/4 × N/4. Y en general, el tama ̃no de los
cuadrantes en el nivel i son de N/2^i × N/2^i


4 Calculo paralelo del histograma

El objetivo de este lab es disenar y construir un programa multihebreado que calcule el histograma de
una imagen cuadrada, dividiendo el c ́alculo entre las hebras usando la descomposici ́on quadtree.

La logica del algoritmo es la siguiente:
1. La hebra inicial (main) crea una hebra en nivel 0.
2. La hebra nivel 0 crea 4 hebras, cada una de las cuales est ́a encargada de calcular el histograma
de sus respectivos cuadrantes.
3. Estas hebras son de nivel 1.
4. La hebra de nivel 0 espera que las 4 hebras de nivel 1 terminen su trabajo.
5. Si no se ha alcanzado el m ́aximo nivel de descomposici ́on, cada hebra de nivel 1, crea otras 4
hebras de nivel 2. Es decir las hebras no calculan el histograma sino que crean hebras hijas las
cuales calculan el histograma de subcuadrantes de dicha hebra.
6. Cada hebra de nivel 1 espera a sus 4 hebras de nivel 2 a que finalicen su trabajo.
7. Este procedimiento se repite hasta que se alcanza el nivel m ́aximo.
8. Cuando se alcanza el nivel m ́aximo, cada hebra pasa a gris su porci ́on de la imagen (ver ecuaci ́on
4) y adem ́as calcula el histograma de su subcuadrante. Por ejemplo, en la Figura 3, existen en
total 13 hebras credas, pero solo las 8 hebras de nivel 2 calculan el histograma de sus cuadrantes.
9. El c ́alculo del histograma de cada subcuadrante se realiza en un histograma local a la hebra, es
decir, cada hebra del  ́ultimo nivel procura memoria dinamica para su histograma. De esta forma
no existe problemas de acceso concurrente a una misma  ́area de memoria.
10. Note que el tama ̃no del histograma siempre es el mismo, independiente del nivel de la hebra.
11. Cuando 4 hebras de un nivel i han calculado cada una un histograma, le corresponde a la hebra
madre, de nivel i − 1, sumar dichos histogramas.
12. Recuerde que cuando una hebra termina su trabajo, ya sea calcula histograma o suma histogra-
mas, finaliza su ejecuci ́on y es eliminada del sistema, por lo que es necesario disponer de un
metodo para que su histograma no se destruya y lo reciba la hebra madre.
13. As ́ı, la hebra madre suma los cuatro histogramas y crea uno consolidado.
14. Este procedimiento continua hasta alcanzar el nivel 0, donde se suman los  ́utlimos 4 histrogramas
y se obtien el histograma final.

Y = R ∗ 0.3 + G ∗ 0.59 + B ∗ 0.11 (4)

donde R, G, y B son los componentes rojos, verdes y azul, respectivamente.

5 Ejecucion
El programa debe ser capaz de trabajar con im ́agenes de distinto tama ̃no y con distintos niveles de
descomposici ́on. Para simplificar, el tama ̃no de la imagen siempre ser ́a potencia de 2, es decir 2^k. De
esta forma, siempre se podr ́a subdividir un cuadrante en 4 subcuadrantes de igual tamano.
La siguiente es la l ́ınea de comandos

$ ./lab2 -i imagen.bmp -o histograma.txt -L niveles -B numerodebins
donde -i especifica la imagen de entrada que estar ́a en formato BMP, -o indica el nombre del archivo
de salida que contiene el histograma, -L es el nivel de descomposici ́on. La opci ́on -B especifica el
n ́umero de bins del histograma, el cual siempre ser ́a tambi ́en potencia de dos. Por ejemplo 256, 128,
64, etc.
El archivo de salida est ́a en formato texto plano (ASCII), donde cada l ́ınea especifica el rango del
bin y su frecuencia, separados por un tab. Por ejemplo, si el n ́umero de bins es 256, el archivo tendr ́ıa
el sigueinte formato
[0, 0] 12
[1, 1] 0
[2, 2] 21
[3, 3] 94
...
Note que hay un tab entre la coma y el valor del l ́ımte superior.
Para el ejemplo de 16 bins, el archivo de salida se ver ́ıa de esta forma
[0, 15] 204
[16, 31] 188
[32, 47] 1023
...
El archivo de salida NO DEBE contener nada m ́as que lo indicado.
El programa debe verificar que el archivo de entrada exista, que el n ́umero de niveles es un n ́umero
entre 0 y 8 y sea potencia de dos, y que el n ́umero de bins sea un n ́umero positivo mayor a 1 y sea
potencia de dos.


