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
 *   (orc@pell.portland.or.us)
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

#include "debug.h"
#include "cwatch.h"

/*
 * print the name of a token
 */
char *
cmdname(int token)
{
    switch (token) {
    case WATCHFOR:	return "watchfor";
    case IGNORE:	return "ignore";
    case REGEX:		return "regex";
    case ECHO:		return "echo";
    case NORMAL:	return "normal";
    case BOLD:		return "bold";
    case UNDERSCORE:	return "underscore";
    case BLINK:		return "blink";
    case INVERSE:	return "inverse";
    case BLACK:		return "black";
    case RED:		return "red";
    case GREEN:		return "green";
    case YELLOW:	return "yellow";
    case BLUE:		return "blue";
    case MAGENTA:	return "magenta";
    case CYAN:		return "cyan";
    case WHITE:		return "white";
    case BLACK_H:	return "black_h";
    case RED_H:		return "red_h";
    case GREEN_H:	return "green_h";
    case YELLOW_H:	return "yellow_h";
    case BLUE_H:	return "blue_h";
    case MAGENTA_H:	return "magenta_h";
    case CYAN_H:	return "cyan_h";
    case WHITE_H:	return "white_h";
    case EXEC:		return "exec";
    case PIPE:		return "pipe";
    case KEEP_OPEN:	return "keep_open";
    case WRITE:		return "write";
    case BELL:		return "bell";
    case THROTTLE:	return "throttle";
    case USE:		return "use";
    case QUIT:		return "quit";
    case CONTINUE:	return "continue";
    case WHEN:		return "when";
    case MAIL:		return "mail";
    case ADDRESSES:	return "addresses";
    case SUBJECT:	return "subject";
    case NL:		return "nl";
    case NUMBER:	return "number";
    case WORD:		return "word";
    }
    return "{undefined}";
} /* cmdname */

static void
pattrib(struct command *c)
{
    if (c->c.echo.attrib)
	printf( " %s", cmdname(c->c.echo.attrib));
}


static void
pc(int c, char *suf)
{
    if (c)
	printf(" %s%s", cmdname(c), suf);
}


static void
prr(int bitmask, int max)
{
    int i, j;
    int ct=0;

    if (bitmask == 0)
	printf("1-%d", max);
    else {
	for (i=0; i < max; i++)
	    if (bitmask & (1<<i)) {
		for (j=i; (bitmask & (1<<j)) && (j<max); j++)
		    ;
		if (ct++) putchar(',');
		if (j > i+1) {
		    printf("%d-%d", i+1, j);
		    i = j;
		}
		else
		    printf("%d", i+1);
	    }
    }
}


static void
pwhen(int comma, struct command *c)
{
    int i;
    int ct;

    if (c->day || c->hour) {
	    printf("%cwhen=", comma);
	prr(c->day, 7);
	putchar(':');
	prr(c->hour, 24);
    }
} /* pwhen */


void
dump(struct pattern *p)
{
    struct command *c;
    int i;
    int s;

    while (p) {
	printf("%s /%s/\n", cmdname(p->what), p->rep);
	for (c = p->cmd; c; c = c->next) {
	    putchar('\t');
	    describe(c);
	}
	putchar('\n');
	p = p->next;
    }
} /* dump */


void
describe(struct command *c)
{
    int i, s, ct=' ';

    printf("%s", cmdname(c->what));
    switch (c->what) {
    case ECHO:		pattrib(c);
			pc(c->c.echo.fg, "");
			pc(c->c.echo.bg, "_H");
			break;
    case MAIL:		if (c->c.mail.nraddr) {
			    printf(" addresses");
			    s = '=';
			    for (i=0; i < c->c.mail.nraddr; i++, s=':')
				printf("%c%s", s, c->c.mail.addr[i]);
			    ct = ',';
			}
			if (c->c.mail.subject)
			    printf("%csubject=\"%s\"", ct, c->c.mail.subject);
			break;
    case WRITE:
			for (s=' ',i=0; i < c->c.mail.nraddr; i++,s=':')
			    printf("%c%s",s, c->c.mail.addr[i]);
			break;
    case BELL:
			if (c->c.echo.count > 0)
			    printf(" %d", c->c.echo.count);
			break;
    case EXEC:		printf(" \"%s\"", c->c.exec.cmd);
			break;
    case PIPE:		printf(" \"%s\"", c->c.exec.cmd);
			if (c->c.exec.keepopen) {
			    printf(",keep_open");
			    ct=',';
			}
			break;
    case THROTTLE:	printf(" %d:%d:%d",
			    c->c.throttle.delay / 3600,
			   (c->c.throttle.delay / 60) % 60,
			    c->c.throttle.delay % 60);
			ct=',';
			printf(",use=%s", (c->c.throttle.use == REGEX)
						    ? "regex"
						    : "message");
			break;
    }
    pwhen(ct,c);
    putchar('\n');
    if (c->what == MAIL)
	printf("# mailcmd = [%s]\n", c->c.mail.mailcmd);
} /* describe */
