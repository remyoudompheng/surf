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



/* ------------------------------------------------------------------------- */
/* polyroot.c: improved polynomial root finder                               */
/* Author:   Stephan Endrass                                                 */
/* Address:  endrass@mi.uni-erlangen.de                                      */
/* Date:     15 november 96                                                  */
/* ------------------------------------------------------------------------- */

#include <math.h>


#include <stdio.h>
#include "degree.h"
#include "mymemory.h"
#include "simple.h"
#include "monomarith.h"
#include "polyarith.h"
#include "polyx.h"
#include "polyroot.h"

/* ------------------------------------------------------------------------- */
/*  External C++ data                                                        */
/* ------------------------------------------------------------------------- */

extern  double numeric_epsilon_data;
extern  int    numeric_iterations_data;
extern  int    numeric_root_finder_data;

/* ------------------------------------------------------------------------- */
/*  The good old bisection method (rootfinder)                               */
/* ------------------------------------------------------------------------- */

static  int     polyx_bisection( polyx *f,double a,double b,
                double fa,double fb,double *root )
{
    double  ab = 0.0;  /*  midpoint of the interval [a,b]  */
    double  fab;       /*  f(ab)                           */
    int     i =  0;    /*  #iterations                     */

    /* ---------------------------------------------------------------- */
    /*  stop iteration process if one of the follwing conditions holds  */
    /*      a) b-a < numeric_epsilon_data                               */
    /*      b) #iterations >= numeric_iterations_data                   */
    /* ---------------------------------------------------------------- */
    while( b - a > numeric_epsilon_data &&
               i < numeric_iterations_data )
    {
        ab  = ( a + b )/2.0;
        fab = f->horner(ab); 

        if( fab == 0.0 )
	{
            *root = ab;
            return  TRUE;
        }

        if( fa < 0.0 )
        {
            if( fab > 0.0 )
            {
                b  = ab;
                fb = fab;
            }
            else
            {
                a  = ab;
                fa = fab;
            }
        }
        else
        {
            if( fab < 0.0 )
            {
                b  = ab;
                fb = fab;
            }
            else
            {
                a  = ab;
                fa = fab;
            }
        }
        i++;
    }
    *root = ab;
 
    return ( i < numeric_iterations_data );
}

/* ------------------------------------------------------------------------- */
/*  The regula falsi method (rootfinder)                                     */
/*  finds a root of f in an interval [a,b] with f(a)*f(b)<0                  */
/* ------------------------------------------------------------------------- */

static  int     polyx_regula_falsi( polyx *f,double a,double b,
            double fa,double fb,double *root )
{
    double  ab = 0.0;  /*  intesection of this secant with z-axis  */
    double  ab_eps;
    double  fab;       /*  f(ab)                                   */
    double  fab_eps;
    double  c;
    double  delta = b - a;

    int     i = 0;     /*  #iterations                             */

    /* ---------------------------------------------------------------- */
    /*  stop iteration process if one of the follwing conditions holds  */
    /*      a) |fab| < numeric_epsilon_data                             */
    /*      b) #iterations >= numeric_iterations_data                   */
    /* ---------------------------------------------------------------- */

    while( delta > numeric_epsilon_data &&
               i < numeric_iterations_data )
    {
        c   = fa/( fa - fb );
        ab  = ( c < 1.0 ? a + delta*c : ( a + b )/2.0 );
        fab = f->horner(ab);

        if( fa < 0.0 )
        {
            if( fab > 0.0 )
            {
                ab_eps  = ab - numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps <= 0.0 )
		{
                    *root = ( ab_eps + ab )/2.0;
                    return  TRUE;
		}
                
                b  = ab_eps;
                fb = fab_eps;
            }
            else /* fab <= 0.0 */
            {
                ab_eps  = ab + numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps >= 0.0 )
		{
                    *root = ( ab + ab_eps )/2.0;
                    return  TRUE;
		}
                
                a = ab_eps;
                fa = fab_eps;
            }
        }
        else /* fa > 0, fb < 0 */
        {   
            if( fab < 0.0 )
            {
                ab_eps  = ab - numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps >= 0.0 )
		{
                    *root = ( ab_eps + ab )/2.0;
                    return  TRUE;
		}

                b = ab_eps;
                fb = fab_eps;
            }
            else /* fab >= 0.0 */
            {   
                ab_eps  = ab + numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps <= 0.0 )
		{
                    *root = ( ab + ab_eps )/2.0;
                    return  TRUE;
		}
                
                 a = ab_eps;
                 fa = fab_eps;
            }
        }

        delta = b - a;
        i++;
    }

    *root = ab;

    return  ( i < numeric_iterations_data );
}

