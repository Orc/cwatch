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
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "config.h"
#include "cwatch.h"
#include "getchunk.h"

#ifdef HAVE_BASIS_OPTIONS_H
#   include <basis/options.h>
#else
#   include "basis/options.h"
#endif


int istail = 0;
int ispipe = 0;
int isfile = 0;
int dumpit = 0;
char *eol = "\n";
pcre *reol;
char *input = "/var/log/syslog";
char *config_file = 0;
char *pgm;

FILE *pipefd = 0;
struct command *pipecmd;


/* run() flags
 */
#define INTERACTIVE	0x01
#define THROTTLED	0x02

struct x_option opts[] = {
    { 'C', 'c', "config-file", "file", "the config file ($HOME/.cwatchrc)" },
    { 'F', 'F', "input-record-separator", "RE", "Use the given RE to delineate each\n"
						"input record" },
    { 't', 't', "tail", "FILE", "examine lines of text as they are added\n"
				"to FILE (/var/log/syslog)" },
    { 'p', 'p', "read-pipe", "COMMAND", "examine input piped in from COMMAND" },
    { 'f', 'f', "examine", "FILE", "do a single pass through FILE" },
    { 'D',  0,  "dump-script", 0, "do a debugging dump of the config file" },
    { '?',  0,  "help", 0, "print usage information and exit." },
    { 'V', 'v', "version", 0, "print version information and exit." },
} ;
#define NROPTS	(sizeof opts/sizeof opts[0])

extern void showopts(FILE*, int, struct x_option*);


void
getoptionsandscript(int argc, char **argv)
{
    int opt;
    char *whine;
    int where;

    x_opterr = 1;

    while ( (opt = x_getopt(argc, argv, NROPTS, opts)) != EOF) {
	switch (opt) {
	case 'C':	config_file = x_optarg; break;
	case 'V':	fprintf(stderr, "cwatch %s\n", VERSION); exit(0);
	case 't':	istail = 1; input = x_optarg; break;
	case 'p':	ispipe = 1; input = x_optarg; break;
	case 'f':	isfile = 1; input = x_optarg; break;
	case 'D':	dumpit = 1; break;
	case 'F':	eol = x_optarg; break;
	default:	fprintf(stderr, "usage: %s [options]\n", pgm);
	case '?':	showopts(stderr, NROPTS, opts);
			exit( !(opt=='?') );
	}
    }
    if ( (reol = pcre_compile(eol, 0, &whine, &where, 0)) == 0) {
	fprintf(stderr, "%s: %s (%s)\n", pgm, whine, eol);
	exit(1);
    }

    switch (istail + ispipe + isfile) {
    case 0 :	istail = 1;
    case 1 :	break;
    default:	fprintf(stderr, "%s: can only use one of -t, -p, -f\n", pgm);
		fprintf(stderr, "usage: %s [options]\n", pgm);
		showopts(stderr, NROPTS, opts);
		exit(1);
    }
}


/*
 * expand() a line, replacing $0..$9 with substrings from the original
 *          pcre pattern match.
 */
static char *
expand(char *src, int ssiz, char *cmd, int argc, int argv[])
{
    char *dest;
    int arg, cpy, size = 2 * strlen(cmd);
    int add, di = 0;
    char *p;

    dest = malloc(size);

    for (p = cmd; *p; ++p) {
	if ( (*p == '$') && (isdigit(p[1]) || p[1] == '*')) {
	    ++p;
	    switch (*p) {
	    case '*':
	    case '0':   arg = 0;
			add = ssiz;
			break;
	    default:	arg = 2 * ((*p)-'0');
			add = argv[arg+1]-argv[arg];
			break;
	    }
	    if (di + add >= size-2) {
		size += add;
		dest = realloc(dest, size *= 2);
	    }
	    for (cpy=arg ? argv[arg] : 0; add > 0; --add)
		dest[di++] = src[cpy++];
	}
	else {
	    if (di >= size-2)
		dest = realloc(dest, size *= 2);
	    dest[di++] = *p;
	}
    }
    dest[di] = 0;

    return dest;
} /* expand */


