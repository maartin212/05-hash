#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

unsigned int stringToHash(const char *word, unsigned int hashTableSize){
	unsigned int counter, hashAddress =0;
	for (counter =0; word[counter]!='\0'; counter++){
  		hashAddress = hashAddress*word[counter] + word[counter] + counter;
	}
	return (hashAddress%hashTableSize);
}

/* ******************************************************************
*                    FUNCIONES AUXILIARES
* *****************************************************************/
bool hash_guardar(hash_t *hash, const char *clave, void *dato);




/* ******************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t * hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;

	hash->tabla = malloc(sizeof(hash_campo_t) * TAM_INICIAL);
	if(! hash->tabla){
		free(hash);
		return NULL;
	}

	hash->cantidad = 0;
	hash->capacidad = TAM_INICIAL;
	hash->carga = 0;
	hash->destruir_dato = destruir_dato;
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato);


void *hash_borrar(hash_t *hash, const char *clave);


void *hash_obtener(const hash_t *hash, const char *clave){
	size_t posicion = stringToHash(clave,hash->capacidad);
	while (strcmp(clave,hash->tabla[posicion].clave) != 0){
		posicion++;
		if(hash->tabla[posicion].estado == VACIO || posicion == hash->capacidad) return NULL;
	}
	return hash->tabla[posicion].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t posicion = stringToHash(clave,hash->capacidad);
	while (strcmp(clave,hash->tabla[posicion].clave) != 0){
		posicion++;
		if(hash->tabla[posicion].estado == VACIO || posicion == hash->capacidad) return false;
	}
	return true;
}

size_t hash_cantidad(const hash_t *hash){
	return(hash->cantidad);
}

void hash_destruir(hash_t *hash){
	if(!hash) return;
	size_t posicion = 0;
	while (posicion < hash->capacidad) {
		if(hash->tabla[posicion].estado == OCUPADO){
			hash->destruir_dato(hash->tabla[posicion].valor);
		}
		posicion++;
	}
	free(hash->tabla);
	free(hash);
}







/* ******************************************************************
*                    PRIMITIVAS DEL ITERADOR
* *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t * iter = malloc(sizeof(hash_iter_t));
	if (!iter) return NULL;

	iter->hash = (hash_t*) hash;
	iter->posicion = 0;
	while(hash->tabla[iter->posicion].estado != OCUPADO){
		iter->posicion++;
		if(iter->posicion == iter->hash->capacidad) return NULL;
	}
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	do{
		iter->posicion++;
		if(iter->posicion == iter->hash->capacidad) return false;
	}while(iter->hash->tabla[iter->posicion].estado != OCUPADO);

	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	size_t posicion = iter->posicion;
	return (iter->hash->tabla[posicion].clave);
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if (iter->posicion == iter->hash->capacidad - 1) return true;
	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}