#include "hash.h"
#include "testing.h"
#include <stddef.h>
#include <stdio.h>


void pruebas_hash_alumno(void){
  hash_t* ejemplo = NULL;
  print_test("hash inicializado a NULL", ejemplo == NULL);
}
