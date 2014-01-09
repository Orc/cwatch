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
#include <stdio.h>
#include <string.h>
#if HAVE_MALLOC_H
#   include <malloc.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "config.h"
#include "cwatch.h"
#include "getchunk.h"

#ifdef HAVE_BASIS_OPTIONS_H
#   include <basis/options.h>
#else
#   include "basis/options.h"
#endif


int tflag = 0;
int pflag = 0;
int fflag = 0;
int Dflag = 0;
int dflag = 0;

int rflag = 0;	/* -r with absolute time */
int rplus = 0;	/* -r with delta time */

char *eol = "\n";
pcre *reol;
char *input = 0;
char *cfgfile = 0;
char *pidfile = 0;
char *pgm;

FILE *pipefd = 0;
struct command *pipecmd;

struct pattern *gthrottle = 0;	/* current use=regex throttle */
int throttled = 0;		/* how many throttles are running? */
time_t expiration;		/* when the next throttle expires */
struct pattern *patterns;	/* for cleanup() */

void expire(int sig);

/* run() flags
 */
#define INTERACTIVE	0x01
#define THROTTLED	0x02

struct x_option opts[] = {
    { 'C', 'c', "config-file", "file", "the config file ($HOME/.cwatchrc)" },
    { 'F', 'F', "input-record-separator", "RE", "Use the given RE to delineate each\n"
						"input record" },
    { 'd', 'd',  "daemon", 0, "Run it as a daemon" },
    { 'p', 'p', "read-pipe", "COMMAND", "examine input piped in from COMMAND" },
    { 'f', 'f', "examine", "FILE", "do a single pass through FILE" },
    { 't', 't', "tail", "FILE", "examine lines of text as they are added\n"
				"to FILE" },
    { '!',  0 , "pid-file", "FILE", "Write the process-id to FILE" },
    { 'r', 'r', "restart-time", "TIME", "Restart at TIME" },
    { 'D',  0 , "dump-script", 0, "do a debugging dump of the config file" },
    { '?',  0 , "help", 0, "print usage information and exit." },
    { 'V', 'v', "version", 0, "print version information and exit." },
} ;
#define NROPTS	(sizeof opts/sizeof opts[0])

extern void showopts(FILE*, int, struct x_option*);

void
usage(int rc)
{
    fprintf(stderr, "usage: %s [options]\n", pgm);
    showopts(stderr, NROPTS, opts);
    exit(rc);
}

/*
 * getrestarttime() parses a -r option and sets the appropriate combination
 * of the rplus value and the rflag.
 */
void
getrestarttime(char *opt)
{
    char *arg = opt;
    int ct;
    char apm[3];
    unsigned int hr, mn, ispm=0;

    if (*opt == '+')
	++opt;
    else
	rflag = 1;

    ct = sscanf(opt, "%u:%u%2s", &hr, &mn, apm);

    if (hr >= ((ct == 3) ? 12 : 24) || (mn > 59) ) {
no:	fprintf(stderr, "%s: garbled time for -r%s\n", pgm, arg);
	fprintf(stderr, "ct=%d, hr=%d, mn=%d, apm=[%s]\n", ct, hr, mn, apm);
        exit(1);
    }

    if (ct == 3) {
	ispm = !strcasecmp(apm, "pm");
	if (! (ispm || strcasecmp(apm, "am") == 0))
	    goto no;
	if (ispm && !rplus)
	    hr += 12;
    }

    /* the rplus time is the hour+minute encoded as minutes */
    rplus = mn + (hr*60);
} /* getrestarttime */


