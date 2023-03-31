#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include "holdall.h"
#include "hashtable.h"
#include "ws.h"

//MACROCONSTANTES
#define DEFAULT_WORD_LEN 63
#define DEFAULT_WORD_NUMBER 10
#define NB_OCC_MAX INT_MAX
#define STD_STREAM "stdin"
#define MIN_FILES 2
#define VALID_OPTIONS {"same-numbers", "top", "uppercasing", \
                       "punctuation-like-space", "initial", "help", NULL}

//MACROFONCTIONS
#define DISPLAY_ERR(s1, s2) {                                                     \
    fprintf(stderr, "'%s' : %s\nTry 'ws --help' for more information.\n", s1, \
    s2);   \
}

#define DISPLAY_HELP { \
    printf("USAGE ws [OPTION] ... FILES\n\n"); \
    printf("Print a list of words shared by text files.\n\n"); \
    printf("Program information :\n\n"); \
    printf("-h,--help\tPrint this help message and exit.\n\n"); \
    printf("Input Control\n\n"); \
    printf( \
    "-i, --initial=VALUE\tSet the maximal number of significant initial letters for words to VALUE. 0 means no limitation. Default is 63.\n\n"); \
    printf( \
    "-p, --punctuation-like-space\tMake the punctuation characters play the same role as space characters in the meaning of words.\n\n"); \
    printf( \
    "-u, --uppercasing\tConvert each lowercase letter of words of the corresponding uppercase letter.\n\n"); \
    printf("Output Control\n\n"); \
    printf( \
    "-s, --same-numbers\tPrint more words than the limit in case of same numbers.\n\n"); \
    printf( \
    "-t, --top=VALUE\tSet the maximal number of words to print to VALUE. 0 means no limitation. Default is 10.\n\n"); \
    printf( \
    "The number of FILES that can be taken into account is between 2 and 63.\n"); \
}

//Définition de la structure word, contient la chaine de caracteres, la
// multiplicité et la présence dans les fichiers d'un mot
typedef struct word word;

struct word {
  char *value;
  size_t mult;
  size_t scheme;
};

//----------------------------------Outils-------------------------------------
//rfree : libére la zone mémoire pointée par ptr
int rfree(void *ptr);

//str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//et Pike pour les chaines de caractères.
size_t str_hashfun(const char *s);

//compar : gère le tri en fonction la présence dans les fichiers, la
// multiplicité et l'ordre lexicographique de chaque mot
int compar(word *w1, word *w2);

//-------------------------- Gestion de l'affichage ---------------------------
int display(size_t *nb_files, word *w, void *fun1);
//fun1 : traite les adresses du fourretout pour préparer l'affichage
void *fun1(int *tflag, word *ptr);
//fun2 : traite les adresses du fourretout pour préparer l'affichage avec
// l'option -s
void *fun2(int t[], word *ptr);

//------------------------------Fonction principale----------------------------

