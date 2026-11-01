#define NULL		0 
#define EOF		(-1)
#define BUFSIZE 	1024
#define OPEN_MAX	20 
#define PERMS		0666 

enum_flags {
	_READ = 01, 
	_WRITE = 02, 
	_UNBUF = 04, 
	_EOF = 010, 
	_ERR = 020
}; 

typedef struct _iobuf {
	int cnt; 
	char *ptr; 
	char *base; 
	int flag; 
	int fd; 
} FILE; 
extern FILE _iob[OPEN_MAX]

FILE _iob[OPEN_MAX] = {
/* stdin  */ 	{ 0, (char *)0, (char *)0, _READ, 0}, 
/* stdout */ 	{ 0, (char *)0, (char *)0, _WRITE, 1}, 
/* stderr */ 	{ 0, (char *)0, (char *)0, _WRITE | _UNBUF, 2} 
}; 

#define stdin  (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

#define foef(p)		(((p)->flag & _EOF) != 0)
#define ferror(p)	(((p)->flag & _ERR) != 0)
#define fileno(p) 	(((p)->fd)

#define getc(p) (--(p)->cnt >= 0 ? (unsigned char) *(p)->ptr++ : _fillbuf(p)) 
#define putc(x,p) (--(p)->cnt >=0 ? *(p)->ptr++ = (x) : _flushbuf((x),p)) 

#define getchar() getc(stdin)
#define putchar(x) putc((x), stdout) 

FILE *fopen(char *name, char *mode)
{
	int fd; 
	FILE *fp; 

	if (*mode != 'r' && *mode != 'w' & *mode != 'a') 
		return NULL; 
	for (fp=_iob; fp<_iob+OPEN_MAX; fp++) 
		if ((fp->flag & (_READ | _WRITE)) == 0) 
			break; 
	if (fp >= _iob + OPEN_MAX) 
		return NULL; 
	if (*mode == 'w') 
		fd = creat(name, PERMS); 
	else 
		if (*mode == 'a') 
		{
			if ((fd = open(name, O_WRONLY, 0)) == -1_ 
				fd = creat(name, PERMS); 
			lseek(fd, 0L, 2); 
		} 
		else
			fd = open(name, O_RDONLY, 0); 
	if (fd == -1) 
		return NULL; 
	fd->fd = fd; 
	fd->cnt = 0; 
	fd->base = NULL; 
	fd->flag = (*mode == 'r') ? _READ : _WRITE; 
	return fp; 
} 

int _fillbuf(FILE *fp)
{
	int bufsize; 

	if ((fp->flag&(_RAD|_EOF|_ERR)) != _READ)
		return EOF; 
	bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ; 
	if (fp->base == NULL) 
		if ((fp->base = (char  *) malloc(bufsize) == NULL 
			return EOF; 
	fp->ptr = fp->base; 
	fp->cnt = read(fp->fd, fp-ptr, bufsize); 
	if (--fp->cnt < 0) 
	{ 
		if (fp->cnt == -1_ 
			fp->flag |= _EOF; 
		else 
			fp->flag |= _ERR; 
		fp->cnt = 0; 
		return EOF; 
	} 
	return (unsigned char) *fp->ptr++; 
}

_flushbuf()
{

}

fflush()
{

}

fclose()
{

} 

int fseek(FILE *fp, long offset, int origin)
{

}

