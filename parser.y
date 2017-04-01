%{
/*
 *   Copyright (c) 2002-2017 David Parsons. All rights reserved.
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
#include <unistd.h>
#include <stdlib.h>
#if HAVE_MALLOC_H
#   include <malloc.h>
#endif

%}

%union {
    long num;
    char *text;
}

%token	WATCHFOR
%token	IGNORE

%token	ECHO
%token	NORMAL
%token	BOLD
%token	UNDERSCORE
%token	BLINK
%token	INVERSE
%token	BLACK
%token	RED
%token	GREEN
%token	YELLOW
%token	BLUE
%token	MAGENTA
%token	CYAN
%token	WHITE
%token	BLACK_H
%token	RED_H
%token	GREEN_H
%token	YELLOW_H
%token	BLUE_H
%token	MAGENTA_H
%token	CYAN_H
%token	WHITE_H
%token	EXEC
%token	PIPE
%token	KEEP_OPEN
%token	WRITE
%token	BELL
%token	THROTTLE
%token	USE
%token	MESSAGE
%token	<text> REGEX
%token	QUIT
%token	CONTINUE
%token	WHEN
%token	MAIL
%token	ADDRESSES
%token	SUBJECT
%token	NL
%token	<num> NUMBER
%token	<text> WORD
%token	COMMA
%token	COLON
%token	EQUAL
%token	DASH

%%
input:		  /* empty */
		| input statement
;


regex:		  REGEX		{ re($1); }
;

statement:	  WATCHFOR regex commands	{ statement(WATCHFOR); }
		| IGNORE REGEX			{   re($2);
						    statement(IGNORE);
						}
;

color:		  NORMAL
		| BOLD		{ attrib(BOLD); }
		| UNDERSCORE	{ attrib(UNDERSCORE); }
		| BLINK		{ attrib(BLINK); }
		| INVERSE	{ attrib(INVERSE); }
		| BLACK		{ fg(BLACK); }
		| RED		{ fg(RED); }
		| GREEN		{ fg(GREEN); }
		| YELLOW	{ fg(YELLOW); }
		| BLUE		{ fg(BLUE); }
		| MAGENTA	{ fg(MAGENTA); }
		| CYAN		{ fg(CYAN); }
		| WHITE		{ fg(WHITE); }
		| BLACK_H	{ bg(BLACK); }
		| RED_H		{ bg(RED); }
		| GREEN_H	{ bg(GREEN); }
		| YELLOW_H	{ bg(YELLOW); }
		| BLUE_H	{ bg(BLUE); }
		| MAGENTA_H	{ bg(MAGENTA); }
		| CYAN_H	{ bg(CYAN); }
		| WHITE_H	{ bg(WHITE); }
;

colors:		  /* empty */
		| colors color
;

days		: NUMBER		{ if (!day($1, 0)) YYERROR; }
		| NUMBER DASH NUMBER	{ if (!day($1, $3)) YYERROR; }
		| days COMMA days
;

hours		: NUMBER		{ if (!hour($1, 0)) YYERROR; }
		| NUMBER DASH NUMBER	{ if (!hour($1, $3)) YYERROR; }
		| hours COMMA hours
;

when		: WHEN EQUAL days COLON hours
;

opts		: /* empty */
		|  when
;

exec:		  EXEC WORD	{ cmdline($2); }
;

number:		  /* empty */
		| NUMBER		{ nrbell($1); }
;

bell:		  BELL number
;

pipeflags:	  /* empty */
		| COMMA KEEP_OPEN 	{ keep_open(); }
;

pipe:		   PIPE WORD pipeflags { cmdline($2); }
;

address:	  WORD			{ address($1); }
		| address COLON WORD	{ address($3); }
;

write:		  WRITE address
;

timespec:	  NUMBER COLON NUMBER COLON NUMBER
{
    delay($1, $3, $5);
}
;

throttleflag	: USE EQUAL MESSAGE	{ use(MESSAGE); }
		| USE EQUAL REGEX	{ use(REGEX); }
		| when
;

throttleargs:	  throttleflag
		| throttleargs COMMA throttleflag
;

throttle:	  THROTTLE timespec
		| THROTTLE timespec throttleargs
;

mailflags:	  ADDRESSES EQUAL address
		| SUBJECT EQUAL WORD	{ subject($3); }
		| when
;

mailargs:	  mailflags
		| mailargs COMMA mailflags
;

mail:		  MAIL mailargs
;

echo:		  ECHO
;

action:		  echo colors opts	{ action(ECHO); }
		| exec opts		{ action(EXEC); }
		| bell opts		{ action(BELL); }
		| pipe			{ action(PIPE); }
		| mail			{ action(MAIL); }
		| write opts		{ action(WRITE); }
		| throttle		{ action(THROTTLE); }
		| CONTINUE opts		{ action(CONTINUE); }
		| QUIT opts		{ action(QUIT); }
;

commands:	  /* empty */
		| commands action
;

%%
