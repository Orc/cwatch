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
 *  3. All advertising materials mentioning features or use of this
 *     software must display the following acknowledgement:
 *     
 *   This product includes software developed by David Parsons
 *   (orc@pell.chi.il.us)
 *
 *  4. My name may not be used to endorse or promote products derived
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
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>

#include "cwatch.h"
#include "debug.h"


static struct command *cmds = 0;
static struct command tmp;

static pcre *cmd_re;
static char *cmd_rep;
static struct pattern *pat = 0;

extern char *pgm;


statement(int what)
{
    struct pattern *p = calloc(1, sizeof *p);
    struct pattern *q;
    char *err;

    p->what = what;
    p->cmd = cmds;
    p->re = cmd_re;
    p->rep = cmd_rep;
    p->study = pcre_study(p->re, 0, &err);

    cmd_re = 0;
    cmd_rep = 0;
    cmds = 0;

#if DEBUG
    printf("statement %s /%s/\n", cmdname(what), p->rep);
#endif
    if (pat == 0)
	pat = p;
    else {
	for (q=pat; q->next; q = q->next)
	    ;
	q->next = p;
    }
}


static char *
mailcmd(char *pgm, struct command *p)
{
    struct passwd *pwd;
    int i, size;
    char *dest;

    size = 2+strlen(pgm);

    if (p->c.mail.nraddr == 0) {
	if ((pwd = getpwuid(getuid())) != 0)
	    size += 1+strlen(pwd->pw_name);
    }
    else for (i=0; i < p->c.mail.nraddr; i++)
	size += 1+strlen(p->c.mail.addr[i]);

    dest = malloc(1+size);

    if (p->c.mail.nraddr) {
	strcpy(dest, pgm);
	for (i=0; i < p->c.mail.nraddr; i++)
	    sprintf(dest+strlen(dest), " %s", p->c.mail.addr[i]);
    }
    else
	sprintf(dest, "%s %s", pgm, pwd->pw_name);
    return dest;
}


action(int order)
{
    struct command *p = calloc(1, sizeof *p);
    struct command *q;

    memcpy(p, &tmp, sizeof tmp);
    memset(&tmp, 0, sizeof tmp);
    p->what = order;
    p->next = 0;

    /* fill in extra fields when the structure is finalized */
    switch (order) {
    case MAIL:
	    p->c.mail.mailcmd = mailcmd(MAILPROG, p);
	    break;
    }

#if DEBUG
    printf("action "); describe(p);
#endif
    if (cmds == 0)
	cmds = p;
    else {
	for (q = cmds; q->next; q = q->next)
	    ;
	q->next = p;
    }
}


re(char *pat)
{
    char *whine;
    int where;

    if ( (cmd_re = pcre_compile(pat, 0, &whine, &where, 0)) ) {
	cmd_rep = strdup(pat);
	return 1;
    }
    
    fprintf(stderr, "cwatch: %.*s[%s]: %s\n", where, pat, pat+where, whine);
    return 0;
}


attrib(int attrib)
{
    tmp.c.echo.attrib = attrib;
}


fg(int color)
{
    tmp.c.echo.fg = color;
}


bg(int color)
{
    tmp.c.echo.bg = color;
}


nrbell(int num)
{
    tmp.c.echo.count = num;
}


keep_open()
{
    tmp.c.exec.keepopen = 1;
}


address(char *mailto)
{
    if (tmp.c.mail.addr == 0)
	tmp.c.mail.addr = malloc(1);
    tmp.c.mail.addr = realloc(tmp.c.mail.addr,
			(1+tmp.c.mail.nraddr) * sizeof tmp.c.mail.addr[0]);

    tmp.c.mail.addr[tmp.c.mail.nraddr] = strdup(mailto);
    tmp.c.mail.nraddr ++;
}


nextre(char *pat)
{
    char *whine;
    int where;

    tmp.c.throttle.nextrep = strdup(pat);
    if ( (tmp.c.throttle.nextre = pcre_compile(pat, 0, &whine, &where, 0)) )
	return 1;
    
    fprintf(stderr, "cwatch: %.*s[%s]: %s\n", where, pat, pat+where, whine);
    return 0;
}


delay(int hr, int min, int sec)
{
    tmp.c.throttle.delay = (hr * 3600) + (min * 60) + sec;
}


use(char *pat)
{
    tmp.c.throttle.use = strdup(pat);
}


subject(char *pat)
{
    tmp.c.mail.subject = strdup(pat);
}


cmdline(char *command)
{
    tmp.c.exec.cmd = strdup(command);
}

static
mask(unsigned *flags, char *name, unsigned first, unsigned last, unsigned max)
{
#if DEBUG
    printf("mask(%d,\"%s\",%d,%d,%d)\n", *flags, name, first, last, max);
#endif
    if (first < 1 || first > max) {
	fprintf(stderr, "bad %s %d\n", name, first);
	return 0;
    }
    if (last == 0) {
	(*flags) |= 1<<(first-1);
	return 1;
    }
    else if (last > max || last < first) {
	fprintf(stderr, "bad % range %d-%d\n", name, first, last);
	return 0;
    }
    while (first <= last) {
	(*flags) |= 1<<(first-1);
	first++;
    }
    return 1;
}

day(unsigned int first, unsigned int last)
{
    return mask(&tmp.day, "day", first, last, 7);
}

hour(unsigned int first, unsigned int last)
{
    return mask(&tmp.hour, "hour", first, last, 24);
}


struct pattern *
compile()
{
    yyparse();
    return pat;
}
