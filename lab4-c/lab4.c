#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
 
static short w = 0, c = 0, l = 0;
 
void wout(char* msg, size_t size)
{
  if (write(STDOUT_FILENO, msg, size) < 0)
    {
      perror("Write to stdout");
      exit(EXIT_FAILURE);
    }
}
 
size_t strlen(char* str)
{
  size_t len = 0;
  while(*(str++))
    len++;
  return len;
}
 
void count(int file, unsigned long long* res)
{
  int r, inword = 0;
  char tmp[25]={0x0};
  char buffer[4096];
  unsigned long long lines=0, words=0, chars=0;
  int i=0;
  do{
    if((r = read(file, buffer, sizeof(buffer))) == 0)
      break;
    if(errno == EISDIR)
      {
	errno = 0;
	break;
      }
    if(r < 0)
    {
      perror("read from file");
      exit(EXIT_FAILURE);
    }
    for(i = 0; i < r;++i)
      {
	if(w != 0)
	  switch (buffer[i]) {
	  case ' ':
	  case '\t':
	  case '\n':
	  case '\r':
	  case '\f':
	  case '\v':
	    if(inword)
	      {
		inword = 0;
		words++;
	      }
	    break;
	  default: inword = 1;
	  }
	if(l != 0 && buffer[i] == '\n') lines++;
      }
    chars+=r;
  }while(1);
  if (inword) words++;
  if(l)
    {
      snprintf(tmp, sizeof(tmp), "%llu", lines);
      wout("       ", 8>strlen(tmp)?8 - strlen(tmp):0);
      wout(tmp, strlen(tmp));
    }
  if(w)
    {
      snprintf(tmp, sizeof(tmp), "%llu",words);
      wout("       ", 8>strlen(tmp)?8 - strlen(tmp):1);
      wout(tmp, strlen(tmp));
    }
  if(c)
    {
      snprintf(tmp, sizeof(tmp), "%llu",chars);
      wout("       ", 8>strlen(tmp)?8 - strlen(tmp):1);
      wout(tmp, strlen(tmp));
    }
  res[0] = lines;
  res[1] = words;
  res[2] = chars;
}
 
int main(int argc, char** argv)
{
  int file = STDIN_FILENO;
  char tmp[25]={0x0};
  int i;
  unsigned long long res[3] = {0};
  unsigned long long total_lines = 0, total_chars = 0, total_words = 0;
  char opt;
  while ((opt = getopt(argc, argv, "wcl")) != -1)
    {
      switch(opt)
	{
	case 'c':
	  c = 1;
	  break;
	case 'w':
	  w = 1;
	  break;
	case 'l':
	  l = 1;
	  break;
	default:
	  wout("USAGE: wc [-lwc] [files]", 24);
	  return EXIT_FAILURE;
	}
    }
  if ( w == 0 && c == 0 && l == 0)
    w = c = l = 1;
  if (argc - optind > 0)
    {
      for( i = optind; i < argc; ++i)
	{
	  file = ((*argv[i] == '-') && (strlen(argv[i])==1))?STDIN_FILENO:open(argv[i], O_RDONLY | O_LARGEFILE);
	  if(file < 0)
	    {
	      perror("open file");
	      continue;
	    }
	  count(file, res);
	  wout(" ",1);
	  wout(argv[i],strlen(argv[i]));
	  wout("\n",1);
	  if(close(file) != 0)
	    {
	      perror("close file");
	      exit(EXIT_FAILURE);
	    }
	  total_lines+=res[0];
	  total_words+=res[1];
	  total_chars+=res[2];
	}
    }
  else
    count(file,res);
  if( argc - optind > 1 )
    {
      if(l)
	{
	  snprintf(tmp,sizeof(tmp), "%llu", total_lines);
	  wout("       ", 8>strlen(tmp)?8 - strlen(tmp):0);
	  wout(tmp, strlen(tmp));
	}
      if(w)
	{
	  snprintf(tmp, sizeof(tmp), "%llu",total_words);
	  wout("       ", 8>strlen(tmp)?8 - strlen(tmp):1);
	  wout(tmp, strlen(tmp));
	}
      if(c)
	{
	  snprintf(tmp, sizeof(tmp), "%llu",total_chars);
	  wout("       ", 8>strlen(tmp)?8 - strlen(tmp):1);
	  wout(tmp, strlen(tmp));
	}
      wout(" total", 7);
    }
  if(argc - optind != 1) wout("\n",1);
  return EXIT_SUCCESS;
}
