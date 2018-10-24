#include "hash.h"
#include <stdlib.h>
#include <stdio.h>

#define TAM_INICIAL 20

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
    size_t largo;
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
 	hash->largo = TAM_INICIAL;
 	hash->carga = 0;

 	hash->tabla = malloc(sizeof(hash_campo_t) * TAM_INICIAL);
 	if(hash->tabla == NULL){
 		free(hash);
 		return NULL;
 	}
 	return hash;
 }


 bool hash_guardar(hash_t *hash, const char *clave, void *dato){

 }


 void *hash_borrar(hash_t *hash, const char *clave){

 }
 void *hash_obtener(const hash_t *hash, const char *clave){

 }
 bool hash_pertenece(const hash_t *hash, const char *clave){

 }
 size_t hash_cantidad(const hash_t *hash){

 }
 void hash_destruir(hash_t *hash){

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
