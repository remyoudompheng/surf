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

#include <polyarith.h>
#include <simple.h>
#include <mymemory.h>
#include <monomarith.h>

#include<cstdio>
#include<cstdlib>
#include<cmath>

/*****************************************************************************/
/* POLYNOMIALS IN X,Y AND Z                                                  */
/*****************************************************************************/


// Global data:

polyxyz NULLPOLYXYZ = { 0, 0, 0 };



namespace {

const double epsilon = 1e-5;

inline bool double_equal(double s, double t)
{
	return fabs(s - t) < epsilon;
}

}

/* ------------------------------------------------------------------------- */
/* allocate memory for  n  monomials in p                                    */
/* ------------------------------------------------------------------------- */

void    new_coeff_polyxyz( polyxyz *p,int n )
{
    p->m   = new_monxyz( n );

    p->n   = n;
    p->deg = DEG_UNSPEC;
}

/* ------------------------------------------------------------------------- */
/* reallocate memory for  n  monomials in p                                  */
/* ------------------------------------------------------------------------- */

void    renew_coeff_polyxyz( polyxyz *p,int n )
{
    p->m = renew_monxyz( p->m,n );

    p->n = n;
}

/* ------------------------------------------------------------------------- */
/* free memory of the monomials in p                                         */
/* ------------------------------------------------------------------------- */

void    delete_coeff_polyxyz( polyxyz *p )
{
    (void)delete_monxyz( p->m );

    *p = NULLPOLYXYZ;
}

/* ------------------------------------------------------------------------- */
/* free memory of the monomials in p with control                            */
/* ------------------------------------------------------------------------- */

void    delete_coeff_polyxyz_control( polyxyz *p )
{
    if( p->n > 0 )
    {
        delete_coeff_polyxyz( p );
    }
}


/* ------------------------------------------------------------------------- */
/* copy constructor                                                          */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_copy( polyxyz *p )
{
    polyxyz r = NULLPOLYXYZ;

    new_coeff_polyxyz( &r,p->n );
    copy_monxyz( r.m,p->m,p->n );
    r.deg = p->deg;

    return  r;
}
    
/* ------------------------------------------------------------------------- */
/* collect identical monomials in  p  and forget zero monomials              */
/* ------------------------------------------------------------------------- */

void    polyxyz_collect( polyxyz *p )
{
    int    last = p->n - 1;
    int    i;
    int    j;

    /* ----------- */
    /* collect ... */
    /* ----------- */

    for( i = p->n - 1; i > 0; i-- )
    {
        j = i - 1;

        /* ------------------------------- */
        /* is there any similar monomial ? */
        /* ------------------------------- */

        while( j >= 0 && !monxyz_power_equal( &p->m[i],&p->m[j] ) )
        {
            j--;
        }
 
        if( j >= 0 )
        {
            /* ----------------- */
            /* yes, there is ... */
            /* ----------------- */

            monxyz_add_self( &p->m[j],&p->m[i] );
    
            if( last > i )
            {
                p->m[i] = p->m[last];
            }

            last--;
        }
    }

    /* ------------------------- */
    /* forget zero monomials ... */
    /* ------------------------- */

    i = 0;

    while( i <= last )
    {
        if( p->m[i].a == 0.0 )
        {
            if( i < last )
            {
                p->m[i] = p->m[last];
            }

            last--;
        }
        else
        {
            i++;
        }
    }

    if( last < 0 )
    {
        last = 0;
        renew_coeff_polyxyz( p,1 );
        p->m[0] = NULLMONXYZ;
    }
    else
    {
        renew_coeff_polyxyz( p,last+1 );
    }
}

/* ------------------------------------------------------------------------- */
/* compare monomials for qsort using lexicographical ordering [z,x,y]        */
/*     1. power of z                                                         */
/*     2. power of x                                                         */
/*     3. power of y                                                         */
/* ------------------------------------------------------------------------- */

int     compare_monxyz( const void *m1,const void *m2 )
{
    monxyz *mon1, *mon2;

    mon1 = (monxyz*)m1;
    mon2 = (monxyz*)m2;

    if( mon2->kz == mon1->kz )
    {
        if( mon2->kx == mon1->kx )
        {
            return  mon2->ky - mon1->ky;
        }
        else
        {
            return  mon2->kx - mon1->kx;
        }
    }
    else
    {
        return  mon2->kz - mon1->kz;
    } 
    
}

/* ------------------------------------------------------------------------- */
/* sort polyxyz with qsort                                                   */
/* ------------------------------------------------------------------------- */

void    polyxyz_sort( polyxyz *p )
{
    if( p->m != (monxyz*)NULL )
    {
        qsort( (void*)p->m,(size_t)p->n,sizeof(monxyz),compare_monxyz );
    }
}

/* ------------------------------------------------------------------------- */
/* polynomial computation of the maximum coefficient                         */
/* ------------------------------------------------------------------------- */

double  polyxyz_maxabs_coeff( polyxyz *p )
{
    int     i;
    double  abs_coeff    = 0.0;
    double  maxabs_coeff = 0.0;

    for( i = 0; i < p->n; i++ )
    {
        abs_coeff = fabs( p->m[i].a );

        if( abs_coeff > maxabs_coeff )
        {
            maxabs_coeff = abs_coeff;
        }
    }

    return  maxabs_coeff;
}

/* ------------------------------------------------------------------------- */
/* polynomial normalization such that maxabs_coeff = 10.0                    */
/* ------------------------------------------------------------------------- */

