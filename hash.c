#define _POSIX_C_SOURCE 200809L

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
https://stackoverflow.com/questions/20462826/hash-function-for-strings-in-c
* *****************************************************************/

size_t stringToHash (const char* word, size_t capacidad){
		size_t result = 5381;
		unsigned char *p;
		p = (unsigned char *) word;
		while (*p != '\0') {
			result = (result << 5) + result + *p;
			++p;
		}
		return result%capacidad;
}

/* ******************************************************************
*                    FUNCIONES AUXILIARES
* *****************************************************************/


hash_campo_t* crear_tabla(size_t capacidad){
	hash_campo_t* tabla = malloc(sizeof(hash_campo_t) * capacidad);
	if(!tabla) return false;
	for (int i = 0 ; i < capacidad ; i++){
		tabla[i].estado = VACIO;
	}
	return tabla;
}

size_t buscar_posicion_clave(const hash_t* hash, const char* clave){
	size_t posicion = stringToHash(clave, (unsigned int)hash->capacidad);
	while(hash->tabla[posicion].estado != VACIO){
		if(hash->tabla[posicion].estado == OCUPADO && (strcmp(hash->tabla[posicion].clave, clave) == 0)){
			return posicion;
		}
		posicion++;
		if(posicion == hash->capacidad) posicion = 0;
	}
	return posicion;
}


size_t buscar_posicion_vacia(const hash_campo_t* tabla, const char* clave, size_t capacidad){
	size_t posicion = stringToHash(clave, (unsigned int)capacidad);
	while(tabla[posicion].estado != VACIO){
		posicion++;
		if(posicion == capacidad) posicion = 0;
	}
	return posicion;
}

bool redimensionar(hash_t * hash, size_t variacion,bool agrandar){
	size_t nueva_capacidad = agrandar ? hash->capacidad * 2 : hash->capacidad / 2;

    	hash_campo_t * nueva_tabla = crear_tabla(nueva_capacidad);

    	for (int i = 0 ; i < hash->capacidad ; i ++){
    		if (hash->tabla[i].estado == OCUPADO){
    			size_t posicion = buscar_posicion_vacia(nueva_tabla, hash->tabla[i].clave, nueva_capacidad);
    			nueva_tabla[posicion].clave = hash->tabla[i].clave;
    			nueva_tabla[posicion].valor = hash->tabla[i].valor;
    			nueva_tabla[posicion].estado = OCUPADO;
    		}
    	}
    	free(hash->tabla);
    	hash->carga = hash->cantidad;
    	hash->capacidad = nueva_capacidad;
    	hash->tabla = nueva_tabla;

    	return true;
}


/* ******************************************************************
*                    PRIMITIVAS DEL HASH
* *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t * hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;

	hash->tabla = crear_tabla(TAM_INICIAL);
	if(!hash->tabla){
		free(hash);
		return NULL;
	}

	hash->cantidad = 0;
	hash->carga = 0;
	hash->capacidad = TAM_INICIAL;
	hash->destruir_dato = destruir_dato;
	return hash;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if ((hash->carga * 100 / hash->capacidad) > FACTOR_AGRANDAMIENTO){
		if(!redimensionar(hash,2,true)) return false;
	}

	size_t posicion = buscar_posicion_clave(hash, clave);
	if (hash_pertenece(hash,clave)){
		if (hash->destruir_dato) hash->destruir_dato(hash->tabla[posicion].valor);
		hash->tabla[posicion].valor = dato;
		return true;
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
	if (!hash_pertenece(hash,clave)) return NULL;
	if (hash->carga * 100/ hash->capacidad < FACTOR_REDUCCION && hash->capacidad > TAM_INICIAL){
		if(!redimensionar(hash,2,false)) return NULL;
	}

	size_t posicion = buscar_posicion_clave(hash, clave);
	hash->tabla[posicion].estado = BORRADO;
	hash->cantidad--;
	void* valor = hash->tabla[posicion].valor;
	free(hash->tabla[posicion].clave);
	if(hash->destruir_dato) hash->destruir_dato(hash->tabla[posicion].valor);
	return valor;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if (!hash_pertenece(hash,clave)) return NULL;
	size_t posicion = buscar_posicion_clave(hash, clave);
	return hash->tabla[posicion].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t posicion = buscar_posicion_clave(hash, clave);
	return hash->tabla[posicion].estado == OCUPADO && (strcmp(hash->tabla[posicion].clave, clave) == 0);
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for (int i  = 0 ; i < hash->capacidad ; i++){
		if (hash->tabla[i].estado == OCUPADO){
			free(hash->tabla[i].clave);
			if(hash->destruir_dato) hash->destruir_dato(hash->tabla[i].valor);
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
	if (!iter) return NULL;

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
	return iter->posicion == iter->hash->capacidad;
}

void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
