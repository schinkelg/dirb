/*
 * DIRB
 *
 * funciones.h - Global Functions
 *
 */

#include "global.h"

/* Funciones */
// dirb.c
void banner(void);
void usage(void);

// get_url.c
struct result get_url(char *resp_url);
size_t get_header(char *ptr, size_t size, size_t nmemb);
size_t get_body(void *ptr, size_t size, size_t nmemb);

// lanza_ataque.c
void lanza_ataque(char *url_base, struct words *wordlist);

// options.c
void get_options(void);

// calculanec.c
int get_necs(const char *direccion);
struct result *calcula_nec(const char *direccion);

// crea_wordlist.c
struct words *crea_wordlist(const char *ficheros);
struct words *crea_wordlist_fich(char *fichero);
struct words *crea_extslist(char *lista);
int count_words(const struct words *list);

// utils.c
void limpia_url(char *limpia);
void guardadir(char *direccion);
void elimina_dupwords(struct words *puntero);
FILE *abrir_file(char *file);
int location_cmp(char *A, char *B);
void location_clean(char *cleaned, char *toelim);
void check_url(char *url);
int islistable(char *direccion);
char kbhit(void);
void cierre(void);
char *code2string(struct code *a, u_int v);
void init_exts(void);
