
//Librerias 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>// permite usar fork, pipes entre otros
#include <pthread.h> //Permite usar hebras

//Estructura utilizada para guardar la información necesaria para que las hebras hagan su 
//calculo
//Struct
struct informacioHebra{
	int*** imagen;
	int nivelActual;
	int nivelFinal;
	int tamano;
	int inicioLargo;
	int inicioAncho;
};


//Funcionameinto revisa si un entero ingresado pertenece a  una potencia de 2 entre 1 y 256
//Entrada: Entero
//Salida: Booleano
bool esPotencia(int numero){
    int contador=1;
    bool resultado=false;
    while (contador<257)
    {
      if (numero==contador)
      {
          resultado= true;
          
      }
      contador=contador*2; 
    }
    return resultado;
    
}

//Funcionamiento: Funcion que obtiene los parametros de entrada necesarios para el funciojnamiento del programa
//Entrada: parametros ingresados por el usuario siguiendo el sgiguiente patron:
//-i: nombre de la imagen de entrada
//-o: nombre archivo Salida
//-L: nivel de descomposicion de 0 1 2 4 u 8
//-B: numero de bins, de 0 a 255
//Salida: No posee retorno, ya que es una función void, pero entrega por puntero los parametros de entreo, char*, entero, char y enetero
void recibirArgumentos(int argc, char *argv[],char **nombreImagen, char **nombreSalida,int *descomposicion,int *bins){
	int opt;
	char *aux2;
	char *aux3;
	aux2 =malloc(20*sizeof(char));
	aux3 =malloc(20*sizeof(char)); 

	if(argc <8){//si se ingresa un numero de argumentos menor a 9, se finaliza la ejecucion del programa
		printf("Se ingreso un numero incorrecto de argumentos\n");
		fprintf(stderr, "Uso correcto: %s [-i nombre de la imagen de entrada] [-o nombre archivo Salida] [-L nivel de descomposicion] [-B: numero de bins] \n",
				   argv[0]);
		   exit(EXIT_FAILURE);
		}
	int N = -1;
	int C = -1;
    
	while((opt = getopt(argc, argv, "i:o:L:B:")) != -1) 
    { 
	   //opt recibe el argumento leido (se hace de forma secuencial) y se ingresa a un switch
	   //En caso de que opt sea -1, se dejaran de buscar entradas y por lo tanto se podrá salir del while

	   switch(opt) 
       {    
            case 'i':
              	(*nombreImagen)=optarg; 
                  

            break;
             case 'o':
              	(*nombreSalida)=optarg; 
                  

            break;
            
	 		case 'L': 
                N = strtol(optarg, &aux2, 10);
                if(optarg!=0 && N==3){
                    fprintf(stderr,  "Uso correcto: %s [-i nombre de la imagen de entrada] [-o nombre archivo Salida] [-L nivel de descomposicion] [-B: numero de bins] \n",
                        argv[0]); 
                        exit(EXIT_FAILURE);
                }
            break;

            case 'B': 
                C = strtol(optarg, &aux3, 10);
                if(optarg!=0 && C==0){
                    fprintf(stderr,  "Uso correcto: %s [-i nombre de la imagen de entrada] [-o nombre archivo Salida] [-L nivel de descomposicion] [-B: numero de bins] \n",
                        argv[0]); 
                        exit(EXIT_FAILURE);
                }
            break;

            default:
                printf("opcions default\n");
                fprintf(stderr, "Uso correcto: %s [-i nombre de la imagen de entrada] [-o nombre archivo Salida] [-L nivel de descomposicion] [-B: numero de bins] \n",argv[0]);
                exit(EXIT_FAILURE);
        }
	}
    
    if(N<0|| N>=9 || N==6||N==5|| N==3|| N==7){
		printf("El valor que acompaña a -L debe ser un número potencia de 2 y estan entre 0 y 8 \n");
		fprintf(stderr, "Uso correcto: %s [-i nombre de la imagen de entrada] [-o nombre archivo Salida] [-L nivel de descomposicion] [-B: numero de bins] \n",argv[0]);
		exit(EXIT_FAILURE);
		}

    if(C<=0 ||C>256|| (esPotencia(C)==false)){
		printf("El valor que acompaña a -B debe ser un número potencia de 2 y estan entre 0 y 255 \n");
		fprintf(stderr, "Uso correcto: %s [-i nombre de la imagen de entrada] [-o nombre archivo Salida] [-L nivel de descomposicion] [-B: numero de bins] \n",argv[0]);
		exit(EXIT_FAILURE);
		}
	(*descomposicion) = N; 
	(*bins) = C;		
}

