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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */



%{
/* polyyacc.y: polynomial parser                                             */
/* Author:   Stephan Endrass                                                 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define  POLYYACC_L

#include <mymemory.h>
#include <monomarith.h>
#include <polyarith.h>
#include <polylexyacc.h>

extern int surface_run_commands;


int    itotexascii( int i,char *s,int l )
{
	if( i < 0 || l < 2 || s==(char*)NULL ) {
		return  0;
	} else {
		/*  find length of 26-adic number i  */
		
		int     foo    = i;
		int     length = ( foo==0 ? 1 : 0 );
		
		while( foo > 0 ) {
			foo /= 26;
			length++;
		}

		/*  test if s is long enough  */

		if( length >= l ) {
			return  0;
		}

		/*  terminate with \0  */

		s[length]='\0';

		while( length > 0 ) {
			length--;
			s[length]= 'a' + (i%26);
			i = i/26;
		}

		return  1;
	}
}


%}

%union {
    char    *str;                       /* possible string value             */
    int     ival;                       /* possible integer value            */
    double  dval;                       /* posiible double value             */
    monxyz  mon;                        /* possible monomial value           */
    polyxyz p;                          /* possible polynomial value         */

    symtab  *sym;                       /* possible symbol referred to       */
}
    
%token <str>  STRING
%token <ival> INTEGER
%token <dval> DOUBLE
%token <mon>  MONOM
%token <sym>  NAME NAME_POLYXYZ NAME_MONOM NAME_DOUBLE NAME_INTEGER NAME_STRING NAME_COMMAND NAME_LABEL
%token TYPE_POLYXYZ TYPE_MONOM TYPE_DOUBLE TYPE_INTEGER TYPE_STRING
%token IF GOTO
%token EQUAL NOTEQUAL BIGGER BIGGERTHAN SMALLER SMALLERTHAN AND OR
%token ITOSTR ITOSTRN
%token ITOASC ITOASCN
%token SQRT SINUS COSINUS TANGENS COTANGENS POWER
%token ARCUS_SINUS ARCUS_COSINUS ARCUS_TANGENS ARCUS_COTANGENS
%token DEGREE LENGTH
%token HESSE HESSIAN_SURFACE HESSIAN_CURVE DIFF ROTATE
%token X_AXIS Y_AXIS Z_AXIS
%token SYSTEM INCLUDE

%type  <p>    p_expression
%type  <mon>  m_expression
%type  <dval> d_expression
%type  <ival> i_expression
%type  <str>  s_expression

%left  AND OR 
%left  EQUAL NOTEQUAL BIGGER BIGGERTHAN SMALLER SMALLERTHAN
%left  '+' '-'
%left  '*' '/' '%'
%left  '^'
%nonassoc UMINUS

%%

picture:            statement_list
                  ;

statement_list:     statement_list statement ';'
                  | statement_list label ':'
                  | statement_list include
                  | statement ';'
                  | label ':'
                  | include
                  ;

label:              NAME_LABEL
                       {
                            if( $1->type != SYM_UNSPEC &&
                                $1->type != SYM_LABEL )
			    {
                                yyerror( "name already in use" );
		     	    }
                            else if( $1->type == SYM_UNSPEC )
                            {

                                yyerror( "label not scanned" );

                                /* -------------------------- */
                                /* determine label adress ... */
                                /* -------------------------- */

                                /*  now done when scnning labels
                                int     i = 0;
                                extern char *main_formula_data;

                                $1->type = SYM_LABEL;
                                
                                while( main_formula_data[$1->label-1+i] 
                                       != ':' )
                                {
                                    if( main_formula_data[$1->label-1+i] 
                                        == '\n' )
                                    {
                                        $1->lineno++;
				    }
                                    i++;
			        }

                                $1->label = $1->label+i;
                                */
			    }
		        }
                  ;

include:            INCLUDE s_expression
                        {
                            printf( "%s\n",$2 );
			} 
                  ;