int main(int argc, char *argv[]) {
  //Initialisations
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *has = holdall_empty();
  const char *valid_options[7] = VALID_OPTIONS;
  char *strtod_end;
  char *files[MAX_FILES];
  size_t current_file = 0;
  int r = EXIT_SUCCESS;
  int tvalue = DEFAULT_WORD_NUMBER;
  int ivalue = DEFAULT_WORD_LEN;
  int sflag = false;
  int uflag = false;
  int pflag = false;
  int errflag = false;
  int o;
  char f_str[] = STD_STREAM;
  //Récupération des options et des fichiers
  while ((o = get_files_options(argc, argv, valid_options)) != -1) {
    switch (o) {
      case 't':
        tvalue = arg_check(&strtod_end);
        errflag = (tvalue == -1);
        break;
      case 'i':
        ivalue = arg_check(&strtod_end);
        errflag = (ivalue == -1);
        break;
      case 's':
        if (optarg != NULL) {
          DISPLAY_ERR(optopt, "Unexpected argument.");
          errflag = true;
        } else {
          sflag = true;
        }
        break;
      case 'u':
        if (optarg != NULL) {
          DISPLAY_ERR(optopt, "Unexpected argument.");
          errflag = true;
        } else {
          uflag = true;
        }
        break;
      case 'p':
        if (optarg != NULL) {
          DISPLAY_ERR(optopt, "Unexpected argument.");
          errflag = true;
        } else {
          pflag = true;
        }
        break;
      case '!':
        if (append_file(optopt, files) != 0) {
          DISPLAY_ERR(optopt, "Max file number reached.");
          errflag = true;
        }
        break;
      case 'h':
        goto help;
        break;
      case '-':
        if (append_file(f_str, files) != 0) {
          DISPLAY_ERR(optopt, "Max file number reached.");
          errflag = true;
        }
        break;
      case '?':
        DISPLAY_ERR(optopt, "Invalid option or file.");
        errflag = true;
        break;
      default:
        errflag = true;
        break;
    }
    //Récupération d'erreur sur la lecture des options et fichiers
    if (errflag) {
      goto error_option;
    }
  }
  //Comptage du nombre de fichiers
  size_t nb_files = file_count(files);
  if (nb_files < 2) {
    goto error_file;
  }
  //Boucle principale
  bool iflag = (ivalue == 0);
  int c;
  while (current_file != nb_files) {
    bool fflag = strcmp(STD_STREAM, files[current_file]) != 0;
    FILE *file = fflag ? fopen(files[current_file], "r") : stdin;
    //Boucle du fichier courant
    while ((c = getc(file)) != EOF) {
      size_t word_length_max = iflag ? DEFAULT_WORD_LEN : (size_t) ivalue;
      char *w = malloc(word_length_max + 1);
      if (w == NULL) {
        goto error_capacity;
      }
      size_t i = 0;
      //Boucle du mot courant
      while (c != EOF && !isspace(c) && !(pflag && ispunct(c))) { //Gestion de
                                                                  // l'option
                                                                  // --punctuation-like-space
        //Gestion de l'option --uppercasing
        if (uflag) {
          c = toupper(c);
        }
        w[i] = (char) c;
        c = getc(file);
        i++;
        if (i == word_length_max) { //Gestion de l'option --initial
          if (iflag) {
            word_length_max *= 2;
            w = realloc(w, word_length_max + 1);
            if (w == NULL) {
              goto error_capacity;
            }
          } else {
            w[i] = '\0';
            if (!isspace(c)) {
              fprintf(stderr, "Word %s... sliced.\n", w);
              while (c != EOF && !isspace(c) && !(pflag && ispunct(c))) {
                c = fgetc(file);
              }
            }
          }
        }
      }
      //Si le mot lu est vide, ne pas l'ajouter à notre table
      if (i == 0) {
        free(w);
        continue;
      }
      w[i] = '\0';
      //Ajout/mise à jour du mot courant dans notre table
      word *cptr = (word *) hashtable_search(ht, w);
      if (cptr != NULL) {
        if (cptr->mult < NB_OCC_MAX) {
          ++cptr->mult;
        }
        cptr->scheme += add_file(cptr->scheme, current_file, nb_files);
      } else {
        char *s = malloc(strlen(w) + 1);
        if (s == NULL) {
          goto error_capacity;
        }
        strcpy(s, w);
        word *wd = malloc(sizeof *wd);
        if (wd == NULL) {
          goto error_capacity;
        }
        wd->value = s;
        wd->mult = 1;
        wd->scheme = ppow(2, nb_files - current_file - 1);
        if (holdall_put(has, wd) != 0) {
          free(wd);
          goto error_capacity;
        }
        if (hashtable_add(ht, wd->value, wd) == NULL) {
          goto error_capacity;
        }
      }
      free(w);
    }
    if (!feof(file)) {
      goto error_read;
    }
    if (fflag) {
      fclose(file);
    } else {
      clearerr(stdin);
    }
    current_file++;
  }
  if (tvalue == 0) {
    tvalue = (int) holdall_count(has);
  }
  //Tri du fourretout
  holdall_sort(has, (int (*)(const void *, const void *))compar);
  printf("--- Info: Number of distinct words: %zu.\n",
      holdall_count(has));
  //Gestion de l'option --same-numbers
  if (sflag) {
    int smult = 0;
    int sfile = 0;
    int t[3] = {
      tvalue, smult, sfile
    };
    holdall_apply_context2(has,
        t, (void *(*)(void *, void *))fun2, &nb_files,
        (int (*)(void *, void *, void *))display);
  } else {  //Gestion de l'option --top
    holdall_apply_context2(has,
        &tvalue, (void *(*)(void *, void *))fun1, &nb_files,
        (int (*)(void *, void *, void *))display);
  }
  goto dispose;
  //Labels pour la gestion des erreurs
error_option:
  fprintf(stderr, "*** Error: an option/file read error occured.\n");
  goto error;
error_file:
  fprintf(stderr, "*** Error: Need at least 2 files.\n");
  goto error;
error_capacity:
  fprintf(stderr, "*** Error: Not enough memory.\n");
  goto error;
error_read:
  fprintf(stderr, "*** Error: A read error occured.\n");
  goto error;
error:
  r = EXIT_FAILURE;
  goto dispose;
help:
  DISPLAY_HELP;
  goto dispose;
dispose:
  hashtable_dispose(&ht);
  if (has != NULL) {
    holdall_apply(has, rfree);
  }
  holdall_dispose(&has);
  return r;
}

//Fin de la fonction principale

//----------------------------------Outils-------------------------------------
size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int rfree(void *ptr) {
  free(((word *) ptr)->value);
  free(ptr);
  return 0;
}

//-------------------------- Gestion de l'affichage ---------------------------
int display(size_t *nb_files, word *w, void *fun1) {
  if (fun1 == NULL) {
    return FUN_FAILURE;
  }
  if (w->mult == NB_OCC_MAX) {
    fprintf(stderr, "** Warning : Max occurences reached for %s.\n", w->value);
  }
  char s[*nb_files + 1];
  calc_scheme(w->scheme, *nb_files, s);
  return printf("%s\t%zu\t%s\n", s, w->mult, w->value) < 0;
}

void *fun1(int *tflag, word *ptr) {
  if (*tflag == 0) {
    return NULL;
  }
  *tflag -= 1;
  return count_file(ptr->scheme) >= 2 ? ptr : NULL;
}

void *fun2(int t[], word *ptr) {
  if (t[0] == 1 && t[1] == 0) {
    t[1] = (int) ptr->mult;
    t[2] = (int) count_file(ptr->scheme);
  }
  if (t[0] == 0 && t[1] != 0) {
    printf("%d  %zu\n", t[2], ptr->scheme);
    return ((int) ptr->mult == t[1]
           && (int) count_file(ptr->scheme) == t[2]) ? ptr : NULL;
  }
  t[0] -= 1;
  return count_file(ptr->scheme) >= 2 ? ptr : NULL;
}

//-------------------------- Fonction de comparaison --------------------------

int compar(word *w1, word *w2) {
  int r = (int) (count_file(w1->scheme) - count_file(w2->scheme));
  if (r == 0) {
    r = (int) (w1->mult - w2->mult);
  }
  if (r == 0) {
    r = strcmp(w2->value, w1->value);
  }
  return r;
}