/*
 * command() expand a commandline, then run it
 */
static void
command(char *src, int size, struct command *cmd, int argc, int argv[])
{
    char *val;

    if (val = expand(src, size, cmd->c.exec.cmd, argc, argv)) {
	system(val);
	free(val);
    }
} /* command */


/*
 * pipeline() writes a line into a pipeline.
 */
static void
pipeline(char *src, int size, struct command *cmd, int argc, int argv[])
{
    char *val = expand(src, size, cmd->c.exec.cmd, argc, argv);

    if (val) {
	if (pipefd && (pipecmd != cmd)) {
	    pclose(pipefd);
	    pipefd = 0;
	}
	if (pipefd == 0)
	    pipefd = popen(val, "w");

	fwrite(src, size, 1, pipefd);
	fputc('\n', pipefd);
	fflush(pipefd);

	if (cmd->c.exec.keepopen) {
	    fflush(pipefd);
	    pipecmd = cmd;
	}
	else {
	    pclose(pipefd);
	    pipefd = 0;
	    pipecmd = 0;
	}
    }
} /* pipeline */


/*
 * echoit() echo a matched line, putting appropriate throttle prefixes in.
 */
static
echoit(char *line, int size, struct command *q, FILE *out)
{
    if (q->root->count)
	fprintf(out, "** %d in %d:%d:%d == ", q->root->count,
		q->root->delay / 3600,
	       (q->root->delay / 60) % 60,
		q->root->delay % 60);

    fwrite(line, size, 1, out);
    fputc('\n', out);
} /* echoit */


/*
 * notnow() sees if we're not in a when clause
 */
#define Btest(m,b)	( (m) & (1<<(b)) )
int
notnow(struct command *q, struct tm *now)
{
    if (q->hour || q->day)
	return ! (Btest(q->day,now->tm_wday) && Btest(q->hour, now->tm_hour));
    return 0;
} /* notnow */


/*
 * mailto() sends mail to people
 */
static
mailto(char *s, int size, struct command *q, int ac, int av[])
{
    FILE *f;

    if ((f = popen(q->c.mail.mailcmd, "w")) != 0) {
	if (q->c.mail.subject) {
	    char *val;
	    val = expand(s, size, q->c.mail.subject, ac, av);
	    fprintf(f, "Subject: %s\n", val);
	    free(val);
	}
	fprintf(f, "\n----\n");
	echoit(s, size, q, f);
	fprintf(f, "----\n      --Cwatch\n");
	pclose(f);
    }
} /* mailto */


/*
 * run() commands associated with a pattern
 */
static int
run(struct pattern *p, char *s, int size, int flags,
		   struct tm *now, int ac, int av[])
{
    struct command *q;
    int keepon = 0;
    int die = 0;
    int count;

    for (q = p->cmd; q; q = q->next) {
	if ( notnow(q,now) ) continue;

	if (q->what == CONTINUE) keepon = 1;

	if ((flags & THROTTLED) == 0) {
	    q->root = p;
	    switch (q->what) {
	    case EXEC:	command(s, size, q, ac, av);
			break;
	    case PIPE:	pipeline(s, size, q, ac, av);
			break;
	    case BELL:	count=0;
			do {
			    write(1, "\007", 1);
			    sleep(1);
			} while (count++ < q->c.echo.count);
			break;	/* documentation implies BELL==ECHO, but
				 * the code doesn't do that.
				 */
	    case ECHO:	echoit(s, size, q, stdout);
			break;
	    case WRITE:	break;
	    case MAIL:	mailto(s, size, q, ac, av);
			break;
	    case THROTTLE:
			if (flags & INTERACTIVE) {
			    p->delay = q->c.throttle.delay;
			    p->throttle = time(0) + p->delay;
			    p->count = 0;
			}
			break;
	    case QUIT:	die = 1;
			break;
	    }
	}
    }
    if (die) exit(0);
    if ( (flags & THROTTLED) == 0) p->count = 0;
    return !keepon;
}


/*
 *  match() a line to a pattern, and run the commands associated with
 *          the pattern as necessary.
 */
