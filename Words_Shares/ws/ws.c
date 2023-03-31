#include "ws.h"

//Initialisations variables globales
size_t fileind = 0;
int argind = 1;
char *optopt = NULL;
char *optarg = NULL;

//-----------------------Fonctions options/fichiers----------------------------

int get_files_options(int argc, char *argv[], const char *valid_options[]) {
  if (argind == argc) {
    return -1;
  }
  int c = '?';
  char *s = argv[argind];
  optopt = s;
  optarg = NULL;
  argind++;
  if (s[0] != OPT_PREFIX) {
    if (check_file(s)) {
      return '!';
    } else {
      return c;
    }
  }
  size_t opt_len = strlen(s);
  if (opt_len == 1) {
    c = '-';
  }
  if (opt_len == OPT_PREFIX_LEN + 1) {
    for (size_t i = 0; valid_options[i] != NULL; i++) {
      if (s[1] == valid_options[i][0]) {
        c = s[1];
      }
    }
    if (argv[argind] != NULL) {
      char *arg = argv[argind];
      if (!check_file(arg) && arg[0] != OPT_PREFIX) {
        optarg = arg;
        argind++;
      }
    }
  }
  if (opt_len > 2 && s[1] == OPT_PREFIX) {
    optarg = strchr(s, '=');
    if (optarg != NULL) {
      s[opt_len - strlen(optarg)] = '\0';
      optarg++;
      if (*optarg == '\0') {
        optarg = NULL;
      }
    }
    s += 2;
    for (size_t i = 0; valid_options[i] != NULL; i++) {
      if (strcmp(s, valid_options[i]) == 0) {
        c = s[0];
      }
    }
  }
  return c;
}

bool check_file(char *file) {
  size_t name_len = strlen(file);
  if (name_len > EXTENSION_LENGTH) {
    char *ext_ptr = file + name_len - EXTENSION_LENGTH;
    if (strcmp(ext_ptr, FILE_EXTENSION) == 0) {
      FILE *fptr = fopen(file, "r");
      if (fptr != NULL) {
        fclose(fptr);
        return true;
      }
    }
  }
  return false;
}

int append_file(char *file, char **files) {
  if (fileind == MAX_FILES) {
    return FUN_FAILURE;
  }
  files[fileind] = file;
  files[fileind + 1] = NULL;
  fileind++;
  return FUN_SUCCESS;
}

size_t file_count(char **files) {
  size_t count = 0;
  while (files[count] != NULL) {
    count++;
  }
  return count;
}

int arg_check(char **endptr) {
  if (optarg == NULL) {
    printf("'%s' : Expected argument\n", optopt);
    return FUN_FAILURE;
  }
  int value = (int) strtod(optarg, endptr);
  if (**endptr != '\0') {
    printf("'%s' : Invalid argument\n", *endptr);
    return FUN_FAILURE;
  }
  if (value < 0) {
    printf("'%s' : Negative argument\n", optopt);
    return FUN_FAILURE;
  }
  return value;
}

//-----------------------------Fonctions mots----------------------------------

size_t add_file(size_t scheme, size_t n_file, size_t files_nb) {
  return is_already_in_scheme(scheme, n_file, files_nb)
         ? 0
         : ppow(2, files_nb - n_file - 1);
}

bool is_already_in_scheme(size_t scheme, size_t n_file,
    size_t files_nb) {
  size_t n = files_nb;
  while (n != 0) {
    if (scheme >= ppow(2, n - 1)) {
      if (n_file == files_nb - n) {
        return true;
      }
      scheme -= ppow(2, n - 1);
    }
    if (files_nb == n_file - n - 1) {
      return false;
    }
    n--;
  }
  return false;
}

void calc_scheme(size_t scheme, size_t files_nb, char *s) {
  size_t i = 0;
  while (files_nb != 0) {
    size_t p = ppow(2, files_nb - 1);
    if (scheme >= p) {
      s[i] = FILE_PRESENCE;
      scheme -= p;
    } else {
      s[i] = FILE_ABSENCE;
    }
    i++;
    --files_nb;
  }
  s[i] = '\0';
}

size_t count_file(size_t scheme) {
  size_t n = 0;
  size_t max_power = 1;
  while (scheme > ppow(2, max_power)) {
    ++max_power;
  }
  while (scheme != 0) {
    if (scheme >= ppow(2, max_power)) {
      n++;
      scheme -= ppow(2, max_power);
    }
    --max_power;
  }
  return n;
}

size_t ppow(size_t n, size_t pow) {
  size_t r = 1;
  for (size_t i = 0; i < pow; ++i) {
    r *= n;
  }
  return r;
}
