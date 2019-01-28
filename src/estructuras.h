/*
 * DIRB
 *
 * estructuas.h - Global Structures
 *
 */

#include "global.h"


/* Estructuras */

// Resultado de una peticion

struct result {
  char *url;
  CURLcode estado;
  int codigo_http;
  int head_size;
  int head_lines;
  int body_size;
  int body_words;
  int body_lines;
  char location[256];
  char server[256];
};


// Codigo HTTP

struct code {
  unsigned int codenum;
  char *desc;
};


// Lista de palabras

struct words {
  void *anterior;
  char *word;
  void *next;
};


/* Opciones de linea de comandos */

struct opciones {
  int use_vhost;
  int use_proxy;
  int use_proxypass;
  int use_pass;
  int use_cookie;
  int verify_ssl;
  int use_agent;
  int ignore_nec;
  int default_nec;
  int debug_level;
  int exitonwarn;
  int save_found;
  int mutations_file;
  int mutations_list;
  int extensions_file;
  int extensions_list;
  int saveoutput;
  int print_location;
  int port;
  int finetuning;
  int interactive;
  int nothide;
  int insensitive;
  int lasting_bar;
  int dont_recurse;
  int add_header;
  int silent_mode;
  int speed;

  /* Parametros de linea de comandos */

  char *mutation_file;
  char *mutation_list;
  char *exts_file;
  char *exts_list;
  char *proxy;
  char *pass_string;
  char *proxypass_string;
  char *cookie;
  char *url_inicial;
  char *mfile;
  char *agente;
  char *vhost;
  char *savefile;
  char *header_string;

  /* Parametros para resume */

  int recursion_level;
  char *current_word;
  char *current_dir;
};







