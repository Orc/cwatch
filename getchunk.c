/*
 *   Copyright (c) 2002 David Parsons. All rights reserved.
 *   
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *  3. My name may not be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *     
 *  THIS SOFTWARE IS PROVIDED BY DAVID PARSONS ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID
 *  PARSONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * getchunk implements a record stream filter that takes an arbitrary
 * FILE * as input, and spits it out in chunks delimited by a given
 * regular expression.
 *
 *  void * newchunk(FILE*,char*)	-- creates a new filter.
 *  char * getchunk(void*,int*)		-- return a pointer to the start of
 *					   the next record, or 0.  The size
 *					   of the (NOT null terminated)
 *					   record is returned.
 *  void freechunk(void*)		-- deletes a filter.
 *
 * to copy cat, you could do:
 *      void *cat = newchunk(stdin, "\n");
 *      while (text = getchunk(cat,&size))
 *            printf("%.*s\n", size, text);
 *      freechunk(cat);
 *
 * This is sort of like the perl ``chop'' if you're sufficiently far away.
 */

#include <stdio.h>
#if HAVE_MALLOC_H
#   include <malloc.h>
#endif
#include <pcre.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include "config.h"

#define MINBLK	128
#define	MAXSTART	(MINBLK*80)

typedef struct {
    FILE *file;
    char *buf;
    pcre *eol;
    int flag;
#define MMAPPED 0x01
    int size;
    int start, end;
} CHUNK;


CHUNK *
newchunk(FILE *file, char *eol)
{
    CHUNK *tmp;
    const char *whine;
    int where;
#if HAVE_MMAP
    struct stat st;
#endif

    if (file == 0 || eol == 0)
	return 0;
    
    if ( (tmp = calloc(1, sizeof *tmp)) ) {
	if ( !(tmp->eol = pcre_compile(eol, 0, &whine, &where, 0)) ) {
	    free(tmp);
	    return 0;
	}
#if HAVE_MMAP
	if ( fstat(fileno(file), &st) == 0
	       && S_ISREG(st.st_mode) 
	       && (tmp->buf = mmap(0, st.st_size,
			PROT_READ, MAP_SHARED, fileno(file), 0)) ) {
	    tmp->flag |= MMAPPED;
	    tmp->size = tmp->end = st.st_size;
	    return tmp;
	}
#endif
	if ( (tmp->buf = malloc(tmp->size = MINBLK)) == 0) {
	    free(tmp);
	    return 0;
	}
	tmp->file = file;
	tmp->start = 0;
	tmp->end = 0;
    }
    return tmp;
} /* newchunk */


char *
getchunk(CHUNK *f, int *size)
{
    int argc, argv[3];
    int rc;
    char *ret;

    *size = 0;

#if HAVE_MMAP
    if (f->flag & MMAPPED) {
	if (f->start >= f->size)
	    return 0;
	rc = pcre_exec(f->eol,0,f->buf,f->size,f->start,0,argv,3);
    }
    else
#endif
    {
	if (f->start >= f->end) {
	    /* if we've exhausted the buffer, reset
	     * the start and end pointers
	     */
	    f->start = f->end = 0;
	    if (feof(f->file))
		return 0;
	}
	else if (f->start > MAXSTART) {
	    /* if we're too deep into the buffer, slide
	     * everything down to the beginning
	     */
#if HAVE_MEMMOVE
	    memmove(f->buf, f->buf + f->start, f->end - f->start);
#elif MEMCPY_CAN_OVERLAP
	    memcpy(f->buf, f->buf + f->start, f->end - f->start);
#else
	    char *s, *d;
	    int size;

	    for (s=f->buf, d=(f->buf + f->start), size=(f->end - f->start);
		    size > 0; )
		*s++ = *d++;
#endif
	    f->end -= f->start;
	    f->start = 0;
	}

	/* check the buffer for the eol re, grabbing more text until we
	 * find it or run out of input.
	 */
	while ( (rc=pcre_exec(f->eol,0,f->buf,f->end,f->start,0,argv,3)) < 0) {
	    int s;

	    if ( (f->size - f->end) < MINBLK) {
		f->size += MINBLK*2;
		f->buf = realloc(f->buf, f->size);
	    }
	    s = fread(f->buf + f->end, 1, MINBLK, f->file);
	    if (s > 0)
		f->end += s;
	    else
		break;
	}
    }
    ret = f->buf + f->start;
    if (rc < 0) {
	*size = f->end - f->start;
	f->start = f->end;
    }
    else {
	*size = argv[0] - f->start;
	f->start = argv[1];
    }
    return ret;
} /* getchunk */


void
freechunk(CHUNK *f)
{
#if HAVE_MMAP
    if (f->flag & MMAPPED) munmap(f->buf, f->end);
    else
#endif
    if (f->buf) free(f->buf);
    free(f);
}



#if TEST
main()
{
    int where;
    char *whine;
    void *f;
    char *text;
    int size;

    f = newchunk(stdin, ":");

    while ( (text = getchunk(f, &size)) )
	printf("[%.*s]\n", size, text);

    freechunk(f);
}
#endif
