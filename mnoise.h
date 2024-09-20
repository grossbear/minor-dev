//////////////////////////////////////////////////////////////////////////////
// mnoise.h
//
// Pseudo random number generation functions.
// Including perlin noise.
//////////////////////////////////////////////////////////////////////////////

#ifndef __NOISE_H__
#define __NOISE_H__


// Functions declarations

//////////////////////////////////////////////////////////////////////////////
// Pseudo random number generation functions
float mprng1(int x);
float mprng2(int x, int y);
float mprng3(int x, int y, int z);
float mprng4(int x, int y, int z, int w);

// Perlin noise
void  minit_noise_data(unsigned int seed);
float mnoise1(float x);
float mnoise2(float vec[2]);
float mnoise3(float vec[3]);
float mnoise4(float vec[4]);

#endif //__NOISE_H__

// Functions definitions

#include <math.h>
//////////////////////////////////////////////////////////////////////////////
// Pseudo random number generation functions
float mprng1(int x)
{
    int n = (x << 13) ^ x;
    float val = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) &
                0x7fffffff) / 1073741824.0f);

    return val;
}

//////////////////////////////////////////////////////////////////////////////
float mprng2(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;

    float val = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) &
                0x7fffffff) / 1073741824.0f);

    return val;
}

//////////////////////////////////////////////////////////////////////////////
float mprng3(int x, int y, int z)
{
    int n = x + y * 57 + z * 131;
    n = (n << 13) ^ n;

    float val = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) &
                0x7fffffff) / 1073741824.0f);

    return val;
}

//////////////////////////////////////////////////////////////////////////////
float mprng4(int x, int y, int z, int w)
{
    int n = x + y * 57 + z * 131 + w * 323;
    n = (n << 13) ^ n;

    float val = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) &
                0x7fffffff) / 1073741824.0f);

    return val;
}

//////////////////////////////////////////////////////////////////////////////

// Perlin noise implementation


// Two operations must be equal (value % NOISE_WRAP_INDEX) ==
//                              (value & NOISE_MOD_MASK)
// So NOISE_WRAP_INDEX must be a power of two and NOISE_MOD_MASK
// Must be a power of 2-1. If we implement indeces as unsigned chars,
// So NOISE_WRAP_INDEX should be less than or equal to 256.
// There's no good reason to change it from 256.

#define NOISE_WRAP_INDEX    256
#define NOISE_MOD_MASK      255

// A large power of two 2, we'll go for 4096, to add to negative numbers
// in order to make them positive
#define NOISE_LARGE_PWR2 4096

unsigned g_permutation_table [ NOISE_WRAP_INDEX * 2 + 2 ];
float g_gradient_table1 [ NOISE_WRAP_INDEX * 2 + 2 ];
float g_gradient_table2 [ NOISE_WRAP_INDEX * 2 + 2 ][2];
float g_gradient_table3 [ NOISE_WRAP_INDEX * 2 + 2 ][3];
float g_gradient_table4 [ NOISE_WRAP_INDEX * 2 + 2 ][4];


//////////////////////////////////////////////////////////////////////////////
void mnr_noise_normalize2(float vec[2])
{
    float len = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);

    vec[0] = vec[0] / len;
    vec[1] = vec[1] / len;
}

//////////////////////////////////////////////////////////////////////////////
void mnr_noise_normalize3(float vec[3])
{
    float len = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);

    vec[0] = vec[0] / len;
    vec[1] = vec[1] / len;
    vec[2] = vec[2] / len;
}

//////////////////////////////////////////////////////////////////////////////
void mnr_noise_normalize4(float vec[4])
{
    float len = sqrt(vec[0] * vec[0] + vec[1] * vec[1] +
            vec[2] * vec[2] + vec[3] * vec[3]);

    vec[0] = vec[0] / len;
    vec[1] = vec[1] / len;
    vec[2] = vec[2] / len;
    vec[3] = vec[3] / len;
}

//////////////////////////////////////////////////////////////////////////////
inline float mnr_noise_pack01(float value)
{
    return value * 0.5f + 0.5f;
}

