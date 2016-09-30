#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SIZE 10000

int capacidad;
int* referencias;
int primera_entrada_libre;

int encuentra_referencia (int m){
	int y;
	for (y = 0; y < primera_entrada_libre; y++){
		if (referencias[y] == m)
		return 1;
	}
	return 0;
}

void inserta_referencia (int m){
	if (!encuentra_referencia(m) ){
		if (primera_entrada_libre > capacidad){
			capacidad += DEFAULT_SIZE * sizeof(int);
			referencias = realloc (referencias, capacidad);
		}
		referencias [primera_entrada_libre++] = m; 
	}
}

// argv[1] es el nombre del fichero de cadenas, argv[2] es el número de entradas de la TLB y argv[3] es el tamaño de página
main (int argc, char* argv[]){
	if (argc < 4){
		printf("Introduzca los argumentos de la función, por ejemplo: ./lru nombre_fichero.txt tamTLB tamPag.\n");
		exit(-1);
	}
	if (argc > 4){
		printf("Demasiados argumentos para la función. Introduzca por ejemplo: ./lru nombre_fichero.txt tamTLB tamPag.\n");
		exit(-1);
	}
	capacidad = DEFAULT_SIZE;
	referencias = malloc (capacidad * sizeof(int));
	primera_entrada_libre = 0;
	FILE *f = fopen (argv[1], "r");
	if (f == NULL){
		printf("Hay un problema con el fichero de datos.\n");
		exit(-1);
	}
	int tamTLB = atoi (argv[2]);
	int tamPag = atoi (argv[3]);
	if (tamTLB == 0 || tamPag == 0){
		printf("Ni tamaño de la página ni el tamaño de la tabla pueden ser cero. Introduzca un valor válido.\n");
		exit(-1);
	}
	if (tamPag > 8){
		printf("Introduzca un valor menor para el tamaño de la página. Un valor elevado puede colapsar la memoria del sistema.\n");
		exit(-1);
	}
	int eflechaponente = 10 - tamPag;
	int marco = 1;
	int i;
	for (i = 0; i < eflechaponente - 1; i++){
		marco = marco * 10;
	}
	int tlb [tamTLB][2];
	for (i = 0; i < tamTLB; i++){
		tlb[i][0] = -1;
		tlb[i][1] = 0;
	}
	char cadena [10];
	int numero;
	// Variable que recorre la TLB
	int flecha = 0;
	int esta = 0;
	int aciertos = 0;
	int fallos = 0;
	int salir = 0;
	int llena = 0;
	while (fgets (cadena, 10, f) != NULL){
		int j;
		numero = atoi (cadena);
		numero = numero / marco;
		*cadena = '\0';
		for (j = 0; j < llena; j++){
			// Buscamos si ya está en la TLB
			if (salir == 0){
				if (numero == tlb[j][0]){
					tlb[flecha][1] = 1;
					esta = 1;
					aciertos++;
					salir = 1;
				}
			}
		}
		salir = 0;
		if (esta == 0){
			if (flecha == tamTLB)
				flecha = 0;
			int w = flecha;
			while (llena == tamTLB){
				if (w >= tamTLB)
					w = 0;
				if (tlb[w][1] == 1){
					tlb[w][1] = 0;
					w++;
				} else {
					if (tlb[w][1] == 0){
						inserta_referencia (numero);
						tlb[w][0] = numero;
						tlb[w][1] = 1;
						fallos++;
						flecha++;
						break;
					}
				}
			}
			if (llena != tamTLB){
				inserta_referencia (numero);
				tlb[flecha][0] = numero;
				tlb[flecha][1] = 1;
				flecha++;
				fallos++;
				llena++;
			}
		}
		numero = 0;
		esta = 0;
	}
	printf("Número de aciertos: '%i'\n", aciertos);
	printf("Número de fallos: '%i'\n", fallos);
	printf("Número total de referencias a memoria: '%i'\n", aciertos + fallos);
	float total = aciertos + fallos;
	float tasa = aciertos * 100.0 / total;
	printf("Tasa de aciertos de la TLB: %f%\n", tasa);
	printf("Número de marcos de página usados: %i\n", primera_entrada_libre);
	printf("\nLa tabla finalizó con el siguiente contenido:\n");
	for (i = 0; i < tamTLB; i++){
		if (tlb[i][0] == -1)
			printf ("N = NULL	Marca = NULL\n");
		else
			printf ("N = %i		Marca = %i\n", tlb[i][0], tlb[i][1]);
	}
	fclose (f);
	exit(0);
}
