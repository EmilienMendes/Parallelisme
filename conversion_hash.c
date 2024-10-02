#include "conversion_hash.h"

// Affichage hash sous format hexadecimal
void print_hash(uint8_t *buff)
{
    for (int i = 0; i < MD5_HASH_BYTES; i++)
    {
        printf("%02x", buff[i]);
    }
    printf("\n");
}

// Hachage de notre chaine de caracteres
void hash(uint8_t *my_hash)
{
    uint8_t buff[MD5_HASH_BYTES];
    calcul_md5(my_hash, MD5_HASH_BYTES, buff);
    memcpy(my_hash, buff, MD5_HASH_BYTES);
}

// Conversion d'un seul caractere en hexadecimal
int hex_to_integer(uint8_t my_char)
{
    if (my_char > '9')
    {
        return my_char + 10 - 'a';
    }
    return my_char - '0';
}

// https://www.includehelp.com/c/convert-ascii-string-to-hexadecimal-string-in-c.aspx

/*
Conversion de la chaine de caractere en caractere hexadecimal
*/
void string_to_hex(const char *input, uint8_t *output, int length)
{
    for (int i = 0; i < length; i++)
    {
        output[i] = hex_to_integer(input[2 * i]) * 16 + hex_to_integer(input[2 * i + 1]);
    }
}

// https://www.w3schools.com/c/c_files_read.php

void recuperer_ligne_fichier(char **buffer, int nb_ligne, const char *filename)
{
    FILE *fptr;
    if ((fptr = fopen(filename, "r")) == NULL)
    {
        printf("Le fichier n'existe pas \n");
        exit(1);
    }
    for (int i = 0; i < nb_ligne; i++)
    {
        buffer[i] = (char *)malloc(sizeof(char) * MAX_SIZE_LINE);
        fgets(buffer[i], sizeof(char) * MAX_SIZE_LINE, fptr);
    }
    fclose(fptr);
}
