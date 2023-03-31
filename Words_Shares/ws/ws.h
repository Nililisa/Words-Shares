#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

//MACROSCONSTANTES
#define FILE_EXTENSION ".txt"
#define EXTENSION_LENGTH strlen(FILE_EXTENSION)
#define OPT_PREFIX '-'
#define OPT_PREFIX_LEN 1
#define MAX_FILES 63
#define FILE_PRESENCE 'x'
#define FILE_ABSENCE '-'
#define FUN_SUCCESS 0
#define FUN_FAILURE -1

//Variables globales
extern size_t fileind;
extern int argind;
extern char *optopt;
extern char *optarg;

//-----------------------Fonctions options/fichiers----------------------------

//get_files_options : lit argv à l'indice argc, renvoie un caractère
// représentant une option, un fichier ou une erreur si la chaîne lue représente
// une option valide au sens de valid_options, un fichier valide, ou autre chose
extern int get_files_options(int argc, char *argv[],
    const char *valid_options[]);

//check_file : renvoie true si le la chaine pointée par file représente un
// fichier texte existant, false sinon
extern bool check_file(char *file);

//file_count : renvoie le nombre de fichiers correctement lus sur l'entrée
extern size_t file_count(char **files);

//append_file : ajoute un pointeur sur file à la fin de files, retourne une
// valeur non nulle si le nombre maximum de fichiers est dépassé, 0 sinon
extern int append_file(char *file, char **files);

//arg_check : vérifie la validité d'un argument donné à une option, renvoie une
// valeur négative et affiche une erreur si l'argument est absent, invalide ou
// un nombre négatif, la valeur de cet argument sinon
extern int arg_check(char **endptr);

//-----------------------------Fonctions mots----------------------------------

//count_file : renvoie le nombre de fichier représenté par scheme
extern size_t count_file(size_t scheme);

//is_already_in_scheme : renvoie true si n_file est déjà dans scheme, false
// sinon
extern bool is_already_in_scheme(size_t scheme, size_t n_file, size_t files_nb);

//add_file : renvoie scheme ajouté de n_file
extern size_t add_file(size_t scheme, size_t n_file, size_t files_nb);

//calc_scheme : assigne à la chaine pointée par s la représentation graphique de
// scheme
extern void calc_scheme(size_t scheme, size_t files_nb, char *s);

//ppow : retourne n^pow
extern size_t ppow(size_t n, size_t pow);