statement:          NAME_COMMAND
                        {
			    if(surface_run_commands) {
                                ((f_v_v_p)$1->ptr)();
                            }
                        }
                  | SYSTEM s_expression
			{
			    if(surface_run_commands) {
                                system ($2);
                            }
			}
                  | TYPE_POLYXYZ NAME
                        {
                            $2->type = SYM_POLYXYZ;
                            $2->p    = NULLPOLYXYZ;
                        }
                  | TYPE_POLYXYZ NAME '=' p_expression
                        {
                            $2->type = SYM_POLYXYZ;
                            $2->p    = $4;
                        }
                  | TYPE_POLYXYZ NAME '=' m_expression
                        {
                            $2->type = SYM_POLYXYZ;
                            $2->p    = monxyz2polyxyz( &$4 );
                        }
                  | TYPE_POLYXYZ NAME '=' d_expression
                        {
                            $2->type = SYM_POLYXYZ;
                            $2->p    = double2polyxyz( $4 );
                        }
                  | TYPE_POLYXYZ NAME '=' i_expression
                        {
                            $2->type = SYM_POLYXYZ;
                            $2->p    = int2polyxyz( $4 );
                        }
                  | NAME_POLYXYZ '=' p_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                else
                                {
                                    delete_coeff_polyxyz_control(
                                            (polyxyz*)$1->ptr );
                                    *((polyxyz*)$1->ptr) = $3;
                                }
                            }
                            else
                            {
                                delete_coeff_polyxyz_control( &$1->p );
                                $1->p = $3;
                            }
                        }
                  | NAME_POLYXYZ '=' m_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                else
                                {
                                    delete_coeff_polyxyz_control(
                                            (polyxyz*)$1->ptr );
                                    *((polyxyz*)$1->ptr) = monxyz2polyxyz( &$3 );
                                }
                            }
                            else
                            {
                                delete_coeff_polyxyz_control( &$1->p );
                                $1->p = monxyz2polyxyz( &$3 );
                            }
                        }
                  | NAME_POLYXYZ '=' d_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                else
                                {
                                    delete_coeff_polyxyz_control(
                                            (polyxyz*)$1->ptr );
                                    *((polyxyz*)$1->ptr) = double2polyxyz( $3 );
                                }
                            }
                            else
                            {
                                delete_coeff_polyxyz_control( &$1->p );
                                $1->p = double2polyxyz( $3 );
                            }
                        }
                  | NAME_POLYXYZ '=' i_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                else
                                {
                                    delete_coeff_polyxyz_control(
                                            (polyxyz*)$1->ptr );
                                    *((polyxyz*)$1->ptr) = int2polyxyz( $3 );
                                }
                            }
                            else
                            {
                                delete_coeff_polyxyz_control( &$1->p );
                                $1->p = int2polyxyz( $3 );
                            }
                        }

                  | TYPE_MONOM NAME
                        {
                            $2->type = SYM_MONOM;
                            $2->mon  = NULLMONXYZ;
                        }
                  | TYPE_MONOM NAME '=' m_expression
                        {
                            $2->type = SYM_MONOM;
                            $2->mon  = $4;
                        }
                  | TYPE_MONOM NAME '=' d_expression
                        {
                            $2->type = SYM_MONOM;
                            $2->mon  = double2monxyz( $4 );
                        }
                  | TYPE_MONOM NAME '=' i_expression
                        {
                            $2->type = SYM_MONOM;
                            $2->mon  = int2monxyz( $4 );
                        }
                  | NAME_MONOM '=' m_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                *((monxyz*)$1->ptr) = $3;
                            }
                            else
                            {
                                $1->mon  = $3;
                            }
                        }
                  | NAME_MONOM '=' d_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                *((monxyz*)$1->ptr) = double2monxyz( $3 );
                            }
                            else
                            {
                                $1->mon  = double2monxyz( $3 );
                            }
                        }
                  | NAME_MONOM '=' i_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                *((monxyz*)$1->ptr) = int2monxyz( $3 );
                            }
                            else
                            {
                            $1->mon  = int2monxyz( $3 );
                            }
                        }

                  | TYPE_DOUBLE NAME
                        {
                            $2->type = SYM_DOUBLE;
                            $2->dval = 0.0;
                        }
                  | TYPE_DOUBLE NAME '=' d_expression
                        {
                            $2->type = SYM_DOUBLE;
                            $2->dval = $4;
                        }
                  | TYPE_DOUBLE NAME '=' i_expression
                        {
                            $2->type = SYM_DOUBLE;
                            $2->dval = (double)$4;
                        }
                  | NAME_DOUBLE '=' d_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                *((double*)$1->ptr) = $3;
                            }
                            else
                            {
                                $1->dval = $3;
                            }
                        }
                  | NAME_DOUBLE '=' i_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                *((double*)$1->ptr) = (double)$3;
                            }
                            else
                            {
                                $1->dval = (double)$3;
                            }
                        }

                  | TYPE_INTEGER NAME
                        {
                            $2->type = SYM_INTEGER;
                            $2->ival = 0;
                        }
                  | TYPE_INTEGER NAME '=' i_expression
                        {
                            $2->type = SYM_INTEGER;
                            $2->ival = $4;
                        }
                  | NAME_INTEGER '=' i_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                *((int*)$1->ptr) = $3;
                            }
                            else
                            {
                                $1->ival = $3;
                            }
                        }

                  | TYPE_STRING NAME
                        {
                            $2->type = SYM_STRING;
                        }
                  | TYPE_STRING NAME '=' STRING
                        {
                            $2->type = SYM_STRING;
                            $2->str  = $4;
                        }
                  | NAME_STRING '=' s_expression
                        {
                            if( $1->surface )
                            {
                                if( $1->constant )
                                {
                                    yyerror( "cannot assign to a constant" );
                                }
                                else
                                {
                                    delete_char ( *((char**)$1->ptr) );
                                    *((char**)$1->ptr) = $3;
                                }
                            }
                            else
                            {
                                delete_char ( $1->str );
                                $1->str = $3;
                            }
                        }
                  | IF '(' i_expression ')' GOTO NAME_LABEL
                        {
                            if($3 == 1)
                            {
				goto_label  = $6->label;
                                goto_line   = $6->lineno;
                                goto_flag   = 1;
                                YYACCEPT;
			    }
                        }
                  | GOTO NAME_LABEL
		        {
				goto_label  = $2->label;
                                goto_line   = $2->lineno;
                                goto_flag   = 1;
                                YYACCEPT;
                        }
                  ;

