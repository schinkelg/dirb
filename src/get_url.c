/*
 * DIRB
 *
 * get_url.c - Lee una url y devuelve una estructura con el resultado
 *
 */


#include "dirb.h"
#include "get_url.h"


/* Local Globales */
struct result estructura;
int in_word;


/*
 * GET_URL: Descarga una URL y devuelve una estrcutra con el resultado
 *
 */

struct result get_url(char *resp_url) {



// Inicializamos
  errores=0;
  in_word = 1;


retry:
  memset(&estructura, 0, sizeof(struct result));
  strcpy(estructura.url,resp_url);
  estructura.body_size=0;
  estructura.head_size=0;
  estructura.body_lines=0;
  estructura.head_lines=1;
  estructura.estado=-1;

  curl_easy_setopt(curl, CURLOPT_URL, resp_url);
  

  estructura.estado=curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &estructura.codigo_http);


  /* Controlamos el resultado */

  if(estructura.codigo_http==0) {
    errores++;

    if(options.debuging>3) printf("[+++] get_url() error: %d\n", estructura.estado);

    if(errores>=MAX_FAILS) {
      if(!options.silent_mode) printf("                                                                               \r");
      IMPRIME("\n(!) FATAL: Too many errors connecting to host\n");
      IMPRIME("    (Possible cause: %s)\n", error2message[estructura.estado].desc);
      dump();
      cierre();
      exit(-1);
      }

    goto retry;
    }


  /* devolvemos la estructura */

  if(options.debuging>1) {
    printf("\nURL: %s\n", estructura.url);
    printf("ESTADO: %d\n", estructura.estado);
    printf("CODIGO_HTTP: %d\n", estructura.codigo_http);
    printf("HEAD_SIZE: %d\n", estructura.head_size);
    printf("HEAD_LINES: %d\n", estructura.head_lines);
    printf("BODY_SIZE: %d\n", estructura.body_size);
    printf("BODY_WORDS: %d\n", estructura.body_words);
    printf("BODY_LINES: %d\n", estructura.body_lines);
    printf("LOCATION: %s\n", estructura.location);
    printf("SERVER: %s\n", estructura.server);
    }

  return estructura;
}



/*
 * GET_HEADER: Funcion de manejo de las cabeceras
 *
 */

size_t get_header(void *ptr, size_t size, size_t nmemb) {
  unsigned int len;
  unsigned int i;


  len=size*nmemb;

  /* DEBUG */

  if(options.debuging>4) printf("[++++] get_header() LEN: %d\n", len);
  if(options.debuging>6) printf("[++++++] get_header() HEADERS: %s\n", (char *)ptr);


  // Si tiene cabecera Location

  if(strncasecmp(ptr, "Location: ", 10)==0 || strncasecmp(ptr, "Content-Location: ", 18)==0) {
    if(strncasecmp(ptr, "Content-Location: ", 18)==0) ptr=ptr+8;
    ptr=ptr+10;
    strncpy(estructura.location, ptr, STRING_SIZE-1);
    limpia_url(estructura.location);
    if(options.debuging>3) printf("[+++] get_header() LOCATION = %s\n", estructura.location);
  }


  // Si tiene cabecera Server
  if(strncasecmp(ptr, "Server: ", 8)==0) {
    strncpy(estructura.server,ptr+8,STRING_SIZE-1);
    if(options.debuging>3) printf("[+++] get_header() SERVER = %s", estructura.server);
  }


  // Tamaño

  estructura.head_size+=len;


  // Numero de lines

  for(i=0;i<len;i++) {
    if(strncmp(ptr+i, "\n", 1)==0) estructura.head_lines++;
  }

  return len;
}




/*
 * GET_BODY: Funcion de manejo del cuerpo HTML
 *
 */

size_t get_body(void *ptr, size_t size, size_t nmemb) {
  unsigned int len;
  unsigned int i;
  int c;

  len=size*nmemb;

  /* DEBUG */

  if(options.debuging>4) printf("[++++] get_body() LEN: %d\n", len);


  // Tenemos que mirar si es listable

  if(listable==-1) {

    if(strstr(ptr, "Parent Directory")!=0 || strstr(ptr, "Up To ")!=0 || strstr(ptr, "Atrás A ")!=0 || strstr(ptr, "Al directorio pri")!=0 || strstr(ptr, "Directory Listing For")!=0) {
    if(options.debuging>3) printf("[+++] get_body() Directory is listable\n");
    listable=1;
      }

    }


  // Contamos bytes, palabras y lineas

  for(i=0;i<len;i++) {

  estructura.body_size++;
    c=(char)*(char *)(ptr+i);

    switch(c) {
      case '\n':
        estructura.body_lines++;
        __attribute__ ((fallthrough));
      case '\r':
      case '\f':
      case '\t':
      case ' ':
      case '\v':
        if(in_word) {
          in_word = 0;
          estructura.body_words++;
        }
        break;
      default:
        in_word = 1;
        break;
    }
  }

  return len;
}
