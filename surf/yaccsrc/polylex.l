/* Hey Emacs, this is -*-c-*- */
/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */




%{
/* ------------------------------------------------------------------------- */
/* polylex.l: polynomial lexer                                               */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     14.8.94                                                         */
/* ------------------------------------------------------------------------- */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <mymemory.h>
#include <monomarith.h>
#include <polyarith.h>
#include <polylexyacc.h>
#include <polyyacc.h>

int the_yyinput( char*,int );

/* ---------- */
/* debug flag */
/* ---------- */

#ifdef  DEBUG
int    control = 1;
#else
int    control = 0;
#endif  /* DEBUG */

/* -------------- */
/* redirect input */
/* -------------- */

#undef  YY_INPUT
#define YY_INPUT( b,r,ms ) ( r = the_yyinput(b,ms) )

%}

%%

\n {
        /* --------------- */
        /* count the lines */
        /* --------------- */

        line_number++;
        char_number += yyleng;
    }

[ \t]+ {
        /* --------------------------------- */
        /* don't worry about spaces and tabs */
        /* --------------------------------- */

        char_number += yyleng;
    }

\/\/[^\n]* {
        /* ------------------- */
        /* enable c++ comments */
        /* ------------------- */

        char_number += yyleng;
    }

\"[^"\n]*["\n] {
        /* ------------------------ */
        /* recognize quoted strings */
        /* ------------------------ */

        static  int     length;

        length = strlen( yytext ) - 2;
        yylval.str = new_char( length + 1 );
        strncpy( yylval.str,&yytext[1],length );
        yylval.str[length] = '\0';

        if( control )
        {
            printf( "lexed string: %s\n",yytext );
            printf( "made  string: %s\n",yylval.str );
        }
        char_number += yyleng;

        return  STRING;
    }

(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))((e[+-]?[0-9]+)?) {
        /* ----------------- */
        /* recognize doubles */
        /* ----------------- */

        yylval.dval = atof( yytext );

        if( control )
        {
            printf( "lexed double: %s\n", yytext );
            printf( "made  double: %f\n",yylval.dval );
        }
        char_number += yyleng;

        return  DOUBLE;
    }

[0-9]+ {
        /* ------------------ */
        /* recognize integers */
        /* ------------------ */

        yylval.ival = atoi( yytext );

        if( control )
        {
            printf( "lexed integer: %s\n",yytext );
            printf( "made  integer: %d\n",yylval.ival );
        }
        char_number += yyleng;

        return  INTEGER;
    }

[xyz] {
        /* ------------------- */
        /* recognize monomials */
        /* ------------------- */

        yylval.mon = atom( yytext );

        if( control )
        {
            printf( "lexed monom: %s\n",yytext );
            printf( "made  monom: " );
            monxyz_print( &yylval.mon );
            printf( "\n" );
        }
        char_number += yyleng;

        return  MONOM;
    }

polyxyz {
        /* ---------------------------- */
        /* recognize keyword  "polyxyz" */
        /* ---------------------------- */

        if( control )
        {
            printf( "lexed keyword: polyxyz\n" );
        }
        char_number += yyleng;

        return  TYPE_POLYXYZ;
    }

poly    {
        /* ---------------------------- */
        /* recognize keyword  "poly" */
        /* ---------------------------- */

        if( control )
        {
            printf( "lexed keyword: poly\n" );
        }
        char_number += yyleng;

        return  TYPE_POLYXYZ;
    }

monxyz {
        /* --------------------------- */
        /* recognize keyword  "monxyz" */
        /* --------------------------- */

        if( control )
        {
            printf( "lexed keyword: monxyz\n" );
        }
        char_number += yyleng;

        return  TYPE_MONOM;
    }

double {
        /* --------------------------- */
        /* recognize keyword  "double" */
        /* --------------------------- */

        if( control )
        {
            printf( "lexed keyword: double\n" );
        }
        char_number += yyleng;

        return  TYPE_DOUBLE;
    }

