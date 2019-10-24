/*
 * DIRB
 *
 * utils.c - Utilidades varias
 *
 */

#include "dirb.h"


void limpia_url(char *limpia) {
  char *ptr;

  ptr=(char *)strchr(limpia, '\r');
  if(ptr!=0) *ptr='\0';
  ptr=(char *)strchr(limpia, '\n');
  if(ptr!=0) *ptr='\0';
  ptr=(char *)strchr(limpia, ' ');
  if(ptr!=0) *ptr='\0';

}


void guardadir(char *direccion) {

  if(!options.silent_mode) printf("                                                                               \r");
  IMPRIME("==> DIRECTORY: %s\n", direccion);
  options.recursion_level++;
  if(options.debug_level>4) printf("[++++] guardadir() RECURSION_LEVEL: %d\n", options.recursion_level);
  dirlist_final->word = malloc(strlen(direccion) + 1);
  strcpy(dirlist_final->word, direccion);
  dirlist_final->next=(struct words *)malloc(sizeof(struct words));
  memset(dirlist_final->next, 0, sizeof(struct words));
  dirlist_final=dirlist_final->next;
  existant=0;

}


/*
 * ABRIR_FILE: Abre un fichero de output
 *
 */

FILE *abrir_file(char *file) {
  FILE *desc;

  if((desc=fopen(file, "a"))==0) {
    printf("\n(!) FATAL: Error opening output file: %s\n", file);
    exit(-1);
    }

  return desc;

}


/*
 * LOCATION_CMP: Compara 2 cabeceras Location
 *
 */

int location_cmp(char *A, char *B) {
  int result=0;
  char *ptr=0;

  if(strncmp(A, "http://", 7)==0 || strncmp(A, "https://", 8)==0) {
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    }

  if(options.debug_level>4) printf("[++++] location_cmp() A[%zu]: '%s'\n", strlen(A), A);

  if(strncmp(B, "http://", 7)==0 || strncmp(B, "https://", 8)==0) {
    ptr=(char *)strchr(B, '/');
    if(ptr!=0) B=ptr+1;
    ptr=(char *)strchr(B, '/');
    if(ptr!=0) B=ptr+1;
    ptr=(char *)strchr(B, '/');
    if(ptr!=0) B=ptr+1;
    }

  if(options.debug_level>4) printf("[++++] location_cmp() B[%zu]: '%s'\n", strlen(B), B);

  result=strncmp(A, B, strlen(A)>strlen(B) ? strlen(A) : strlen(B));

  if(options.debug_level>4) printf("[++++] location_cmp() RESULT: %d (%zu)\n", result, strlen(A)>strlen(B) ? strlen(A) : strlen(B));

  return result;

}


/*
 * LOCATION_CLEAN: Limpia la cacebera location
 *
 */

void location_clean(char *cleaned, char *toelim) {
  char *ptr=0;
  char *A=cleaned;

  if(options.debug_level>3) printf("[+++] location_clean() TOCLEAN: %s | TOELIM: %s\n", cleaned, toelim);

  // Jump to uri-path
  if(strncmp(A, "http://", 7)==0 || strncmp(A, "https://", 8)==0) {
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A++;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A++;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A++;
  }

  // Remove arguments
  ptr= strchr(A, '?');
  if (0 != ptr) {
    *ptr = 0;
  }

  // Remove string
  ptr= strstr(A, toelim);
  if (0 != ptr) {
    *ptr = 0;
  }

  if(options.debug_level>3) printf("[+++] location_clean() CLEANED: %s\n", cleaned);
}


/*
 * CHECK_URL: Comprueba que la URL inicial tiene el formato correcto
 *
 */

void check_url(char *url) {

  if(options.debug_level>4) printf("[++++] check_url() URL: %s\n", url);

  if(strncmp(url, "http://", 7)!=0 && strncmp(url, "https://", 8)!=0) {
    printf("\n(!) FATAL: Invalid URL format: %s\n", url);
    printf("    (Use: \"http://host/\" or \"https://host/\" for SSL)\n");
    exit(-1);
    }

}


/*
 * ISLISTABLE: Comprueba si un directorio es listable o no
 *
 */

int islistable(char *direccion) {

  listable=-1;
  get_url(direccion);

  if(listable==-1) listable=0;

  return listable;

}


/*
 * KBHIT: Comprueba si alguna tecla ha sido pulsada (no bloqueante)
 *
 */

char kbhit(void){
  struct timeval tv;
  fd_set read_fd;
  char key = 0;
  static struct termios term;

  tcgetattr(0, &term);
  term.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term);

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&read_fd);
  FD_SET(0, &read_fd);

  if(select(1, &read_fd, NULL, NULL, &tv) == -1) key = -1;

  if(FD_ISSET(0, &read_fd)) {
    read(0, &key, 1);
    } else {
  key = 0;
    }

  if(options.debug_level>4) printf("[++++] kbhit() %d\n", key);

  return(key);
}


/*
 * CIERRE: Codigo de finalizacion
 *
 */

void cierre(void) {
  struct tm *ptr;
  time_t tm;

  tm = time(NULL);
  ptr = localtime(&tm);

  if(!options.silent_mode) printf("                                                                               \r");
  IMPRIME("\n-----------------\n");

  IMPRIME("END_TIME: %s", asctime(ptr));
  IMPRIME("DOWNLOADED: %d - FOUND: %d\n", descargadas, encontradas);

  if(options.saveoutput) fclose(outfile);
}


/*
 * CODE2STRING: Convierte un codigo en su cadena equivalente
 *
 */

char *code2string(struct code *a, unsigned int v) {
  unsigned int i=0;

  while(a[i].codenum!=v && a[i].codenum!=0xff) {
  i++;
  }
  return a[i].desc;
}


/*
 * INIT_EXTS: Inicializa el array de extensiones
 *
 */

void init_exts(void) {

  if(exts_num==0) {

    exts_base=(struct words *)calloc(1, sizeof(struct words));
    exts_base->word = "";
    exts_num=1;

  }
}