void    polyxyz_norm_self( polyxyz *p )
{
    double  a = polyxyz_maxabs_coeff( p );

    if( !double_equal( a,0.0 ) )
    {
        polyxyz_mult_double_self( p,10.0/a );
    }
}


/* ------------------------------------------------------------------------- */
/* polynomial degree calculation                                             */
/* ------------------------------------------------------------------------- */

void    polyxyz_set_degree( polyxyz *p )
{
    if( p->n <= 0 )
    {
        p->deg = DEG_UNSPEC;
    }
    else
    {
        int     i;
        int     i_deg;
        int     max_deg = -1;

        for( i = 0; i < p->n; i++ )
        {
            i_deg = p->m[i].kx + p->m[i].ky + p->m[i].kz;

            if( max_deg < i_deg )
            {
                max_deg = i_deg ;
            }
        }

        p->deg = max_deg;
    }
}

/* ------------------------------------------------------------------------- */
/* polynomial output to stdout                                               */
/* ------------------------------------------------------------------------- */

void    polyxyz_print( polyxyz *p )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        if( i > 0 )
        {
            if( p->m[i].a >= 0.0 )
            {
                fprintf( stderr," +" );
            }
            else
            {
                fprintf( stderr," " );
            }
        }
     
        monxyz_print( &p->m[i] );
    }
    fprintf( stderr,"\n" );
}

/* ------------------------------------------------------------------------- */
/* polynomial addition: return = p + q                                       */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_add( polyxyz *p,polyxyz *q )
{
    polyxyz r = NULLPOLYXYZ;

    if( p->n == 0 )
    {
        r = polyxyz_copy( q );
        return  r;
    }
    if( q->n == 0 )
    {
        r = polyxyz_copy( p );
        return  r;
    }

    new_coeff_polyxyz( &r,p->n + q->n );

    copy_monxyz( &r.m[0]   ,&p->m[0],p->n );
    copy_monxyz( &r.m[p->n],&q->m[0],q->n );

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial addition: return = p + q                                       */
/* assume that p and q are sorted by polyxyz_sort                            */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_add_sorted( polyxyz *p,polyxyz *q )
{
    polyxyz r = NULLPOLYXYZ;

    /* exclude trivial cases */

    if( p->n == 0 )
    {
        r = polyxyz_copy( q );
        return  r;
    }
    if( q->n == 0 )
    {
        r = polyxyz_copy( p );
        return  r;
    }

    int     n = p->n + q->n;
    int     ip = 0,
            iq = 0,
            ir = 0;
    int     tst;

    /* find number of monomials of p+q */

    while( ip < p->n || iq < q->n )
    {
        tst = compare_monxyz( &(p->m[ip]),&(q->m[iq]) );

        if( tst == 0 )        n--, ip++,ip++;
        else if( tst > 0 )    ip++;
	else                  iq++;
    }

    /* initialize monomials of p+q */

    new_coeff_polyxyz( &r,n );

    while( ip < p->n || iq < q->n )
    {
        tst = compare_monxyz( &(p->m[ip]),&(q->m[iq]) );

        if( tst == 0 )        r.m[ir] = p->m[ip], r.m[ir].a += q->m[ip].a,
                                                  ip++, iq++,ir++;
        else if( tst > 0 )    r.m[ir] = p->m[ip], ip++,      ir++;
	else                  r.m[ir] = q->m[ip], iq++,      ir++;
    }

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial addition: p += q                                               */
/* ------------------------------------------------------------------------- */

void    polyxyz_add_self( polyxyz *p,polyxyz *q )
{
    if( q->n <= 0 )
    {
        return;
    }

    int     n = p->n;

    renew_coeff_polyxyz( p,p->n + q->n );

    copy_monxyz( &p->m[n],&q->m[0],q->n );

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial subtraction: return = p - q                                    */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_sub( polyxyz *p,polyxyz *q )
{
    polyxyz  r;
    int      i;

    new_coeff_polyxyz( &r,p->n + q->n );

    copy_monxyz( &r.m[0]   ,p->m,p->n );
    copy_monxyz( &r.m[p->n],q->m,q->n );

    for( i = p->n; i < r.n; i++ )
    {
        monxyz_neg_self( &r.m[i] );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}


/* ------------------------------------------------------------------------- */
/* polynomial multiplication: return = p*q                                   */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_mult( polyxyz *p,polyxyz *q )
{
    int     i;
    int     j;
    int     k = 0;
    polyxyz r = NULLPOLYXYZ;

    new_coeff_polyxyz( &r,p->n*q->n );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j < q->n; j++ )
        {
            r.m[k] = monxyz_mult( p->m[i], &q->m[j] );
            k++;
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial multiplication: p *= q                                         */
/* ------------------------------------------------------------------------- */

void    polyxyz_mult_self( polyxyz *p,polyxyz *q )
{
    int     i;
    int     j;
    int     k = 0;
    int     n;
    monxyz  *m = (monxyz*)NULL;

    n = p->n*q->n;
    m = new_monxyz( n );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j < q->n; j++ )
        {
            m[k] = monxyz_mult( p->m[i],&q->m[j] );
            k++;
        }
    }

    delete_coeff_polyxyz( p );

    p->m = m;
    p->n = n;

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}


/* ------------------------------------------------------------------------- */
/* polynomial monom multiplication: p *= m                                   */
/* ------------------------------------------------------------------------- */

void    polyxyz_mult_monxyz_self( polyxyz *p,monxyz *m )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_mult_self( &p->m[i],m );
    }
}


/* ------------------------------------------------------------------------- */
/* polynomial scalar multiplication: p *= mult                               */
/* ------------------------------------------------------------------------- */

void    polyxyz_mult_double_self( polyxyz *p,double mult )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_mult_double_self( &p->m[i],mult );
    }
}

