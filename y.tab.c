#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28.2.1 2001/07/19 05:46:39 peter Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
#if defined(__cplusplus) || __STDC__
static int yygrowstack(void);
#else
static int yygrowstack();
#endif
#define YYPREFIX "yy"
#line 2 "parser.y"
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

#line 43 "parser.y"
typedef union {
    long num;
    char *text;
} YYSTYPE;
#line 66 "y.tab.c"
#define YYERRCODE 256
#define WATCHFOR 257
#define IGNORE 258
#define ECHO 259
#define NORMAL 260
#define BOLD 261
#define UNDERSCORE 262
#define BLINK 263
#define INVERSE 264
#define BLACK 265
#define RED 266
#define GREEN 267
#define YELLOW 268
#define BLUE 269
#define MAGENTA 270
#define CYAN 271
#define WHITE 272
#define BLACK_H 273
#define RED_H 274
#define GREEN_H 275
#define YELLOW_H 276
#define BLUE_H 277
#define MAGENTA_H 278
#define CYAN_H 279
#define WHITE_H 280
#define EXEC 281
#define PIPE 282
#define KEEP_OPEN 283
#define WRITE 284
#define BELL 285
#define THROTTLE 286
#define USE 287
#define MESSAGE 288
#define REGEX 289
#define QUIT 290
#define CONTINUE 291
#define WHEN 292
#define MAIL 293
#define ADDRESSES 294
#define SUBJECT 295
#define NL 296
#define NUMBER 297
#define WORD 298
#define COMMA 299
#define COLON 300
#define EQUAL 301
#define DASH 302
const short yylhs[] = {                                        -1,
    0,    0,    2,    1,    1,    4,    4,    4,    4,    4,
    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
    4,    4,    4,    4,    4,    4,    5,    5,    6,    6,
    6,    7,    7,    7,    8,    9,    9,   10,   11,   11,
   12,   13,   13,   14,   15,   15,   16,   17,   18,   18,
   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
   23,   24,   25,   25,   25,   25,   25,   25,   25,   25,
   25,    3,    3,
};
const short yylen[] = {                                         2,
    0,    2,    1,    3,    2,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    0,    2,    1,    3,
    3,    1,    3,    3,    5,    0,    1,    2,    0,    1,
    2,    0,    2,    3,    1,    3,    2,    5,    3,    3,
    1,    1,    3,    2,    3,    3,    3,    1,    1,    3,
    2,    1,    3,    2,    2,    1,    1,    2,    1,    2,
    2,    0,    2,
};
const short yydefred[] = {                                      1,
    0,    0,    0,    2,    3,   72,    5,    0,   62,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   66,
    0,   69,   67,   27,   73,   38,    0,   45,    0,   40,
   41,    0,    0,    0,   37,   71,   70,    0,    0,   58,
   59,    0,   64,   65,   68,    0,    0,   44,    0,    0,
    0,   51,   52,    0,    0,    0,    0,    0,    6,    7,
    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
   18,   19,   20,   21,   22,   23,   24,   25,   26,   28,
   63,   43,   46,    0,    0,    0,    0,    0,    0,   57,
   60,    0,   49,   50,   53,    0,    0,    0,   48,   30,
    0,    0,    0,    0,    0,   33,    0,
};
const short yydgoto[] = {                                       1,
    4,    6,    8,   80,   46,   88,  103,   35,   36,   18,
   31,   19,   48,   20,   29,   21,   33,   53,   54,   22,
   41,   42,   23,   24,   25,
};
const short yysindex[] = {                                      0,
 -239, -286, -276,    0,    0,    0,    0, -257,    0, -289,
 -272, -266, -260, -258, -252, -252, -280, -252, -252,    0,
 -252,    0,    0,    0,    0,    0, -256,    0, -255,    0,
    0, -254, -281, -259,    0,    0,    0, -253, -251,    0,
    0, -250,    0,    0,    0, -101, -236,    0, -247, -245,
 -248,    0,    0, -244, -241, -266, -240, -280,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -243, -268, -281, -242, -277, -255,    0,
    0, -238,    0,    0,    0, -235, -241, -234,    0,    0,
 -233, -237, -232, -227, -234,    0, -232,
};
const short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    4,    0,    0,
    0,    0,   38,    0,   68,   68,    0,   68,   68,    0,
   68,    0,    0,    0,    0,    0,   84,    0,   54,    0,
    0,    0,   98,    0,    0,    0,    0,    0,    0,    0,
    0,  114,    0,    0,    0,   68,    0,    0,    0,    0,
    0,    0,    0,  128,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -269,    0,    1,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -229,   17,  144,    0,    0,    0,  158,
};
const short yygindex[] = {                                      0,
    0,    0,    0,    0,    0,  -53,  -44,  -17,  -11,    0,
    0,    0,    0,    0,    8,    0,    0,  -14,    0,    0,
   15,    0,    0,    0,    0,
};
#define YYTABLESIZE 451
const short yytable[] = {                                      40,
   56,    9,    5,    4,   37,   51,   43,   44,   26,   45,
   34,   34,    7,   38,   39,   52,   32,    2,    3,   93,
   94,   97,   98,   10,   11,   27,   12,   13,   14,   29,
   29,   28,   15,   16,   81,   17,   30,   39,   32,   34,
   40,   55,   47,  101,   49,   50,   82,   56,   58,   57,
   83,   84,   85,   47,   86,   87,   92,   90,   99,   96,
  107,  100,  102,   89,  104,   97,  105,   36,   52,  106,
   31,   95,   91,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   42,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   54,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   61,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   55,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   35,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   34,   59,   60,
   61,   62,   63,   64,   65,   66,   67,   68,   69,   70,
   71,   72,   73,   74,   75,   76,   77,   78,   79,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   34,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   56,   56,   56,
    4,    4,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   32,   32,   32,    0,    0,    0,    0,
    0,   56,   56,    0,   56,   56,   56,    0,    0,    0,
   56,   56,    0,   56,   39,   39,   39,   32,   32,   56,
   32,   32,   32,    0,    0,    0,   32,   32,    0,   32,
   47,   47,   47,    0,    0,   32,    0,    0,   39,   39,
    0,   39,   39,   39,   36,   36,   36,   39,   39,   39,
   39,    0,    0,    0,   47,   47,    0,   47,   47,   47,
   42,   42,   42,   47,   47,   47,   47,    0,   36,   36,
    0,   36,   36,   36,   54,   54,   54,   36,   36,    0,
   36,    0,    0,    0,   42,   42,    0,   42,   42,   42,
   61,   61,   61,   42,   42,    0,   42,    0,   54,   54,
    0,   54,   54,   54,   55,   55,   55,   54,   54,    0,
   54,    0,    0,    0,   61,   61,    0,   61,   61,   61,
   35,   35,   35,   61,   61,    0,   61,    0,   55,   55,
    0,   55,   55,   55,   34,   34,   34,   55,   55,    0,
   55,    0,    0,    0,   35,   35,    0,   35,   35,   35,
    0,    0,    0,   35,   35,    0,   35,    0,   34,   34,
    0,   34,   34,   34,    0,    0,    0,   34,   34,    0,
   34,
};
const short yycheck[] = {                                      17,
    0,  259,  289,    0,   16,  287,   18,   19,  298,   21,
  292,  292,  289,  294,  295,   33,    0,  257,  258,  288,
  289,  299,  300,  281,  282,  298,  284,  285,  286,  299,
  300,  298,  290,  291,   46,  293,  297,    0,  297,  292,
   58,  301,  299,   97,  300,  300,  283,  301,  299,  301,
  298,  297,  301,    0,  299,  297,  300,  298,  297,  302,
  105,  297,  297,   56,  302,  299,  299,    0,   86,  297,
  300,   86,   58,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,  260,  261,
  262,  263,  264,  265,  266,  267,  268,  269,  270,  271,
  272,  273,  274,  275,  276,  277,  278,  279,  280,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  292,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,
  257,  258,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  257,  258,  259,   -1,   -1,   -1,   -1,
   -1,  281,  282,   -1,  284,  285,  286,   -1,   -1,   -1,
  290,  291,   -1,  293,  257,  258,  259,  281,  282,  299,
  284,  285,  286,   -1,   -1,   -1,  290,  291,   -1,  293,
  257,  258,  259,   -1,   -1,  299,   -1,   -1,  281,  282,
   -1,  284,  285,  286,  257,  258,  259,  290,  291,  292,
  293,   -1,   -1,   -1,  281,  282,   -1,  284,  285,  286,
  257,  258,  259,  290,  291,  292,  293,   -1,  281,  282,
   -1,  284,  285,  286,  257,  258,  259,  290,  291,   -1,
  293,   -1,   -1,   -1,  281,  282,   -1,  284,  285,  286,
  257,  258,  259,  290,  291,   -1,  293,   -1,  281,  282,
   -1,  284,  285,  286,  257,  258,  259,  290,  291,   -1,
  293,   -1,   -1,   -1,  281,  282,   -1,  284,  285,  286,
  257,  258,  259,  290,  291,   -1,  293,   -1,  281,  282,
   -1,  284,  285,  286,  257,  258,  259,  290,  291,   -1,
  293,   -1,   -1,   -1,  281,  282,   -1,  284,  285,  286,
   -1,   -1,   -1,  290,  291,   -1,  293,   -1,  281,  282,
   -1,  284,  285,  286,   -1,   -1,   -1,  290,  291,   -1,
  293,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 302
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"WATCHFOR","IGNORE","ECHO",
"NORMAL","BOLD","UNDERSCORE","BLINK","INVERSE","BLACK","RED","GREEN","YELLOW",
"BLUE","MAGENTA","CYAN","WHITE","BLACK_H","RED_H","GREEN_H","YELLOW_H","BLUE_H",
"MAGENTA_H","CYAN_H","WHITE_H","EXEC","PIPE","KEEP_OPEN","WRITE","BELL",
"THROTTLE","USE","MESSAGE","REGEX","QUIT","CONTINUE","WHEN","MAIL","ADDRESSES",
"SUBJECT","NL","NUMBER","WORD","COMMA","COLON","EQUAL","DASH",
};
const char * const yyrule[] = {
"$accept : input",
"input :",
"input : input statement",
"regex : REGEX",
"statement : WATCHFOR regex commands",
"statement : IGNORE REGEX",
"color : NORMAL",
"color : BOLD",
"color : UNDERSCORE",
"color : BLINK",
"color : INVERSE",
"color : BLACK",
"color : RED",
"color : GREEN",
"color : YELLOW",
"color : BLUE",
"color : MAGENTA",
"color : CYAN",
"color : WHITE",
"color : BLACK_H",
"color : RED_H",
"color : GREEN_H",
"color : YELLOW_H",
"color : BLUE_H",
"color : MAGENTA_H",
"color : CYAN_H",
"color : WHITE_H",
"colors :",
"colors : colors color",
"days : NUMBER",
"days : NUMBER DASH NUMBER",
"days : days COMMA days",
"hours : NUMBER",
"hours : NUMBER DASH NUMBER",
"hours : hours COMMA hours",
"when : WHEN EQUAL days COLON hours",
"opts :",
"opts : when",
"exec : EXEC WORD",
"number :",
"number : NUMBER",
"bell : BELL number",
"pipeflags :",
"pipeflags : COMMA KEEP_OPEN",
"pipe : PIPE WORD pipeflags",
"address : WORD",
"address : address COLON WORD",
"write : WRITE address",
"timespec : NUMBER COLON NUMBER COLON NUMBER",
"throttleflag : USE EQUAL MESSAGE",
"throttleflag : USE EQUAL REGEX",
"throttleflag : when",
"throttleargs : throttleflag",
"throttleargs : throttleargs COMMA throttleflag",
"throttle : THROTTLE timespec",
"throttle : THROTTLE timespec throttleargs",
"mailflags : ADDRESSES EQUAL address",
"mailflags : SUBJECT EQUAL WORD",
"mailflags : when",
"mailargs : mailflags",
"mailargs : mailargs COMMA mailflags",
"mail : MAIL mailargs",
"echo : ECHO",
"action : echo colors opts",
"action : exec opts",
"action : bell opts",
"action : pipe",
"action : mail",
"action : write opts",
"action : throttle",
"action : CONTINUE opts",
"action : QUIT opts",
"commands :",
"commands : commands action",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 3:
#line 102 "parser.y"
{ re(yyvsp[0].text); }
break;
case 4:
#line 105 "parser.y"
{ statement(WATCHFOR); }
break;
case 5:
#line 106 "parser.y"
{   re(yyvsp[0].text);
						    statement(IGNORE);
						}
break;
case 7:
#line 112 "parser.y"
{ attrib(BOLD); }
break;
case 8:
#line 113 "parser.y"
{ attrib(UNDERSCORE); }
break;
case 9:
#line 114 "parser.y"
{ attrib(BLINK); }
break;
case 10:
#line 115 "parser.y"
{ attrib(INVERSE); }
break;
case 11:
#line 116 "parser.y"
{ fg(BLACK); }
break;
case 12:
#line 117 "parser.y"
{ fg(RED); }
break;
case 13:
#line 118 "parser.y"
{ fg(GREEN); }
break;
case 14:
#line 119 "parser.y"
{ fg(YELLOW); }
break;
case 15:
#line 120 "parser.y"
{ fg(BLUE); }
break;
case 16:
#line 121 "parser.y"
{ fg(MAGENTA); }
break;
case 17:
#line 122 "parser.y"
{ fg(CYAN); }
break;
case 18:
#line 123 "parser.y"
{ fg(WHITE); }
break;
case 19:
#line 124 "parser.y"
{ bg(BLACK); }
break;
case 20:
#line 125 "parser.y"
{ bg(RED); }
break;
case 21:
#line 126 "parser.y"
{ bg(GREEN); }
break;
case 22:
#line 127 "parser.y"
{ bg(YELLOW); }
break;
case 23:
#line 128 "parser.y"
{ bg(BLUE); }
break;
case 24:
#line 129 "parser.y"
{ bg(MAGENTA); }
break;
case 25:
#line 130 "parser.y"
{ bg(CYAN); }
break;
case 26:
#line 131 "parser.y"
{ bg(WHITE); }
break;
case 29:
#line 138 "parser.y"
{ if (!day(yyvsp[0].num, 0)) YYERROR; }
break;
case 30:
#line 139 "parser.y"
{ if (!day(yyvsp[-2].num, yyvsp[0].num)) YYERROR; }
break;
case 32:
#line 143 "parser.y"
{ if (!hour(yyvsp[0].num, 0)) YYERROR; }
break;
case 33:
#line 144 "parser.y"
{ if (!hour(yyvsp[-2].num, yyvsp[0].num)) YYERROR; }
break;
case 38:
#line 155 "parser.y"
{ cmdline(yyvsp[0].text); }
break;
case 40:
#line 159 "parser.y"
{ nrbell(yyvsp[0].num); }
break;
case 43:
#line 166 "parser.y"
{ keep_open(); }
break;
case 44:
#line 169 "parser.y"
{ cmdline(yyvsp[-1].text); }
break;
case 45:
#line 172 "parser.y"
{ address(yyvsp[0].text); }
break;
case 46:
#line 173 "parser.y"
{ address(yyvsp[0].text); }
break;
case 48:
#line 180 "parser.y"
{
    delay(yyvsp[-4].num, yyvsp[-2].num, yyvsp[0].num);
}
break;
case 49:
#line 185 "parser.y"
{ use(MESSAGE); }
break;
case 50:
#line 186 "parser.y"
{ use(REGEX); }
break;
case 57:
#line 199 "parser.y"
{ subject(yyvsp[0].text); }
break;
case 63:
#line 213 "parser.y"
{ action(ECHO); }
break;
case 64:
#line 214 "parser.y"
{ action(EXEC); }
break;
case 65:
#line 215 "parser.y"
{ action(BELL); }
break;
case 66:
#line 216 "parser.y"
{ action(PIPE); }
break;
case 67:
#line 217 "parser.y"
{ action(MAIL); }
break;
case 68:
#line 218 "parser.y"
{ action(WRITE); }
break;
case 69:
#line 219 "parser.y"
{ action(THROTTLE); }
break;
case 70:
#line 220 "parser.y"
{ action(CONTINUE); }
break;
case 71:
#line 221 "parser.y"
{ action(QUIT); }
break;
#line 786 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
