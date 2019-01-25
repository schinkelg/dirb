project('dirb', 'c')
libcurl = dependency('libcurl')
src = ['src/dirb.h', 'src/calculanec.c', 'src/crea_wordlist.c', 'src/dirb.c', 'src/get_url.c', 'src/lanza_ataque.c', 'src/options.c', 'src/utils.c']
executable('dirb', sources : src, dependencies : libcurl, install : true)