/* ------------------------------------------------------------------------- */
/*  The pegasus method (rootfinder)                                          */
/*  finds a root of f in an interval [a,b] with f(a)*f(b)<0                  */
/* ------------------------------------------------------------------------- */

static  int     polyx_pegasus( polyx *f,double a,double b,
            double fa,double fb,double *root )
{
    double  ab = 0.0;  /*  intesection of this secant with z-axis  */
    double  ab_eps;
    double  fab;       /*  f(ab)                                   */
    double  fab_eps;
    double  c;
    double  delta = b - a;
    int     i = 0;     /*  #iterations                             */

    /* ---------------------------------------------------------------- */
    /*  stop iteration process if one of the follwing conditions holds  */
    /*      a) |b-a| < numeric_epsilon_data                             */
    /*      b) #iterations >= numeric_iterations_data                   */
    /* ---------------------------------------------------------------- */
    while( delta > numeric_epsilon_data &&
               i < numeric_iterations_data )
    {
        c   = fa/( fa - fb );
        ab  = ( c < 1.0 ? a + delta*c : ( a + b )/2.0 );
        fab = f->horner(ab);

        if( fa < 0.0 )
        {
            if( fab > 0.0 )
            {
                ab_eps  = ab - numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps <= 0.0 )
	        {
                    *root = ( ab_eps + ab )/2.0;
                    return  TRUE;
		}
               
                b  = ab_eps;
                fb = fab_eps;
            }
            else /* fab <= 0.0 */
            {   
                fb *= ( fab < fa ? ( 1.0 - fab/fa ) : 0.5 );

                ab_eps  = ab + numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps >= 0.0 )
	        {
                    *root = ( ab + ab_eps )/2.0;
                    return  TRUE;
		}
               
                a  = ab_eps;
                fa = fab_eps;
            }
        }    
        else /* fa > 0.0 */
        {
            if( fab < 0.0 )
            {
                ab_eps  = ab - numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps >= 0.0 )
	        {
                    *root = ( ab_eps + ab )/2.0;
                    return  TRUE;
		}
               
                b = ab;
                fb = fab;
            }
            else /* fab >= 0.0 */
            {
                fb *= ( fab < fa ? ( 1.0 - fab/fa ) : 0.5 );

                ab_eps  = ab + numeric_epsilon_data;
                fab_eps = f->horner(ab_eps);

                if( fab_eps <= 0.0 )
	        {
                    *root = ( ab + ab_eps )/2.0;
                    return  TRUE;
		}
               
                a  = ab_eps;
                fa = fab_eps;
            }
        }
        delta = b - a;
        i++;
    }
    *root = ab;

    return  ( i < numeric_iterations_data );
}

/* ------------------------------------------------------------------------- */
/*  The anderson-bjoerck method (rootfinder)                                 */
/*  finds a root of f in an interval [a,b] with f(a)*f(b)<0                  */
/* ------------------------------------------------------------------------- */

static  int     polyx_anderson_bjoerck( polyx *f,double a,double b,
            double fa,double fb,double *root )
{
    double  h1,h2,h3;
    double  alpha;
    double  beta;
    double  gamma;
    double  sign;
    double  ab = ( a+ b )/2.0;
    double  fab = f->horner(ab);
    double  t = 0.0;
    int     i = 0;

    while( b - a > numeric_epsilon_data &&
               i < numeric_iterations_data )
    {
        h1 = fa*( b - ab );
        h2 = fab*( b - a );
        h3 = fb*( ab - a );

        alpha =  2*( h1 - h2 + h3 );

        if( fabs( alpha ) > 0.0 )
	{
            beta  = -h1*( ab + b ) + h2*( a + b ) - h3*( a + ab );
            gamma =  h1*ab*b       - h2*a*b       + h3*a*ab;

            sign = ( fa*fab*alpha <= 0.0 ? -1.0 : 1.0 );

            t = ( -beta + sign*sqrt( beta*beta-alpha*gamma ))/alpha;
	}
        else
	{
            t = ( fa*fab <= 0.0 ? a + ab : ab + b )/2.0;
        }

        if( t < ab )
	{
            b  = ab;
            fb = fab;
        }
        else
	{
            a  = ab;
            fa = fab;
	}

        ab  = t;
        fab = f->horner(t);

        i++;
    }
    *root = t;

    return  ( i < numeric_iterations_data );
}

