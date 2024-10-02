#include <time.h>
#include "conversion_hash.h"

int find_i(char *filesystem)
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

    uint32 multiplier = 50000000;
    int trouve = FALSE;
    for (int id = 0; id < mot_de_passe_teste; id++)
    {
        char *pwd = liste_mot_de_passe[id];
        uint8 my_hash[MD5_HASH_BYTES];
        // On fait le premier hash hors de la fonction pour avoir une taille standard de 16 octets
        calcul_md5((unsigned char *)pwd, strlen(pwd) - 1, my_hash);
        for (int i = 0; i < 10; i++)
        {
            for (unsigned int j = 0; j < multiplier - 1; j++)
            {
                hash(my_hash);
            }
            // Comme on a fais un hash pour la premiere itteration, on hash une fois de moins pour i = 0
            if (i != 0)
            {
                hash(my_hash);
            }
            for (int k = 0; k < nb_mot_de_passe_systeme; k++)
            {
                if (memcmp(liste_mots_de_passe_systeme_hexa[k], my_hash, MD5_HASH_BYTES) == 0)
                {
                    printf("Valeur de i : %ld  pour le systeme %s \n", (id + 1) * multiplier, filesystem);

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

                    return (i + 1) * multiplier;
                }
            }
        }
    }
    return -1;
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

    for (int id = 0; id < mot_de_passe_teste; id++)
    {
        char *pwd = liste_mot_de_passe[id];
        int len_pwd = strlen(pwd) - 1;
        uint8 my_hash[MD5_HASH_BYTES];
        calcul_md5((unsigned char *)pwd, len_pwd, my_hash);
        // On fait le premier hash hors de la fonction pour avoir une taille standard de 16 octets

        for (unsigned int i = 0; i < i_trouve - 1; i++)
        {
            hash(my_hash);
        }
        if (memcmp(mot_de_passe_systeme_hexa, my_hash, MD5_HASH_BYTES) == 0)
        {

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
            return pwd;
        }
    }
    return NULL;
}

double sequential_version()
{
    time_t debut;
    time_t fin;
    char buffer[64];
    debut = time(NULL);
    // Travail
    for (int j = 0; j < 5; j++)
    {
        sprintf(buffer, "liste_mots_de_passe/systeme_%d.phl", j + 1);
        int i = find_i(buffer);
        char *password = find_root(buffer, i);
        printf("Mot de passe pour  %s : ", buffer);
        printf("%s \n", password);
    }

    fin = time(NULL);
    return fin - debut;
}

int main(int argc, char **argv)
{
    double time = sequential_version();
    printf("Temps pris : %f secondes\n", time);
}