void
getoptionsandscript(int argc, char **argv)
{
    int opt;
    char *whine;
    int where;

    x_opterr = 1;

    while ( (opt = x_getopt(argc, argv, NROPTS, opts)) != EOF) {
	switch (opt) {
	case 'C':	cfgfile = x_optarg; break;
	case 'V':	fprintf(stderr, "cwatch %s\n", VERSION); exit(0);
	case 't':	tflag = 1; input = x_optarg; break;
	case 'p':	pflag = 1; input = x_optarg; break;
	case 'f':	fflag = 1; input = x_optarg; break;
	case 'd':	dflag = 1; break;
	case 'D':	Dflag = 1; break;
	case '!':	pidfile = x_optarg; break;
	case 'F':	eol = x_optarg; break;
	case 'r':	getrestarttime(x_optarg); break;
	default:	usage( !(opt=='?') );
	}
    }
    if ( (reol = pcre_compile(eol, 0, &whine, &where, 0)) == 0) {
	fprintf(stderr, "%s: (-F %s): %s\n", pgm, eol, whine);
	exit(1);
    }

    switch (tflag + pflag + fflag + Dflag) {
    case 0 : 	tflag = 1;
    case 1 :	break;
    default:	fprintf(stderr, "%s: can only use one of -t, -p, -f, or --dump-script\n", pgm);
		usage(1);
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
	if ( (*p == '$') && (isdigit(p[1]) || p[1] == '*' || p[1] == '@')) {
	    ++p;
	    switch (*p) {
	    case '*':
	    case '0':   arg = -1;
			cpy = 0;
			add = ssiz;
			break;
	    case '@':   arg = 0;
			cpy = argv[0];
			add = argv[1]-cpy;
			break;
	    default:	arg = 2 * ((*p)-'0');
			cpy = argv[arg];
			add = argv[arg+1]-cpy;
			break;
	    }
	    if (di + add >= size-2) {
		size += add;
		dest = realloc(dest, size *= 2);
	    }
	    for (; add > 0; --add)
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


typedef (*echofn)(char*,int,struct command *, FILE*);

/*
 * echoun() spits out a throttle message for a throttle that's just
 *          expired.
 */
static
echoun(char *line, int size, struct command *q, FILE *out)
{
    struct pattern *p = q->root;
    int real_delay;

#ifdef HAVE_DIFFTIME
    real_delay = p->delay - difftime(p->throttle, p->when);
#else
    real_delay = p->delay - (p->throttle - p->when);
#endif
    if (real_delay < 0) real_delay = 0;

    fprintf(out, "** %d in %02d:%02d:%02d == ", 1 + p->count,
	    real_delay / 3600,
	   (real_delay / 60) % 60,
	    real_delay % 60);

    if (p->trigger->c.throttle.use == REGEX)
	fprintf(out, "/%s/\n", p->rep);
    else
	fprintf(out, "%s\n", p->trigger->c.throttle.msg);
    fflush(out);
} /* echoun */


/*
 * echoit() echo a matched line, putting appropriate throttle prefixes in.
 */
static
echoit(char *line, int size, struct command *q, FILE *out)
{
    fwrite(line, size, 1, out);
    fputc('\n', out);
    fflush(out);
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
mailto(char *s, int size, struct command *q, int ac, int av[], echofn echo)
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
	(*echo)(s, size, q, f);
	fprintf(f, "----\n      --Cwatch\n");
	pclose(f);
    }
} /* mailto */


/*
 * writeto() write(1)'s people
 */
static
writeto(char *s, int size, struct command *q, int ac, int av[], echofn echo)
{
    char **recipients = q->c.mail.addr;
    int  nrrecipients = q->c.mail.nraddr;
    char bfr[17+1+80];	/* sizeof("write 2>/dev/null") + sizeof(" ")
			   + 79 characters name + sizeof("\0") */
    FILE *f;
    int idx;

    for (idx=0; idx < nrrecipients; idx++) {
	sprintf(bfr, "write 2>/dev/null %.79s", recipients[idx]);

	if ( (f = popen(bfr, "w")) != 0) {
	    (*echo)(s, size, q, f);
	    pclose(f);
	}
    }
} /* writeto */


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

	q->root = p;
	if (flags & THROTTLED) {
	    if (p->count == 0) continue;

	    switch (q->what) {
	    case WRITE: writeto(s, size, q, ac, av, echoun);
			break;
	    case MAIL:  mailto(s, size, q, ac, av, echoun);
			break;
	    case ECHO:  echoun(s, size, q, stdout);
			break;
	    }
	}
	else {
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
	    case WRITE:	writeto(s, size, q, ac, av, echoit);
			break;
	    case MAIL:	mailto(s, size, q, ac, av, echoit);
			break;
	    case THROTTLE:
			p->delay = q->c.throttle.delay;
			p->throttle = time(0) + p->delay;
			p->trigger = q;
			p->count = 0;
			if (q->c.throttle.use == REGEX)
			    gthrottle = p;
			else {
			    if (q->c.throttle.msg)
				free(q->c.throttle.msg);
			    q->c.throttle.msg = expand(s,size,"$@",ac,av);
			}
			throttled ++ ;
			break;
	    case QUIT:	die = 1;
			break;
	    }
	}
    }
    if (die) exit(0);
    return !keepon;
}


