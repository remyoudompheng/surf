/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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


/* polylexyacc.h: include scanner                                            */
/* Author:   Stephan Endrass                                                 */

#ifndef POLYLEXYACC_H
#define POLYLEXYACC_H

#include <stdio.h>
#include <monomarith.h>
#include <polyarith.h>

typedef void (*f_v_v_p)(void);

/* types of variables we have  */

typedef enum
{
    SYM_UNSPEC,                         /* is a symbol, but don't know which */
    SYM_STRING,                         /* symbol denotes a string           */
    SYM_INTEGER,                        /* symbol denotes an integer         */
    SYM_DOUBLE,                         /* symbol denotes a double           */
    SYM_MONOM,                          /* symbol denotes a monomial         */
    SYM_POLYXYZ,                        /* symbol denotes a polynomial       */
    SYM_COMMAND,                        /* symbol denotes a command          */
    SYM_LABEL                           /* symbol denotes a label            */

} symtyp;


/*  This is an entry in the symbol table */

struct  sym_i_tab
{
    char    *name;                      /* name of the symbol                */
    symtyp  type;                       /* type it denotes                   */

    int     surface;                    /* surface variable flag             */
    int     constant;                   /* is constant variable              */
    void    *ptr;                       /* pointer to surface variable       */


    char    *str;                       /* string value of symbol            */
    int     ival;                       /* integer value of symbol           */
    double  dval;                       /* double value of symbol            */
    monxyz  mon;                        /* monomial value of symbol          */
    polyxyz p;                          /* polynomoial value of symbol       */
    int     label;                      /* label value of symbol             */
    int     lineno;                     /* line number of label              */

    struct sym_i_tab  *next;            /* If  NULL, last element            */
    struct sym_i_tab  *prev;            /* If  NULL, first element           */
};

/*  Strange, but g++ is very buggy: */

typedef struct sym_i_tab symtab;

extern int control;

/*  Global data: */

extern int yyleng;
extern long error_begin_char;
extern long char_number;
extern long line_number;
extern long goto_label;
extern long goto_line;
extern int goto_flag;
extern char yyerrorstring[];


/* Prototypes: */

#ifdef  __cplusplus
extern "C" {
#endif  /* __cplusplus */

void yyrestart(FILE*);
int yyparse(void);
void set_the_yyinput(char*, long, long);

symtab* symtab_add_name(const char*);
symtab* symtab_add_surface_name(const char*, symtyp, int, void*);
symtab* symtab_find_name(const char*);
symtab* symtab_lookup_name(const char*);

void symtab_delete_name(const char*);
void symtab_delete_user_names(void);
void symtab_delete_surface_names(void);
void symtab_delete_total(void);
void symtab_clean(symtab*);

void scan_labels(char*);
void yyerror(const char*);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* POLYLEXYACC_H */
