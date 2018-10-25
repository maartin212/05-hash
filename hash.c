#include "hash.h"
#include <stdlib.h>
#include <stdio.h>

#define TAM_INICIAL 20
#define FACTOR_AGRANDAMIENTO 75
#define FACTOR_REDUCCION 25

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef enum estado{
	VACIO,OCUPADO,BORRADO
} estado_t;


typedef struct hash_campo{
  char *clave;
  void *valor;
  estado_t estado;
} hash_campo_t;

struct hash{
	size_t cantidad;
	size_t capacidad;
	size_t carga;
	hash_campo_t *tabla; //hash->tabla[i].clave
	hash_destruir_dato_t destruir_dato;
};


struct hash_iter{
	hash_t *hash;
	size_t posicion;
};

/* ******************************************************************
*                    FUNCION DE HASH
https://stackoverflow.com/questions/14409466/simple-hash-functions
* *****************************************************************/

unsigned int stringToHash(char *word, unsigned int hashTableSize){
unsigned int counter, hashAddress =0;
for (counter =0; word[counter]!='\0'; counter++){
  hashAddress = hashAddress*word[counter] + word[counter] + counter;
}
return (hashAddress%hashTableSize);
}

/* ******************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t * hash = malloc(sizeof(hash_t));
	if(hash == NULL) return NULL;

	hash->cantidad = 0;
	hash->destruir_dato = destruir_dato;
	hash->capacidad = TAM_INICIAL;
	hash->carga = 0;

	hash->tabla = malloc(sizeof(hash_campo_t) * TAM_INICIAL);
	if(hash->tabla == NULL){
		free(hash);
		return NULL;
	}
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if(hash->carga * 100 / hash->capacidad > FACTOR_AGRANDAMIENTO){
		//VER COMO SACAR LOS BORRADOS
		hash_campo_t* aux = realloc(hash->tabla, sizeof(hash_campo_t) * 2 * hash->capacidad);
		if(!aux) return 0;
		hash->tabla = aux;
		hash->capacidad *= 2;
	}
	unsigned int posicion;
	posicion = buscar_clave(hash, clave);
	if(!posicion){
		posicion = stringToHash(clave, hash->capacidad);
		while(hash->tabla[posicion]){
			posicion++;
		}
	}
	hash->tabla[posicion].clave = clave;
	hash->tabla[posicion].valor = dato;
	hash->tabla[posicion].estado = OCUPADO;
	hash->cantidad++;
	hash->carga++;
	return 1;
}

void *hash_borrar(hash_t *hash, const char *clave){
	if(hash->carga * 100 / hash->capacidad < FACTOR_REDUCCION){
		//VER COMO SACAR LOS BORRADOS
		hash_campo_t* aux = realloc(hash->tabla, sizeof(hash_campo_t) / 2 * hash->capacidad);
		if(!aux) return NULL;
		hash->tabla = aux;
		hash->capacidad /= 2;
	}
	unsigned int posicion = buscar_clave(hash, clave);
	if(!posicion) return NULL;
	void* dato = hash->tabla[posicion].valor;
	hash->tabla[posicion].estado = BORRADO;
	hash->hash_destruir_dato_t(hash->tabla[posicion].dato);
	hash->cantidad--;
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	unsigned int posicion = buscar_clave(hash, clave);
	if(!posicion) return NULL;
	return hash->tabla[posicion].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	return buscar_clave(hash, clave) != NULL;
}

size_t hash_cantidad(const hash_t *hash){
 return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	if(!hash) return NULL;
	unsigned int posicion = 0;
	while (posicion < hash->capacidad) {
		if(hash->tabla[posicion].estado == OCUPADO){
			hash->hash_destruir_dato_t(hash->tabla[posicion].valor);
		}
	}
	free(hash->tabla);
	free(hash);
}

unsigned int buscar_clave(const hash_t* hash, const char* clave){
	unsigned int posicion = stringToHash(clave, hash->capacidad);
	while(hash->tabla[posicion].estado == OCUPADO){
		if(hash->tabla[posicion].clave == clave) return posicion;
		posicion++;
		if(posicion > hash->capacidad){
			posicion = 0
		}
	}
	return NULL;
}

/* ******************************************************************
*                    PRIMITIVAS DEL ITERADOR
* *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){

}
bool hash_iter_avanzar(hash_iter_t *iter){

}
const char *hash_iter_ver_actual(const hash_iter_t *iter){

}
bool hash_iter_al_final(const hash_iter_t *iter){

}
void hash_iter_destruir(hash_iter_t* iter){

}
