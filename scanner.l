%{
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
#include "y.tab.h"

int line_number = 1;

#define yywrap() 1
%}
%%

watchfor		{ return WATCHFOR; }
ignore			{ return IGNORE; }
echo			{ return ECHO; }
normal			{ return NORMAL; }
bold			{ return BOLD; }
underscore		{ return UNDERSCORE; }
blink			{ return BLINK; }
inverse			{ return INVERSE; }
black			{ return BLACK; }
red			{ return RED; }
green			{ return GREEN; }
yellow			{ return YELLOW; }
blue			{ return BLUE; }
magenta			{ return MAGENTA; }
cyan			{ return CYAN; }
white			{ return WHITE; }
black_h			{ return BLACK_H; }
red_h			{ return RED_H; }
green_h			{ return GREEN_H; }
yellow_h		{ return YELLOW_H; }
blue_h			{ return BLUE_H; }
magenta_h		{ return MAGENTA_H; }
cyan_h			{ return CYAN_H; }
white_h			{ return WHITE_H; }
exec			{ return EXEC; }
pipe			{ return PIPE; }
keep_open		{ return KEEP_OPEN; }
write			{ return WRITE; }
bell			{ return BELL; }
throttle		{ return THROTTLE; }
use			{ return USE; }
quit			{ return QUIT; }
continue		{ return CONTINUE; }
when			{ return WHEN; }
mail			{ return MAIL; }
addresses		{ return ADDRESSES; }
subject			{ return SUBJECT; }
message			{ return MESSAGE; }
regex			{ return REGEX; }
	
#[^\n]*			;
\n			{ line_number++; }
0x[0-9a-fA-F]+		{ yylval.v.num = strtol(yytext, 0, 16); return NUMBER; }
0[0-9]*			{ yylval.v.num = strtol(yytext, 0, 8); return NUMBER; }
[1-9][0-9]*		{ yylval.v.num = atoi(yytext); return NUMBER; }
"/"[^/]*		{   if (yytext[yyleng-1] == '\\')
				yymore();
			    else {
				input();
				yylval.v.text = realloc(yylval.v.text, yyleng);
				strcpy(yylval.v.text, 1+yytext);
				return REGEX;
			    }
			}
\"[^"]*			{   if (yytext[yyleng-1] == '\\')
				yymore();
			    else {
				input();
				yylval.v.text = realloc(yylval.v.text, yyleng);
				strcpy(yylval.v.text, 1+yytext);
				return WORD;
			    }
			}
[a-zA-Z0-9.@!]+		{
			    yylval.v.text = realloc(yylval.v.text, yyleng);
			    strcpy(yylval.v.text, yytext);
			    return WORD;
			}
":"			{ return COLON; }
"="			{ return EQUAL; }
","			{ return COMMA; }
"-"			{ return DASH; }
.			;
%%
int
openinput(char *file)
{
    return !!(yyin = fopen(file, "r"));
}

int
defaultinput(char *rules)
{
    yy_scan_string(rules);

    return 1;
}
