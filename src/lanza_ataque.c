/*
 * DIRB
 *
 * lanza_ataque.c - Obtiene la wordlist de ataque y lanza get_url sobre cada palabra
 *
 */

#include "dirb.h"
#include "http_codes.h"


/*
 * LANZA_ATAQUE: Lanza el bucle de peticiones y muestra los resultados
 *
 */

void lanza_ataque(char *inicio, struct words *wordlist) {
  char *location_temp = 0;
  char *tested_url2 = 0;
  char *tested_url = 0;
  char *tested_word = 0;
  char *url_base = 0;
  int alert = 0;
  int alert_found = 0;
  int character = 0;
  int columns = 0;
  int exts_pos = 0;
  struct result tested_estruct2;
  struct result tested_estruct;
  struct winsize ws;

  // Inicializamos
  if(ioctl(0,TIOCGWINSZ,&ws)!=0) {
    columns=79;
    } else {
	columns=ws.ws_col-1;
	if(options.debug_level>2) printf("[++] lanza_ataque() COLUMNS: %d\n", columns);
	}

  next_dir=0;
  alert=0;
  alert_found=0;

  exts_current=exts_base;
  exts_pos=0;

  url_base = malloc(strlen(inicio)+1);
  strcpy(url_base, inicio);
  character=0;


  IMPRIME("\n---- Scanning URL: %s ----\n", url_base);
  get_necs(url_base);

  // Bucle de peticiones
  /////////////////////////////////////////////////////////////////////////////

  while(1) {
    // Inicializamos cada bucle
    memset(&tested_estruct, 0, sizeof(struct result));
    memset(&tested_estruct2, 0, sizeof(struct result));
    
    existant=1;
    fflush(outfile);


    // Comprobamos si hay que dar otra vuelta

    if(options.debug_level>3) printf("[+++] lanza_ataque() BUCLE1: %s - %d\n", (char *)wordlist->next, next_dir);

    if(wordlist->next==0 || next_dir) {

      // Entramos en un directorio

      if(options.recursion_level && !options.dont_recurse) {
        next_dir=0;
        alert=0;
        alert_found=0;

        if(options.debug_level>2) printf("[++] lanza_ataque() RECURSE: %d\n", options.recursion_level);
        url_base = malloc(strlen(dirlist_current->word)+1);
        strcpy(url_base, dirlist_current->word);
        limpia_url(url_base);

        if(!options.silent_mode) printf("%*c\r", columns, ' ');
        IMPRIME("\n---- Entering directory: %s ----\n", url_base);

        get_necs(url_base);
        wordlist=wordlist_base;
        options.recursion_level--;
        dirlist_current=dirlist_current->next;


        // Pruebas de directorio
        //////////////////////////////////////////////////

        // Comprobamos si el directorio es listable

        if(islistable(url_base)!=0) {
          IMPRIME("(!) WARNING: Directory IS LISTABLE. No need to scan it.\n");
          IMPRIME("    (Use mode '-w' if you want to scan it anyway)\n");
          if(options.exitonwarn) {
            next_dir=1;
            continue;
            }
          }

        // Si esta activada la opcion interactive, preguntamos

        if(options.interactive) {
          printf("(?) Do you want to scan this directory (y/n)? ");
          fflush(stdout);
          character=' ';
          while(character!='n' && character!='y') character=kbhit();
          if(options.debug_level>4) printf("[++++] lanza_ataque() CHAR: %d\n", character);
          if(character=='n') {
            printf("\nSkipping directory.\n");
            next_dir=1;
            continue;
            }
          }

        // Reiniciamos el bucle

        continue;

        } else {

          if(options.debug_level>4) printf("[++++] lanza_ataque() Se acabo la wordlist\n");
          break;

        } // Fin pruebas directorio

      }	// Fin comprobacion vuelta


    // Pasamos la wordlist
    ///////////////////////////////////////////////////////////////////////////

    // Obtenemos la palabra de la lista

    if(exts_pos>=exts_num) {
      exts_current=exts_base;
      exts_pos=0;
      wordlist=wordlist->next;
      }

    if (tested_word) free(tested_word);
    tested_word = malloc(strlen(wordlist->word) + strlen(exts_current->word)+1);
    strcpy(tested_word, wordlist->word);
    limpia_url(tested_word);

    if(strlen(tested_word)==0) {
      if(options.debug_level>4) printf("[++++] lanza_ataque() VACIO\n");
      continue;
      }

    // Concatenamos la extension correspondiente

    strcat(tested_word, exts_current->word);

    exts_current=exts_current->next;

    if(options.debug_level>3) if(!options.silent_mode) printf("%*c\r", columns, ' ');
    if(options.debug_level>3) printf("[+++] lanza_ataque() PALABRA: %s\n", tested_word);


    // Generamos la URL y la limpiamos
    tested_url = malloc(strlen(url_base)+strlen(tested_word)+1);
    strcpy(tested_url, url_base);
    strcat(tested_url, tested_word);
    limpia_url(tested_word);


    // Hacemos la peticion

    if(options.speed) usleep(options.speed*1000);

    tested_estruct=get_url(tested_url);


    // Mostramos los resultados

    if(options.debug_level>3) printf("[+++] lanza_ataque() ESTADO: %d\n", tested_estruct.estado);

    descargadas++;
    if(!options.silent_mode) printf("%*c\r", columns, ' ');
    if(!options.silent_mode) printf("--> Testing: %.*s\r", columns-13, tested_estruct.url);
    if(!options.silent_mode) fflush(stdout);


    // -------------------------------- HOTKEYS -------------------------------

    // Comprobamos si se ha pulsado la tecla n

    character=kbhit();

    if(character=='n') {
      if(!options.silent_mode) printf("%*c\r", columns, ' ');
      IMPRIME("+ Going to next directory.\n");
      next_dir=1;
      }

    // Comprobamos si se ha pulsado la tecla q

    if(character=='q') {
      if(!options.silent_mode) printf("%*c\r", columns, ' ');
      IMPRIME("+ Quitting.\n");
      cierre();
      exit(0);
      }

    // Comprobamos si se ha pulsado la tecla r

    if(character=='r') {
      if(!options.silent_mode) printf("%*c\r", columns, ' ');
      IMPRIME("+ Remaining scan stats:\n");
      IMPRIME("Words: %d | Directories: %d\n", count_words(wordlist) * exts_num, count_words(dirlist_current));
      }

    // Analisis del codigo devuelto
    switch(tested_estruct.codigo_http) {

      case 200:
        alert=0;

        if(options.finetuning==1) tested_estruct.body_size=tested_estruct.body_words;

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http && tested_estruct.body_size==nec[exts_pos]->body_size) {
          existant=0;
          } else {
		  existant=1;
          if(options.debug_level>3) printf("[+++] lanza_ataque() 200: %d - %d\n", nec[exts_pos]->body_size, tested_estruct.body_size);
          }

        break;

      case 301:
      case 302:
        alert=0;
        location_temp = malloc(strlen(tested_estruct.location)+1);
        strcpy(location_temp, tested_estruct.location);
        if(options.finetuning==1) location_clean(location_temp, tested_word);

        if(options.debug_level>3) printf("[+++] lanza_ataque() Location: '%s'\n", location_temp);
        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http && (strcmp(location_temp, nec[exts_pos]->location)==0)) {
          existant=0;
        } else {
          existant=1;
          // test for directory
          // one extra for slash from barra()
          tested_url2 = malloc(strlen(tested_estruct.url)+2);
          strcpy(tested_url2, tested_estruct.url);
          barra(tested_url2);

          if(options.debug_level>3) printf("[+++] lanza_ataque() Directory_compare: '%s' - '%s'\n", tested_estruct.location, tested_url2);

          if(location_cmp(tested_estruct.location, tested_url2)==0) guardadir(tested_url2);
        }

        break;

      case 401:
      case 403:
      case 500:
        alert++;

        if(alert>MAX_ALERT) {
          if(!options.silent_mode) printf("%*c\r", columns, ' ');
          IMPRIME("(!) WARNING: All responses for this directory seem to be CODE = %d.\n", tested_estruct.codigo_http);
          IMPRIME("    (Use mode '-w' if you want to scan it anyway)\n");
          if(options.exitonwarn) {
            next_dir=1;
            } else {
            alert=0;
            }
          }
        tested_url2 = malloc(strlen(tested_estruct.url)+3);
        strcpy(tested_url2, tested_estruct.url);
        

        if(strncmp(tested_url2+strlen(tested_url2)-1, "\x2f", 1)==0) {
          memcpy(tested_url2+strlen(tested_url2)-1, "_\x00", 2);
          } else {
          memcpy(tested_url2+strlen(tested_url2), "_\x00", 2);
          }

        limpia_url(tested_url2);

        tested_estruct2=get_url(tested_url2);

        if(options.debug_level>3) printf("[+++] lanza_ataque() tested_url2: %d\n", tested_estruct2.codigo_http);

        if(tested_estruct2.codigo_http==tested_estruct.codigo_http || tested_estruct.codigo_http==nec[exts_pos]->codigo_http) {
          existant=0;
          } else {
          existant=1;
          }

        break;

      default:
        alert=0;

        if(options.finetuning==1) tested_estruct.body_size=tested_estruct.body_words;

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http) {
          existant=0;
          if(options.finetuning==1 && tested_estruct.body_size!=nec[exts_pos]->body_size) existant=1;
          } else {
          existant=1;
          }

      break;
    }


    // Mostramos la informacion si es interesante
    ///////////////////////////////////////////////////////////////////////////

    if(existant && tested_estruct.codigo_http!=options.default_nec && tested_estruct.codigo_http!=options.ignore_nec) {

      encontradas++;
      if(!options.silent_mode) printf("%*c\r", columns, ' ');
      IMPRIME("+ %s (CODE:%d|SIZE:%d)\n", tested_estruct.url, tested_estruct.codigo_http, tested_estruct.body_size);
      //code2string(code2message, tested_estruct.codigo_http)

      if(options.print_location && strlen(tested_estruct.location)) {
        limpia_url(tested_estruct.location);
        IMPRIME("  (Location: '%s')\n", tested_estruct.location);
        }

      // Exceso de eventos repetidos (pasa algo raro)
      alert_found++;

      if(alert_found>MAX_ALERT) {
        if(!options.silent_mode) printf("%*c\r", columns, ' ');
        IMPRIME("(!) WARNING: Too many responses for this directory seem to be FOUND.\n");
        IMPRIME("    (Something is going wrong - Try Other Scan Mode)\n");
		IMPRIME("    (Use mode '-w' if you want to scan it anyway)\n");
        if(options.exitonwarn) {
		  next_dir=1;
		  } else {
		  alert_found=0;
          }
        }

      } else {

	  // Opcion -v, mostrar todo

      if(options.nothide) {
        if(!options.silent_mode) printf("%*c\r", columns, ' ');
        IMPRIME("+ %s (CODE:%d|SIZE:%d)\n", tested_estruct.url, tested_estruct.codigo_http, tested_estruct.body_size);

        if(options.print_location && strlen(tested_estruct.location)) {
          limpia_url(tested_estruct.location);
          IMPRIME("    (Location: '%s')\n", tested_estruct.location);
          }
        }
      }

    exts_pos++;
    free(tested_url);
    } // fin while
}