static int
match(struct pattern *p, char *line, int size, int interactive, time_t now)
{
    int argv[30];
    int argc = pcre_exec(p->re, p->study, line, size, 0, 0, argv, 30);
    int flags = interactive ? INTERACTIVE : 0;
    
    if (argc >= 0) {
	if (p->what == IGNORE)
	    return 1;
	else {
	    if (interactive && (p->throttle > now)) {
		p->count++;
		flags |= THROTTLED;
	    }
	    return run(p, line, size, flags, localtime(&now), argc, argv);
	}
    }

    return 0;
} /* match */


/*
 * scan() a FILE* for patterns
 */
static
scan(FILE *file, struct pattern *p)
{
    struct pattern *q;
    char *text;
    void *f;
    int len;

    if ( (f = newchunk(file, eol)) ) {
	while ( (text = getchunk(f, &len)) ) {
	    for (q = p; q; q = q->next)
		if (match(q, text, len, 0, time(0)))
		    break;
	}
	freechunk(f);
    }
} /* scan */


/*
 * dopipe() runs a command and looks at the output from it
 */
static
dopipe(struct pattern *p)
{
    FILE *file;

    if ( (file = popen(input, "r")) == 0) {
	perror(input);
	exit(1);
    }
    scan(file, p);
    pclose(file);
} /* dopipe */


/*
 * dotail() tails a command (via rebuilding input as ``tail -f $input''
 *          and calling dopipe.)
 */
static
dotail(struct pattern *p)
{
    char *cmd;
    static char *tailcmd = "tail -1f";

    if (access(input, R_OK) != 0) {
	perror(input);
	exit(1);
    }
    if (cmd = malloc(strlen(input) + strlen(tailcmd) + 3)) {
	sprintf(cmd, "%s %s", tailcmd, input);
	input = cmd;
	dopipe(p);
    }
} /* dotail */


/*
 * dofile() looks through an already existing file.
 */
static
dofile(struct pattern *p)
{
    FILE *file;

    if (strcmp(input, "-") == 0)
	file = stdin;
    else if ( (file = fopen(input, "r")) < 0) {
	fprintf(stderr, "%s: cannot open %s\n", pgm, input);
	exit(1);
    }
    scan(file, p);
    fclose(file);
} /* dofile */


int errors = 0;

yyerror(char *s)
{
    extern int line_number;

    fprintf(stderr, "%s on line %d\n", s, line_number);
    errors++;
}


float
main(int argc, char **argv)
{
    struct pattern *p;

#ifdef HAVE_BASENAME
    pgm = basename(argv[0]);
#else
    if (pgm = strrchr(argv[0], '/'))
	argv[0] = ++pgm;
    else
	pgm = argv[0];
#endif

    getoptionsandscript(argc, argv);

#if 0
    /* if it's running as a daemon, fork off a child and restart
     * it every time it dies. (commented out for now because I'm
     * not exactly sure how swatch does it.)
     */
    while (daemon) {
	int status;
	pid_t pid;
	pid_t rc;

	if ( (pid = fork()) < 0) {
	    perror(pgm);
	    sleep(1);
	}
	else if (pid > 0)
	    break;
	else do {
	    rc = wait(&status);
	} while (rc != pid && kill(pid, 0) == 0);
    }
#endif

    if (config_file == 0) {
	char *home = getenv("HOME");
	config_file = malloc( 2 + strlen("/.cwatchrc")
				+ strlen( home ? home : "" ) );
	sprintf(config_file, "%s/.cwatchrc", home ? home : "");
	/* if no config file is provided, attempt to open a default
	 * file.  If that fails use a compiled-in default
	 */
	if (!openinput(config_file))
	    defaultinput("watchfor /.*/ echo");
    }
    else if ( !openinput(config_file) ) {
	fprintf(stderr, "%s: no such file %s\n", pgm, config_file);
	exit(1);
    }

    if ( (p = compile()) == 0 || errors) 
	exit(1);

    if (dumpit)
	dump(p);
    else if (istail)
	dotail(p);
    else if (ispipe)
	dopipe(p);
    else
	dofile(p);
    exit(0);
}