/*
 * unthrottle() spits out whatever commands are needed after 
 * throttle expires
 */
void
unthrottle(struct pattern *p, time_t now)
{
    if (p->throttle) {
	run(p, 0, 0, THROTTLED, localtime(&now), 0, 0);
	p->count = 0;
	p->throttle = 0;
	p->delay = 0;
	p->trigger = 0;
	--throttled;
    }
} /* unthrottle */


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
	if (gthrottle && (gthrottle != p)) {
	    unthrottle(gthrottle, now);
	    gthrottle = 0;
	}
	if (p->what == IGNORE)
	    return 1;
	else {
	    if (p->throttle > now) {
		int ok;
		char *m = expand(line, size, "$@", argc, argv);

#define T	(p->trigger->c.throttle)
		ok = T.msg && !strcmp(T.msg, m);
#undef T
		free(m);

		if (ok) {
		    p->when = now;
		    p->count++;
		    return 1;
		}
		unthrottle(p, now);
	    }
	    p->when = now;
	    return run(p, line, size, flags, localtime(&now), argc, argv);
	}
    }

    return 0;
} /* match */


/*
 * setalarm() if any throttles are running
 */
void
setalarm()
{
    alarm(0);

    if (throttled) {
	struct pattern *q;
	time_t now = time(0);
	time_t next = 0;

	for (q = patterns; q; q = q->next)
	    if (q->throttle && (next == 0 || q->throttle < next))
		next = q->throttle;

	signal(SIGALRM, expire);
	alarm( (next > now) ? (next-now) : 10);
    }
} /* setalarm */


/*
 * restart_alarm() sets the restart alarm, if it's set
 */
void
restart_alarm()
{
    if (rplus)
	if (rflag) {
	    /* at absolute time ... */
	    time_t now = time(0);
	    int hr, min;
	    int delay;
	    struct tm *t;

	    t = localtime(&now);
	    hr = rplus / 60;
	    min = rplus % 60;

	    /* hours delay */
	    delay =  60 * ((t->tm_hour - hr) + (hr < t->tm_hour) ? 24 : 0);
	    /* plus minutes delay */
	    delay += (t->tm_min - min) + (min < t->tm_min) ? 60 : 0;

	    alarm(60*delay);
	}
	else {
	    /* .. or after some interval */
	    alarm(60*rplus);
	}
} /* restart_alarm */


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
    time_t now;
    int remainder;

    if ( (f = newchunk(file, eol)) ) {
	while ( (text = getchunk(f, &len)) ) {
	    alarm(0);
	    now = time(0);
	    for (q = p; q; q = q->next) {
		if (q->throttle && (q->throttle <= now))
		    unthrottle(q, now);
		else if (match(q, text, len, 0, now))
		    break;
	    }
	    setalarm();
	}
	freechunk(f);
	alarm(0);
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
    else if ( (file = fopen(input, "r")) == 0) {
	fprintf(stderr, "%s: cannot open %s\n", pgm, input);
	exit(1);
    }
    scan(file, p);
    fclose(file);
} /* dofile */


/*
 * parser error catcher -- put here because we don't want to run if we
 * have any errors.
 */
int errors = 0;

yyerror(char *s)
{
    extern int line_number;

    fprintf(stderr, "%s on line %d\n", s, line_number);
    errors++;
} /* yyerror */


/*
 * broadcast a death signal to everyone in our process group
 */
void
alldie(int sig)
{
    signal(sig, SIG_IGN);
    kill(0, sig);
    exit(0);
} /* alldie */


/*
 * broadcast a signal to everyone in our process group, but
 * don't die.
 */