//Funcion que crea un archivo de salida que reune los resultados de los resultados el hebras
//Entrada: char* con el nombre dle archivo de salida, la cantidad de bins y la lista con los resultados
//Salida: archivo que contiene todos los resultados de las hebras y los histogramas en los bins indicados por el usuriao
void archivoSalida(char* nombreSalida, int l, int lista[l][3]){
    FILE* salida;
    salida=fopen(nombreSalida,"w");
    salida=fopen(nombreSalida,"a+");
    int contador=0;
    // Se ve ek tamaño necesario para utilizar el proceso comi char
	
    while (contador<l)
    {   
        int uno= lista[contador][0];
        int dos= lista[contador][1];
        int tres= lista[contador][2];
        
        fprintf(salida,"[%d,    %d]   %d\n",uno,dos, tres);
        
        contador=contador+1;
    }
    
    fclose(salida);

}

//Funcionamiento: Se leen los historigramas de las hijas hebras y se suman
//Entrada: Los histogramas de las 4 hijas hebras
//Salida: solo una lista con la suma de histogramas
int* sumaHistograma(int* lista1,int* lista2,int* lista3,int* lista4){
    int* sumaTotal=(int*)malloc(sizeof(int)*256);
    int i=0;
    while (i<256)
    {
        sumaTotal[i]=lista1[i]+lista2[i]+lista3[i]+lista4[i];
        i=i+1;
    }
    return sumaTotal;
}

//funcioamiento: crea una lista de 256 espacio y lasm incializa con valor 0
//Entradas: ninguna ya que no necesita paramtros de entradaa
//SAlidas una lista de enteros de tamaño 256 compuesta solo de 0
int* iniciarHistograma(){
    int* histograma1=(int*)malloc(256*sizeof(int)); 
    for (int i = 0; i < 256; i++)
    {
        histograma1[i]=0;
    }
    return histograma1;
    
}

//Entradas:char con cada uno de los valores rgba de la imagen, enteros con altura y anchura de la imagen a trabajar,
//matriz de enteros inicialmente vacia, de tres dimenciones que guardará los datos de la imagen
//Funcionamiento:Traspaso de los datos de la imagen desde una lista de caracteres a una matriz de valores enteros,
//donde cada posición estará orientada a un pixel con sus respectivos valores RGB
//Salidas:no existen ratornos, pero generarán cambios sobre la matriz "imagen" desde estar inicialmente vacia
// a poseer valores int correspondientes a la imagen ingresada.
void rgbMatrix(unsigned char *image,int width,int height,int imagen[height][width][3]){
    int size=width*height*4;
    int w=0; 
    int h=0;
    int r;
    int g;
    int b;
    for(int i=0;i<size;i+=4){
        if(w==width){;
            w=0;
            h+=1;
        }
        r=(image[i]);
        g=(image[i+1]);
        b=(image[i+2]);
        imagen[h][w][0]=r;
        imagen[h][w][1]=g;
        imagen[h][w][2]=b;
        //printf("%d,%d,%d\n ",r,g,b);
        w+=1;
    }
}


//Funcionamiento:Se calculan los valores por cada pixel desde RGB a escala de grises desde la matriz con valores RGB a una matriz
// de enteros con los valores de la escala de grises
//Entradas: La imagen que se desea cambiar, el inicio y fin de la seccion que se desea ccambiar, cuanto se debe avanzar y el lugar
//donde se guardara
//Salidas: Sin retorno, cambio en los valores dentro de la matriz Grey,antes vacia, ahora compuesta por enteros.
void rgbGreySection(int*** imagen,int x, int y, int mov,int Grey[mov*mov]){
    //int size=width*height*3;
    float resultFloat;
    int resultInt;
    int l=0;
    for(int i=x;i<(x+mov);i++){
        for(int j=y;j<(y+mov);j++){
            resultFloat=(imagen[i][j][0])*0.3+(imagen[i][j][1])*0.59+(imagen[i][j][2])*0.11;
            resultInt=resultFloat;
            Grey[l]=resultInt;
            l++;
            //printf("%d ",Grey[i][j]);
            //if(j
        }
    }   
}