/* ------------------------------------------------------------------------- */
/* polynomial scalar division: p /= div                                      */
/* ------------------------------------------------------------------------- */

void    polyxyz_div_double_self( polyxyz *p,double div )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_div_double_self( &p->m[i],div );
    }
}

/* ------------------------------------------------------------------------- */
/* polynomial power: return = p^n                                            */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* here we have to do a sort of loop which C doesn't provide:                */
/*                                                                           */
/*                                                                           */
/*      -----      /               \                                         */
/*      \         /                 \                                        */
/*       \        |       s         |      i          i                      */
/*        >       |                 |       0          (n-1)                 */
/*       /        |  i , ... ,i     |     a  * ... * a                       */
/*      /         \   0       (n-1) /      0          (n-1)                  */
/*      -----      \               /                                         */
/* i + ... + i     = s                                                       */
/*  0         (n-1)                                                          */
/*                                                                           */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_power( polyxyz *p,int s )
{
    if( s < 0 )
    {
        fprintf( stderr,"can't calculate the %d-th power of a polynomial\n",s );
        exit( 1 );
    }
    else if( s == 0 )
    {
        polyxyz r = NULLPOLYXYZ;
        new_coeff_polyxyz( &r,1 );
        r.n = 1;
        r.deg = 0;
        r.m[0] = ONEMONXYZ;
        
        return  r;
    }
    else if( s == 1 )
    {
        polyxyz r = NULLPOLYXYZ;
        new_coeff_polyxyz( &r,p->n );
        copy_monxyz( &r.m[0],&p->m[0],p->n );
        r.deg = p->deg;

        return  r;
    }
    else if( p->n == 1 )
    {
        polyxyz r = NULLPOLYXYZ;
        new_coeff_polyxyz( &r,p->n );
        r.m[0] = monxyz_power( p->m[0],s );
        r.deg = p->deg*s;

        return  r;
    }
    else
    {
        int     i;
        int     j;
        int     k = 0;

        int     n = p->n;
        int     coeff_n = binom_coeff( n + s - 1,s );
        int     *index = new int [n]; //new_int( n );

        int     last = n - 1;
        int     bef  = n - 2;
        bool stop_flag = false;

        polyxyz r = NULLPOLYXYZ;
        monxyz  dummy;


        new_coeff_polyxyz( &r,coeff_n );

        /* ------------------------------------------- */
        /* start configuration of index is (0,...,0,s) */
        /* ------------------------------------------- */

        for( i = 0; i < last; i++ )
        {
            index[i] = 0;
        }
        index[last] = s;

        do
        {
            r.m[k] = monxyz_power( p->m[0],index[0] );

            for( i = 1; i < n; i++ )
            {
                dummy = monxyz_power( p->m[i],index[i] );
                monxyz_mult_self( &r.m[k],&dummy );
            }

            monxyz_mult_double_self( &r.m[k],multinom_coeff( s,index,n ) );

            k++;

            /* ---------------------- */
            /* compute the next index */
            /* ---------------------- */

#ifdef  DEBUG
            for(  i = 0; i < n; i++ )
            {
                fprintf( stderr,"%d ",index[i] );
            }

            fprintf( stderr,"\n" );
#endif  /* DEBUG */

            if( index[last] > 0 )
            {
                index[last]--;
                index[bef]++;
            }
            else
            {
                j = bef;

                while( index[j] == 0 )
                {
                    j--;
                }

                if( j <= 0 )
                {
			stop_flag = true;
                }
                else
                {
                   index[j-1]++;
                   index[last] = index[j] - 1;
                   index[j] = 0;
                }
            }
        } while( !stop_flag );

        /* ----------------------------------------- */
        /* end configuration of index is (n,0,...,0) */
        /* ----------------------------------------- */

        polyxyz_collect( &r );
        polyxyz_set_degree( &r );

        delete [] index; // delete_int( index );
	index = 0;
        return  r;
    }
}


/* ------------------------------------------------------------------------- */
/* polynomial negation: p = -p                                               */
/* ------------------------------------------------------------------------- */

void    polyxyz_neg_self( polyxyz *p )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_neg_self( &p->m[i] );
    }   
}


/* ------------------------------------------------------------------------- */
/* polynomial scaling: p = p( x*X,y*Y,z*Z )                                  */
/* ------------------------------------------------------------------------- */

void    polyxyz_scale_self( polyxyz *p,double x,double y,double z )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
#ifdef SUN
        monxyz_mult_double_self( &p->m[i],
				 sunpow( x,p->m[i].kx )*
				 sunpow( y,p->m[i].ky )*
				 sunpow( z,p->m[i].kz ) );
#else
        monxyz_mult_double_self( &p->m[i],
				 pow( x,p->m[i].kx )*
				 pow( y,p->m[i].ky )*
				 pow( z,p->m[i].kz ) );
#endif /* SUN */
    }
}

/* ------------------------------------------------------------------------- */
/* maximal number of coefficients of a polynomial of degree n                */
/* ------------------------------------------------------------------------- */

