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





/* ------------------------------------------------------------------------- */
/* bezierarith.c: improved bezier arithmetic                                 */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     8 march 96                                                      */
/* ------------------------------------------------------------------------- */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include"degree.h"
#include"mymemory.h"
#include"simple.h"
#include"monomarith.h"
#include"polyarith.h"
#include"bezierarith.h"

static  int     root_of_bezier_positive           ( bezier*,double*,int* );
static  int     root_of_bezier_positive_open      ( bezier*,double*,int* );
static  int     root_of_bezier_positive_multi     ( bezier*,double*,int* );
static  int     root_of_bezier_positive_multi_open( bezier*,double*,int* );
static  int     root_of_bezier_negative           ( bezier*,double*,int* );
static  int     root_of_bezier_negative_open      ( bezier*,double*,int* );
static  int     root_of_bezier_negative_multi     ( bezier*,double*,int* );
static  int     root_of_bezier_negative_multi_open( bezier*,double*,int* );

/* ------------------------------------------------------------------------- */
/*  definitions                                                              */
/* ------------------------------------------------------------------------- */

#define DIVIDE_TO_LEFT( bez,s,t,i,j )                                 \
    for( i = 1; i <= (bez).n; i++ )                                   \
    {                                                                 \
        for( j = (bez).n; j >= i; j-- )                               \
        {                                                             \
            (bez).p[j] = s*(bez).p[j-1] + t*(bez).p[j];               \
        }                                                             \
    }

#define DIVIDE_TO_RIGHT( bez,s,t,i,j,k )                              \
    for( i = 1; i <= (bez).n; i++ )                                   \
    {                                                                 \
        for( j = 0, k = (bez).n - i; j <= k; j++ )                    \
        {                                                             \
            (bez).p[j] = s*(bez).p[j] + t*(bez).p[j+1];               \
        }                                                             \
    }

#define DIVIDE_TO_RIGHT_BOTH( bez,bez_left,s,t,i,j,k )                \
    (bez_left).p[0] = (bez).p[0];                                     \
                                                                      \
    for( i = 1; i <= (bez).n; i++ )                                   \
    {                                                                 \
        for( j = 0, k = (bez).n - i; j <= k; j++ )                    \
        {                                                             \
            (bez).p[j] = s*(bez).p[j] + t*(bez).p[j+1];               \
	}                                                             \
                                                                      \
        (bez_left).p[i] = (bez).p[0];                                 \
    }

#define DIVIDE_TO_LEFT_BOTH( bez,bez_right,s,t,i,j )                  \
    (bez_right).p[(bez).n] = (bez).p[(bez).n];                        \
                                                                      \
    for( i = 1; i <= (bez).n; i++ )                                   \
    {                                                                 \
        for( j = (bez).n; j >= i; j-- )                               \
        {                                                             \
            (bez).p[j] = s*(bez).p[j-1] + t*(bez).p[j];               \
	}                                                             \
                                                                      \
        (bez_right).p[(bez).n-i] = (bez).p[(bez).n];                  \
    }

#define LEFT_BORDER (-1)

/* ------------------------------------------------------------------------- */
/*  module epsilon                                                           */
/* ------------------------------------------------------------------------- */

// static  double  epsilon = 1.0e-6;
#define epsilon 1.0e-6

/* ------------------------------------------------------------------------- */
/*  Find the biggest root of a polynomial in [a,b] by converting it into a   */
/*  two bezier-curves:                                                       */
/*                                                                           */
/*                   left curve       right curve                            */
/*                |  |  |  |  |  |   |   |   |   |   |                       */
/*    <-----------+--------------|-------------------+------->               */
/*                a              0                   b                       */
/* ------------------------------------------------------------------------- */

