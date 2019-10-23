/*
 * DIRB
 *
 * options.c
 *
 */

#include "dirb.h"


void get_options(void) {
  struct tm *ptr;
  time_t tm;

  tm = time(NULL);
  ptr = localtime(&tm);

  // -o

  if(options.saveoutput==1) {
    printf("OUTPUT_FILE: %s\n", options.savefile);
    outfile=abrir_file(options.savefile);

    fprintf(outfile, "\n");
    fprintf(outfile, "-----------------\n");
    fprintf(outfile, "DIRB v"VERSION"    \n");
    fprintf(outfile, "By The Dark Raver\n");
    fprintf(outfile, "-----------------\n");
    fprintf(outfile, "\n");
    fprintf(outfile, "OUTPUT_FILE: %s\n", options.savefile);
    }

  IMPRIME("Start time: %s", asctime(ptr));
  IMPRIME("Base URL:   %s\n", options.url_inicial);
  IMPRIME("Wordlist:   %s\n", options.mfile);

  // -a

  if(options.use_agent==1) {
    IMPRIME("USER_AGENT: %s\n", options.agente);
    }

  // -c

  if(options.use_cookie==1) {
    IMPRIME("COOKIE: %s\n", options.cookie);
    }

  // -d

  if(options.debug_level) {
    IMPRIME("DEBUG_LEVEL: %d\n", options.debug_level);
    }

  // -f

  if(options.finetuning==1) {
    IMPRIME("OPTION: Fine tunning of NOT_FOUND detection\n");
    }

  // -g

  if(options.save_found==1) {
    IMPRIME("OPTION: Saving Found URLs to disk\n");
    }

  // -h

  if(options.use_vhost==1) {
    IMPRIME("VHOST: %s\n", options.vhost);
    }

  // -H

  if(options.add_header) {
    IMPRIME("ADDED_HEADERS: \n--\n%s\n--\n", options.header_string);
    }

  // -i

  if(options.insensitive==1) {
    IMPRIME("OPTION: Using Case-Insensitive Searches\n");
    }

  // -j

  if(options.port) {
    IMPRIME("PORT: %d\n", options.port);
    }

  // -l

  if(options.print_location==1) {
    IMPRIME("OPTION: Printing LOCATION header\n");
    }

  // -N

  if(options.ignore_nec) {
    IMPRIME("OPTION: Ignoring NOT_FOUND code -> %d\n", options.ignore_nec);
    }

  // -p

  if(options.use_proxy==1) {
    IMPRIME("PROXY: %s\n", options.proxy);
    }

  // -P

  if(options.use_proxypass==1) {
    IMPRIME("PROXY AUTHORIZATION: %s\n", options.proxypass_string);
    }

  // -r

  if(options.dont_recurse==1) {
    IMPRIME("OPTION: Not Recursive\n");
    }

  // -R

  if(options.interactive==1) {
    IMPRIME("OPTION: Interactive Recursion\n");
    }

  // -s

  if(options.verify_ssl==1) {
    IMPRIME("OPTION: Verifying SSL certificate\n");
    }

  // -S

  if(options.silent_mode==1) {
    IMPRIME("OPTION: Silent Mode\n");
    }

  // -t

  if(options.lasting_bar==0) {
    IMPRIME("OPTION: NOT forcing an ending '/' on URLs\n");
    }

  // -u

  if(options.use_pass==1) {
    IMPRIME("AUTHORIZATION: %s\n", options.pass_string);
    }

  // -v

  if(options.nothide==1) {
    IMPRIME("OPTION: Show Not Existant Pages\n");
    }

  // -w

  if(options.exitonwarn==0) {
    IMPRIME("OPTION: Not Stoping on warning messages\n");
    }

  // -x

  if(options.extensions_file==1) {
    struct words *exts_current;

    IMPRIME("EXTENSIONS_FILE: %s | ", options.exts_file);

    exts_base=crea_wordlist(options.exts_file);
    exts_current=exts_base;

    while(exts_current->next!=0) {
      IMPRIME("(%s)", exts_current->word);
      exts_current=exts_current->next;
      exts_num++;
      }

    IMPRIME(" [NUM = %d]\n", exts_num);
    }

  // -z

  if(options.speed) {
    IMPRIME("SPEED_DELAY: %d miliseconds\n", options.speed);
    }
}
