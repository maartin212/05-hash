#include "hash.h"
#include <stdlib.h>
#include <stdio.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

 struct hash{

 };
 struct hash_iter{

 };

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

 hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

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