//////////////////////////////////////////////////////////////////////////////
void mnr_generate_vectors(int rand_pos)
{
    int i,j;

    // Setting first part of gradient tables
    for(i = 0; i < NOISE_WRAP_INDEX; i++)
    {
        g_gradient_table1[i] = mprng1( rand_pos++ );

        for(j = 0; j < 2; j++)
            g_gradient_table2[i][j] = mprng1( rand_pos++ );
        mnr_noise_normalize2( g_gradient_table2[i] );

        for(j = 0; j < 3; j++)
            g_gradient_table3[i][j] = mprng1( rand_pos++ );
        mnr_noise_normalize3( g_gradient_table3[i] );

        for(j = 0; j < 4; j++)
            g_gradient_table4[i][j] = mprng1( rand_pos++ );
        mnr_noise_normalize4( g_gradient_table4[i] );
    }
}

//////////////////////////////////////////////////////////////////////////////
void mnr_duplicate_data_table()
{
    int i,j;

    // Copy data to the second part of gradient tables
    for(i = 0; i < NOISE_WRAP_INDEX + 2; i++)
    {
        g_gradient_table1[ NOISE_WRAP_INDEX + i ] = g_gradient_table1[i];

        for(j = 0; j < 2; j++)
            g_gradient_table2[ NOISE_WRAP_INDEX + i ][j] =
                g_gradient_table2[i][j];

        for(j = 0; j < 3; j++)
            g_gradient_table3[ NOISE_WRAP_INDEX + i ][j] =
                g_gradient_table3[i][j];

        for(j = 0; j < 4; j++)
            g_gradient_table4[ NOISE_WRAP_INDEX + i ][j] =
                g_gradient_table4[i][j];
    }
}

//////////////////////////////////////////////////////////////////////////////
void mnr_set_gradient_tables(int rand_pos)
{
    mnr_generate_vectors( rand_pos );
    mnr_duplicate_data_table();
}

//////////////////////////////////////////////////////////////////////////////
void mnr_table_shuffle(unsigned perm_tab[])
{
    int i,j;
    // Shuffle data in the table
    for(i = NOISE_WRAP_INDEX - 1; i >= 0; i--)
    {
        unsigned index = perm_tab[i];
        float value = mprng1( i );
        value = mnr_noise_pack01( value );
        j = (int) ( value * (float) NOISE_MOD_MASK );

        perm_tab[i] = perm_tab[j];
        perm_tab[j] = index;
    }
}

//////////////////////////////////////////////////////////////////////////////
void mnr_set_permutation_table(int rand_pos)
{
    int i;

    // Set initial values in table
    for(i = 0; i < NOISE_WRAP_INDEX; i++)
        g_permutation_table[i] = (unsigned) i;

    // Shuffle data in table
    mnr_table_shuffle( g_permutation_table );

    // Copy values to second part of table
    for(i = 0; i < NOISE_WRAP_INDEX + 2; i++)
        g_permutation_table[ NOISE_WRAP_INDEX + i ] =
        g_permutation_table[ i ];

}


//////////////////////////////////////////////////////////////////////////////
inline float mnr_noise_scurve(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

//////////////////////////////////////////////////////////////////////////////
inline float mnr_noise_lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Perlin noise generation functions

//////////////////////////////////////////////////////////////////////////////
// Initialize noise data tables
void minit_noise_data(unsigned int seed)
{
    seed = seed % 0xffff;
    int rand_pos = (int) seed;

    mnr_set_gradient_tables( rand_pos );
    mnr_set_permutation_table( rand_pos );
}


//////////////////////////////////////////////////////////////////////////////
float mnoise1(float x)
{
    float t = x + NOISE_LARGE_PWR2;
    int bx0 = ((int) t) & NOISE_MOD_MASK;
    int bx1 = (bx0 + 1) & NOISE_MOD_MASK;

    float rx0 = t - (int)t;
    float rx1 = rx0 - 1.0f;

    float sx = mnr_noise_scurve(rx0);

    unsigned pm_idx0 = g_permutation_table[ bx0 ];
    unsigned pm_idx1 = g_permutation_table[ bx1 ];

    float grad_val0 = g_gradient_table1[ pm_idx0 ];
    float grad_val1 = g_gradient_table1[ pm_idx1 ];
    grad_val0 = rx0 * grad_val0;
    grad_val1 = rx1 * grad_val1;

    float value = mnr_noise_lerp(sx, grad_val0, grad_val1);

    return value;
}

//////////////////////////////////////////////////////////////////////////////
float mnoise2(float vec[2])
{
    return 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
float mnoise3(float vec[3])
{
    return 0.0f;
}

//////////////////////////////////////////////////////////////////////////////
float mnoise4(float vec[4])
{
    return 0.0f;

}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