void
restart(int sig)
{
    typedef void (*handler)(int);

    signal(sig, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    kill(0, SIGINT);
    sleep(1);

    signal(sig, restart);
    signal(SIGINT, alldie);
} /* restart */


/*
 * clean up and close pipes, then exit
 */
void
cleanup(int sig)
{
    struct pattern *p;
    time_t now = time(0);

    for (p=patterns; p; p = p->next)
	if (p->count)
	    unthrottle(p, now);

    if (pipefd) pclose(pipefd);
    exit(0);
} /* cleanup */


/*
 * walk the list of patterns seeing if anything has timed out
 */
void
expire(int sig)
{
    struct pattern *p;
    time_t now = time(0);

    alarm(0);
    if (gthrottle && (gthrottle->throttle <= now)) {
	unthrottle(gthrottle, now);
	gthrottle = 0;
    }
    for (p=patterns; p; p = p->next) {
	if (p->throttle && (p->throttle <= now))
	    unthrottle(p, now);
    }
    setalarm();
} /* expire */


float
main(int argc, char **argv)
{
    time_t t;
    int isadaemon = 0;
    int i;

#ifdef HAVE_BASENAME
    pgm = basename(argv[0]);
#else
    if (pgm = strrchr(argv[0], '/'))
	argv[0] = ++pgm;
    else
	pgm = argv[0];
#endif

    getoptionsandscript(argc, argv);

    isadaemon = (tflag || pflag) && !(Dflag || fflag || dflag);

    /* don't buffer output when it's going to a file
     */
    if (!isatty(fileno(stdout)))
	setbuf(stdout, 0);

    /* write out the pidfile if needed
     */
    if (pidfile) {
	FILE *pf = fopen(pidfile, "w");

	if (pf) {
	    fprintf(pf, "%d\n", getpid());
	    fclose(pf);
	}
	else {
	    perror(pidfile);
	    exit(1);
	}
    }

    /* print out a nice little banner
     */
    if (tflag || pflag) {
	time(&t);
	printf("*** cwatch "VERSION" (pid:%d) started at %s",
		getpid(), ctime(&t));
    }

    /* if it's NOT running as a daemon, fork off a child and restart
     * it if we get a HUP or ALRM.
     */
    while (isadaemon) {
	int status;
	pid_t pid;
	pid_t rc;

	setsid();

	/* Ignore the signals we aren't interested in */
	for (i=0; i < NSIG; i++)
	    signal(i, SIG_IGN);

	/* if the parent gets a kill, kill all children and die */
	signal(SIGINT,  alldie);
	signal(SIGTERM, alldie);
	signal(SIGQUIT, alldie);

	/* if the parent gets a hup, restart everything */
	signal(SIGHUP, restart);
	signal(SIGALRM, restart);

	if ( (pid = fork()) < 0) {
	    perror(pgm);
	    if (errno == ECHILD)
		sleep(5);
	    else
		exit(1);
	}
	else if (pid == 0) 
	    break;
	else {
	    restart_alarm();
	    do {
		rc = wait(&status);
	    } while (rc != pid && kill(pid, 0) == 0);
	}
	if (WEXITSTATUS(status) == 99)
	    exit(1);
	time(&t);
	printf("*** cwatch "VERSION" (pid:%d) restarted at %s",
		getpid(), ctime(&t));
    }

    /* default the signals we aren't interested in */
    for (i=0; i < NSIG; i++)
	signal(i, SIG_DFL);

    /* when cwatch is running, any kill makes it die */
    signal(SIGINT,  cleanup);
    signal(SIGQUIT, cleanup);
    signal(SIGTERM, cleanup);
    signal(SIGHUP,  isadaemon ? SIG_IGN : cleanup);

    if (cfgfile == 0) {
	char *home = getenv("HOME");
	cfgfile = malloc( 2 + strlen("/.cwatchrc")
				+ strlen( home ? home : "" ) );
	sprintf(cfgfile, "%s/.cwatchrc", home ? home : "");
	/* if no config file is provided, attempt to open a default
	 * file.  If that fails use a compiled-in default
	 */
	if (!openinput(cfgfile))
	    defaultinput("watchfor /.*/ echo");
    }
    else if ( !openinput(cfgfile) ) {
	fprintf(stderr, "%s: no such file %s\n", pgm, cfgfile);
	exit(99);
    }

    if (input == 0)
	if (access(input = "/var/log/messages", R_OK) != 0)
	    if (access(input = "/var/log/syslog", R_OK) != 0) {
		fprintf(stderr, "%s: can't find a file to examine\n", pgm);
		exit(99);
	    }

    if ( (patterns = compile()) == 0 || errors) 
	exit(99);

    if (Dflag)
	dump(patterns);
    else if (tflag)
	dotail(patterns);
    else if (pflag)
	dopipe(patterns);
    else
	dofile(patterns);
    cleanup(0);
} /* main */