s_expression:       s_expression '+' s_expression
                        {
                            size_t  len1,len2,len;

                            len1 = strlen( $1 );
                            len2 = strlen( $3 );
                            len  = len1 + len2;
                            
                            $$ = new_char( len + 1 );

                            strncpy( $$,$1,len1 );
                            strncpy( &($$[len1]),$3,len2 );
                            $$[len]='\0';

                            delete_char ( $1 );
                            delete_char ( $3 );
			    $1=$3=0;
			}
                  | STRING
                        {
                            $$ = $1;
			}
                  | ITOSTR '(' i_expression ')'
                        {
                            static  char    buf[50];
                            size_t  length;

                            sprintf( buf,"%d",$3 );
                            length = strlen( buf );
                            $$ = new_char( length + 1 );
                            strncpy( $$,buf,length );
                            $$[length] = '\0';
			}
                  | ITOASC '(' i_expression ')'
                        {
                            static  char    buf[50];
                            size_t  length;

                            itotexascii( $3,buf,49 );
                            length = strlen( buf );
                            $$ = new_char( length + 1 );
                            strncpy( $$,buf,length );
                            $$[length] = '\0';
			}
                  |ITOSTRN '(' i_expression ',' i_expression ')'
                        {
                            static  char    buf[50];
                            size_t  length;

                            sprintf( buf,"%d",$5 );
                            length = strlen( buf );

                            if( $3 > 50 || $3 < length )
                            {
                               yyerror( "ridoculous value in itostrn" );
			    }
                            else
                            {
                                int    i,zeroes;
                                zeroes = $3 - length;

                                $$ = new_char( $3 + 1 );

                                for( i=0; i < $3; i++ )
                                {
                                    $$[i] = '0';
			        }
                                strncpy( &($$[zeroes]),buf,length );
                                $$[$3] = '\0';
                            }
                        }
                  |ITOASCN '(' i_expression ',' i_expression ')'
                        {
                            static  char    buf[50];
                            size_t  length;

                            itotexascii( $5,buf,49 );
                            length = strlen( buf );

                            if( $3 > 50 || $3 < length )
                            {
                               yyerror( "ridoculous value in itoascn" );
			    }
                            else
                            {
                                int    i,zeroes;
                                zeroes = $3 - length;

                                $$ = new_char( $3 + 1 );

                                for( i=0; i < $3; i++ )
                                {
                                    $$[i] = 'a';
			        }
                                strncpy( &($$[zeroes]),buf,length );
                                $$[$3] = '\0';
                            }
                        }
                  ;
  