/* ------------------------------------------------------------------------- */
/*  The Nweton method (rootfinder)                                           */
/*  finds a root of f in an interval [a,b] with f(a)*f(b)<0                  */
/*  Works only if  f  is convex in [a,b]                                     */
/* ------------------------------------------------------------------------- */

static  int     polyx_newton_right( polyx *f,double b,
            double fb,double dfb,double *root )
{
    double  fb_old = fb;
    int     i = 0;

    while( fb*fb_old > 0.0 &&
                   i < numeric_iterations_data )
    {
        if( dfb == 0.0 )
	{
            *root = b;
            return  TRUE;
	}

        b     -= fb/dfb + numeric_epsilon_data;
        fb_old = fb;
        fb     = f->horner(b);
        dfb    = f->dx_horner(b);
        i++;
    }
    *root = b;

    return  ( i < numeric_iterations_data );
}

static  int     polyx_newton_left( polyx *f,double a,
                    double fa,double dfa,double *root )
{
    double  fa_old = fa;
    int     i = 0;

    while( fa*fa_old > 0.0 &&
                   i < numeric_iterations_data )
    {
        if( dfa == 0.0 )
	{
            *root = a;
            return  TRUE;
	}

        a     -= fa/dfa - numeric_epsilon_data;
        fa_old = fa;
        fa     = f->horner(a);
        dfa    = f->dx_horner(a);
        i++;
    }
    *root = a;

    return  ( i < numeric_iterations_data );
}

static  int     polyx_newton( polyx *f,double a,double b,
            double fa,double fb,double *root )
{
    double  dfa = f->dx_horner (a);
    double  dfb = f->dx_horner (b);

    if( fabs( dfb ) > fabs( dfa ) )
    {
        return  polyx_newton_right( f,b,fb,dfb,root );
    }
    else
    {
        return  polyx_newton_left( f,a,fa,dfa,root );
    }
}

/* ------------------------------------------------------------------------- */
/*  array of methods                                                         */
/* ------------------------------------------------------------------------- */

int    (*polyx_inclusion_method[5])( polyx*,double,double,
               double,double,double* ) =
{
    polyx_bisection,
    polyx_regula_falsi,
    polyx_pegasus,
    polyx_anderson_bjoerck,
    polyx_newton
};

/* ------------------------------------------------------------------------- */
/*  The nifty short all roots finder                                         */
/*      Finds all roots of  f  in the interval [a,b] together with           */
/*      multiplicities: we use the fact that  f  is strictly monotonous      */
/*      in the intervals between the roots of  f' . To copmpute all roots    */
/*      of  f'  the same algotithm is used and so on. Computing a root in    */
/*      an interval where  f  is monotonous is done by the well-known        */
/*      pegasus-method.  The advantage of this method is that                */
/*                                                                           */
/*          a) we get as a side result the multiplicities of the roots and   */
/*          b) multiple roots are handled correctly                          */
/* ------------------------------------------------------------------------- */

