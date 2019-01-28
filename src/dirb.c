/*
 * DIRB
 *
 * dirb.c - Nucleo central del programa
 *
 */

#include "dirb.h"
#include <malloc.h>


/*
 * MAIN: Nucleo del programa
 *
 */

int main(int argc, char *const *argv) {
  struct words *palabras;
  int c=0;

  banner();

  // Inicializaciones globales
  memset(&options, 0, sizeof(struct opciones));

  options.exitonwarn=1;
  options.ignore_nec=0;
  options.default_nec=404;
  options.lasting_bar=1;
  options.speed=0;
  options.add_header=0;

  encontradas=0;
  descargadas=0;
  listable=0;
  exts_num=0;

  options.agente = AGENT_STRING;
  struct curl_slist *slist;

  dirlist_current=(struct words *)malloc(sizeof(struct words));
  memset(dirlist_current, 0, sizeof(struct words));
  dirlist_base=dirlist_current;
  dirlist_final=dirlist_current;

  curl=curl_easy_init();

  if(!curl) {
    printf("[+++] get_url() libcurl failed. Exiting...\n");
    exit(-1);
  }

  slist = 0;
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, options.agente);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, get_header);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_body);

  if(options.port) {
    curl_easy_setopt(curl, CURLOPT_PORT, options.port);
  }

  if(options.use_vhost) {    
    slist = curl_slist_append(slist, options.vhost);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
  }

  if(options.add_header) {
    slist = curl_slist_append(slist, options.header_string);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
  }

  if(options.use_proxy) {
    curl_easy_setopt(curl, CURLOPT_PROXY, options.proxy);
  }

  if(options.use_proxypass) {
    curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, options.proxypass_string);
  }

  if(options.use_pass) {
    curl_easy_setopt(curl, CURLOPT_USERPWD, options.pass_string);
  }

  if(options.use_cookie) {
    curl_easy_setopt(curl, CURLOPT_COOKIE, options.cookie);
  }

  if(options.verify_ssl==0) {
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
  }
  // Recepcion de parametros

  if(argc<2) {
    usage();
    exit(-1);
    }

  options.url_inicial = argv[1];

  // Does the URL end with a forward slash?
  if (argv[1][strlen(argv[1])-1] != '/') {
    options.url_inicial = malloc(strlen(argv[1])+2);
    strcpy(options.url_inicial, argv[1]);
    strcat(options.url_inicial, "/");

  } else {
    options.url_inicial = argv[1];
  }
  printf(options.url_inicial);
  if(argc==2 || strncmp(argv[2], "-", 1)==0) {
    options.mfile = DEFAULT_WORDLIST;
    optind+=1;
  } else {
    options.mfile = argv[2];
    optind+=2;
  }

  while((c = getopt(argc,argv,"a:c:d:fgh:H:ij:lm:M:n:N:o:p:P:rRsSvwx:X:u:tz:"))!= -1){
    switch(c) {
      case 'a':
        options.use_agent=1;
        options.agente = (char*) malloc(strlen(optarg)+1);
        strcpy(options.agente, optarg);
        break;
      case 'c':
        options.use_cookie=1;
        options.cookie = (char*) malloc(strlen(optarg)+1);
        strcpy(options.cookie, optarg);
        break;
      case 'd':
        options.debug_level=atoi(optarg);
        break;
      case 'f':
        options.finetuning=1;
        break;
      case 'g':
        options.save_found=1;
        break;
      case 'h':
        options.use_vhost=1;
        options.vhost = malloc(strlen(optarg)+7);
        snprintf(options.vhost, strlen(optarg)+6, "Host: %s", optarg);
        break;
      case 'H':
        if(options.add_header) {
          char * tmp = malloc(strlen(options.header_string) + strlen(optarg) + 2);
          strcpy(tmp, options.header_string);
          strcat(tmp, "\n");
          strcat(tmp, optarg);
          free(options.header_string);
          options.header_string = tmp;
        } else {
          options.header_string = malloc(strlen(optarg)+1);
  	      strcpy(options.header_string, optarg);
	      }
        options.add_header++;
        break;
      case 'i':
        options.insensitive=1;
        break;
      case 'j':
        options.port=atoi(optarg);
        break;
      case 'l':
        options.print_location=1;
        break;
      case 'm':
        options.mutations_file=1;
        options.mutation_file = malloc(strlen(optarg)+1);
        strcpy(options.mutation_file, optarg);
        break;
      case 'M':
        options.mutations_list=1;
        options.mutation_list = malloc(strlen(optarg)+1);
        strcpy(options.mutation_list, optarg);
        break;
      case 'N':
        options.ignore_nec=atoi(optarg);
        break;
      case 'o':
        options.saveoutput=1;
        options.savefile = malloc(strlen(optarg)+1);
        strcpy(options.savefile, optarg);
        break;
      case 'p':
        options.use_proxy=1;
        options.proxy = malloc(strlen(optarg)+1);
        strcpy(options.proxy, optarg);
        break;
      case 'P':
        options.use_proxypass=1;
        options.proxypass_string = malloc(strlen(optarg)+1);
        strcpy(options.proxypass_string, optarg);
        break;
      case 'r':
        options.dont_recurse=1;
        break;
      case 'R':
        options.interactive=1;
        break;
      case 's':
        options.verify_ssl=1;
        break;
      case 'S':
        options.silent_mode=1;
        break;
      case 't':
        options.lasting_bar=0;
        break;
      case 'u':
        options.use_pass=1;
        options.pass_string = malloc(strlen(optarg)+1);
        strcpy(options.pass_string, optarg);
        break;
      case 'v':
        options.nothide=1;
        break;
      case 'w':
        options.exitonwarn=0;
        break;
      case 'x':
        options.extensions_file=1;
        options.exts_file = malloc(strlen(optarg)+1);
        strcpy(options.exts_file, optarg);
        break;
      case 'X':
        options.extensions_list=1;
        options.exts_list = malloc(strlen(optarg)+1);
        strcpy(options.exts_list, optarg);
        break;
      case 'z':
        options.speed=(atoi(optarg)<=0)?0:atoi(optarg);
        break;
      default:
        printf("\n(!) FATAL: Incorrect parameter\n");
        exit(-1);
        break;
        }
      }

  // Chequeos iniciales

  get_options();

  init_exts();

  IMPRIME("\n-----------------\n\n");

  // Creamos la lista de palabras
  printf("file: %s", options.mfile);
  palabras=crea_wordlist(options.mfile);

  struct mallinfo mi;
  mi = mallinfo();
  printf("Total allocated space (uordblks):      %d\n", mi.uordblks);


  // Lanzamos el bucle de descarga

  lanza_ataque(options.url_inicial, palabras);
  mi = mallinfo();
  printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
  // Finalizamos

  cierre();

  mi = mallinfo();
  printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
  exit(0);

}



