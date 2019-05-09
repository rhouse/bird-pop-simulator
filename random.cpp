/*
    random.cpp  --  Open source random number generator

	Note that this is pure C99 code, but the source file name uses
	suffix cpp for convenience.
*/

/* 
   This is part of parsim, a C++ program for simulating animal populations
   Coded by Roger House.

   Copyright (C) 2003-2007 by Roger House.
   All rights reserved.

   NOTE:  This copyright only applies to the portions of this file written 
   by Roger House.  The rest of the file is copyright by Nishimura and 
   Matsumoto as noted in their copyright notice below.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   email: roger @ rogerfhouse.com (remove space)
*/


/***********************  BEGIN OPEN SOURCE CODE  ************************/
/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/


#include <math.h>

#include "random.h"

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

/*
    2004 Nov 09:  The original code has been modified by Roger House so 
    that all the random number generation functions take a randomInfo 
    pointer as a parameter, so that multiple, independent random streams 
    can be generated. 
*/

struct randomInfo
{
    bool            initialized;
    unsigned long   seed;
    bool            gotOneNormalValue;
    double          randomNormalValue;
    int             mti;
    unsigned long   mt[N];    
};
typedef struct randomInfo randomInfo;


static void init_genrand(randomInfo *r)    /* initializes mt[0:N-1] using a seed */
{
    int i;

    r->mt[0] = r->seed & 0xffffffffUL;

    for (i = 1;  i < N;  i++)
    {
        r->mt[i] = (1812433253UL * (r->mt[i-1] ^ (r->mt[i-1] >> 30)) + i); 

        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */

        r->mt[i] &= 0xffffffffUL;       /* for >32 bit machines */
    }

    r->mti = N;
    r->initialized       = true;
    r->gotOneNormalValue = false;
    r->randomNormalValue = 0;

}   /* end init_genrand */


static unsigned long genrand_int32(randomInfo *r)  /* gen random in [0, 2^32-1] */
{
    static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */
    unsigned long y;

    if (!r->initialized)
        init_genrand(r);

    if (r->mti >= N)        /* generate N words at one time */
    {
        int kk;

        for (kk = 0;  kk < N-M;  kk++)
        {
            y = (r->mt[kk] & UPPER_MASK) | (r->mt[kk+1] & LOWER_MASK);
            r->mt[kk] = r->mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        for (;  kk < N-1;  kk++)
        {
            y = (r->mt[kk] & UPPER_MASK) | (r->mt[kk+1] & LOWER_MASK);
            r->mt[kk] = r->mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        y = (r->mt[N-1] & UPPER_MASK) | (r->mt[0] & LOWER_MASK);
        r->mt[N-1] = r->mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        r->mti = 0;
    }
  
    y = r->mt[r->mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;

}   /* end genrand_int32 */


static double genrand_res53(randomInfo *r)
/*
    Return a random double in [0,1) with 53-bit resolution.
    This real version is due to Isaku Wada, 2002/01/09 added.
*/
{ 
    unsigned long a = genrand_int32(r) >> 5;
    unsigned long b = genrand_int32(r) >> 6; 

    return (a*67108864.0 + b) * (1.0/9007199254740992.0); 
} 

/************************  END OPEN SOURCE CODE  *************************/


static double genrand_normal(randomInfo *r)
/*
    This function was added by Roger House.

    genrand_normal generates a normally distributed random number with zero 
    mean and unit standard deviation.  The algorithm is gasdev, taken from 
    p217 of "Numerical Recipes in C" by W.H. Press, B.P. Flannery, 
    S.A. Teukolsky, and W.T. Vetterling, Cambridge University Press, 1988.
    The algorithm has been modified to use genrand_res53 as a source of
    uniform random numbers.
*/
{
    double fac, R, v1, v2;

    if (!r->initialized)
        init_genrand(r);

    if (r->gotOneNormalValue)           /* If a random number is left over */
    {                                   /*   from the last call, return it */ 
        r->gotOneNormalValue = false;
        return r->randomNormalValue;
    }

    do
    {
        v1 = 2 * genrand_res53(r) - 1;  /* Pick v1 and v2 as uniform numbers */
        v2 = 2 * genrand_res53(r) - 1;  /*   in a square extending from -1   */
        R = v1*v1 + v2*v2;              /*   to +1 in each direction         */
                                        /* If the point (v1, v2) is not in   */
    } while (R >= 1 || R < 1e-30);      /*   the unit circle, try again      */

    fac = sqrt(-2*log(R)/R);            /* Make the Box-Muller transformation*/
                                        /*   to get two normal numbers       */
    r->gotOneNormalValue = true;        /* Keep one for next call, and return*/
    r->randomNormalValue = v1 * fac;    /*   one                             */

    return v2 * fac;

}   /* end genrand_normal */


static double RandomNormal ( double mean, double stddev, randomInfo *r )
{
    return  stddev * genrand_normal(r) + mean ;
}


static randomInfo randInfo = { false, 5489UL };


/*
	NOTE that only the next two functions are visible outside this
	file; all other functions are static.
*/


double genNormalRandomNumber ( double mean, double stddev )
{
	return RandomNormal ( mean, stddev, &randInfo ) ;
}



double genUniformRandomNumberInUnitInterval ( void )
//
// Return a random double in [0,1) with 53-bit resolution.
//
{
	return genrand_res53 ( &randInfo ) ;
}

/* end random.cpp */
