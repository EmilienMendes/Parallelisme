#include <mpi.h>
#include <omp.h>
#include <time.h>
#include "conversion_hash.h"

// Affichage hash sous format hexadecimal
void print_hash(uint8 *buff)
{
    for (int i = 0; i < MD5_HASH_BYTES; i++)
    {
        printf("%02x", buff[i]);
    }
    printf("\n");
}

// Hachage de notre chaine de caracteres
void hash(uint8 *my_hash)
{
    uint8 buff[MD5_HASH_BYTES];
    calcul_md5(my_hash, MD5_HASH_BYTES, buff);
    memcpy(my_hash, buff, MD5_HASH_BYTES);
}

// Conversion d'un seul caractere en hexadecimal
int hex_to_integer(uint8 my_char)
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
void string_to_hex(const char *input, uint8 *output, int length)
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

int find_i_parallel(char *filesystem)
{
    // Prend les 3 premiers mots de passe de la liste
    int mot_de_passe_teste = 3;
    char **liste_mot_de_passe = (char **)malloc(mot_de_passe_teste * sizeof(char *));
    recuperer_ligne_fichier(liste_mot_de_passe, mot_de_passe_teste, "liste_mots_de_passe/high_frequency_passwords_list.txt");

    // Recuper les 10 chaines de haches du systeme
    int nb_mot_de_passe_systeme = 10;
    char **liste_mots_de_passe_systeme = (char **)malloc(nb_mot_de_passe_systeme * sizeof(char *));
    recuperer_ligne_fichier(liste_mots_de_passe_systeme, nb_mot_de_passe_systeme, filesystem);

    // Recuperer les haches sous format hexadecimal
    unsigned char **liste_mots_de_passe_systeme_hexa = (unsigned char **)malloc(nb_mot_de_passe_systeme * sizeof(char *));
    for (int l = 0; l < nb_mot_de_passe_systeme; l++)
    {
        liste_mots_de_passe_systeme_hexa[l] = (unsigned char *)malloc(sizeof(char) * (MD5_HASH_BYTES));
        string_to_hex(liste_mots_de_passe_systeme[l], liste_mots_de_passe_systeme_hexa[l], MD5_HASH_BYTES);
    }

    int i, k = 0;
    unsigned int i_trouve = 0;
    int trouve = FALSE;
    omp_set_num_threads(mot_de_passe_teste);

#pragma omp parallel private(i) shared(trouve, liste_mots_de_passe_systeme_hexa)
    {
        int id = omp_get_thread_num();
        char *pwd = liste_mot_de_passe[id];
        uint8 my_hash[MD5_HASH_BYTES];

        // On fait le premier hash hors de la fonction pour avoir une taille standard de 16 octets
        calcul_md5((unsigned char *)pwd, strlen(pwd) - 1, my_hash);

        uint32 multiplier = 50000000;
        uint32 compteur = 1;
        for (i = 0; i < 10 && !trouve; i++)
        {
            for (unsigned int j = 0; j < multiplier - 1; j++)
            {
                hash(my_hash);
                compteur++;
            }
            // Comme on a fais un hash pour la premiere itteration, on hash une fois de moins pour i = 0
            if (i != 0)
            {
                hash(my_hash);
                compteur++;
            }
            for (k = 0; k < nb_mot_de_passe_systeme; k++)
            {
                if (memcmp(liste_mots_de_passe_systeme_hexa[k], my_hash, MD5_HASH_BYTES) == 0)
                {
                    trouve = TRUE;
// Atomic + rapid que critical, cependant operation arithmetique seulement donc on commence i_trouve a 0 pour i ajouter la valeur de i trouve
#pragma omp atomic
                    i_trouve += (i + 1) * multiplier;

                    printf("Process %d a trouve pour la valeur de i : %ld  pour le systeme %s \n", id, (i + 1) * multiplier, filesystem);
                }
            }
        }
    }

    for (int i = 0; i < mot_de_passe_teste; i++)
    {
        free(liste_mot_de_passe[i]);
    }

    for (int i = 0; i < nb_mot_de_passe_systeme; i++)
    {
        free(liste_mots_de_passe_systeme[i]);
        free(liste_mots_de_passe_systeme_hexa[i]);
    }

    free(liste_mot_de_passe);
    free(liste_mots_de_passe_systeme);
    free(liste_mots_de_passe_systeme_hexa);

    return i_trouve;
}

