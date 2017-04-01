#ifndef YYERRCODE
#define YYERRCODE 256
#endif

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
typedef union {
    long num;
    char *text;
} YYSTYPE;
extern YYSTYPE yylval;
