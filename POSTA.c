#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAM_INICIAL 30
#define FACTOR_AGRANDAMIENTO 70
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
	hash_campo_t *tabla; 
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

/* ******************************************************************
*                    FUNCIONES AUXILIARES
* *****************************************************************/


bool redimensionar(hash_t * hash, float variacion){
	size_t nueva_capacidad = hash->capacidad * variacion;
	hash_campo_t * nueva_tabla = malloc(sizeof(hash_campo_t) * nueva_capacidad);
	if(nueva_tabla == NULL) return false;

	for (int i = 0 ; i < nueva_capacidad ; i++){
		nueva_tabla[i].estado = VACIO;
	}
	
	for (int i = 0 ; i < hash->capacidad ; i ++){

		if (hash->tabla[i].estado == OCUPADO){

			size_t posicion = stringToHash(hash->tabla[i].clave,nueva_capacidad);
			while(nueva_tabla[posicion].estado == OCUPADO){
				posicion++;

				if(posicion == nueva_capacidad) posicion = 0;
			}
			
			nueva_tabla[posicion].clave = hash->tabla[i].clave;
			nueva_tabla[posicion].valor = hash->tabla[i].valor;
			nueva_tabla[posicion].estado = OCUPADO;
		}
	}
	free(hash->tabla);
	hash->carga = hash->capacidad;
	hash->capacidad = nueva_capacidad;
	hash->tabla = nueva_tabla;
	
	return true;
}

/* ******************************************************************
*                    PRIMITIVAS DEL HASH
* *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t * hash = malloc(sizeof(hash_t));
	if(hash == NULL) return NULL;

	hash->tabla = malloc(sizeof(hash_campo_t) * TAM_INICIAL);
	if(hash->tabla == NULL){
		free(hash);
		return NULL;
	}

	hash->cantidad = 0;
	hash->carga = 0;
	hash->capacidad = TAM_INICIAL;
	hash->destruir_dato = destruir_dato;
	for (int i = 0 ; i < TAM_INICIAL ; i++){
		hash->tabla[i].estado = VACIO;
	}
	return hash;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if ( (hash->carga * 100 / hash->capacidad) > FACTOR_AGRANDAMIENTO){
		if(!redimensionar(hash,2)) return false;
	}
	size_t posicion = stringToHash(clave,hash->capacidad);

	if (hash_pertenece(hash,clave)){
		while (strcmp(hash->tabla[posicion].clave,clave) != 0 ){
			posicion++;
			if(posicion == hash->capacidad) posicion = 0;
		}
		
		if (hash->destruir_dato){
			hash->destruir_dato(hash->tabla[posicion].valor);
		}


		hash->tabla[posicion].valor = dato;
		return true;
	}
	else{
		while(hash->tabla[posicion].estado != VACIO){
			posicion++;
			if(posicion == hash->capacidad) posicion = 0;
		}
	}
	char *copia_clave = strdup(clave);
	
	hash->tabla[posicion].valor = dato;
	hash->tabla[posicion].estado = OCUPADO;
	hash->tabla[posicion].clave = copia_clave;
	hash->cantidad++;
	hash->carga++;

	
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	if (hash->carga * 100/ hash->capacidad < FACTOR_REDUCCION && hash->capacidad > TAM_INICIAL){
		if(!redimensionar(hash,0.5)) return NULL;
	}
	if (! hash_pertenece(hash,clave)) return NULL;

	size_t posicion = stringToHash(clave,hash->capacidad);
	while (strcmp(hash->tabla[posicion].clave,clave) != 0){
		posicion++;
		if(posicion == hash->capacidad) posicion = 0;
	}
	hash->tabla[posicion].estado = BORRADO;
	hash->cantidad--;
	return(hash->tabla[posicion].valor);
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if (!hash_pertenece(hash,clave)) return NULL;

	size_t posicion = stringToHash(clave,hash->capacidad);
	while(strcmp(hash->tabla[posicion].clave,clave) != 0){
		posicion ++;
		if(posicion == hash->capacidad) posicion = 0;
	}
	return hash->tabla[posicion].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t posicion = stringToHash(clave,hash->capacidad);
	size_t referencia = posicion;

	while(hash->tabla[posicion].estado != VACIO){
		if (strcmp(hash->tabla[posicion].clave,clave) == 0 && hash->tabla[posicion].estado == OCUPADO)
			return true;
		posicion++;
		if(posicion == hash->capacidad) posicion = 0;
		if(posicion == referencia) return false;
	}
	return false;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for (int i  = 0 ; i < hash->capacidad ; i++){
		if (hash->tabla[i].estado != VACIO){
			free(hash->tabla[i].clave);
			
			if(hash->destruir_dato)
				hash->destruir_dato(hash->tabla[i].valor);
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
