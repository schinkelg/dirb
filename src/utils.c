/*
 * DIRB
 *
 * utils.c - Utilidades varias
 *
 */

#include "dirb.h"

char *mkstr(const char * const first, const char * const second) {
  char * newstr = malloc(strlen(first) + strlen(second) + 1);
  strcpy(newstr, first);
  strcat(newstr, second);
  return newstr;
}

void limpia_url(char *limpia) {
  char *ptr;

  ptr=(char *)strchr(limpia, '\r');
  if(ptr!=0) *ptr='\0';
  ptr=(char *)strchr(limpia, '\n');
  if(ptr!=0) *ptr='\0';
  ptr=(char *)strchr(limpia, ' ');
  if(ptr!=0) *ptr='\0';

}


void guardadir(char *directory) {

  if(!options.silent_mode) printf("                                                                               \r");
  IMPRIME("==> DIRECTORY: %s\n", directory);
  options.recursion_level++;
  dirlist_final->word = malloc(strlen(directory) + 1);
  strcpy(dirlist_final->word, directory);
  dirlist_final->next=(struct words *)calloc(1, sizeof(struct words));
  dirlist_final=dirlist_final->next;
  existant=0;

}


FILE *abrir_file(char *file) {
  FILE *desc;

  if((desc=fopen(file, "a"))==0) {
    printf("\n(!) FATAL: Error opening output file: %s\n", file);
    exit(-1);
    }

  return desc;

}


int location_cmp(char *A, char *B) {
  int result=0;
  char *ptr=0;

  if(strncmp(A, "http://", 7)==0 || strncmp(A, "https://", 8)==0) {
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr;
    }

  char *loc = strchr(A, ';');
  if(loc!=0) {
    *loc=0;
  }

  if(options.debug_level>4) printf("[++++] location_cmp() A[%zu]: '%s'\n", strlen(A), A);

  if(strncmp(B, "http://", 7)==0 || strncmp(B, "https://", 8)==0) {
    ptr=(char *)strchr(B, '/');
    if(ptr!=0) B=ptr+1;
    ptr=(char *)strchr(B, '/');
    if(ptr!=0) B=ptr+1;
    ptr=(char *)strchr(B, '/');
    if(ptr!=0) B=ptr;
    }

  if(options.debug_level>4) printf("[++++] location_cmp() B[%zu]: '%s'\n", strlen(B), B);


  result=strncmp(A, B, strlen(A)>strlen(B) ? strlen(A) : strlen(B));

  if(options.debug_level>4) printf("[++++] location_cmp() RESULT: %d (%zu)\n", result, strlen(A)>strlen(B) ? strlen(A) : strlen(B));

  return result;

}


void location_clean(char *cleaned, char *toelim) {
  char *ptr=0;
  char *A=cleaned;

  if(options.debug_level>3) printf("[+++] location_clean() TOCLEAN: %s | TOELIM: %s\n", cleaned, toelim);

  // Jump to uri-path
  if(strncmp(A, "http://", 7)==0 || strncmp(A, "https://", 8)==0) {
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
    ptr=(char *)strchr(A, '/');
    if(ptr!=0) A=ptr+1;
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


void check_url(char *url) {

  if(options.debug_level>4) printf("[++++] check_url() URL: %s\n", url);

  if(strncmp(url, "http://", 7)!=0 && strncmp(url, "https://", 8)!=0) {
    printf("\n(!) FATAL: Invalid URL format: %s\n", url);
    printf("    (Use: \"http://host/\" or \"https://host/\" for SSL)\n");
    exit(-1);
    }

}


int islistable(char *directory) {

  listable=-1;
  get_url(directory);

  if(listable==-1) listable=0;

  return listable;

}


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


void cierre(void) {
  struct tm *ptr;
  time_t tm;

  tm = time(NULL);
  ptr = localtime(&tm);

  if(!options.silent_mode) printf("                                                                               \r");
  IMPRIME("\n-----------------\n");

  IMPRIME("END_TIME: %s", asctime(ptr));

  if(options.saveoutput) fclose(outfile);
}


char *code2string(struct code *a, unsigned int v) {
  unsigned int i=0;

  while(a[i].codenum!=v && a[i].codenum!=0xff) {
  i++;
  }
  return a[i].desc;
}


void init_exts(void) {

  if(exts_num==0) {

    exts_base=(struct words *)calloc(1, sizeof(struct words));
    exts_base->word="";
    exts_num=1;

  }
}
