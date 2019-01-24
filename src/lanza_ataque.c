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
  char tested_word[STRING_SIZE];
  char tested_url[STRING_SIZE];
  char tested_url2[STRING_SIZE];
  struct result tested_estruct;
  struct result tested_estruct2;
  int alert_found;
  int alert;
  char location_temp[STRING_SIZE];
  char url_base[STRING_SIZE];
  int character;
  int exts_pos;
  struct winsize ws;
  int columns;

  // Inicializamos

  if(ioctl(0,TIOCGWINSZ,&ws)!=0) {
    columns=79;
    } else {
	columns=ws.ws_col-1;
	if(options.debuging>2) printf("[++] lanza_ataque() COLUMNS: %d\n", columns);
	}

  next_dir=0;
  alert=0;
  alert_found=0;

  exts_current=exts_base;
  exts_pos=0;

  strcpy(url_base, inicio);
  character=0;


  IMPRIME("\n---- Scanning URL: %s ----\n", url_base);
  get_necs(url_base);

  // Bucle de peticiones
  /////////////////////////////////////////////////////////////////////////////

  while(1) {

    // Inicializamos cada bucle
    memset(&tested_estruct, 0, sizeof(struct result));
    existant=1;
    fflush(outfile);


    // Comprobamos si hay que dar otra vuelta

    if(options.debuging>3) printf("[+++] lanza_ataque() BUCLE1: %s - %d\n", (char *)wordlist->siguiente, next_dir);

    if(wordlist->siguiente==0 || next_dir) {

      // Entramos en un directorio

      if(options.recursion_level && !options.dont_recurse) {
        next_dir=0;
        alert=0;
        alert_found=0;

        if(options.debuging>2) printf("[++] lanza_ataque() RECURSE: %d\n", options.recursion_level);

        strcpy(url_base, dirlist_current->word);
        limpia_url(url_base);

        if(!options.silent_mode) printf("%*c\r", columns, ' ');
        IMPRIME("\n---- Entering directory: %s ----\n", url_base);

        get_necs(url_base);
        wordlist=wordlist_base;
        options.recursion_level--;
        dirlist_current=dirlist_current->siguiente;


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
          if(options.debuging>4) printf("[++++] lanza_ataque() CHAR: %d\n", character);
          if(character=='n') {
            printf("\nSkipping directory.\n");
            next_dir=1;
            continue;
            }
          }

        // Reiniciamos el bucle

        continue;

        } else {

          if(options.debuging>4) printf("[++++] lanza_ataque() Se acabo la wordlist\n");
          break;

        } // Fin pruebas directorio

      }	// Fin comprobacion vuelta


    // Pasamos la wordlist
    ///////////////////////////////////////////////////////////////////////////

    // Obtenemos la palabra de la lista

    if(exts_pos>=exts_num) {
      exts_current=exts_base;
      exts_pos=0;
      wordlist=wordlist->siguiente;
      }

    strncpy(tested_word, wordlist->word, STRING_SIZE-1);
    limpia_url(tested_word);

    if(strlen(tested_word)==0) {
      if(options.debuging>4) printf("[++++] lanza_ataque() VACIO\n");
      continue;
      }


    // Inicializamos resume

    strncpy(options.current_dir, url_base, STRING_SIZE);
    strncpy(options.current_word, tested_word, STRING_SIZE);


    // Concatenamos la extension correspondiente

    strncat(tested_word, exts_current->word, STRING_SIZE-strlen(tested_word));

    exts_current=exts_current->siguiente;

    if(options.debuging>3) if(!options.silent_mode) printf("%*c\r", columns, ' ');
    if(options.debuging>3) printf("[+++] lanza_ataque() PALABRA: %s\n", tested_word);


    // Generamos la URL y la limpiamos

    strncpy(tested_url, url_base, STRING_SIZE);
    strncat(tested_url, tested_word, STRING_SIZE-strlen(tested_url));
    limpia_url(tested_word);


    // Hacemos la peticion

    if(options.speed) usleep(options.speed*1000);

    tested_estruct=get_url(tested_url);


    // Mostramos los resultados

    if(options.debuging>3) printf("[+++] lanza_ataque() ESTADO: %d\n", tested_estruct.estado);

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
    ///////////////////////////////////////////////////////////////////////////

    switch(tested_estruct.codigo_http) {

      case 200:
        alert=0;

        if(options.finetunning==1) tested_estruct.body_size=tested_estruct.body_words;

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http && tested_estruct.body_size==nec[exts_pos]->body_size) {
          existant=0;
          } else {
		  existant=1;
          if(options.debuging>3) printf("[+++] lanza_ataque() 200: %d - %d\n", nec[exts_pos]->body_size, tested_estruct.body_size);
          }

        break;

      case 301:
      case 302:
        alert=0;

        strncpy(location_temp, tested_estruct.location, STRING_SIZE);
        if(options.finetunning==1) location_clean(location_temp+strcmp(location_temp, nec[exts_pos]->location), tested_word);

        if(options.debuging>3) printf("[+++] lanza_ataque() Location: '%s'\n", location_temp);

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http && (strncasecmp(location_temp, nec[exts_pos]->location, STRING_SIZE-1)==0)) {
          existant=0;
          } else {
          existant=1;

          // Comprobamos si es un directorio
          strcpy(tested_url2, tested_estruct.url);
          barra(tested_url2);

          if(options.debuging>3) printf("[+++] lanza_ataque() Direcroty_compare: '%s' - '%s'\n", tested_estruct.location, tested_url2);

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

        strcpy(tested_url2, tested_estruct.url);
        memset(&tested_estruct2, 0, sizeof(struct result));

        if(strncmp(tested_url2+strlen(tested_url2)-1, "\x2f", 1)==0) {
          memcpy(tested_url2+strlen(tested_url2)-1, "_\x00", 2);
          } else {
          memcpy(tested_url2+strlen(tested_url2), "_\x00", 2);
          }

        limpia_url(tested_url2);

        tested_estruct2=get_url(tested_url2);

        if(options.debuging>3) printf("[+++] lanza_ataque() tested_url2: %d\n", tested_estruct2.codigo_http);

        if(tested_estruct2.codigo_http==tested_estruct.codigo_http || tested_estruct.codigo_http==nec[exts_pos]->codigo_http) {
          existant=0;
          } else {
          existant=1;
          }

        break;

      default:
        alert=0;

        if(options.finetunning==1) tested_estruct.body_size=tested_estruct.body_words;

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http) {
          existant=0;
          if(options.finetunning==1 && tested_estruct.body_size!=nec[exts_pos]->body_size) existant=1;
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

    } // fin while
}
