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





/* ------------------------------------------------------------------------- */
/* monomarith.c: monomial arithmetic                                         */
/* ------------------------------------------------------------------------- */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mymemory.h"
#include "simple.h"
#include "monomarith.h"

/*****************************************************************************/
/* Global data                                                               */
/*****************************************************************************/

/* monxy   NULLMONXY = */
/* { */
/*     0.0, */
/*     0, */
/*     0 */
/* }; */

/* monxy   ONEMONXY = */
/* { */
/*     1.0, */
/*     0, */
/*     0 */
/* }; */

monxyz  NULLMONXYZ =
{
    0.0,
    0,
    0,
    0
};

monxyz  ONEMONXYZ =
{
    1.0,
    0,
    0,
    0
};

/* ------------------------------------------------------------------------- */
/* allocate memory for  n  monomials and initialize with the nullmonomial    */
/* ------------------------------------------------------------------------- */

monxyz  *new_monxyz( int n )
{
    int     i;
    monxyz  *m;

    if( n < 1 )
    {
        fprintf( stderr,"can't allocate %d monxyz\n",n );
	return 0;
    }

    m = (monxyz*)malloc( n*sizeof(monxyz) );

    if( m == (monxyz*)NULL )
    {
        fprintf( stderr,"can't allocate %d monxyz\n",n );
        exit( 1 );
    }

    for( i = 0; i < n; i++ )
    {
        m[i] = NULLMONXYZ;
    }

    return  m;
}

/* ------------------------------------------------------------------------- */
/* reallocate memory for  n  monomials                                       */
/* ------------------------------------------------------------------------- */

