/*
 * DIRB
 *
 * global.h - Global Includes
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <libgen.h>
#include "../config.h"


/* Constantes */

#define MAX_FAILS   	3
#define TIMEOUT     	50
#define BIGWORDLIST   	100000
#define AGENT_STRING  	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.2)"
#define MAX_ALERT   	100

#define DEFAULT_WORDLIST  "/usr/share/dirb/wordlists/common.txt"