int     Polyx_all_roots( polyx *f,double a,double b,
                double *roots,int *mults )
{
    switch( f->n )
    {
        /* ----------------- */
        /*  zero polynomial  */
        /* ----------------- */
        case -1:

            roots[0] = b;
            mults[0]  = 1;
            return  1;

        /* --------------------- */
        /*  constant polynomial  */
        /* --------------------- */
        case 0:

            return 0;

        /* ------------------- */
        /*  linear polynomial  */
        /* ------------------- */
        case 1:
        {
            double  r = -f->a[0]/f->a[1];

            if( r > b || r < a )
	    {
                return  0;
            }
            else
	    {
                roots[0] = r;
                mults[0] = 1;
                return  1;
            }
        }

        /* --------------------------- */
        /*  polynomial of degree >= 2  */
        /* --------------------------- */
        default:
        {
            int     i,j;
            int     dnum;                 /* number of roots of  f'         */
            int     intervals;            /* number of monotonous intervals */
            int     nvals;                /* number of interval borders     */
            int     num = 0;              /* number of roots                */

            double  droots[MAX_DEGREE+2]; /* roots of f'                    */
            int     dmults[MAX_DEGREE+2]; /* multiplicities of roots of f'  */
            double  fvaluea,fvalueb;

            double  da[MAX_DEGREE];       /* f'                             */
            polyx   df;

            /* ------------ */
            /*  compute f'  */
            /* ------------ */
            df.a = da;				// start of HACK...See ChangeLog entry 1999-02-16
            f->fastDerive (df);

            /* --------------------- */        
            /*  compute roots of f'  */
            /* --------------------- */        
            dnum = Polyx_all_roots( &df,a,b,&(droots[1]),&(dmults[1]));

            /* ---------------------- */
            /*  initialize intervals  */
            /* ---------------------- */    
            intervals = dnum + 1;
            nvals     = intervals + 1;

            droots[0] = b;
            droots[intervals] = a;

            /* ---------------------- */
            /*  invesigate intervals  */
            /* ---------------------- */
            fvalueb = f->horner(b);

            for( i = 0, j = 1; i < intervals; i++,j++ )
            {
		fvaluea = f->horner ( droots[j] );

                /* ---------------------------------------- */
                /*  either right interval border is a root  */
                /* ---------------------------------------- */
                if( fabs( fvalueb ) < numeric_epsilon_data )
	        {
                    roots[num] = droots[i];
                    mults[num] = dmults[i] + 1;
                    num++;
                }
                else if( fabs( fvaluea ) > numeric_epsilon_data )
                {
		    /* ------------------------------------- */
		    /*  or the open interval conains a root  */
		    /* ------------------------------------- */
                    if( fvaluea*fvalueb < 0.0 )
	            {
                        if( polyx_inclusion_method
                                [numeric_root_finder_data]
                                ( f,droots[j],droots[i],
                                fvaluea,fvalueb,&(roots[num])))
			{
                            mults[num] = 1;
                            num++;
			}
	            }
                }
                /* ----------------------------------- */
                /*  or left interval border is a root  */
                /* ----------------------------------- */
                else if( j == intervals )
	        {
                    roots[num] = a;
                    mults[num] = dmults[intervals] + 1;
                    num++;
                }
                fvalueb = fvaluea;
            }
	    df.a = 0;			// end of HACK...See ChangeLog entry 1999-02-16
            return  num;
        }
    }
}

/* ------------------------------------------------------------------------- */
/*  The nifty short all roots finder                                         */
/*      Finds all roots and extremal values of  f  in the interval [a,b]     */
/*      with multiplicities: we use the fact that  f  is strictly monotonous */
/*      and convex in the intervals between the roots of  f' and  f''. To    */
/*      copmpute all roots of  f'  the same algotithm is used and so on.     */
/*      Computing a root in an interval where  f  is monotonous and convex   */
/*      is done by well-known methods such as bisection, regula falsi,       */
/*      pegasus, anderson-bjoerck or newton.  The advantage of this method   */
/*      is that                                                              */
/*                                                                           */
/*          a) we get as a side result the multiplicities of the roots and   */
/*          b) multiple roots are handled correctly                          */
/*                                                                           */
/*      well, if the precision of double if satisfying ...                   */
/*      If combined with newton or bisection, it works up to degree 50 !     */
/* ------------------------------------------------------------------------- */

