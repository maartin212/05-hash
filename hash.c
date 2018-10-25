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

bool redimensionar(hash_t * hash,size_t nuevo_tam){
	hash_campo_t* aux = realloc(hash->tabla, sizeof(hash_campo_t) * (nuevo_tam*hash->capacidad));
	if (!aux) return false;

	hash->tabla = aux;
	for (int i = hash->capacidad; i < nuevo_tam*hash->capacidad ; i++){
		hash->tabla[i].estado = VACIO;
	}
	hash->capacidad *= nuevo_tam;
	return true;
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
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t * hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;

	hash->tabla = malloc(sizeof(hash_campo_t) * TAM_INICIAL);
	if(!hash->tabla){
		free(hash);
		return NULL;
	}
	hash->cantidad = 0;
	hash->capacidad = TAM_INICIAL;
	hash->carga = 0;
	hash->destruir_dato = destruir_dato;
	for (int i = 0 ; i < hash->capacidad ; i++){
		hash->tabla[i].estado = VACIO;
	}
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if ( hash->carga * 100/hash->capacidad > FACTOR_AGRANDAMIENTO){
		if(!redimensionar(hash,2)) return false;
	}
	size_t posicion = stringToHash(clave,hash->capacidad);
	if (strcmp(hash->tabla[posicion].clave,clave)){ // En caso de que la clave ya estaba guardada.
		hash->tabla[posicion].valor = dato;
		return true;
	}
	else if (hash->tabla[posicion].estado == OCUPADO){ // Colisiones
		while(hash->tabla[posicion].estado == OCUPADO){
			posicion++;
		}
	}
	hash->tabla[posicion].estado = OCUPADO;
	hash->tabla[posicion].valor = dato;
	strcpy(hash->tabla[posicion].clave,clave);
	hash->cantidad++;
	hash->carga++;

	return true;
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
	hash->hash_destruir_dato_t(hash->tabla[posicion].valor);
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



/* ******************************************************************
*                    PRIMITIVAS DEL ITERADOR
* *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t * iter = malloc(sizeof(hash_iter_t));
	if(!iter) return NULL;

	iter->posicion = 0;
	iter->hash = (hash_t*)hash;
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (iter->posicion < iter->hash->capacidad-1){
		iter->posicion++;
		return true;
	}
	return false;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	size_t posicion = iter->posicion;
	return(iter->hash->tabla[posicion].clave);
}

bool hash_iter_al_final(const hash_iter_t *iter){
	size_t posicion = iter->posicion;
	if (posicion + 1 == iter->hash->capacidad){
		return true;
	}
	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