/*
 * BANNER: Muestra el banner de presentacion del programa
 *
 */

void banner(void) {
  printf("dirb v"VERSION"    \n");
  printf("\n");
}


/*
 * OPCIONES: Muestra el menu de opciones disponibles
 *
 */

void usage(void) {

  printf("Usage: dirb <url_base> [<wordlist_file(s)>] [options]\n");

  printf(" <url_base> : Base URL to scan.\n");
  printf(" <wordlist_file(s)> : List of wordfiles. (wordfile1,wordfile2,wordfile3...)\n");

  printf("\nOPTIONS\n");
  printf(" -a <agent_string> : Specify your custom USER_AGENT.\n");
  printf(" -c <cookie_string> : Set a cookie for the HTTP request.\n");
  printf(" -d <debug_level> : Activate DEBUGing.\n");
  printf(" -f : Fine tunning of NOT_FOUND (404) detection.\n");
  printf(" -h <vhost_string> : Use your custom Virtual Host header.\n");
  printf(" -H <header_string> : Add a custom header to the HTTP request.\n");
  printf(" -i : Use case-insensitive search.\n");
  printf(" -j : Use port to connect to.\n");
  printf(" -l : Print \"Location\" header when found.\n");
  printf(" -N <nf_code>: Ignore responses with this HTTP code.\n");
  printf(" -o <output_file> : Save output to disk.\n");
  printf(" -p <proxy[:port]> : Use this proxy. (Default port is 1080)\n");
  printf(" -P <proxy_username:proxy_password> : Proxy Authentication.\n");
  printf(" -r : Don't search recursively.\n");
  printf(" -R : Interactive recursion. (Asks for each directory)\n");
  printf(" -S : Silent Mode. Don't show tested words. (For dumb terminals)\n");
  printf(" -t : Don't force an ending '/' on URLs.\n");
  printf(" -u <username:password> : HTTP Authentication.\n");
  printf(" -v : Show also NOT_FOUND pages.\n");
  printf(" -w : Don't stop on WARNING messages.\n");
  printf(" -X <extensions> / -x <exts_file> : Append each word with this extensions.\n");
  printf(" -z <milisecs> : Add a miliseconds delay to not cause excessive Flood.\n");

  printf("\nEXAMPLES\n");
  printf(" ./dirb http://url/directory/ (Simple Test)\n");
  printf(" ./dirb http://url/ -X .html (Test files with '.html' extension)\n");
  printf(" ./dirb http://url/ /usr/share/dirb/wordlists/vulns/apache.txt (Test with apache.txt wordlist)\n");
  printf(" ./dirb https://secure_url/ (Simple Test with SSL)\n");
}