int {
        /* ------------------------ */
        /* recognize keyword  "int" */
        /* ------------------------ */

        if( control )
        {
            printf( "lexed keyword: int\n" );
        }
        char_number += yyleng;

        return  TYPE_INTEGER;
    }

string {
        /* ---------------------------- */
        /* recognize keyword  "string" */
        /* --------------------------- */

        if( control )
        {
            printf( "lexed keyword: string\n" );
        }
        char_number += yyleng;

        return  TYPE_STRING;
    }

goto    { char_number += yyleng; return  GOTO; }
if      { char_number += yyleng; return  IF; }

"=="    { char_number += yyleng; return EQUAL; }
"!="    { char_number += yyleng; return NOTEQUAL; }
"<"     { char_number += yyleng; return SMALLER; }
"<="    { char_number += yyleng; return SMALLERTHAN; }
">"     { char_number += yyleng; return BIGGER; }
">="    { char_number += yyleng; return BIGGERTHAN; }
"&&"    { char_number += yyleng; return AND; }
"||"    { char_number += yyleng; return OR; }

"#include"  { char_number += yyleng; return INCLUDE; }

itostrn { char_number += yyleng; return  ITOSTRN; }
itostr  { char_number += yyleng; return  ITOSTR; }
itoascn { char_number += yyleng; return  ITOASCN; }
itoasc  { char_number += yyleng; return  ITOASC; }
sqrt    { char_number += yyleng; return  SQRT; }
sin     { char_number += yyleng; return  SINUS; }
cos     { char_number += yyleng; return  COSINUS; }
tan     { char_number += yyleng; return  TANGENS; }
cot     { char_number += yyleng; return  COTANGENS; }
pow     { char_number += yyleng; return  POWER; }  
arcsin  { char_number += yyleng; return  ARCUS_SINUS; }
arccos  { char_number += yyleng; return  ARCUS_COSINUS; }
arctan  { char_number += yyleng; return  ARCUS_TANGENS; }
arccot  { char_number += yyleng; return  ARCUS_COTANGENS; }

deg     { char_number += yyleng; return  DEGREE; }
len     { char_number += yyleng; return  LENGTH; }

diff            { char_number += yyleng; return  DIFF; }
hessian_curve   { char_number += yyleng; return  HESSIAN_CURVE; }
hessian_surface { char_number += yyleng; return  HESSIAN_SURFACE; }
hesse           { char_number += yyleng; return  HESSIAN_SURFACE; }

"rotate(" { char_number += yyleng; return  ROTATE; }
xAxis   { char_number += yyleng; return  X_AXIS; }
yAxis   { char_number += yyleng; return  Y_AXIS; }
zAxis   { char_number += yyleng; return  Z_AXIS; }
system	{ char_number += yyleng; return SYSTEM;  }

[a-zA-Z_][a-zA-Z_0-9]* {
        /* --------------- */
        /* recognize names */
        /* --------------- */

        yylval.sym = symtab_lookup_name( yytext );

        if( control )
        {
            printf( "lexed name: %s\n",yytext );
            printf( "made  name: %s\n",yylval.sym->name );
        }
        char_number += yyleng;
 
        switch( yylval.sym->type )
        {
            case SYM_LABEL:
                return  NAME_LABEL;
            case SYM_COMMAND:
                return  NAME_COMMAND;
            case SYM_POLYXYZ:
                return  NAME_POLYXYZ;
            case SYM_MONOM:
                return  NAME_MONOM;
            case SYM_STRING:
                return  NAME_STRING;
            case SYM_DOUBLE:
                return  NAME_DOUBLE;
            case SYM_INTEGER:
                return  NAME_INTEGER;
            case SYM_UNSPEC:
                return  NAME;
        }
    }

.   {
        /* ---------------- */
        /* all the rest ... */
        /* ---------------- */

        if( control )
        {
            printf( "lexed char: %c\n",yytext[0] );
        }
        char_number += yyleng; 
        return  yytext[0];
    }

%%


/* ------------------------------------------------------------------------- */
/* end of file: polylex.l                                                    */
/* ------------------------------------------------------------------------- */