int     zero_of_bezier_single_root_no_help(
                polyx *f,double a,double b,double *root )
{
    double  s,t;
    int     i,j,k;
    int     number_of_zeroes = 0;

    /* ---------------------------------- */
    /*  calculate the right coefficients  */
    /* ---------------------------------- */

    if( b > 0.0 )
    {
        double  right[MAX_DEGREE+1];
        bezier  bez_right;

        bez_right.p = right;
        bez_right.n = f->n;

        for( i = 0; i <= bez_right.n; i++ )
        {
            bez_right.p[i] = f->a[i]/binom_coeff( bez_right.n,i );
        }

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */
 
        s = 1.0/( b + 1.0 ); t = 1.0 - s;

        DIVIDE_TO_LEFT( bez_right,s,t,i,j );

        if( a > 0.0 )
        {
            /* ------------------------------- */
            /*  we need an other de Casteljau  */
            /* ------------------------------- */

            s = ( b - a )/( b*( a + 1.0 ) ); t = 1.0 - s;

            DIVIDE_TO_RIGHT( bez_right,s,t,i,j,k );

            /* --------------------------------------------- */
            /*  now the bezier curve on [a,b] is calculated  */
            /* --------------------------------------------- */

            bez_right.a = a; bez_right.b = b;

            root_of_bezier_positive( &bez_right,root,&number_of_zeroes );
            return  number_of_zeroes;
        }
        else
        {
            /* --------------------------------------------------- */
            /*  now the right bezier curve on [0,b] is calculated  */
            /* --------------------------------------------------- */

            bez_right.a = 0.0; bez_right.b = b;

            if( a >= 0.0 )
            {
                root_of_bezier_positive(
                    &bez_right,root,&number_of_zeroes );
                return  number_of_zeroes;
            }
            else if( root_of_bezier_positive(
                    &bez_right,root,&number_of_zeroes ) )
            {
                return  number_of_zeroes;
            }
        }
    }

    /* --------------------------------- */
    /*  calculate the left coefficients  */
    /* --------------------------------- */

    if( a < 0.0 )
    {
        int     minus_one = ( f->n % 2 == 0 ? 1 : -1 );

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = f->n;

        for( i = 0, j = bez_left.n; i <= bez_left.n; i++, j-- )
        {
            bez_left.p[i] = minus_one*f->a[j]/binom_coeff( bez_left.n,i );
            minus_one = -minus_one;
        }

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */
 
        s = a/( a - 1.0 ); t = 1.0 - s;

        DIVIDE_TO_RIGHT( bez_left,s,t,i,j,k );

        if( b < 0.0 )
        {
            /* ------------------------------- */
            /*  we need an other de Casteljau  */
            /* ------------------------------- */

            s = ( a - 1.0  )*b/( ( b - 1.0  )*a ); t = 1.0 - s;

            DIVIDE_TO_LEFT( bez_left,s,t,i,j );

            /* --------------------------------------------- */
            /*  now the bezier curve on [a,b] is calculated  */
            /* --------------------------------------------- */

            bez_left.a = a; bez_left.b = b;

            root_of_bezier_negative( &bez_left,root,&number_of_zeroes );
            return number_of_zeroes;
        }
        else
        {
            /* --------------------------------------------- */
            /*  now the bezier curve on [a,0] is calculated  */
            /* --------------------------------------------- */

            bez_left.a = a; bez_left.b = 0.0;

            root_of_bezier_negative( &bez_left,root,&number_of_zeroes );
            return number_of_zeroes;
        }
    }

    printf( "2. Was schiefgegangen a=%f b=%f!!!\n",a,b );
    return  0;
}

/* ------------------------------------------------------------------------- */
/*  Find the biggest root of a polynomial in [a,b[ by converting it into a   */
/*  two bezier-curves:                                                       */
/*                                                                           */
/*                   left curve       right curve                            */
/*                |  |  |  |  |  |   |   |   |   |   |                       */
/*    <-----------+--------------|-------------------+------->               */
/*                a              0                   b                       */
/* ------------------------------------------------------------------------- */

int     zero_of_bezier_single_root_no_help_open(
                polyx *f,double a,double b,double *root )
{
    double  s,t;
    int     i,j,k;
    int     number_of_zeroes = 0;

    /* ---------------------------------- */
    /*  calculate the right coefficients  */
    /* ---------------------------------- */

    if( b > 0.0 )
    {
        double  right[MAX_DEGREE+1];
        bezier  bez_right;

        bez_right.p = right;
        bez_right.n = f->n;

        for( i = 0; i <= bez_right.n; i++ )
        {
            bez_right.p[i] = f->a[i]/binom_coeff( bez_right.n,i );
        }

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */
 
        s = 1.0/( b + 1.0 ); t = 1.0 - s;

        DIVIDE_TO_LEFT( bez_right,s,t,i,j );

        if( a > 0.0 )
        {
            /* ------------------------------- */
            /*  we need an other de Casteljau  */
            /* ------------------------------- */

            s = ( b - a )/( b*( a + 1.0 ) ); t = 1.0 - s;

            DIVIDE_TO_RIGHT( bez_right,s,t,i,j,k );

            /* --------------------------------------------- */
            /*  now the bezier curve on [a,b] is calculated  */
            /* --------------------------------------------- */

            bez_right.a = a; bez_right.b = b;

            root_of_bezier_positive_open( &bez_right,root,&number_of_zeroes );
            return  number_of_zeroes;
        }
        else
        {
            /* --------------------------------------------------- */
            /*  now the right bezier curve on [0,b] is calculated  */
            /* --------------------------------------------------- */

            bez_right.a = 0.0; bez_right.b = b;

            if( a >= 0.0 )
            {
                root_of_bezier_positive_open(
                    &bez_right,root,&number_of_zeroes );
                return  number_of_zeroes;
            }
            else if( root_of_bezier_positive_open(
                    &bez_right,root,&number_of_zeroes ) )
            {
                return  number_of_zeroes;
            }
        }
    }

    /* --------------------------------- */
    /*  calculate the left coefficients  */
    /* --------------------------------- */

    if( a < 0.0 )
    {
        int     minus_one = ( f->n % 2 == 0 ? 1 : -1 );

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = f->n;

        for( i = 0, j = bez_left.n; i <= bez_left.n; i++, j-- )
        {
            bez_left.p[i] = minus_one*f->a[j]/binom_coeff( bez_left.n,i );
            minus_one = -minus_one;
        }

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */
 
        s = a/( a - 1.0 ); t = 1.0 - s;

        DIVIDE_TO_RIGHT( bez_left,s,t,i,j,k );

        if( b < 0.0 )
        {
            /* ------------------------------- */
            /*  we need an other de Casteljau  */
            /* ------------------------------- */

            s = ( a - 1.0  )*b/( ( b - 1.0  )*a ); t = 1.0 - s;

            DIVIDE_TO_LEFT( bez_left,s,t,i,j );

            /* --------------------------------------------- */
            /*  now the bezier curve on [a,b] is calculated  */
            /* --------------------------------------------- */

            bez_left.a = a; bez_left.b = b;

            root_of_bezier_negative_open( &bez_left,root,&number_of_zeroes );
            return number_of_zeroes;
        }
        else
        {
            /* --------------------------------------------- */
            /*  now the bezier curve on [a,0] is calculated  */
            /* --------------------------------------------- */

            bez_left.a = a; bez_left.b = 0.0;

            if( b == 0.0 )
	    {
                root_of_bezier_negative_open( &bez_left,root,&number_of_zeroes );
	    }
            else
	    {
                root_of_bezier_negative( &bez_left,root,&number_of_zeroes );
	    }

            return number_of_zeroes;
        }
    }

    printf( "2. Was schiefgegangen a=%f b=%f!!!\n",a,b );
    return  0;
}