//Funcion que calcula el hitograma de cada pedacito perteneciente a la hebra hija de cada nivel final.
//Entradas: una estrcutura infohebra en void
//Salidas: una lista de enteros que contiene las veces que repite un ntono de gris en dicha seccion
int *calculoHistograma (void *hebr)
{
    struct informacioHebra hebra;
    hebra= *((struct informacioHebra *)hebr);
    int tam=hebra.tamano;
    int Grey[tam*tam];
    rgbGreySection(hebra.imagen,hebra.inicioLargo, hebra.inicioAncho, tam,Grey);
    int* histograma1=iniciarHistograma();
    for (int i = 0; i < (tam*tam); i++)
    {
        for (int j = 0; j < 256; j++)
        {
            if (Grey[i]==j)
            {
                histograma1[j]=histograma1[j]+1;
            }   
        }
    }
    
    return histograma1;


}

//Entradas:nombre del archivo bmp a ingresar (imagen), lista tamaño 2 de enteros, inicialmnete vacia.
//Funcionamiento: se lee el archivo bmp (imagen) y se almacenan sus datos en tres listas de caracteres, "header", "data", que 
//posee la información de la imagen y otra lista que contendra el resto de valores del header antes no incluidos (en caso de haber) 
//Salidas: lista de caracteres "data" correspondiente a la información de cada pixel dentro de la imagen ingresada.
unsigned char* readBMP(char* filename,int imagesize[2])
{
       int i;
       FILE* f = fopen(filename, "rb");
       unsigned char header[54];
       fread(header, sizeof(unsigned char), 54, f); // se leen los 54-bytes del header
       int offset=*(int*)&header[10];
       offset=offset;
       //printf("\noffset: %d\n",offset);
       unsigned char header2[offset-54];
       fread(header2,sizeof(unsigned char),(offset-54),f); //se lee resto del header
       //se extraen tamaño de la imagen desde el header
       int width = *(int*)&header[18];
       int height = *(int*)&header[22];
       if(width!=height){
           exit(1);
           }
       //printf("%d,%d",width,height);
       imagesize[0]=height;
       imagesize[1]=width; 
       int size = 4 * width * height; // se reservan 4 bits por pixel RGB y A que no sera considerado
       //habia un mallo revisar version 1 
       unsigned char* data = (unsigned char*)malloc(size*sizeof(unsigned char)); 
       fread(data, sizeof(unsigned char), size, f); // se lee la informacion de la imagen
       fclose(f);

       //Se voltean los valores de BRG a RGB
       for(i = 0; i < size; i += 4)
       {  
               unsigned char tmp = data[i];
               data[i] = data[i+2];
               data[i+2] = tmp;
               //printf("%d,%d,%d,%d\n",data[i],data[i+1],data[i+2],data[i+3]);
       }
       return data;
}