p_expression:       '(' p_expression ')'
                        {
                            $$ = $2;
                        }
                  | p_expression '+' p_expression      
                        {
                            polyxyz_add_self( &$1,&$3 );
                            delete_coeff_polyxyz( &$3 );
                            $$ = $1;
                        }
                  | p_expression '+' m_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$3 );

                            polyxyz_add_self( &$1,&p );
                            delete_coeff_polyxyz( &p );
                            $$ = $1;
                        }
                  | p_expression '+' d_expression      
                        {
                            polyxyz p = double2polyxyz( $3 );

                            polyxyz_add_self( &$1,&p );
                            delete_coeff_polyxyz( &p );
                            $$ = $1;
                        }
                  | p_expression '+' i_expression      
                        {
                            polyxyz p = int2polyxyz( $3 );

                            polyxyz_add_self( &$1,&p );
                            delete_coeff_polyxyz( &p );
                            $$ = $1;
                        }

                  | m_expression '+' p_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );

                            polyxyz_add_self( &$3,&p );
                            delete_coeff_polyxyz( &p );
                            $$ = $3;
                        }
                  | d_expression '+' p_expression      
                        {
                            polyxyz p = double2polyxyz( $1 );

                            polyxyz_add_self( &$3,&p );
                            delete_coeff_polyxyz( &p );
                            $$ = $3;
                        }
                  | i_expression '+' p_expression      
                        {
                            polyxyz p = int2polyxyz( $1 );

                            polyxyz_add_self( &$3,&p );
                            delete_coeff_polyxyz( &p );
                            $$ = $3;
                        }

                  | m_expression '+' m_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );
                            polyxyz q = monxyz2polyxyz( &$3 );

                            polyxyz_add_self( &p,&q );
                            delete_coeff_polyxyz( &q );
                            $$ = p;
                        }

                  | m_expression '+' d_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );
                            polyxyz q = double2polyxyz( $3 );

                            polyxyz_add_self( &p,&q );
                            delete_coeff_polyxyz( &q );
                            $$ = p;
                        }
                  | m_expression '+' i_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );
                            polyxyz q = int2polyxyz( $3 );

                            $$ = polyxyz_add( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }

                  | d_expression '+' m_expression      
                        {
                            polyxyz p = double2polyxyz( $1 );
                            polyxyz q = monxyz2polyxyz( &$3 );

                            $$ = polyxyz_add( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }
                  | i_expression '+' m_expression      
                        {
                            polyxyz p = int2polyxyz( $1 );
                            polyxyz q = monxyz2polyxyz( &$3 );

                            $$ = polyxyz_add( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }

                  | p_expression '-' p_expression      
                        {
                            $$ = polyxyz_sub( &$1,&$3 );

                            if( control )
                            {
                                printf( "calculating p_expression - p_expression\n" );
                                polyxyz_print( &$1 );
                                printf( " - " );
                                polyxyz_print( &$3 );
                                printf( "\n" );
                            }

                            delete_coeff_polyxyz( &$1 );
                            delete_coeff_polyxyz( &$3 );
                        }
                  | p_expression '-' m_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$3 );

                            $$ = polyxyz_sub( &$1,&p );

                            delete_coeff_polyxyz( &$1 );
                            delete_coeff_polyxyz( &p );
                        }
                  | p_expression '-' d_expression      
                        {
                            polyxyz p = double2polyxyz( $3 );

                            $$ = polyxyz_sub( &$1,&p );

                            delete_coeff_polyxyz( &$1 );
                            delete_coeff_polyxyz( &p );
                        }
                  | p_expression '-' i_expression      
                        {
                            polyxyz p = int2polyxyz( $3 );

                            $$ = polyxyz_sub( &$1,&p );

                            delete_coeff_polyxyz( &$1 );
                            delete_coeff_polyxyz( &p );
                        }

                  | m_expression '-' p_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );

                            $$ = polyxyz_sub( &p,&$3 );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &$3 );
                        }
                  | d_expression '-' p_expression      
                        {
                            polyxyz p = double2polyxyz( $1 );

                            $$ = polyxyz_sub( &p,&$3 );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &$3 );
                        }
                  | i_expression '-' p_expression      
                        {
                            polyxyz p = int2polyxyz( $1 );

                            $$ = polyxyz_sub( &p,&$3 );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &$3 );
                        }

                  | m_expression '-' m_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );
                            polyxyz q = monxyz2polyxyz( &$3 );

                            $$ = polyxyz_sub( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }

                  | m_expression '-' d_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );
                            polyxyz q = double2polyxyz( $3 );

                            $$ = polyxyz_sub( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }
                  | m_expression '-' i_expression      
                        {
                            polyxyz p = monxyz2polyxyz( &$1 );
                            polyxyz q = int2polyxyz( $3 );

                            $$ = polyxyz_sub( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }

                  | d_expression '-' m_expression      
                        {
                            polyxyz p = double2polyxyz( $1 );
                            polyxyz q = monxyz2polyxyz( &$3 );

                            $$ = polyxyz_sub( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }
                  | i_expression '-' m_expression      
                        {
                            polyxyz p = int2polyxyz( $1 );
                            polyxyz q = monxyz2polyxyz( &$3 );

                            $$ = polyxyz_sub( &p,&q );

                            delete_coeff_polyxyz( &p );
                            delete_coeff_polyxyz( &q );
                        }

                  | p_expression '*' p_expression      
                        {
                            $$ = polyxyz_mult( &$1,&$3 );

                            if( control )
                            {
                                printf( "calculating p_expression * p_expression\n" );
                                polyxyz_print( &$1 );
                                printf( " * " );
                                polyxyz_print( &$3 );
                                printf( "\n" );
                            }

                            delete_coeff_polyxyz( &$1 );
                            delete_coeff_polyxyz( &$3 );
                        }
                  | p_expression '*' m_expression      
                        {
                            polyxyz_mult_monxyz_self( &$1,&$3 );
                            $$ = $1;
                        }
                  | p_expression '*' d_expression      
                        {
                            polyxyz_mult_double_self( &$1,$3 );
                            $$ = $1;
                        }
                  | p_expression '*' i_expression      
                        {
                            polyxyz_mult_double_self( &$1,(double)$3 );
                            $$ = $1;
                        }

                  | m_expression '*' p_expression      
                        {
                            polyxyz_mult_monxyz_self( &$3,&$1 );
                            $$ = $3;
                        }
                  | d_expression '*' p_expression      
                        {
                            polyxyz_mult_double_self( &$3,$1 );
                            $$ = $3;
                        }
                  | i_expression '*' p_expression      
                        {
                            polyxyz_mult_double_self( &$3,(double)$1 );
                            $$ = $3;
                        }

                  | p_expression '/' d_expression
                        {
                            polyxyz_div_double_self( &$1,$3 );

                            if( control )
                            {
                                printf( "calculating p_expression / d_expression\n" );
                                polyxyz_print( &$1 );
                                printf( " / %f\n",$3 );
                            }

                            $$ = $1;
                        }
                  | p_expression '/' i_expression
                        {
                            polyxyz_div_double_self( &$1,(double)$3 );

                            if( control )
                            {
                                printf( "calculating p_expression / i_expression\n" );
                                polyxyz_print( &$1 );
                                printf( " / %d\n",$3 );
                            }

                            $$ = $1;
                        }

                  | p_expression '^' i_expression
                        {
                            $$ = polyxyz_power( &$1,$3 );

                            if( control )
                            {
                                printf( "calculating p_expression ^ INTEGER\n" );
                                polyxyz_print( &$1 );
                                printf( " ^ %d\n",$3 );
                            }

                            delete_coeff_polyxyz( &$1 );
                        }
                  | '-' p_expression %prec UMINUS    
                        {
                            polyxyz_neg_self( &$2 );

                            if( control )
                            {
                                printf( "calculating - p_expression\n" );
                                printf( " - " );
                                polyxyz_print( &$2 );
                                printf( "\n" );
                            }
   
                            $$ = $2;
                        }
                  | '+' p_expression %prec UMINUS
                        {
                            $$ = $2;

                            if( control )
                            {
                                printf( "calculating + p_expression\n" );
                                printf( " + " );
                                polyxyz_print( &$2 );
                                printf( "\n" );
                            }
                        }
                  | DIFF '(' p_expression ',' m_expression ')'
                        {
                            polyxyz_dmonxyz_self( &$3,&$5,0 );
                            $$ = $3;
                        }
                  | DIFF '(' p_expression ',' m_expression ',' i_expression ')'
                        {
                            polyxyz_dmonxyz_self( &$3,&$5,$7 );
                            $$ = $3;
                        }
                  | DIFF '(' p_expression ',' i_expression ',' i_expression ')'
                        {
                            polyxyz_dw_n_self( &$3,$7 );
                            $$ = $3;
                        }
                  | HESSIAN_CURVE '(' p_expression ')'
                        {
                            polyxyz_hessian_curve_self( &$3 );
                            $$ = $3;
                        }
                  | HESSIAN_SURFACE '(' p_expression ')'
                        {
                            polyxyz_hessian_surface_self( &$3 );
                            $$ = $3;
                        }
                  | ROTATE p_expression ',' d_expression ',' X_AXIS ')'
                        {
                            polyxyz_rotate_x_self( &$2,$4 );
                            $$ = $2;
			}
                  | ROTATE p_expression ',' d_expression ',' Y_AXIS ')'
                        {
                            polyxyz_rotate_y_self( &$2,$4 );
                            $$ = $2;
			}
                  | ROTATE p_expression ',' d_expression ',' Z_AXIS ')'
                        {
                            polyxyz_rotate_z_self( &$2,$4 );
                            $$ = $2;
			}
                  | NAME_POLYXYZ                           
                        {
                            if( $1->surface )
                            {
                                $$ = polyxyz_copy( (polyxyz*)$1->ptr );
                            }
                            else
                            {
                                $$ = polyxyz_copy( &$1->p );
                            }

                            if( control )
                            {
                                printf( "calculating NAME\n" );
                                printf( "POLYXYZ: " );
                                polyxyz_print( &$1->p );
                                printf( "\n" );
                            }
                        }
                  ;

m_expression:       m_expression '*' m_expression
                        { $$ = monxyz_mult( $1,&$3 ); }
                  | m_expression '*' i_expression
                        { $$ = monxyz_mult_double( $1,(double)$3 ); }
                  | m_expression '*' d_expression
                        { $$ = monxyz_mult_double( $1,$3 ); }
                  | i_expression '*' m_expression
                        { $$ = monxyz_mult_double( $3,(double)$1 ); }
                  | d_expression '*' m_expression
                        { $$ = monxyz_mult_double( $3,$1 ); }
                  | m_expression '/' i_expression
                        { $$ = monxyz_div_double( $1,(double)$3 ); }
                  | m_expression '/' d_expression
                        { $$ = monxyz_div_double( $1,$3 ); }
                  | m_expression '^' i_expression
                        { $$ = monxyz_power( $1,$3 ); }
                  | '-' m_expression %prec UMINUS
                        { $$ = monxyz_neg( $2 ); }
                  | '+' m_expression %prec UMINUS
                        { $$ = $2; }
                  | MONOM
                        { $$ = $1; }
                  | NAME_MONOM
                        {
                            if( $1->surface )
                            {
                                $$ = *((monxyz*)$1->ptr);
                            }
                            else
                            {
                                $$ = $1->mon;
                            }

                            if( control )
                            {
                                printf( "calculating NAME\n" );
                                printf( "MONXYZ: " );
                                monxyz_print( &$1->mon );
                            }
                        }
                  | '(' m_expression ')'
                        { $$ = $2; }
                  ;

d_expression:       '(' d_expression ')' 
                        { $$ = $2; }
                  | SQRT '(' d_expression ')'
                        { $$ = sqrt( $3 ); }
                  | SQRT '(' i_expression ')'
                        { $$ = sqrt( (double)$3 ); }
                  | POWER '(' d_expression ',' d_expression ')'
                        { $$ =
#ifdef SUN
                             sunpow( $3,$5 );
#else
                             pow( $3,$5 );
#endif /* SUN */ 
			}
                  | SINUS '(' d_expression ')'
                        { $$ = sin( $3 ); }
                  | SINUS '(' i_expression ')'
                        { $$ = sin( (double)$3 ); }
                  | COSINUS '(' d_expression ')'
                        { $$ = cos( $3 ); }
                  | COSINUS '(' i_expression ')'
                        { $$ = cos( (double)$3 ); }
                  | TANGENS '(' d_expression ')'
                        { $$ = tan( $3 ); }
                  | TANGENS '(' i_expression ')'
                        { $$ = tan( (double)$3 ); }
                  | COTANGENS '(' d_expression ')'
                        { $$ = cos( $3 )/sin( $3 ); }
                  | COTANGENS '(' i_expression ')'
                        { $$ = cos( (double)$3 )/sin( (double)$3 ); }
                  | ARCUS_SINUS '(' d_expression ')'
                        { $$ = sin( $3 ); }
                  | ARCUS_SINUS '(' i_expression ')'
                        { $$ = asin( (double)$3 ); }
                  | ARCUS_COSINUS '(' d_expression ')'
                        { $$ = acos( $3 ); }
                  | ARCUS_COSINUS '(' i_expression ')'
                        { $$ = acos( (double)$3 ); }
                  | ARCUS_TANGENS '(' d_expression ')'
                        { $$ = atan( $3 ); }
                  | ARCUS_TANGENS '(' i_expression ')'
                        { $$ = atan( (double)$3 ); }
                  | ARCUS_COTANGENS '(' d_expression ')'
                        { $$ = M_PI_2 - atan( $3 ); }
                  | ARCUS_COTANGENS '(' i_expression ')'
                        { $$ = M_PI_2 - atan( (double)$3 ); }
                  | d_expression '+' d_expression
                        { $$ = $1 + $3; }
                  | d_expression '+' i_expression
                        { $$ = $1 + (double)$3; }
                  | i_expression '+' d_expression
                        { $$ = (double)$1 + $3; }
                  | d_expression '-' d_expression
                        { $$ = $1 - $3; }
                  | d_expression '-' i_expression
                        { $$ = $1 - (double)$3; }
                  | i_expression '-' d_expression
                        { $$ = (double)$1 - $3; }
                  | d_expression '*' d_expression
                        { $$ = $1 * $3; }
                  | d_expression '*' i_expression
                        { $$ = $1 * (double)$3; }
                  | i_expression '*' d_expression
                        { $$ = (double)$1 * $3; }
                  | d_expression '/' d_expression
                        { $$ = $1 / $3; }
                  | d_expression '/' i_expression
                        { $$ = $1 / (double)$3; }
                  | i_expression '/' d_expression
                        { $$ = (double)$1 / $3; }
                  | i_expression '/' i_expression
                        { $$ = (double)$1 / (double)$3; }
                  | d_expression '^' d_expression
                        { $$ = pow( $1,$3 ); }
                  | d_expression '^' i_expression
                        { $$ = pow( $1,(double)$3 ); }
                  | i_expression '^' d_expression
                        { $$ = pow( (double)$1,$3 ); }
                  | '-' d_expression %prec UMINUS
                        { $$ = -$2; }
                  | '+' d_expression %prec UMINUS
                        { $$ = $2; }
                  | DOUBLE
                        { $$ = $1; }
                  | NAME_DOUBLE
                        {
                            if( $1->surface )
                            {
                                $$ = *((double*)$1->ptr);
                            }
                            else
                            {
                                $$ = $1->dval;
                            }

                            if( control )
                            {
                                printf( "calculating NAME\n" );
                                printf( "DOUBLE: %f\n",$1->dval );
                            }
                        }
                  ;

i_expression:       '(' i_expression ')'
                        { $$ = $2; }
                  | DEGREE '(' p_expression ')'
                        { $$ = $3.deg; };
                  | LENGTH '(' p_expression ')'
                        { $$ = $3.n; };
                  | i_expression AND i_expression
                        { $$ = ( $1 && $3 ); }
                  | i_expression OR i_expression
                        { $$ = ( $1 || $3 ); }
                  | i_expression EQUAL i_expression
                        { $$ = ( $1 == $3 ); }
                  | i_expression NOTEQUAL i_expression
                        { $$ = ( $1 != $3 ); }
                  | i_expression BIGGER i_expression 
                        { $$ = ( $1 > $3 ); }
                  | i_expression BIGGERTHAN i_expression 
                        { $$ = ( $1 >= $3 ); }
                  | i_expression SMALLER i_expression 
                        { $$ = ( $1 < $3 ); }
                  | i_expression SMALLERTHAN i_expression 
                        { $$ = ( $1 <= $3 ); }
                  | d_expression EQUAL d_expression
                        { $$ = ( $1 == $3 ); }
                  | d_expression NOTEQUAL d_expression
                        { $$ = ( $1 != $3 ); }
                  | d_expression BIGGER d_expression 
                        { $$ = ( $1 > $3 ); }
                  | d_expression BIGGERTHAN d_expression 
                        { $$ = ( $1 >= $3 ); }
                  | d_expression SMALLER d_expression 
                        { $$ = ( $1 < $3 ); }
                  | d_expression SMALLERTHAN d_expression 
                        { $$ = ( $1 <= $3 ); }
                  | i_expression EQUAL d_expression
                        { $$ = ( $1 == $3 ); }
                  | i_expression NOTEQUAL d_expression
                        { $$ = ( $1 != $3 ); }
                  | i_expression BIGGER d_expression 
                        { $$ = ( $1 > $3 ); }
                  | i_expression BIGGERTHAN d_expression 
                        { $$ = ( $1 >= $3 ); }
                  | i_expression SMALLER d_expression 
                        { $$ = ( $1 < $3 ); }
                  | i_expression SMALLERTHAN d_expression 
                        { $$ = ( $1 <= $3 ); }
                  | d_expression EQUAL i_expression
                        { $$ = ( $1 == $3 ); }
                  | d_expression NOTEQUAL i_expression
                        { $$ = ( $1 != $3 ); }
                  | d_expression BIGGER i_expression 
                        { $$ = ( $1 > $3 ); }
                  | d_expression BIGGERTHAN i_expression 
                        { $$ = ( $1 >= $3 ); }
                  | d_expression SMALLER i_expression 
                        { $$ = ( $1 < $3 ); }
                  | d_expression SMALLERTHAN i_expression 
                        { $$ = ( $1 <= $3 ); }
                  | i_expression '+' i_expression
                        { $$ = $1 + $3; }
                  | i_expression '-' i_expression
                        { $$ = $1 - $3; }
                  | i_expression '*' i_expression
                        { $$ = $1 * $3; }
                  | i_expression '%' i_expression
                        { $$ = $1 % $3; }
                  | i_expression '^' i_expression
                        { $$ = (int)pow( (double)$1,(double)$3 ); }
                  | '-' i_expression %prec UMINUS
                        { $$ = -$2; }
                  | '+' i_expression %prec UMINUS
                        { $$ = $2; }
                  | INTEGER
                        { $$ = $1; }
                  | NAME_INTEGER
                        {
                            if( $1->surface )
                            {
                                $$ = *((int*)$1->ptr);
                            }
                            else
                            {
                                $$ = $1->ival;
                            }

                            if( control )
                            {
                                printf( "calculating NAME\n" );
                                printf( "INTEGER: %d\n",$1->ival );
                            }
                            break;
                        }
                  ;

%%

/* ------------------------------------------------------------------------- */
/* end of file: polyyacc.y                                                   */
/* ------------------------------------------------------------------------- */
