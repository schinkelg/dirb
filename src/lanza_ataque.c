/*
 * DIRB
 *
 * lanza_ataque.c
 *
 */

#include "dirb.h"
#include "http_codes.h"


void bruteforce(const char *directory, struct words *wordlist) {
  char *location_temp = 0;
  char *tested_url = 0;
  char *tested_url2 = 0;
  char *url_base = 0;
  int alert = 0;
  int alert_found = 0;
  int exts_pos = 0;
  struct result tested_estruct2;
  struct result tested_estruct;

  alert=0;
  alert_found=0;

  exts_current=exts_base;

  url_base = malloc(strlen(directory)+1);
  strcpy(url_base, directory);

  IMPRIME("\n---- Scanning URL: %s ----\n", url_base);
  get_necs(url_base);

  while(wordlist->word) {
    memset(&tested_estruct, 0, sizeof(struct result));
    memset(&tested_estruct2, 0, sizeof(struct result));

    existant=1;
    fflush(outfile);

    tested_url = malloc(strlen(url_base)+strlen(wordlist->word)+strlen(exts_current->word)+1);
    strcpy(tested_url, url_base);
    strcat(tested_url, wordlist->word);
    strcat(tested_url, exts_current->word);

    if(options.speed) usleep(options.speed*1000);

    tested_estruct=get_url(tested_url);

    switch(tested_estruct.codigo_http) {

      case 200:
        alert=0;

        if(options.finetuning==1) tested_estruct.body_size=tested_estruct.body_words;

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http && tested_estruct.body_size==nec[exts_pos]->body_size) {
          existant=0;
        } else {
          existant=1;
        }

        break;

      case 301:
      case 302:
        alert=0;
        location_temp = malloc(strlen(tested_estruct.location)+1);
        strcpy(location_temp, tested_estruct.location);
        if(options.finetuning==1) location_clean(location_temp, tested_url);

        if(tested_estruct.codigo_http==nec[exts_pos]->codigo_http && (strcmp(location_temp, nec[exts_pos]->location)==0)) {
          existant=0;
        } else {
          existant=1;
          // test for directory
          tested_url2 = malloc(strlen(tested_estruct.url)+2);
          strcpy(tested_url2, tested_estruct.url);
          if(tested_url2[strlen(tested_url2)-1]!='/') strcat(tested_url2, "\x2f");
          if(location_cmp(tested_estruct.location, tested_url2)==0) guardadir(tested_url2);
        }

        break;

      case 401:
      case 403:
      case 500:
        alert++;

        if(alert>MAX_ALERT) {
          IMPRIME("(!) WARNING: All responses for this directory seem to be CODE = %d.\n", tested_estruct.codigo_http);
          IMPRIME("    (Use mode '-w' if you want to scan it anyway)\n");
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

    if(existant && tested_estruct.codigo_http!=options.default_nec && tested_estruct.codigo_http!=options.ignore_nec) {

      IMPRIME("+ %s (CODE:%d|SIZE:%d)\n", tested_estruct.url, tested_estruct.codigo_http, tested_estruct.body_size);

      if(options.print_location && strlen(tested_estruct.location)) {
        limpia_url(tested_estruct.location);
        IMPRIME("  (Location: '%s')\n", tested_estruct.location);
      }

      alert_found++;

      if(alert_found>MAX_ALERT) {
        IMPRIME("(!) WARNING: Too many responses for this directory seem to be FOUND.\n");
        IMPRIME("    (Something is going wrong - Try Other Scan Mode)\n");
        IMPRIME("    (Use mode '-w' if you want to scan it anyway)\n");
      }

    } else {

      if(options.nothide) {
        IMPRIME("+ %s (CODE:%d|SIZE:%d)\n", tested_estruct.url, tested_estruct.codigo_http, tested_estruct.body_size);

        if(options.print_location && strlen(tested_estruct.location)) {
          limpia_url(tested_estruct.location);
          IMPRIME("    (Location: '%s')\n", tested_estruct.location);
        }
      }
    }

    exts_pos++;
    free(tested_url);

    exts_current=exts_current->next;

    if(exts_pos>=exts_num) {
      exts_current=exts_base;
      exts_pos=0;
      wordlist=wordlist->next;
    }
  }
}