//Funcionamiento: Se crean las hebras hijas hebras del primer nivel y se invoca una funcion recrusiva hasta llegar al nivel deseado
//cuando se alcance ese nivel se calcua el hisrtograma total de cada hebra y de gform recursiva se va aumentando
//Entrada: estructura con al hebra inicial en el nivel 0
//Salida: histogramas totales de la imagen
void* separacionHebras(void *hebr){
    
	/* Atributos para la creación del thread */	
	pthread_attr_t atributos;

	/* error devuelto por la función de creación del thread */
	int error1;
    int error2;
    int error3;
    int error4;  

    //Se castea la hebra ingresada
    struct informacioHebra hebra;
    hebra= *((struct informacioHebra *)hebr);
    //Caso base:  si se alcanza el nivel deseado se calculara el valor del histograma de
    if (hebra.nivelActual==hebra.nivelFinal){
        //se crea una hebra cuadno solo se deberia hacer el calculo
        
        int* histograma1=(int*)malloc(256*sizeof(int)); 
        histograma1=calculoHistograma(hebr);
        pthread_exit ((void *)histograma1);

    //Caso recursivo
    }else{
        // Identificador de cada hilo 
        pthread_t idHebra1;
        pthread_t idHebra2;
        pthread_t idHebra3;
        pthread_t idHebra4;
        /* Valor que va a devolver el thread hijo */
        int* histograma1=(int*)malloc(256*sizeof(int));
        int* histograma2=(int*)malloc(256*sizeof(int));
        int* histograma3=(int*)malloc(256*sizeof(int));
        int* histograma4=(int*)malloc(256*sizeof(int));

        //se separa la imagen original en 4 cuadrantes
        int contador=1;
        struct informacioHebra hebra1;
        struct informacioHebra hebra2;
        struct informacioHebra hebra3;
        struct informacioHebra hebra4;
        
        while (contador<5)
        {
            //Cuadrante 1
            if(contador==1){
                
                hebra1.imagen=hebra.imagen;
                hebra1.nivelActual=hebra.nivelActual+1;
                hebra1.nivelFinal=hebra.nivelFinal;
                hebra1.tamano=hebra.tamano/2;
                hebra1.inicioLargo=hebra.inicioLargo;
                hebra1.inicioAncho=hebra.inicioAncho;
                contador=contador+1;
            }
            //Cuadrante 2
            else if (contador==2)
            {
                
                hebra2.imagen=hebra.imagen;
                hebra2.nivelActual=hebra.nivelActual+1;
                hebra2.nivelFinal=hebra.nivelFinal;
                hebra2.tamano=hebra.tamano/2;
                hebra2.inicioLargo=hebra.inicioLargo+hebra.tamano/2;
                hebra2.inicioAncho=hebra.inicioAncho;
                contador=contador+1;
            }
            //Cuadrante 3
            else if (contador==3)
            {
                
                hebra3.imagen=hebra.imagen;
                hebra3.nivelActual=hebra.nivelActual+1;
                hebra3.nivelFinal=hebra.nivelFinal;
                hebra3.tamano=hebra.tamano/2;
                hebra3.inicioLargo=hebra.inicioLargo;
                hebra3.inicioAncho=hebra.inicioAncho+hebra.tamano/2;
                contador=contador+1;
            //Cuadrante 4
            }else{
             
                
                hebra4.imagen=hebra.imagen;
                hebra4.nivelActual=hebra.nivelActual+1;
                hebra4.nivelFinal=hebra.nivelFinal;
                hebra4.tamano=hebra.tamano/2;
                hebra4.inicioLargo=hebra.inicioLargo+hebra.tamano/2;
                hebra4.inicioAncho=hebra.inicioAncho+hebra.tamano/2;
                contador=contador+1;
            }
        }

        /* Inicializamos los atributos con sus valores por defecto */
        pthread_attr_init (&atributos);

        /* Aseguramos que el thread que vamos a poder esperar por el thread que
        * vamos a crear. */
        pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_JOINABLE);

        // Se crean las diferentes hebras, y se llama nuevamente a esta misma funcion 
        error1 = pthread_create(&idHebra1, &atributos, separacionHebras, &hebra1);
        error2 = pthread_create(&idHebra2, &atributos, separacionHebras, &hebra2);
        error3 = pthread_create(&idHebra3, &atributos, separacionHebras, &hebra3);
        error4 = pthread_create(&idHebra4, &atributos, separacionHebras, &hebra4);

        /* Comprobamos el error al arrancar el thread */
        if (error1 != 0)
        {
            perror ("No puedo crear thread");
            exit (-1);
        }
        if (error2 != 0)
        {
            perror ("No puedo crear thread");
            exit (-1);
        }
        if (error3 != 0)
        {
            perror ("No puedo crear thread");
            exit (-1);
        }
        if (error4 != 0)
        {
            perror ("No puedo crear thread");
            exit (-1);
        }
        // Se espera a que las 4 hebras terminen su ejecución
        pthread_join (idHebra1, (void **)&histograma1);
        pthread_join (idHebra2, (void **)&histograma2);
        pthread_join (idHebra3, (void **)&histograma3);
        pthread_join (idHebra4, (void **)&histograma4);
        //Cuando terminene es decir se alcando el caso base y se devulve se debe sumar el resultado
        int* suma=sumaHistograma(histograma1,histograma2,histograma3,histograma4);
        //Se envia el reusltado al lugar original
        pthread_exit ((void *)suma);
    }
}

//Funcion que contendra la hebra inicial y que sera la raiz de nuestro arbol de quatree
//Entradas: la imagen que se utilizara, la cantidad niveles en lsoq eu se descompondra el arbol, el tamaño total de la imagen
//Salidas: una lista de enteros que tendran ala suma total de hebras
int* hebraInicial(int*** imagen, int descomposicion,int tamanoImagen){
    struct informacioHebra hebra;
    hebra.imagen=imagen;
    hebra.nivelActual=0;
    hebra.nivelFinal=descomposicion;
    hebra.tamano=tamanoImagen;
    hebra.inicioLargo=0;
    hebra.inicioAncho=0;
    /* Identificador del thread hijo */
	pthread_t idHilo;
	/* Atributos para la creación del thread */	
	pthread_attr_t atributos;

	/* error devuelto por la función de creación del thread */
	int error;

	/* Valor que va a devolver el thread hijo */
    int* valorDevuelto=(int*)malloc(256*sizeof(int));
 
    int largo=256;

	/* Inicializamos los atributos con sus valores por defecto */
	pthread_attr_init (&atributos);

	/* Aseguramos que el thread que vamos a poder esperar por el thread que
	 * vamos a crear. */
	pthread_attr_setdetachstate (&atributos, PTHREAD_CREATE_JOINABLE);

	/* Creamos el thread.
	 * En idHilo nos devuelve un identificador para el nuevo thread,
	 * Pasamos atributos del nuevo thread NULL para que los coja por defecto
	 * Pasamos la función que se ejecutará en el nuevo hilo
	 * Pasamos NULL como parámetro para esa función. */
	error = pthread_create(&idHilo, &atributos,separacionHebras, &hebra);

	/* Comprobamos el error al arrancar el thread */
	if (error != 0)
	{
		perror ("No puedo crear thread");
		exit (-1);
	}

    // se espera al total de la ejecucion de la separacion de hebras
	pthread_join (idHilo, (void **)&valorDevuelto);

    return valorDevuelto;
}

