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
	unsigned int counter, hashAddress = 0;
	for (counter = 0; word[counter]!='\0'; counter++){
  		hashAddress = hashAddress*word[counter] + word[counter] + counter;
	}
	return (hashAddress%hashTableSize);
}

bool redimensionar(hash_t * hash, size_t agrandar){
	hash_campo_t* aux = hash->tabla;
	size_t nueva_capacidad = agrandar ? hash->capacidad * 2 : hash->capacidad / 2;
	hash->tabla = malloc(sizeof(hash_campo_t) * (nueva_capacidad));
	if (!aux) return false;
	size_t vieja_capacidad = hash->capacidad;
	hash->capacidad = nueva_capacidad;
	hash->carga = 0;
	hash->cantidad = 0;
	for(int i = 0; i < hash->capacidad; i++){
		hash->tabla[i].estado = VACIO;
	}
	for(int i = 0; i < vieja_capacidad; i++){
		if(aux[i].estado == OCUPADO){
			hash_guardar(hash, aux[i].clave, aux[i].valor);
			// hash->destruir_dato(aux[i].valor);
		}
	}
	free(aux);
	return true;
}

int buscar_clave(const hash_t* hash, const char* clave){
	unsigned int posicion = stringToHash(clave, (unsigned int)hash->capacidad);
	while(hash->tabla[posicion].estado != VACIO){
		if(strcmp(hash->tabla[posicion].clave, clave) == 0 && hash->tabla[posicion].estado == OCUPADO) return posicion;
		posicion++;
		if(posicion == hash->capacidad){
			posicion = 0;
		}
	}
	return -1;
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
	if(hash->carga * 100 / hash->capacidad > FACTOR_AGRANDAMIENTO){
		redimensionar(hash, 1);
	}
	unsigned int posicion;
	posicion = buscar_clave(hash, clave);
	if(posicion != -1){
		hash->tabla[posicion].valor = dato;
		return true;
 	}
	posicion = stringToHash(clave, (unsigned int)hash->capacidad);
	while(hash->tabla[posicion].estado == OCUPADO){
		posicion++;
		if(posicion == hash->capacidad){
			posicion = 0;
		}
	}
	char* copia_clave = malloc(sizeof(char) * strlen(clave));
	strcpy(copia_clave,clave);
	hash->tabla[posicion].clave = copia_clave;
	hash->tabla[posicion].valor = dato;
	hash->tabla[posicion].estado = OCUPADO;
	hash->cantidad++;
	hash->carga++;
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	bool reducir = hash->cantidad * 100 / hash->capacidad < FACTOR_REDUCCION &&
								 hash->capacidad > TAM_INICIAL;
	if(reducir){
		redimensionar(hash, 0);
	}
	int posicion = buscar_clave(hash, clave);
	if(posicion == -1) return NULL;
	void* dato = hash->tabla[posicion].valor;
	hash->tabla[posicion].estado = BORRADO;
	// if(hash->destruir_dato){
	// 	hash->destruir_dato(hash->tabla[posicion].valor);
	// }
	hash->cantidad--;
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	int posicion = buscar_clave(hash, clave);
	if(posicion == -1) return NULL;
	return hash->tabla[posicion].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	return buscar_clave(hash, clave) != -1;
}

size_t hash_cantidad(const hash_t *hash){
 return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	if(!hash) return;
	size_t posicion = 0;
	while (posicion < hash->capacidad) {
		if(hash->tabla[posicion].estado == OCUPADO && hash->destruir_dato){
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
	if (iter == NULL) return NULL;

	iter->hash = (hash_t*) hash;
	iter->posicion = 0;
	while(hash->tabla[iter->posicion].estado != OCUPADO){
		iter->posicion++;
		if(iter->posicion == iter->hash->capacidad)
			break;
	}
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (iter->posicion == iter->hash->capacidad) return false;

	do{
		iter->posicion++;
	}while(iter->posicion < iter->hash->capacidad && iter->hash->tabla[iter->posicion].estado != OCUPADO);

	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (iter->posicion == iter->hash->capacidad) return NULL;
	size_t posicion = iter->posicion;
	return (iter->hash->tabla[posicion].clave);
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if (iter->posicion == iter->hash->capacidad) return true;
	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
