#include "ansi.h"

size_t	strnlen(const char	*s,
		size_t		maxlen)
{
  size_t	len;

  for (len = 0; len < maxlen; len++, s++)
  {
    if (!*s)
      break;
  }
  return (len);
}

char	*strdup(const char	*str)
{
  size_t	len;
  char		*copy;

  len = strlen(str) + 1;
  if ((copy = malloc(len * sizeof (char))) == NULL)
    return (NULL);
  memcpy(copy, str, len);
  return (copy);
}

char	*strndup(const char	*str,
		 size_t		n)
{
  size_t	len;
  char	*copy;

  len = strnlen(str, n);
  if ((copy = malloc(len + 1)) == NULL)
    return (NULL);
  memcpy(copy, str, len);
  copy[len] = '\0';
  return (copy);
}

/*# define MY_DEBUG_*/
# define MY_BUF_SIZE (512 * sizeof (char))

/*
** Teste l'existence du caractere dans la chaine
** S'il est trouve : on renvoie sa position,
** Sinon on renvoie la taille de la chaine
*/
int	getline_test_char(char	*str,
			  int	len,
			  char	c)
{
  int	i = 0;

  while ((i < len) && (str[i] != c))
    i++;
  return (i);
}

long	getline_cp_line(char	**linep,
			size_t	*linecapp,
			char	*buf,
			int	len,
			int	j)
{
  long	i = 0;

  while ((i < len) && (buf[i] != '\n') && (buf[i] != '\0'))
    i++;
  if ((i + (MY_BUF_SIZE * j)) >= (*linecapp))
  {
    (*linecapp) = i + (MY_BUF_SIZE * j);
	(*linep) = realloc((*linep), (i + (MY_BUF_SIZE * j) + 1) * sizeof (char));
  }
  memcpy((*linep) + (MY_BUF_SIZE * j * sizeof (char)), buf, i * sizeof (char));
  (*linep)[(i + (MY_BUF_SIZE * j))] = '\0';
  buf[MY_BUF_SIZE - 1] = '\0';
  return (i);
}

ssize_t		getline(char	**linep,
			size_t	*linecapp,
			FILE	*stream)
{
  char		buf[MY_BUF_SIZE];
  long		i = 0;
  long		j = 0;
  ssize_t	len = 0;

  memset(buf, 0, MY_BUF_SIZE * sizeof (char));
  len = fread(buf, sizeof (char), MY_BUF_SIZE, stream);
   if (len == 0) /* fichier vide ou deja lu */
    return (-1);
  while ((len == MY_BUF_SIZE) &&
	 (getline_test_char(buf, len, '\n') == MY_BUF_SIZE))
  {
    i += getline_cp_line(linep, linecapp, buf, len, j);
    len = fread(buf, sizeof (char), MY_BUF_SIZE, stream);
    j++;
  }
  i += getline_cp_line(linep, linecapp, buf, len, j);
  if (len == MY_BUF_SIZE) /* on revient en arriere si pas fin de fichier */
    fseek(stream, 1 - ((MY_BUF_SIZE * (j + 1)) - i), SEEK_CUR);
  len = i;
  return (len);
}

/* Print a message and return to caller.
 * Caller specifies "errnoflag" and "level". */
void	err_doit(int		errnoflag,
		 const char	*fmt,
		 va_list	ap)
{
  int	errno_save, n;
  char	buf[MAXLINE];

  errno_save = errno;			/* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
  vsnprintf(buf, sizeof (buf), fmt, ap);	/* this is safe */
#else
  vsprintf(buf, fmt, ap);		/* this is not safe */
#endif
  n = strlen(buf);
  if (errnoflag)
    snprintf(buf+n, sizeof (buf)-n, ": %s", strerror(errno_save));
  strcat(buf, "\n");

  fflush(stdout);		/* in case stdout and stderr are the same */
  fputs(buf, stderr);
  fflush(stderr);
}

/* Fatal error unrelated to a system call.
 * Print a message and terminate. */
void	err_quit(const char	*fmt, ...)
{
  va_list	ap;

  va_start(ap, fmt);
  err_doit(0, fmt, ap);
  va_end(ap);
  exit(1);
}

int	snprintf(char		*buf,
		 size_t		size,
		 const char	*fmt, ...)
{
  unsigned int	n;
  va_list	ap;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);	/* Sigh, some vsprintf's return ptr, not length */
  n = strlen(buf);
  va_end(ap);
  if (n >= size)
    err_quit("snprintf: '%s' overflowed array", fmt);
  return (n);
}