//Funcion encargada de tranformar la imegn que se encuentra en una lista estatica a una dumanica para poder ser usada en la estrcutura
//Entradas: el largo, ancho, y la matriz que se desea transformar, en este caso que contiene una imagen
//Salidas:matriz dinamica de 3 dimenncioens que contiene una imagen RGB
int*** estaticToDinamic(int secciones,int cantidadCaracteres,int matrizADN[secciones][cantidadCaracteres][3]){
	int*** soluciones = (int***)malloc(secciones*sizeof(int**));
	for(int i=0; i<secciones; i++)
	{
		soluciones[i] = (int**)malloc(cantidadCaracteres*sizeof(int*));
	}

    for (int j = 0; j < secciones; j++)
    {
        for (int s = 0; s < secciones; s++)
        {
            soluciones[j][s] = (int*)malloc(3*sizeof(int));
        }
        
    }
  
    for (int l = 0; l < secciones; l++)
    {
        for (int m = 0; m < secciones; m++)
        {
            for (int n = 0; n < 3; n++)
            {
                soluciones[l][m][n]=matrizADN[l][m][n];
                //printf(" caracter %c",soluciones[m][n]);
            }
        }
    }
	return soluciones;
}

//Funcion que separa el histograma en la cantidad de bins pedidos por el usauarios y los prepara para el archivo de salida
//Entrdas: lista de enteros  con los hisogramas,la cantidadd e bins y la matriz en donde se guarda el resultado
//Salidas: No tiene pues es una funcion void
void  construccionHistograma(int *lista,int rango,int matriz[rango][3]){
    //2 a 256
    int diferencia=(256/rango)-1;
    int x=0;
    int pos=0;
    int count=0;
    int inicio;
    int final;
    for(int i=0;i<256;i++){
        if(x==0){
            inicio=i;
            final=i+diferencia;
            matriz[pos][0]=inicio;
            matriz[pos][1]=final;
            }
        count=count+lista[i];
        
        
        if(i==final){
            matriz[pos][2]=count;
            x=0;
            pos++;
            count=0;
        }else{
            x=x+1;
            
        }
    }
}

int main (int argc, char* argv[]) {
    //Se pide memroia para lo paramatros de entrada
	int descomposicion;
	int bins;
	char *nombreImagen;
	nombreImagen =(char*)malloc(20*sizeof(char));
	char *nombreSalida;
	nombreSalida =(char*)malloc(20*sizeof(char));
    //Se obtiene los argumentos
    recibirArgumentos(argc, argv,&nombreImagen, &nombreSalida,&descomposicion,&bins);
    //se generan las variables a utilizar:guarda alto y ancho    
    int imagesize[2];
    //Se guarda una imagen en char que aun no a sido procesada
    unsigned char* image=readBMP(nombreImagen,imagesize);
    //Se guarda memoria para la imagen a procesar
    int imagen[imagesize[0]][imagesize[1]][3];
    //se almacenan los datos de la imagen en una matriz con los valores RGB
    rgbMatrix(image,imagesize[1],imagesize[0],imagen);
    //Se tranfomra a matriz dinamica para su posterior uso
    int*** imagenDinamica=estaticToDinamic(imagesize[1],imagesize[1],imagen);
    // Se guara memoria para el resultado
    int* lista=(int*)malloc(256*sizeof(int));
    //Dr llrva a cabo todo el proceso con las hebras
    lista=hebraInicial(imagenDinamica, descomposicion,imagesize[0]);    
    // Se guarda memora para la matriz con los resultados finales
    int matriz[bins][3];
    //Se contruye el histograma final
    construccionHistograma(lista,bins, matriz);
    //Se crea alarchivo de salida 
    archivoSalida(nombreSalida, bins, matriz);
    //Se termina el programa
    return 0;

}