int     polyxyz_max_coeff( int n )
{
    return  ( n + 1 )*( n + 2 )*( n+ 3 )/6;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in direction of the X-axis: return = p( x-x0,y,z )       */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_shift_x( polyxyz *p,double x0 )
{
    polyxyz r;

    int     kx;
    int     ky;
    int     kz;
    int     i;
    int     j;
    int     k;
    int     c = 0;

    new_coeff_polyxyz( &r,polyxyz_max_coeff( p->deg ) );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j <= p->m[i].kx; j++ )
        {
            kx = j;
            ky = p->m[i].ky;
            kz = p->m[i].kz;

            k = 0;
            while( k < c && ( kx != r.m[k].kx || ky != r.m[k].ky ||
                                                 kz != r.m[k].kz ) )
            {
                k++;
            }

            if( k == c )
            {
                r.m[k].kx = kx;
                r.m[k].ky = ky;
                r.m[k].kz = kz;
                c++;
            }

            r.m[k].a += p->m[i].a*
#ifdef SUN
                        sunpow( -x0,p->m[i].kx - j )*
#else
                        pow( -x0,p->m[i].kx - j )*
#endif /* SUN*/
                        binom_coeff( p->m[i].kx,j );
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return r;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in direction of the X-axis: p = p( x-x0,y,z )            */
/* ------------------------------------------------------------------------- */

void    polyxyz_shift_x_self( polyxyz *p,double x0 )
{
    polyxyz  r;

    r = polyxyz_shift_x( p,x0 );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in direction of the Y-axis: return = p( x,y-y0,z )       */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_shift_y( polyxyz *p,double y0 )
{
    polyxyz r;

    int     kx;
    int     ky;
    int     kz;
    int     i;
    int     j;
    int     k;
    int     c = 0;

    new_coeff_polyxyz( &r,polyxyz_max_coeff( p->deg ) );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j <= p->m[i].ky; j++ )
        {
            kx = p->m[i].kx;
            ky = j;
            kz = p->m[i].kz;

            k = 0;
            while( k < c && ( kx != r.m[k].kx || ky != r.m[k].ky ||
                                                 kz != r.m[k].kz ) )
            {
                k++;
            }

            if( k == c )
            {
                r.m[k].kx = kx;
                r.m[k].ky = ky;
                r.m[k].kz = kz;
                c++;
            }

            r.m[k].a += p->m[i].a*
#ifdef SUN
                        sunpow( -y0,p->m[i].ky - j )*
#else
                        pow( -y0,p->m[i].ky - j )*
#endif /* SUN */
                        binom_coeff( p->m[i].ky,j );
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return r;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in direction of the Y-axis: p = p( x,y-y0,z )            */
/* ------------------------------------------------------------------------- */

void    polyxyz_shift_y_self( polyxyz *p,double y0 )
{
    polyxyz  r;

    r = polyxyz_shift_y( p,y0 );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in direction of the Z-axis: return = p( x,y,z-z0 )       */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_shift_z( polyxyz *p,double z0 )
{
    polyxyz r;

    int     kx;
    int     ky;
    int     kz;
    int     i;
    int     j;
    int     k;
    int     c = 0;

    new_coeff_polyxyz( &r,polyxyz_max_coeff( p->deg ) );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j <= p->m[i].kz; j++ )
        {
            kx = p->m[i].kx;
            ky = p->m[i].ky;
            kz = j;

            k = 0;
            while( k < c && ( kx != r.m[k].kx || ky != r.m[k].ky ||
                                                 kz != r.m[k].kz ) )
            {
                k++;
            }

            if( k == c )
            {
                r.m[k].kx = kx;
                r.m[k].ky = ky;
                r.m[k].kz = kz;
                c++;
            }

            r.m[k].a += p->m[i].a*
#ifdef SUN
                        sunpow( -z0,p->m[i].kz - j )*
#else
                        pow( -z0,p->m[i].kz - j )*
#endif /* SUN */
                        binom_coeff( p->m[i].kz,j );
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return r;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in direction of the Z-axis: p = p( x,y,z-z0 )            */
/* ------------------------------------------------------------------------- */

void    polyxyz_shift_z_self( polyxyz *p,double z0 )
{
    polyxyz  r;

    r = polyxyz_shift_z( p,z0 );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial shift in all directions: p = p( x-x0,y-y0,z-z0 )               */
/* ------------------------------------------------------------------------- */

void    polyxyz_shift_self( polyxyz *p,double x0,double y0,double z0 )
{
    polyxyz_shift_x_self( p,x0 );
    polyxyz_shift_y_self( p,y0 );
    polyxyz_shift_z_self( p,z0 );
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round the X-axis: return = rotx( p,alpha )            */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_rotate_x( polyxyz *p,double alpha )
{
    int     c = 0;
    int     i;
    int     j;
    int     k;
    int     l;
    int     kx;
    int     ky;
    int     kz;

    polyxyz r;

    double  sinalpha;
    double  cosalpha;

    new_coeff_polyxyz( &r,polyxyz_max_coeff( p->deg ) );

    sinalpha = sin( alpha );
    cosalpha = cos( alpha );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j <= p->m[i].ky; j++ )
        {
            for( k = 0; k <= p->m[i].kz; k++ )
            {
                kx = p->m[i].kx;
                ky = j + k;
                kz = p->m[i].ky - j + p->m[i].kz - k;

                l = 0;
                while( l < c && ( kx != r.m[l].kx || ky != r.m[l].ky ||
                                                     kz != r.m[l].kz ) )
                {
                    l++;
                }

                if( l == c )
                {
                    r.m[l].kx = kx;
                    r.m[l].ky = ky;
                    r.m[l].kz = kz;
                    c++;
                }

                r.m[l].a += p->m[i].a*
                        binom_coeff( p->m[i].ky,j )*
                        binom_coeff( p->m[i].kz,k )*
#ifdef SUN
                        sunpow( sinalpha,p->m[i].ky - j + k )*
                        sunpow( cosalpha,j + p->m[i].kz - k )*
                        sunpow(     -1.0,k                  );
#else
                        pow( sinalpha,p->m[i].ky - j + k )*
                        pow( cosalpha,j + p->m[i].kz - k )*
                        pow(     -1.0,k                  );
#endif /* SUN */
            }
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round the X-axis: p = rotx( p,alpha )                 */
/* ------------------------------------------------------------------------- */

void    polyxyz_rotate_x_self( polyxyz *p,double alpha )
{
    polyxyz  r;

    r = polyxyz_rotate_x( p,alpha );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round the Y-axis: return = roty( p,alpha )            */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_rotate_y( polyxyz *p,double alpha )
{
    int     c = 0;
    int     i;
    int     j;
    int     k;
    int     l;
    int     kx;
    int     ky;
    int     kz;

    polyxyz r;

    double  sinalpha;
    double  cosalpha;

    new_coeff_polyxyz( &r,polyxyz_max_coeff( p->deg ) );

    sinalpha = sin( alpha );
    cosalpha = cos( alpha );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j <= p->m[i].kx; j++ )
        {
            for( k = 0; k <= p->m[i].kz; k++ )
            {
                kx = j + k;
                ky = p->m[i].ky;
                kz = p->m[i].kx - j + p->m[i].kz - k;

                l = 0;
                while( l < c && ( kx != r.m[l].kx || ky != r.m[l].ky ||
                                                     kz != r.m[l].kz ) )
                {
                    l++;
                }

                if( l == c )
                {
                    r.m[l].kx = kx;
                    r.m[l].ky = ky;
                    r.m[l].kz = kz;
                    c++;
                }

                r.m[l].a += p->m[i].a*
                        binom_coeff( p->m[i].kx,j )*
                        binom_coeff( p->m[i].kz,k )*
#ifdef SUN
                        sunpow( sinalpha,p->m[i].kx - j + k )*
                        sunpow( cosalpha,j + p->m[i].kz - k )*
                        sunpow(     -1.0,k                  );
#else
                        pow( sinalpha,p->m[i].kx - j + k )*
                        pow( cosalpha,j + p->m[i].kz - k )*
                        pow(     -1.0,k                  );
#endif /* SUN */
            }
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round the Y-axis: p = roty( p,alpha )                 */
/* ------------------------------------------------------------------------- */

void    polyxyz_rotate_y_self( polyxyz *p,double alpha )
{
    polyxyz  r;

    r = polyxyz_rotate_y( p,alpha );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round the Z-axis: return = rotz( p,alpha )            */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_rotate_z( polyxyz *p,double alpha )
{
    int     c = 0;
    int     i;
    int     j;
    int     k;
    int     l;
    int     kx;
    int     ky;
    int     kz;

    polyxyz r;

    double  sinalpha;
    double  cosalpha;

    new_coeff_polyxyz( &r,polyxyz_max_coeff( p->deg ) );

    sinalpha = sin( alpha );
    cosalpha = cos( alpha );

    for( i = 0; i < p->n; i++ )
    {
        for( j = 0; j <= p->m[i].kx; j++ )
        {
            for( k = 0; k <= p->m[i].ky; k++ )
            {
                kx = j + k;
                ky = p->m[i].kx - j + p->m[i].ky - k;
                kz = p->m[i].kz;

                l = 0;
                while( l < c && ( kx != r.m[l].kx || ky != r.m[l].ky ||
                                                     kz != r.m[l].kz ) )
                {
                    l++;
                }

                if( l == c )
                {
                    r.m[l].kx = kx;
                    r.m[l].ky = ky;
                    r.m[l].kz = kz;
                    c++;
                }

                r.m[l].a += p->m[i].a*
                        binom_coeff( p->m[i].kx,j )*
                        binom_coeff( p->m[i].ky,k )*
#ifdef SUN
                        sunpow( sinalpha,p->m[i].kx - j + k )*
                        sunpow( cosalpha,j + p->m[i].ky - k )*
                        sunpow(     -1.0,k                  );
#else
                        pow( sinalpha,p->m[i].kx - j + k )*
                        pow( cosalpha,j + p->m[i].ky - k )*
                        pow(     -1.0,k                  );
#endif /* SUN */
            }
        }
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round the Z-axis: p = rotz( p,alpha )                 */
/* ------------------------------------------------------------------------- */

void    polyxyz_rotate_z_self( polyxyz *p,double alpha )
{
    polyxyz  r;

    r = polyxyz_rotate_z( p,alpha );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial rotation round all axes: p = rotz( rotx( roty( p,ay ),ax ),az )*/
/* ------------------------------------------------------------------------- */

void    polyxyz_rotate_self( polyxyz *p,double ax,double ay,double az )
{
    polyxyz_rotate_y_self( p,ay );
    polyxyz_rotate_x_self( p,ax );
    polyxyz_rotate_z_self( p,az );
}

/* ------------------------------------------------------------------------- */
/* polynomial conversion to perspective                                      */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_perspective( polyxyz *p,double z0 )
{
    int     i;
    int     j;
    int     k;
    int     n;
    int     new_deg;
    int     new_deg_max = 1;
    double  z1 = -1.0/z0;
    polyxyz r = NULLPOLYXYZ;

    for( i = 0; i < p->n; i++ )
    {
        new_deg = 2*p->m[i].kx + 2*p->m[i].ky + p->m[i].kz;

        if( new_deg_max < new_deg )
        {
            new_deg_max = new_deg;
        }
    }

    new_coeff_polyxyz( &r,polyxyz_max_coeff( new_deg_max ) );

    for( i = 0, k = 0; i < p->n; i++ )
    {
        n = p->m[i].kx + p->m[i].ky;

        for( j = 0; j <= n; j++ )
        {
            r.m[k].a = p->m[i].a*binom_coeff( n,j )*
#ifdef SUN
                       sunpow( z1,j );
#else
                       pow( z1,j );
#endif /* SUN */
            r.m[k].kx = p->m[i].kx;
            r.m[k].ky = p->m[i].ky;
            r.m[k].kz = p->m[i].kz + j;

            k++;
        }
    }
    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial conversion to perspective                                      */
/* ------------------------------------------------------------------------- */

void    polyxyz_perspective_self( polyxyz *p,double z0 )
{
    polyxyz r;

    r = polyxyz_perspective( p,z0 );

    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial hessian determinant (3x3):                                     */
/*                            | p_xx p_xy p_xw |                             */
/*                    return  | p_yx p_yy p_yw |                             */
/*                            | p_wx p_wy p_ww |                             */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_hessian_curve( polyxyz *p )
{
    polyxyz r1,r2;

    polyxyz dxx = polyxyz_dx_n( p,2 );
    polyxyz dxy = polyxyz_dvector( p,1,1,0,0 );
    polyxyz dxw = polyxyz_dvector( p,1,0,0,1 );
    polyxyz dyy = polyxyz_dy_n( p,2 );
    polyxyz dyw = polyxyz_dvector( p,0,1,0,1 );
    polyxyz dww = polyxyz_dw_n( p,2 );

    r1 = polyxyz_mult( &dxx,&dyy );
    polyxyz_mult_self( &r1,&dww );

    r2 = polyxyz_mult( &dxy,&dyw );
    polyxyz_mult_self( &r2,&dxw );
    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult( &dxw,&dxy );
    polyxyz_mult_self( &r2,&dyw );
    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult( &dxw,&dyy );
    polyxyz_mult_self( &r2,&dxw );
    polyxyz_mult_double_self( &r2,-1.0 );
    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult( &dyw,&dyw );
    polyxyz_mult_self( &r2,&dxx );
    polyxyz_mult_double_self( &r2,-1.0 );
    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult( &dww,&dxy );
    polyxyz_mult_self( &r2,&dxy );
    polyxyz_mult_double_self( &r2,-1.0 );
    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    return  r1;
}

void    polyxyz_hessian_curve_self( polyxyz *p )
{
    polyxyz r = polyxyz_hessian_curve( p );
    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial hessian determinant (4x4):                                     */
/*                            | p_xx p_xy p_xz p_xw |                        */
/*                    return  | p_yx p_yy p_yz p_yw |                        */
/*                            | p_zx p_zy p_zz p_zw |                        */
/*                            | p_wx p_wy p_wz p_ww |                        */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_hessian_surface( polyxyz *p )
{
    polyxyz p_x,p_y,p_z,p_w; 

    polyxyz p_xx,p_xy,p_xz,p_xw;
    polyxyz      p_yy,p_yz,p_yw;
    polyxyz           p_zz,p_zw;
    polyxyz                p_ww; 

    polyxyz r1,r2;

    int     deg_d = p->deg - 1;
    int     deg_dd = p->deg - 2;

    p_x = polyxyz_dx( p ); if( p_x.n > 0 ) p_x.deg = deg_d;
    p_y = polyxyz_dy( p ); if( p_y.n > 0 ) p_y.deg = deg_d;
    p_z = polyxyz_dz( p ); if( p_z.n > 0 ) p_z.deg = deg_d;
    p_w = polyxyz_dw( p ); if( p_w.n > 0 ) p_w.deg = deg_d;

    p_xx = polyxyz_dx( &p_x ); if( p_xx.n > 0 ) p_xx.deg = deg_dd;
    p_xy = polyxyz_dy( &p_x ); if( p_xy.n > 0 ) p_xy.deg = deg_dd;
    p_xz = polyxyz_dz( &p_x ); if( p_xz.n > 0 ) p_xz.deg = deg_dd;
    p_xw = polyxyz_dw( &p_x ); if( p_xw.n > 0 ) p_xw.deg = deg_dd;
    
    p_yy = polyxyz_dy( &p_y ); if( p_yy.n > 0 ) p_yy.deg = deg_dd;
    p_yz = polyxyz_dz( &p_y ); if( p_yz.n > 0 ) p_yz.deg = deg_dd;
    p_yw = polyxyz_dw( &p_y ); if( p_yw.n > 0 ) p_yw.deg = deg_dd;

    p_zz = polyxyz_dz( &p_z ); if( p_zz.n > 0 ) p_zz.deg = deg_dd;
    p_zw = polyxyz_dw( &p_z ); if( p_zw.n > 0 ) p_zw.deg = deg_dd;

    p_ww = polyxyz_dw( &p_w ); if( p_ww.n > 0 ) p_ww.deg = deg_dd;

    r1 = polyxyz_mult     ( &p_xx,&p_yy ); 	/* dxx*dyy*dzz*dww */
         polyxyz_mult_self( &r1,&p_zz );
         polyxyz_mult_self( &r1,&p_ww );

    r2 = polyxyz_mult     ( &p_xx,&p_yz );	/* 2*( dxx*dyz*dzw*dwy ) */
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_self( &r2,&p_yw );
         polyxyz_mult_double_self( &r2,2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_yy,&p_xz );	/* 2*( dyy*dxz*dzw*dwx ) */
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_self( &r2,&p_xw );
         polyxyz_mult_double_self( &r2,2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_zz,&p_xy );	/* 2*( dzz*dxy*dyw*dwx ) */
         polyxyz_mult_self( &r2,&p_yw );
         polyxyz_mult_self( &r2,&p_xw );
         polyxyz_mult_double_self( &r2,2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_ww,&p_xy );	/* 2*( dww*dxy*dyz*dzx ) */
         polyxyz_mult_self( &r2,&p_yz );
         polyxyz_mult_self( &r2,&p_xz );
         polyxyz_mult_double_self( &r2,2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xy,&p_yz );	/* -2*( dxy*dyz*dzw*dwx ) */
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_self( &r2,&p_xw );
         polyxyz_mult_double_self( &r2,-2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xy,&p_yw );	/* -2*( dxy*dyw*dwz*dzx ) */
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_self( &r2,&p_xz );
         polyxyz_mult_double_self( &r2,-2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xz,&p_yz );	/* -2*( dxz*dzy*dyw*dwx ) */
         polyxyz_mult_self( &r2,&p_yw );
         polyxyz_mult_self( &r2,&p_xw );
         polyxyz_mult_double_self( &r2,-2.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xx,&p_yy );	/* -( dxx*dyy*dzw^2 ) */
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_double_self( &r2,-1.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xx,&p_zz );	/* -( dxx*dzz*dyw^2 ) */
         polyxyz_mult_self( &r2,&p_yw );
         polyxyz_mult_self( &r2,&p_yw );
         polyxyz_mult_double_self( &r2,-1.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xx,&p_ww );	/* -( dxx*dww*dyz^2 ) */
         polyxyz_mult_self( &r2,&p_yz );
         polyxyz_mult_self( &r2,&p_yz );
         polyxyz_mult_double_self( &r2,-1.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_yy,&p_zz );	/* -( dyy*dzz*dxw^2 ) */
         polyxyz_mult_self( &r2,&p_xw );
         polyxyz_mult_self( &r2,&p_xw );
         polyxyz_mult_double_self( &r2,-1.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_yy,&p_ww );	/* -( dyy*dww*dxz^2 ) */
         polyxyz_mult_self( &r2,&p_xz );
         polyxyz_mult_self( &r2,&p_xz );
         polyxyz_mult_double_self( &r2,-1.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_zz,&p_ww );	/* -( dzz*dww*dxy^2 ) */
         polyxyz_mult_self( &r2,&p_xy );
         polyxyz_mult_self( &r2,&p_xy );
         polyxyz_mult_double_self( &r2,-1.0 );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xy,&p_xy );	/* dxy^2*dzw^2 */
         polyxyz_mult_self( &r2,&p_zw );
         polyxyz_mult_self( &r2,&p_zw );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xz,&p_xz );	/* dxz^2*dyw^2 */
         polyxyz_mult_self( &r2,&p_yw );
         polyxyz_mult_self( &r2,&p_yw );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    r2 = polyxyz_mult     ( &p_xw,&p_xw );	/* dxw^2*dyz^2 */
         polyxyz_mult_self( &r2,&p_yz );
         polyxyz_mult_self( &r2,&p_yz );

    polyxyz_add_self( &r1,&r2 );
    delete_coeff_polyxyz( &r2 );

    delete_coeff_polyxyz( &p_x );
    delete_coeff_polyxyz( &p_y );
    delete_coeff_polyxyz( &p_z );
    delete_coeff_polyxyz( &p_w );

    delete_coeff_polyxyz( &p_xx );
    delete_coeff_polyxyz( &p_xy );
    delete_coeff_polyxyz( &p_xz );
    delete_coeff_polyxyz( &p_xw );
    delete_coeff_polyxyz( &p_yy );
    delete_coeff_polyxyz( &p_yz );
    delete_coeff_polyxyz( &p_yw );
    delete_coeff_polyxyz( &p_zz );
    delete_coeff_polyxyz( &p_zw );
    delete_coeff_polyxyz( &p_ww );

    return  r1;
}

/* ------------------------------------------------------------------------- */
/* polynomial hessian determinant (4x4):                                     */
/*                            | p_xx p_xy p_xz p_xw |                        */
/*                     p =    | p_yx p_yy p_yz p_yw |                        */
/*                            | p_zx p_zy p_zz p_zw |                        */
/*                            | p_wx p_wy p_wz p_ww |                        */
/* ------------------------------------------------------------------------- */

void    polyxyz_hessian_surface_self( polyxyz *p )
{
    polyxyz r = polyxyz_hessian_surface( p );
    delete_coeff_polyxyz( p );
    *p = r;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to X: return = Dx(p)                   */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dx( polyxyz *p )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dx( p->m[i] );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to X: p = Dx(p)                        */
/* ------------------------------------------------------------------------- */

void    polyxyz_dx_self( polyxyz *p )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dx_self( &p->m[i] );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to X: return = D^nx(p)          */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dx_n( polyxyz *p,int n )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dx_n( p->m[i],n );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to X: p = D^nx(p)               */
/* ------------------------------------------------------------------------- */

void    polyxyz_dx_self( polyxyz *p,int n )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dx_n_self( &p->m[i],n );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to Y: return = Dy(p)                   */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dy( polyxyz *p )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dy( p->m[i] );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to Y: p = Dy(p)                        */
/* ------------------------------------------------------------------------- */

void    polyxyz_dy_self( polyxyz *p )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dy_self( &p->m[i] );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to Y: return = D^ny(p)          */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dy_n( polyxyz *p,int n )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dy_n( p->m[i],n );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to Y: p = D^ny(p)               */
/* ------------------------------------------------------------------------- */

void    polyxyz_dy_n_self( polyxyz *p,int n )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dy_n_self( &p->m[i],n );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to Z: return = Dz(p)                   */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dz( polyxyz *p )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dz( p->m[i] );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to Z: p = Dz(p)                        */
/* ------------------------------------------------------------------------- */

void    polyxyz_dz_self( polyxyz *p )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dz_self( &p->m[i] );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to Z: return = D^nz(p)          */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dz_n( polyxyz *p,int n )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dz_n( p->m[i],n );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to Z: p = D^nz(p)               */
/* ------------------------------------------------------------------------- */

void    polyxyz_dz_n_self( polyxyz *p,int n )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dz_n_self( &p->m[i],n );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to W: return = Dw(p)                   */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dw( polyxyz *p )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dw( p->m[i],p->deg );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to W: p = Dw(p)                        */
/* ------------------------------------------------------------------------- */

void    polyxyz_dw_self( polyxyz *p )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dw_self( &p->m[i],p->deg );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to W: return = D^nw(p)          */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dw_n( polyxyz *p,int n )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dw_n( p->m[i],p->deg,n );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
}

/* ------------------------------------------------------------------------- */
/* polynomial n-fold derivative with respect to W: p = D^nw(p)               */
/* ------------------------------------------------------------------------- */

void    polyxyz_dw_n_self( polyxyz *p,int n )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dw_n_self( &p->m[i],p->deg,n );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}


/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to a monmial x^iy^jz^k:                */
/* return = d^ix d^jy d^kz d^lw( p )                                         */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dvector( polyxyz *p,int i,int j,int k,int l )
{
    int     c;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( c = 0; c < p->n; c++ )
    {
        r.m[c] = monxyz_dvector( p->m[c],p->deg,i,j,k,l );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
    
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to a monmial x^iy^jz^k:                */
/* p = d^ix d^jy d^kz d^lw( p )                                              */
/* ------------------------------------------------------------------------- */

void    polyxyz_dvector_self( polyxyz *p,int i,int j,int k,int l )
{
    int     c;

    for( c = 0; c < p->n; c++ )
    {
        monxyz_dvector_self( &p->m[c],p->deg,i,j,k,l );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to a monmial x^iy^jz^k:                */
/* return = d^ix d^jy d^kz d^lw( p )                                         */
/* ------------------------------------------------------------------------- */

polyxyz polyxyz_dmonxyz( polyxyz *p,monxyz *m,int d )
{
    int     i;
    polyxyz r;

    new_coeff_polyxyz( &r,p->n );

    for( i = 0; i < p->n; i++ )
    {
        r.m[i] = monxyz_dmonxyz( p->m[i],p->deg,m,d );
    }

    polyxyz_collect( &r );
    polyxyz_set_degree( &r );

    return  r;
    
}

/* ------------------------------------------------------------------------- */
/* polynomial derivative with respect to a monmial x^iy^jz^k:                */
/* p = d^ix d^jy d^kz d^lw( p )                                              */
/* ------------------------------------------------------------------------- */

void    polyxyz_dmonxyz_self( polyxyz *p,monxyz *m,int d )
{
    int     i;

    for( i = 0; i < p->n; i++ )
    {
        monxyz_dmonxyz_self( &p->m[i],p->deg,m,d );
    }

    polyxyz_collect( p );
    polyxyz_set_degree( p );
}

/* ------------------------------------------------------------------------- */
/* polynomial conversion: p = (polyxyz)i                                     */
/* ------------------------------------------------------------------------- */

polyxyz int2polyxyz( int i )
{
    polyxyz p = NULLPOLYXYZ;

    new_coeff_polyxyz( &p,1 );

    p.m[0] = int2monxyz( i );
    p.deg  = 0;

    return  p;
}

/* ------------------------------------------------------------------------- */
/* polynomial conversion: p = (polyxyz)d                                     */
/* ------------------------------------------------------------------------- */

polyxyz double2polyxyz( double d )
{
    polyxyz p = NULLPOLYXYZ;

    new_coeff_polyxyz( &p,1 );

    p.m[0] = double2monxyz( d );
    p.deg  = 0;

    return  p;
}

/* ------------------------------------------------------------------------- */
/* polynomial conversion: p = (polyxyz)m                                     */
/* ------------------------------------------------------------------------- */

polyxyz monxyz2polyxyz( monxyz *m )
{
    polyxyz p = NULLPOLYXYZ;

    new_coeff_polyxyz( &p,1 );

    p.m[0] = *m;
    p.deg  = m->kx + m->ky + m->kz;

    return  p;
}

/* ------------------------------------------------------------------------- */
/* end of file: polyarith.c                                                  */
/* ------------------------------------------------------------------------- */