monxyz *renew_monxyz( monxyz *m,int n )
{
    if( m == (monxyz*)NULL )
    {
        return  new_monxyz( n );
    }
    else
    {   
        if( n < 1 )
        {
            fprintf( stderr,"can't reallocate %d monxyz\n",n );
            exit( 1 );
        }

        m = (monxyz*)realloc( (void*)m,n*sizeof(monxyz) );

        if( m == (monxyz*)NULL )
        {
            fprintf( stderr,"can't reallocate %d monxyz\n",n );
            exit( 1 );
        }

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* free memory of the monomials stored at  m                                 */
/* ------------------------------------------------------------------------- */

monxyz *delete_monxyz( monxyz *m )
{
    if( m != (monxyz*)NULL )
    {   
        free( (void*)m );
    }
    return  (monxyz*)NULL;
}

/* ------------------------------------------------------------------------- */
/* copy monomials                                                            */
/* ------------------------------------------------------------------------- */

void    copy_monxyz( monxyz *m1,monxyz *m2,int n )
{
    memcpy( (void*)m1,(void*)m2,n*sizeof(monxyz) );
}

/* ------------------------------------------------------------------------- */
/* monomial power comparison                                                 */
/* ------------------------------------------------------------------------- */

int     monxyz_power_equal( monxyz *m1,monxyz *m2 )
{
    return  m1->kx == m2->kx && m1->ky == m2->ky && m1->kz == m2->kz;
}

/* ------------------------------------------------------------------------- */
/* monomial comparison                                                       */
/* ------------------------------------------------------------------------- */

/* int     monxyz_equal( monxyz *m1,monxyz *m2 ) */
/* { */
/*     return  monxyz_power_equal( m1,m2 ) && double_equal( m1->a,m2->a ); */
/* } */

/* ------------------------------------------------------------------------- */
/* monomial output                                                           */
/* ------------------------------------------------------------------------- */

void    monxyz_print( monxyz *m )
{
    fprintf( stderr,"%.4f*x^%d*y^%d*z^%d",m->a,m->kx,m->ky,m->kz );
}

/* ========================================================================= */
/* arithmetics                                                               */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/* monomial addition: return = m1 + m2                                       */
/* ------------------------------------------------------------------------- */

/* monxyz   monxyz_add( monxyz m1,monxyz *m2 ) */
/* { */
/*     m1.a += m2->a; */

/*     return  m1; */
/* } */

/* ------------------------------------------------------------------------- */
/* monomial addition: m1 += m2                                               */
/* ------------------------------------------------------------------------- */

void    monxyz_add_self( monxyz *m1,monxyz *m2 )
{
    m1->a += m2->a;
}

/* ------------------------------------------------------------------------- */
/* monomial subtraction: return = m1 - m2                                    */
/* ------------------------------------------------------------------------- */

/* monxyz   monxyz_sub( monxyz m1,monxyz *m2 )  */
/* {  */
/*     m1.a -= m2->a; */

/*     return  m1; */
/* }  */

/* ------------------------------------------------------------------------- */
/* monomial subtraction: m1 -= m2                                            */
/* ------------------------------------------------------------------------- */

/* void    monxyz_sub_self( monxyz *m1,monxyz *m2 )  */
/* {  */
/*     m1->a -= m2->a; */
/* }  */

 
/* ------------------------------------------------------------------------- */
/* monomial multiplication: return = m1*m2                                   */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_mult( monxyz m1,monxyz *m2 ) 
{ 
    m1.a  *= m2->a;
    m1.kx += m2->kx;
    m1.ky += m2->ky;
    m1.kz += m2->kz;

    return  m1;
} 

/* ------------------------------------------------------------------------- */
/* monomial multiplication: m1 *= m2                                         */
/* ------------------------------------------------------------------------- */

void    monxyz_mult_self( monxyz *m1,monxyz *m2 ) 
{ 
    m1->a  *= m2->a;
    m1->kx += m2->kx;
    m1->ky += m2->ky;
    m1->kz += m2->kz;
} 
/* ------------------------------------------------------------------------- */
/* monomial scalar multiplication: return = m*mult                           */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_mult_double( monxyz m,double mult )
{
    m.a *= mult;

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial scalar multiplication: m *= mult                                 */
/* ------------------------------------------------------------------------- */

void    monxyz_mult_double_self( monxyz *m,double mult )
{
    m->a *= mult;
}

/* ------------------------------------------------------------------------- */
/* monomial scalar division: return = m/div                                  */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_div_double( monxyz m,double div )
{
    if( div == 0.0 )
    {
        fprintf( stderr,"can't divide by zero\n" );
        exit( 1 );
    }
    m.a /= div;

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial scalar division: m /= div                                        */
/* ------------------------------------------------------------------------- */

void    monxyz_div_double_self( monxyz *m,double div )
{
    if( div == 0.0 )
    {
        fprintf( stderr,"can't divide by zero\n" );
        exit( 1 );
    }
    m->a /= div;
}

/* ------------------------------------------------------------------------- */
/* monomial power: return = m^n                                              */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_power( monxyz m,int n )
{
    if( n > 0 )
    {
#ifdef SUN
        m.a  = sunpow( m.a,(double)n );
#else
        m.a  = pow( m.a,(double)n );
#endif /* SUN */
        m.kx *= n;
        m.ky *= n;
        m.kz *= n;
    }
    else if( n == 0 )
    {
        m.kx = m.ky = m.kz = 0;
        m.a  = 1.0;
    }
    else
    {
        fprintf( stderr,"can't take the %d-th power of a monomial\n",n );
        exit( 1 );
    }

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial power: m ^= n                                                    */
/* ------------------------------------------------------------------------- */

/* void    monxyz_power_self( monxyz *m,int n ) */
/* { */
/*     if( n > 0 ) */
/*     { */
/*         m->kx *= n; */
/*         m->ky *= n; */
/*         m->kz *= n; */
/*     } */
/*     else if( n == 0 ) */
/*     { */
/*         m->kx = m->ky = m->kz = 0; */
/*         m->a = 1.0; */
/*     } */
/*     else */
/*     { */
/*         fprintf( stderr,"can't take the %d-th power of a monomial\n",n ); */
/*         exit( 1 ); */
/*     } */
/* } */

/* ------------------------------------------------------------------------- */
/* monomial negation: return = -m                                            */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_neg( monxyz m )
{
    m.a = -m.a;

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial negation:  m = -m                                                */
/* ------------------------------------------------------------------------- */

void    monxyz_neg_self( monxyz *m )
{
    m->a = -m->a;
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to x: return = dx( m )                   */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dx( monxyz m )
{
    if( m.kx == 0 )
    {
        return  NULLMONXYZ;
    }
    else
    {
        m.a *= m.kx;
        m.kx--;

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to x: m = dx( m )                        */
/* ------------------------------------------------------------------------- */

void    monxyz_dx_self( monxyz *m )
{
    if( m->kx == 0 )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        m->a *= m->kx;
        m->kx--;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to x: return = d^nx( m )          */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dx_n( monxyz m,int n )
{
    if( m.kx == 0 || n > m.kx )
    {
        return  NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m.a *= m.kx;
            m.kx--;
            
	}

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to x: m = d^nx( m )               */
/* ------------------------------------------------------------------------- */

void    monxyz_dx_n_self( monxyz *m,int n )
{
    if( m->kx == 0 || n > m->kx )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m->a *= m->kx;
            m->kx--;
	}
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to y: return = dy( m )                   */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dy( monxyz m )
{
    if( m.ky == 0 )
    {
        return  NULLMONXYZ;
    }
    else
    {
        m.a *= m.ky;
        m.ky--;

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to y: m = dy( m )                        */
/* ------------------------------------------------------------------------- */

void    monxyz_dy_self( monxyz *m )
{
    if( m->ky == 0 )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        m->a *= m->ky;
        m->ky--;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to y: return = d^ny( m )          */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dy_n( monxyz m,int n )
{
    if( m.ky == 0 || n > m.ky )
    {
        return  NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m.a *= m.ky;
            m.ky--;
            
	}

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to y: m = d^ny( m )               */
/* ------------------------------------------------------------------------- */

void    monxyz_dy_n_self( monxyz *m,int n )
{
    if( m->ky == 0 || n > m->ky )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m->a *= m->ky;
            m->ky--;
	}
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to z: return = dz( m )                   */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dz( monxyz m )
{
    if( m.kz == 0 )
    {
        return  NULLMONXYZ;
    }
    else
    {
        m.a *= m.kz;
        m.kz--;

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to x: m = dz( m )                        */
/* ------------------------------------------------------------------------- */

void    monxyz_dz_self( monxyz *m )
{
    if( m->kz == 0 )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        m->a *= m->kz;
        m->kz--;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to z: return = d^nz( m )          */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dz_n( monxyz m,int n )
{
    if( m.kz == 0 || n > m.kz )
    {
        return  NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m.a *= m.kz;
            m.kz--;
            
	}

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to z: m = d^nz( m )               */
/* ------------------------------------------------------------------------- */

void    monxyz_dz_n_self( monxyz *m,int n )
{
    if( m->kz == 0 || n > m->kz )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m->a *= m->kz;
            m->kz--;
	}
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to w: return = dw( m )                   */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dw( monxyz m,int deg )
{
    int    kw = deg - m.kx - m.ky - m.kz;

    if( kw <= 0 )
    {
        return  NULLMONXYZ;
    }
    else
    {
        m.a *= kw;

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to w: m = dw( m )                        */
/* ------------------------------------------------------------------------- */

void    monxyz_dw_self( monxyz *m,int deg )
{
    int    kw = deg - m->kx -m->ky - m->kz;

    if( kw <= 0 )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        m->a *= kw;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to w: return = d^nw( m )          */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dw_n( monxyz m,int deg,int n )
{
    int    kw = deg - m.kx - m.ky - m.kz;

    if( kw <= 0 || kw < n )
    {
        return  NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m.a *= kw;
            kw--;
	}

        return  m;
    }
}

/* ------------------------------------------------------------------------- */
/* monomial n-fold derivative with respect to w: m = d^nw( m )               */
/* ------------------------------------------------------------------------- */

void    monxyz_dw_n_self( monxyz *m,int deg,int n )
{
    int    kw = deg - m->kx -m->ky - m->kz;

    if( kw <= 0 || kw < n )
    {
        *m = NULLMONXYZ;
    }
    else
    {
        int i;

        for( i=0; i<n; i++ )
	{
            m->a *= kw;
            kw--;
	}
    }
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to a monmial x^iy^jz^k:                  */
/* return = d^ix d^jy d^kz d^lw( m )                                         */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dvector( monxyz m,int deg_m,int i,int j,int k,int l )
{
    if( l > 0 ) monxyz_dw_n_self( &m,deg_m,l );
    if( i > 0 ) monxyz_dx_n_self( &m,i );    
    if( j > 0 ) monxyz_dy_n_self( &m,j );    
    if( k > 0 ) monxyz_dz_n_self( &m,k );    

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to a monmial x^iy^jz^k:                  */
/* m = d^ix d^jy d^kz d^lw( m )                                              */
/* ------------------------------------------------------------------------- */

void    monxyz_dvector_self( monxyz *m,int deg_m,int i,int j,int k,int l )
{
    if( l > 0 ) monxyz_dw_n_self( m,deg_m,l );
    if( i > 0 ) monxyz_dx_n_self( m,i );    
    if( j > 0 ) monxyz_dy_n_self( m,j );    
    if( k > 0 ) monxyz_dz_n_self( m,k );    
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to a monmial x^iy^jz^k:                  */
/* return = d^ix d^jy d^kz d^lw( m )                                         */
/* ------------------------------------------------------------------------- */

monxyz  monxyz_dmonxyz( monxyz m,int deg_m,monxyz *d,int deg_d )
{
    int d__kw = deg_d - d->kx -d->ky - d->kz;

    if( d__kw > 0 ) monxyz_dw_n_self( &m,deg_m,d__kw );
    if( d->kx > 0 ) monxyz_dx_n_self( &m,d->kx );    
    if( d->ky > 0 ) monxyz_dy_n_self( &m,d->ky );    
    if( d->kz > 0 ) monxyz_dz_n_self( &m,d->kz );    

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial derivative with respect to a monmial x^iy^jz^k:                  */
/* m = d^ix d^jy d^kz d^lw( m )                                              */
/* ------------------------------------------------------------------------- */

void    monxyz_dmonxyz_self( monxyz *m,int deg_m,monxyz *d,int deg_d )
{
    int d__kw = deg_d - d->kx -d->ky - d->kz;

    if( d__kw > 0 ) monxyz_dw_n_self( m,deg_m,d__kw );
    if( d->kx > 0 ) monxyz_dx_n_self( m,d->kx );    
    if( d->ky > 0 ) monxyz_dy_n_self( m,d->ky );    
    if( d->kz > 0 ) monxyz_dz_n_self( m,d->kz );    
}

/* ------------------------------------------------------------------------- */
/* monomial conversion: m = (monxyz)i                                        */
/* ------------------------------------------------------------------------- */

monxyz  int2monxyz( int i )
{
    monxyz   m = NULLMONXYZ;

    m.a = (double)i;

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial conversion: m = (monxyz)d                                        */
/* ------------------------------------------------------------------------- */

monxyz  double2monxyz( double d )
{
    monxyz   m = NULLMONXYZ;

    m.a = d;

    return  m;
}

/* ------------------------------------------------------------------------- */
/* monomial conversion: m = (monxyz)s                                        */
/* ------------------------------------------------------------------------- */

monxyz  atom( char *s )
{
    monxyz   m = ONEMONXYZ;

    switch( s[0] )
    {
        case 'x' : m.kx++; break;
        case 'X' : m.kx++; break;
        case 'y' : m.ky++; break;
        case 'Y' : m.ky++; break;
        case 'z' : m.kz++; break;
        case 'Z' : m.kz++; break;
    }

    return  m;
}

/* ------------------------------------------------------------------------- */
/* end of file: monomarith.c                                                 */
/* ------------------------------------------------------------------------- */