int     polyx_all_extreme( polyx *f,double a,double b,
                           double *roots,int *flags )
{
    switch( f->n )
    {
        /* ----------------- */
        /*  zero polynomial  */
        /* ----------------- */
        case -1:

            roots[0] = b;
            flags[0]  = 1;
            return  TRUE;

        /* --------------------- */
        /*  constant polynomial  */
        /* --------------------- */
        case 0:

            return FALSE;

        /* ------------------- */
        /*  linear polynomial  */
        /* ------------------- */
        case 1:
        {
            double  r = -f->a[0]/f->a[1];

            if( r < a || r > b )
	    {
                return  FALSE;
            }
            else
	    {
                roots[0] = r;
                flags[0] = 1;
                return  TRUE;
            }
        }

        /* --------------------------- */
        /*  polynomial of degree >= 2  */
        /* --------------------------- */
        default:
        {
            int     i,j;
            int     dnum;                 /* number of roots of  f'         */
            int     intervals;            /* number of monotonous intervals */
            int     nvals;                /* number of interval borders     */
            int     num = 0;              /* number of roots                */

            double  droots[2*MAX_DEGREE+2]; /* roots of f'                  */
            int     dflags[2*MAX_DEGREE+2]; /* multiplicities of roots of f'*/
            double  fvaluea,fvalueb;

            double  da[MAX_DEGREE];       /* f'                             */
            polyx   df;

            /* ------------ */
            /*  compute f'  */
            /* ------------ */
            df.a = da;					// start of HACK...See ChangeLog entry 1999-02-16
            f->fastDerive ( df );

            /* --------------------- */        
            /*  compute roots of f'  */
            /* --------------------- */        
            dnum = polyx_all_extreme( &df,a,b,&(droots[1]),&(dflags[1]));

            /* ---------------------- */
            /*  initialize intervals  */
            /* ---------------------- */    
            intervals = dnum + 1;
            nvals     = intervals + 1;

            droots[0] = b;
            dflags[0] = 0;
            droots[intervals] = a;
            dflags[intervals] = 0;

            /* ---------------------- */
            /*  invesigate intervals  */
            /* ---------------------- */
            fvalueb = f->horner(b);

            for( i = 0, j = 1; i < intervals; i++,j++ )
            {
	        fvaluea = f->horner(droots[j] );

                /* ---------------------------------------- */
                /*  either right interval border is a root  */
                /* ---------------------------------------- */
                if( fabs( fvalueb ) < numeric_epsilon_data )
	        {
                    roots[num] = droots[i];
                    flags[num] = dflags[i] + 1;
                    num++;
                }
                else
		{
                    if( dflags[i] > 0 )
		    {
                        roots[num] = droots[i];
                        flags[num] = 0;
                        num++;
		    }

                    if( fabs( fvaluea ) > numeric_epsilon_data )
                    {
	    	        /* ------------------------------------- */
		        /*  or the open interval conains a root  */
		        /* ------------------------------------- */
                        if( fvaluea*fvalueb < 0.0 )
	                {
                            if( polyx_inclusion_method
                                [numeric_root_finder_data]
                                ( f,droots[j],droots[i],
                                fvaluea,fvalueb,&(roots[num])))
			    {
                                flags[num] = 1;
                                num++;
			    }
	                }
                    }
                    /* ----------------------------------- */
                    /*  or left interval border is a root  */
                    /* ----------------------------------- */
                    else if( j == intervals )
	            {
                        roots[num] = a;
                        flags[num] = dflags[intervals] + 1;
                        num++;
                    }
		}
                fvalueb = fvaluea;
            }
	    
	    df.a = 0;					// end of HACK...See ChangeLog entry 1999-02-16 
	    
            return  num;
        }
    }
}

/* ------------------------------------------------------------------------- */
/*  Just a simple call of the function which calculetes all extremal values  */
/* ------------------------------------------------------------------------- */

int     polyx_all_roots( polyx *f,double a,double b,
                           double *roots,int *flags )
{
    double  hroots[2*MAX_DEGREE];
    int     hflags[2*MAX_DEGREE];

    int     i;
    int     num = 0;
    int     hnum = polyx_all_extreme( f,a,b,hroots,hflags );

    for( i = 0; i < hnum; i++ )
    {
        if( hflags[i] > 0 )
	{
            roots[num] = hroots[i];
            flags[num] = hroots[i];
            num++;
	}
    }
    return  num;
}

/* ------------------------------------------------------------------------- */
/* end of file: polyroot.c                                                   */
/* ------------------------------------------------------------------------- */
