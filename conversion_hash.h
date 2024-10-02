#ifndef CONVERSION_HASH
#define CONVERSION_HASH
// MD5 128 bits -> 16 octets
#define MD5_HASH_BYTES 16
#define MAX_SIZE_LINE 128

#define FALSE 0
#define TRUE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "md5.h"

void print_hash(uint8_t *);
void hash(uint8_t *);
int hex_to_integer(uint8_t);
void string_to_hex(const char *, uint8_t *, int);
void recuperer_ligne_fichier(char **, int , const char *);


#endif