char *find_root(char *filesystem, unsigned int i_trouve)
{
    // Prend les 10  mots de passe de la liste
    int mot_de_passe_teste = 10;
    char **liste_mot_de_passe = (char **)malloc(mot_de_passe_teste * sizeof(char *));
    recuperer_ligne_fichier(liste_mot_de_passe, mot_de_passe_teste, "liste_mots_de_passe/high_frequency_passwords_list.txt");

    // Recupere les 10 chaines de haches du systeme
    int nb_mot_de_passe_systeme = 1;
    char **liste_mots_de_passe_systeme = (char **)malloc(nb_mot_de_passe_systeme * sizeof(char *));
    recuperer_ligne_fichier(liste_mots_de_passe_systeme, nb_mot_de_passe_systeme, filesystem);

    // Recuperer le hache du root
    unsigned char *mot_de_passe_systeme_hexa = (unsigned char *)malloc(sizeof(char) * (MD5_HASH_BYTES));
    string_to_hex(liste_mots_de_passe_systeme[0], mot_de_passe_systeme_hexa, MD5_HASH_BYTES);

    int thread_trouve = -1;
    omp_set_num_threads(mot_de_passe_teste);
#pragma omp parallel shared(thread_trouve, mot_de_passe_systeme_hexa)
    {
        int id = omp_get_thread_num();

        char *pwd = liste_mot_de_passe[id];
        int len_pwd = strlen(pwd) - 1;

        uint8 my_hash[MD5_HASH_BYTES];

        // On fait le premier hash hors de la fonction pour avoir une taille standard de 16 octets
        calcul_md5((unsigned char *)pwd, len_pwd, my_hash);

        uint32 compteur = 1;
        for (unsigned int i = 0; i < i_trouve - 1; i++)
        {
            hash(my_hash);
            compteur++;
        }
        if (memcmp(mot_de_passe_systeme_hexa, my_hash, MD5_HASH_BYTES) == 0)
        {
// Atomic + rapid que critical, cependant operation arithmetique seulement donc on commence i_trouve a 0 pour i ajouter la valeur de i trouve
#pragma omp atomic
            thread_trouve += id + 1;
        }
    }
    int mem_size = sizeof(liste_mot_de_passe[thread_trouve]);
    char *mdp_root = (char *)malloc(mem_size);
    memcpy(mdp_root, liste_mot_de_passe[thread_trouve], mem_size);

    for (int i = 0; i < mot_de_passe_teste; i++)
    {
        free(liste_mot_de_passe[i]);
    }

    for (int i = 0; i < nb_mot_de_passe_systeme; i++)
    {
        free(liste_mots_de_passe_systeme[i]);
    }

    free(liste_mot_de_passe);
    free(liste_mots_de_passe_systeme);
    free(mot_de_passe_systeme_hexa);
    return mdp_root;
}

void MPI_version()
{
    double debut;
    double fin;
    debut = omp_get_wtime();
    MPI_Init(NULL, NULL);
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    char buffer[64];
    while (world_rank < 5)
    {
        sprintf(buffer, "liste_mots_de_passe/systeme_%d.phl", world_rank + 1);
        // Travail
        char *password = bruteforce_sequentiel(buffer) int i = find_i(buffer);
        char *pssword = find_root(buffer, i);
        printf("Mot de passe pour  %s :", buffer);
        printf("%s \n", password);
        world_rank += world_size
    }
    MPI_Finalize();
    fin = omp_get_wtime();
    return fin - debut;
}

int main(int argc, char **argv)
{
    long time = MPI_version();
    printf("Temps pris : %f secondes\n", time);
}