/* ------------------------------------------------------------------------- */
/*  Find the biggest root of a polynomial in [a,b] where old_root is an      */
/*  approximation by converting it into two bezier-curves:                   */
/*                                                                           */
/*                   left curve       right curve                            */
/*                |  |  |  |  |  |   |   |   |   |   |                       */
/*    <-----------+--------------|-------------------+------->               */
/*                a              0                   b                       */
/* ------------------------------------------------------------------------- */

int     zero_of_bezier_single_root_help(
                polyx *f,double a,double b,double *root,double old_root )
{
    if( old_root <= a || old_root >= b )
    {
        return  zero_of_bezier_single_root_no_help( f,a,b,root );
    }
    else
    {
        double  s,t;
        int     i,j,k;
        int     number_of_zeroes = 0;

        /* ---------------------------------- */
        /*  calculate the right coefficients  */
        /* ---------------------------------- */

        if( b > 0.0 )
        {
            double  right[MAX_DEGREE+1];
            bezier  bez_right;

            bez_right.p = right;
            bez_right.n = f->n;

            for( i = 0; i <= bez_right.n; i++ )
            {
                bez_right.p[i] = f->a[i]/binom_coeff( bez_right.n,i );
            }

            /* ---------------------- */
            /*  perform de Casteljau  */
            /* ---------------------- */
     
            s = 1.0/( b + 1.0 ); t = 1.0 - s;

            DIVIDE_TO_LEFT( bez_right,s,t,i,j ); 
    
            if( a > 0.0 )
            {
                /* ------------------------------- */
                /*  we need an other de Casteljau  */
                /* ------------------------------- */

                s = ( b - a )/( b*( a + 1.0 ) ); t = 1.0 - s;

                DIVIDE_TO_RIGHT( bez_right,s,t,i,j,k );

                /* --------------------------------------------- */
                /*  now the bezier curve on [a,b] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= a || old_root >= b )
                {
                    bez_right.a = a; bez_right.b = b;

                    root_of_bezier_positive(
                        &bez_right,root,&number_of_zeroes );
                    return  number_of_zeroes;
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( b - old_root )*( a + 1.0 )/
                        ( ( b - a )*( old_root + 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_RIGHT_BOTH( bez_right,bez_mid,s,t,i,j,k );

                    bez_right.a = old_root; bez_right.b = b;

                    if( root_of_bezier_positive(
                            &bez_right,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
                    }
                    else
                    {
                        bez_mid.a = a; bez_mid.b = old_root;

                        root_of_bezier_positive(
                            &bez_mid,root,&number_of_zeroes );
                        return  number_of_zeroes;
		    }
                }
            }
            else
            {
                /* --------------------------------------------- */
                /*  now the bezier curve on [0,b] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= 0.0 || old_root >= b )
                {
                    bez_right.a = 0.0; bez_right.b = b;

                    if( root_of_bezier_positive(
                            &bez_right,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
                    }
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( b - old_root )/( b*( old_root + 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_RIGHT_BOTH( bez_right,bez_mid,s,t,i,j,k );

                    bez_right.a = old_root; bez_right.b = b;

                    if( a >= 0.0 )
                    {
                        if( root_of_bezier_positive(
                                &bez_right,root,&number_of_zeroes ) )
                        {
                            return  number_of_zeroes;
		        }
                        else
                        {
                            bez_mid.a = 0.0; bez_mid.b = old_root;

                            root_of_bezier_positive(
                                &bez_mid,root,&number_of_zeroes );
                            return  number_of_zeroes;
	                }
                    }
                    else
                    {
                        if( root_of_bezier_positive(
                                &bez_right,root,&number_of_zeroes ) )
                        {
                            return  number_of_zeroes;
		        }
                        else
                        {
                            bez_mid.a = 0.0; bez_mid.b = old_root;
 
                            if( root_of_bezier_positive(
                                &bez_mid,root,&number_of_zeroes ) )
                            {
                                return  number_of_zeroes;
			    }
		        }
                    }
                }
            }
        }

        /* --------------------------------- */
        /*  calculate the left coefficients  */
        /* --------------------------------- */
    
        if( a < 0.0 )
        {
            int    minus_one = ( f->n % 2 == 0 ? 1 : -1 );

            double  left[MAX_DEGREE+1];
            bezier  bez_left;

            bez_left.p = left;
            bez_left.n = f->n;

            for( i = 0, j = bez_left.n; i <= bez_left.n; i++, j-- )
            {
                bez_left.p[i] = minus_one*f->a[j]/binom_coeff( bez_left.n,i );
                minus_one = -minus_one;
            }
    
            /* ---------------------- */
            /*  perform de Casteljau  */
            /* ---------------------- */
     
            s = a/( a - 1.0 ); t = 1.0 - s;

            DIVIDE_TO_RIGHT( bez_left,s,t,i,j,k );
    
            if( b < 0.0 )
            {
                /* ------------------------------- */
                /*  we need an other de Casteljau  */
                /* ------------------------------- */

                s = ( a - 1.0 )*b/( ( b - 1.0 )*a ); t = 1.0 - s;

                DIVIDE_TO_LEFT( bez_left,s,t,i,j );
        
                /* --------------------------------------------- */
                /*  now the bezier curve on [a,b] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= a || old_root >= b )
                {
                    bez_left.a = a; bez_left.b = b;

                    root_of_bezier_negative(
                        &bez_left,root,&number_of_zeroes );
                    return  number_of_zeroes;
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( b - old_root )*( a - 1.0 )/
                        ( ( b - a )*( old_root - 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_LEFT_BOTH( bez_left,bez_mid,s,t,i,j );

                    bez_mid.a = old_root; bez_mid.b = b;

                    if( root_of_bezier_negative(
                            &bez_mid,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
	            }
                    else
                    {
                        bez_left.a = a; bez_left.b = old_root;

                        root_of_bezier_negative(
                            &bez_left,root,&number_of_zeroes );
                        return  number_of_zeroes;
		    }
	        }
	    }
            else
            {
                /* --------------------------------------------- */
                /*  now the bezier curve on [a,0] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= a || old_root >= 0.0 )
                {
                    bez_left.a = a; bez_left.b = 0.0;

                    root_of_bezier_negative(
                        &bez_left,root,&number_of_zeroes );
                    return  number_of_zeroes;
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( a - 1.0 )*old_root/( a*( old_root - 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_LEFT_BOTH( bez_left,bez_mid,s,t,i,j );

                    bez_mid.a = old_root; bez_mid.b = 0.0;

                    if( root_of_bezier_negative(
                            &bez_mid,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
	            }
                    else
                    {
                        bez_left.a = a; bez_left.b = old_root;

                        root_of_bezier_negative(
                            &bez_left,root,&number_of_zeroes );
                        return  number_of_zeroes;
		    }
	        }
	    }
        }
    }
    printf( "3. Was schiefgegangen ... \n" );
    return  0;
}

/* ------------------------------------------------------------------------- */
/*  Find the biggest root of a polynomial in [a,b] where old_root is an      */
/*  approximation by converting it into two bezier-curves:                   */
/*                                                                           */
/*                   left curve       right curve                            */
/*                |  |  |  |  |  |   |   |   |   |   |                       */
/*    <-----------+--------------|-------------------+------->               */
/*                a              0                   b                       */
/* ------------------------------------------------------------------------- */

int     zero_of_bezier_single_root_help_open(
                polyx *f,double a,double b,double *root,double old_root )
{
    if( old_root <= a || old_root >= b )
    {
        return  zero_of_bezier_single_root_no_help_open( f,a,b,root );
    }
    else
    {
        double  s,t;
        int     i,j,k;
        int     number_of_zeroes = 0;

        /* ---------------------------------- */
        /*  calculate the right coefficients  */
        /* ---------------------------------- */

        if( b > 0.0 )
        {
            double  right[MAX_DEGREE+1];
            bezier  bez_right;

            bez_right.p = right;
            bez_right.n = f->n;

            for( i = 0; i <= bez_right.n; i++ )
            {
                bez_right.p[i] = f->a[i]/binom_coeff( bez_right.n,i );
            }

            /* ---------------------- */
            /*  perform de Casteljau  */
            /* ---------------------- */
     
            s = 1.0/( b + 1.0 ); t = 1.0 - s;

            DIVIDE_TO_LEFT( bez_right,s,t,i,j ); 
    
            if( a > 0.0 )
            {
                /* ------------------------------- */
                /*  we need an other de Casteljau  */
                /* ------------------------------- */

                s = ( b - a )/( b*( a + 1.0 ) ); t = 1.0 - s;

                DIVIDE_TO_RIGHT( bez_right,s,t,i,j,k );

                /* --------------------------------------------- */
                /*  now the bezier curve on [a,b] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= a || old_root >= b )
                {
                    bez_right.a = a; bez_right.b = b;

                    root_of_bezier_positive_open(
                        &bez_right,root,&number_of_zeroes );
                    return  number_of_zeroes;
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( b - old_root )*( a + 1.0 )/
                        ( ( b - a )*( old_root + 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_RIGHT_BOTH( bez_right,bez_mid,s,t,i,j,k );

                    bez_right.a = old_root; bez_right.b = b;

                    if( root_of_bezier_positive_open(
                            &bez_right,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
                    }
                    else
                    {
                        bez_mid.a = a; bez_mid.b = old_root;

                        root_of_bezier_positive(
                            &bez_mid,root,&number_of_zeroes );
                        return  number_of_zeroes;
		    }
                }
            }
            else
            {
                /* --------------------------------------------- */
                /*  now the bezier curve on [0,b] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= 0.0 || old_root >= b )
                {
                    bez_right.a = 0.0; bez_right.b = b;

                    if( root_of_bezier_positive_open(
                            &bez_right,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
                    }
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( b - old_root )/( b*( old_root + 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_RIGHT_BOTH( bez_right,bez_mid,s,t,i,j,k );

                    bez_right.a = old_root; bez_right.b = b;

                    if( a >= 0.0 )
                    {
                        if( root_of_bezier_positive_open(
                                &bez_right,root,&number_of_zeroes ) )
                        {
                            return  number_of_zeroes;
		        }
                        else
                        {
                            bez_mid.a = 0.0; bez_mid.b = old_root;

                            root_of_bezier_positive(
                                &bez_mid,root,&number_of_zeroes );
                            return  number_of_zeroes;
	                }
                    }
                    else
                    {
                        if( root_of_bezier_positive_open(
                                &bez_right,root,&number_of_zeroes ) )
                        {
                            return  number_of_zeroes;
		        }
                        else
                        {
                            bez_mid.a = 0.0; bez_mid.b = old_root;
 
                            if( root_of_bezier_positive(
                                &bez_mid,root,&number_of_zeroes ) )
                            {
                                return  number_of_zeroes;
			    }
		        }
                    }
                }
            }
        }

        /* --------------------------------- */
        /*  calculate the left coefficients  */
        /* --------------------------------- */
    
        if( a < 0.0 )
        {
            int    minus_one = ( f->n % 2 == 0 ? 1 : -1 );

            double  left[MAX_DEGREE+1];
            bezier  bez_left;

            bez_left.p = left;
            bez_left.n = f->n;

            for( i = 0, j = bez_left.n; i <= bez_left.n; i++, j-- )
            {
                bez_left.p[i] = minus_one*f->a[j]/binom_coeff( bez_left.n,i );
                minus_one = -minus_one;
            }
    
            /* ---------------------- */
            /*  perform de Casteljau  */
            /* ---------------------- */
     
            s = a/( a - 1.0 ); t = 1.0 - s;

            DIVIDE_TO_RIGHT( bez_left,s,t,i,j,k );
    
            if( b < 0.0 )
            {
                /* ------------------------------- */
                /*  we need an other de Casteljau  */
                /* ------------------------------- */

                s = ( a - 1.0 )*b/( ( b - 1.0 )*a ); t = 1.0 - s;

                DIVIDE_TO_LEFT( bez_left,s,t,i,j );
        
                /* --------------------------------------------- */
                /*  now the bezier curve on [a,b] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= a || old_root >= b )
                {
                    bez_left.a = a; bez_left.b = b;

                    root_of_bezier_negative_open(
                        &bez_left,root,&number_of_zeroes );
                    return  number_of_zeroes;
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( b - old_root )*( a - 1.0 )/
                        ( ( b - a )*( old_root - 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_LEFT_BOTH( bez_left,bez_mid,s,t,i,j );

                    bez_mid.a = old_root; bez_mid.b = b;

                    if( root_of_bezier_negative_open(
                            &bez_mid,root,&number_of_zeroes ) )
                    {
                        return  number_of_zeroes;
	            }
                    else
                    {
                        bez_left.a = a; bez_left.b = old_root;

                        root_of_bezier_negative(
                            &bez_left,root,&number_of_zeroes );
                        return  number_of_zeroes;
		    }
	        }
	    }
            else
            {
                /* --------------------------------------------- */
                /*  now the bezier curve on [a,0] is calculated  */
                /* --------------------------------------------- */

                if( old_root <= a || old_root >= 0.0 )
                {
                    bez_left.a = a; bez_left.b = 0.0;

                    if( b == 0.0 )
		    {
                        root_of_bezier_negative_open(
                            &bez_left,root,&number_of_zeroes );
		    }
                    else
		    {
                        root_of_bezier_negative(
                            &bez_left,root,&number_of_zeroes );
		    }
                    return  number_of_zeroes;
                }
                else
                {
                    double  mid[MAX_DEGREE+1];
                    bezier  bez_mid;

                    bez_mid.p = mid;
                    bez_mid.n = f->n;

                    /* ----------------------- */
                    /*  subdivide at old root  */
                    /* ----------------------- */

                    s = ( a - 1.0 )*old_root/( a*( old_root - 1.0 ) );
                    t = 1.0 - s;

                    DIVIDE_TO_LEFT_BOTH( bez_left,bez_mid,s,t,i,j );

                    bez_mid.a = old_root; bez_mid.b = 0.0;

                    if( ( b == 0.0 && root_of_bezier_negative_open(
                            &bez_mid,root,&number_of_zeroes ) ) ||
                        ( b != 0.0 && root_of_bezier_negative(
                            &bez_mid,root,&number_of_zeroes ) ) )
                    {
                        return  number_of_zeroes;
	            }
                    else
                    {
                        bez_left.a = a; bez_left.b = old_root;

                        root_of_bezier_negative(
                            &bez_left,root,&number_of_zeroes );
                        return  number_of_zeroes;
		    }
	        }
	    }
        }
    }
    printf( "3. Was schiefgegangen ... \n" );
    return  0;
}

/* ------------------------------------------------------------------------- */
/*  Find all roots of a polynomial in [a,b] by converting it into two        */
/*  bezier-curves:                                                           */
/*                                                                           */
/*                   left curve       right curve                            */
/*                |  |  |  |  |  |   |   |   |   |   |                       */
/*    <-----------+--------------|-------------------+------->               */
/*                a              0                   b                       */
/* ------------------------------------------------------------------------- */

int     zero_of_bezier_multi_root_no_help(
                polyx *f,double a,double b,double *root )
{
    double  s,t;
    int     i,j,k;
    int     number_of_zeroes = 0;
    int     status = 0;

    /* ---------------------------------- */
    /*  calculate the right coefficients  */
    /* ---------------------------------- */

    if( a >= b )
    {
        printf( "Shit: %f %f\n",a,b );
    }
    if( b > 0.0 )
    {
        double  right[MAX_DEGREE+1];
        bezier  bez_right;

        bez_right.p = right;
        bez_right.n = f->n;

        for( i = 0; i <= bez_right.n; i++ )
        {
            bez_right.p[i] = f->a[i]/binom_coeff( bez_right.n,i );
        }

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */
 
        s = 1.0/( b + 1.0 ); t = 1.0 - s;

        DIVIDE_TO_LEFT( bez_right,s,t,i,j );

        if( a > 0.0 )
        {
            /* ------------------------------- */
            /*  we need an other de Casteljau  */
            /* ------------------------------- */

            s = ( b - a )/( b*( a + 1.0 ) ); t = 1.0 - s;

            DIVIDE_TO_RIGHT( bez_right,s,t,i,j,k );

            /* --------------------------------------------- */
            /*  now the bezier curve on [a,b] is calculated  */
            /* --------------------------------------------- */

            bez_right.a = a; bez_right.b = b;

            root_of_bezier_positive_multi( &bez_right,root,&number_of_zeroes );
            return  number_of_zeroes;
        }
        else
        {
            /* --------------------------------------------------- */
            /*  now the right bezier curve on [0,b] is calculated  */
            /* --------------------------------------------------- */

            bez_right.a = 0.0; bez_right.b = b;

            if( a >= 0.0 )
            {
                root_of_bezier_positive_multi(
                    &bez_right,root,&number_of_zeroes );
                return  number_of_zeroes;
            }
            else
            {
                status = root_of_bezier_positive_multi(
                    &bez_right,root,&number_of_zeroes );
            }
        }
    }

    /* --------------------------------- */
    /*  calculate the left coefficients  */
    /* --------------------------------- */

    if( a < 0.0 )
    {
        int     minus_one = ( f->n % 2 == 0 ? 1 : -1 );

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = f->n;

        for( i = 0, j = bez_left.n; i <= bez_left.n; i++, j-- )
        {
            bez_left.p[i] = minus_one*f->a[j]/binom_coeff( bez_left.n,i );
            minus_one = -minus_one;
        }

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */
 
        s = a/( a - 1.0 ); t = 1.0 - s;

        DIVIDE_TO_RIGHT( bez_left,s,t,i,j,k );

        if( b < 0.0 )
        {
            /* ------------------------------- */
            /*  we need an other de Casteljau  */
            /* ------------------------------- */

            s = ( a - 1.0  )*b/( ( b - 1.0  )*a ); t = 1.0 - s;

            DIVIDE_TO_LEFT( bez_left,s,t,i,j );

            /* --------------------------------------------- */
            /*  now the bezier curve on [a,b] is calculated  */
            /* --------------------------------------------- */

            bez_left.a = a; bez_left.b = b;

            root_of_bezier_negative_multi( &bez_left,root,&number_of_zeroes );
            return number_of_zeroes;
        }
        else
        {
            /* --------------------------------------------- */
            /*  now the bezier curve on [a,0] is calculated  */
            /* --------------------------------------------- */

            bez_left.a = a; bez_left.b = 0.0;

            if( b <= 0.0 || status != LEFT_BORDER )
	    {
                root_of_bezier_negative_multi(
                        &bez_left,root,&number_of_zeroes );
	    }
            else
            {
                root_of_bezier_negative_multi_open(
                        &bez_left,root,&number_of_zeroes );
	    }
            return  number_of_zeroes;
        }
    }
    printf( "1. Was schiefgegangen !!!\n" );
    return  0;
}

/* ------------------------------------------------------------------------- */
/*  find the biggest root of the bezier curve  bez, which represents a       */
/*  polynomial in a interval [a,b] with 0 <= a < b                           */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_positive( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n,j = bez->n - 1,k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s*bez->a/( bez->a + 1.0 ) + t*bez->b/( bez->b + 1.0 );
    x = r/( 1.0 - r );

    /* -------------------------------------- */
    /*  Do not subdivide if x == a or x == b  */
    /* -------------------------------------- */

    if( x - bez->a < epsilon || bez->b - x < epsilon )
    {
        root[(*number_of_roots)++] = x;

        return  TRUE;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_positive( bez,root,number_of_roots ) )
        {
            return  TRUE;
        }
        else
        {
            return  root_of_bezier_positive(
                        &bez_left,root,number_of_roots );
	}
    }
}

/* ------------------------------------------------------------------------- */
/*  find the biggest root of the bezier curve  bez, which represents a       */
/*  polynomial in a interval [a,b[ with 0 <= a < b                           */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_positive_open( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n-1,j = bez->n - 2,k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s*bez->a/( bez->a + 1.0 ) + t*bez->b/( bez->b + 1.0 );
    x = r/( 1.0 - r );

    /* -------------------------------------- */
    /*  Do not subdivide if x == a or x == b  */
    /* -------------------------------------- */

    if( x - bez->a < epsilon || bez->b - x < epsilon )
    {
        root[(*number_of_roots)++] = x;

        return  TRUE;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_positive_open( bez,root,number_of_roots ) )
        {
            return  TRUE;
        }
        else
        {
            return  root_of_bezier_positive(
                        &bez_left,root,number_of_roots );
	}
    }
}

/* ------------------------------------------------------------------------- */
/*  find all roots of the bezier curve  bez, which represents a polynomial   */
/*  in an interval [a,b] with 0 <= a < b                                     */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_positive_multi( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n, j = bez->n - 1, k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s*bez->a/( bez->a + 1.0 ) + t*bez->b/( bez->b + 1.0 );
    x = r/( 1.0 - r );

    /* -------------------------------------- */
    /*  Do not subdivide if x == a or x == b  */
    /* -------------------------------------- */

    if( x + epsilon > bez->b )
    {
        root[(*number_of_roots)++] = x;

        root_of_bezier_positive_multi_open( bez,root,number_of_roots );

        return  TRUE;
    }
    else if( x - epsilon < bez->a )
    {
        root[(*number_of_roots)++] = x;

        return  LEFT_BORDER;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_positive_multi(
                bez,root,number_of_roots ) == LEFT_BORDER )
	{
            root_of_bezier_positive_multi_open(
                &bez_left,root,number_of_roots );
	}
        else
        {
            root_of_bezier_positive_multi(
                &bez_left,root,number_of_roots );
	}  

        return  ( (*number_of_roots) > 0 );
    }
}
/* ------------------------------------------------------------------------- */
/*  find all roots of the bezier curve  bez, which represents a polynomial   */
/*  in an interval [a,b[ with 0 <= a < b                                     */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_positive_multi_open( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n - 1, j = bez->n - 2, k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s*bez->a/( bez->a + 1.0 ) + t*bez->b/( bez->b + 1.0 );
    x = r/( 1.0 - r );

    /* --------------------------- */
    /*  Do not subdivide if x == a */
    /* --------------------------- */

    if( x - epsilon < bez->a )
    {
        root[(*number_of_roots)++] = x;

        return  LEFT_BORDER;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_positive_multi_open(
                bez,root,number_of_roots ) == LEFT_BORDER )
	{
            root_of_bezier_positive_multi_open(
                    &bez_left,root,number_of_roots );
	}
        else
        {
            root_of_bezier_positive_multi(
                    &bez_left,root,number_of_roots );
	}
        return  ( (*number_of_roots) > 0 );
    }
}

/* ------------------------------------------------------------------------- */
/*  find the biggest root of the bezier curve  bez, which represents a       */
/*  polynomial in a interval [a,b] with a < b <= 0                           */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_negative( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n,j = bez->n - 1,k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s/( 1.0 - bez->a ) + t/( 1.0 - bez->b );
    x = ( r - 1.0 )/r;

    /* -------------------------------------- */
    /*  Do not subdivide if x == a or x == b  */
    /* -------------------------------------- */

    if( x - bez->a < epsilon || bez->b - x < epsilon )
    {
        root[(*number_of_roots)++] = x;

        return  TRUE;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_negative( bez,root,number_of_roots ) )
        {
            return  TRUE;
        }
        else
        {
            return  root_of_bezier_negative(
                        &bez_left,root,number_of_roots );
	}
    }
}

/* ------------------------------------------------------------------------- */
/*  find the biggest root of the bezier curve  bez, which represents a       */
/*  polynomial in a interval [a,b[ with a < b <= 0                           */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_negative_open( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n-1,j = bez->n - 2,k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s/( 1.0 - bez->a ) + t/( 1.0 - bez->b );
    x = ( r - 1.0 )/r;

    /* -------------------------------------- */
    /*  Do not subdivide if x == a or x == b  */
    /* -------------------------------------- */

    if( x - bez->a < epsilon || bez->b - x < epsilon )
    {
        root[(*number_of_roots)++] = x;

        return  TRUE;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_negative_open( bez,root,number_of_roots ) )
        {
            return  TRUE;
        }
        else
        {
            return  root_of_bezier_negative(
                        &bez_left,root,number_of_roots );
	}
    }
}

/* ------------------------------------------------------------------------- */
/*  find all roots of the bezier curve  bez, which represents a polynomial   */
/*  in an interval [a,b] with a < b <= 0                                     */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_negative_multi( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n, j = bez->n - 1,k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s/( 1.0 - bez->a ) + t/( 1.0 - bez->b );
    x = ( r - 1.0 )/r;

    /* -------------------------------------- */
    /*  Do not subdivide if x == a or x == b  */
    /* -------------------------------------- */

    if( x + epsilon > bez->b )
    {
        root[(*number_of_roots)++] = x;

        root_of_bezier_negative_multi_open( bez,root,number_of_roots );

        return  TRUE;
    }
    else if( x - epsilon < bez->a )
    {
        root[(*number_of_roots)++] = x;

        return  LEFT_BORDER;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_negative_multi(
                bez,root,number_of_roots ) == LEFT_BORDER )
	{
            root_of_bezier_negative_multi_open(
                    &bez_left,root,number_of_roots );
	}
        else
	{
            root_of_bezier_negative_multi(
                    &bez_left,root,number_of_roots );
	}
        return  ( (*number_of_roots) > 0 );
    }
}

/* ------------------------------------------------------------------------- */
/*  find all roots of the bezier curve  bez, which represents a polynomial   */
/*  in an interval [a,b[ with a < b <= 0 subject to the condition that       */
/*  the right interval border b is a root                                    */
/* ------------------------------------------------------------------------- */

static  int     root_of_bezier_negative_multi_open( bezier *bez,double *root,
                int *number_of_roots )
{
    int     i = bez->n - 1, j = bez->n - 2,k;
    int     not_found = TRUE;

    double  diff,alpha,delta,r,s,t,x;

    /* ------------------------------- */
    /*  Find first crossing of x-axis  */
    /* ------------------------------- */

    while( i > 0 && ( not_found = ( bez->p[i]*bez->p[j] > 0.0 ) ) )
    {
        i = j;
        j--;
    }

    /* ------------------------ */
    /*  No crossing => no root  */
    /* ------------------------ */

    if( not_found )
    {
        return  FALSE;
    }

    delta = 1.0/bez->n;
    diff  = bez->p[j] - bez->p[i];

    alpha = fabs( bez->p[j]*delta/diff );
    t = j*delta + alpha;
    s = 1.0 - t;

    r = s/( 1.0 - bez->a ) + t/( 1.0 - bez->b );
    x = ( r - 1.0 )/r;

    /* ---------------------------- */
    /*  Do not subdivide if x == a  */
    /* ---------------------------- */

    if( x - epsilon < bez->a )
    {
        root[(*number_of_roots)++] = x;

        return  LEFT_BORDER;
    }
    else
    {
        /* ---------------- */
        /*  subdivide at x  */
        /* ---------------- */

        double  left[MAX_DEGREE+1];
        bezier  bez_left;

        bez_left.p = left;
        bez_left.n = bez->n;
        bez_left.a = bez->a;
        bez_left.b = x;

        bez->a = x;

        /* ---------------------- */
        /*  perform de Casteljau  */
        /* ---------------------- */

        DIVIDE_TO_RIGHT_BOTH( *bez,bez_left,s,t,i,j,k );

        /* --------------------------------------------- */
        /*  investigate first right hand side, if fails  */
        /*  left hand side                               */
        /* --------------------------------------------- */

        if( root_of_bezier_negative_multi_open(
                bez,root,number_of_roots ) == LEFT_BORDER )
	{
            root_of_bezier_negative_multi_open(
                    &bez_left,root,number_of_roots );
	}
        else
	{
            root_of_bezier_negative_multi(
                    &bez_left,root,number_of_roots );
	}
        return  ( (*number_of_roots) > 0 );
    }
}

/* ------------------------------------------------------------------------- */
/* end of file: bezierarith.c                                                */
/* ------------------------------------------------------------------------- */











