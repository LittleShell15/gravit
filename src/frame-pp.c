/*

This file is part of
Gravit - A gravity simulator
Copyright 2003-2005 Gerald Kaszuba
Copyright 2012-2014 Frank Moehle

Gravit is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Gravit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gravit; if not, write to the Free Software
Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#include "gravit.h"

#ifdef _OPENMP
// experimental OMP support
#include <omp.h> // VC has to include this header to build the correct manifest to find vcom.dll or vcompd.dll
#endif

/* ************************************************************************** */
/* How to allocate memory with 16byte alignment?                              */
/* ************************************************************************** */
#ifdef WIN32

    // Windows: use _aligned_malloc
    #include <stdlib.h>
    #include <malloc.h>

    #if defined(__MINGW32__) || defined(mingw32) || defined(MINGW)
        // MinGW
        #define MALLOC_ALIGNED(target, size, alignment) {target =  (float*) __mingw_aligned_malloc(size, alignment);}
        #define FREE_ALIGNED(target)                    {__mingw_aligned_free(target);}
    #else
        // Microsoft
        #define MALLOC_ALIGNED(target, size, alignment) {target =  (float*) _aligned_malloc(size, alignment);}
        #define FREE_ALIGNED(target)                    {_aligned_free(target);}
    #endif

#else

  #if defined(__INTEL_COMPILER) && defined(HAVE__MM_MALLOC)
    // intel icc - prefer _mm_malloc()
    #define MALLOC_ALIGNED(target, size, alignment) {target =  (float*) _mm_malloc(size, alignment);}
    #define FREE_ALIGNED(target)                    {_mm_free(target);}

  #else
    // linux, unix, MacOS:  use (posix_)memalign
    #include <stdlib.h>
    //#include <malloc.h>

    //#if defined(HAVE_MEMALIGN) && !defined(HAVE_WORKING_POSIX_MEMALIGN)
    #if defined(HAVE_MEMALIGN)
        #include <malloc.h>

        // use memalign -- seems this is the best choice for most unix/linux versions
        #define MALLOC_ALIGNED(target, size, alignment) {target =  (float*) memalign(alignment, size);}
        #define FREE_ALIGNED(target)                    {free(target);}

    #else

        // all others use posix_memalign
        #define MALLOC_ALIGNED(target, size, alignment) {if (posix_memalign( (void **) &(target), alignment, size) != 0) target=NULL;}
        #define FREE_ALIGNED(target)                    {free(target);}

    #endif
  #endif
#endif
/* ************************************************************************** */



// __restrict__ tell the compiler that two pointer will not point to the same location
// if your compiler complains, just remove __restrict__

#ifdef _MSC_VER
#define __restrict__ __restrict
//#define __restrict__ __declspec(restrict)
//#define __restrict__
#endif




typedef struct {
    float * __restrict__ x;
    float * __restrict__ y;
    float * __restrict__ z;
    float * __restrict__ mass;
} particle_vectors;

typedef struct {
    float * __restrict__ x;
    float * __restrict__ y;
    float * __restrict__ z;
} acc_vectors;




/*  Optimizations:
    ===============
    * before processing, copy particle data to vector-friendly arrays
    * delay multiplication with G
    * after processing, write back results
*/


#if !defined(USE_FIXED_PHYSICS) && !defined(USE_MODIFIED_PHYSICS)
#define MIN_STEP2 0.05
#else
#if defined(USE_MODIFIED_PHYSICS)
#define MIN_STEP2 0.25
#else
#define MIN_STEP2 0.75
#endif
#endif

HOT
static void do_processFramePP(particle_vectors pos, acc_vectors accel,
                              int start, int amount) {
    int i;
    //int particles_max = state.particleCount;

    // apply gravity to every specified velocity
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 256)
#endif
    for (i = start; i < amount; i++) {
        //VectorNew(p1_pos);
        float p1_pos_x;
        float p1_pos_y;
        float p1_pos_z;
        float p1_mass;

        //VectorNew(p1_vel);
        //VectorZero(p1_vel);
        float p1_accel_x = 0.0f;
        float p1_accel_y = 0.0f;
        float p1_accel_z = 0.0f;

        int j;

        p1_pos_x = pos.x[i];
        p1_pos_y = pos.y[i];
        p1_pos_z = pos.z[i];
        p1_mass   = pos.mass[i];


#ifdef __INTEL_COMPILER
    #pragma ivdep
    #pragma vector always
    #pragma vector aligned
	__assume_aligned(pos.x, 32);
	__assume_aligned(pos.y, 32);
	__assume_aligned(pos.z, 32);
	__assume_aligned(pos.mass, 32);
	__assume_aligned(accel.x, 32);
	__assume_aligned(accel.y, 32);
	__assume_aligned(accel.z, 32);
#endif
#ifdef __GNUC__
    #pragma GCC ivdep
#endif
        for (j = 0; j < i; j++) {
            //VectorNew(dv);
            float dv_x;
            float dv_y;
            float dv_z;
            float squareDistance;
            float force1;
            float force2;
            float invDistance;

            dv_x = pos.x[j] - p1_pos_x;
            dv_y = pos.y[j] - p1_pos_y;
            dv_z = pos.z[j] - p1_pos_z;

            // get distance^2 between the two
            squareDistance  = dv_x * dv_x;
            squareDistance += dv_y * dv_y;
            squareDistance += dv_z * dv_z;
            squareDistance += MIN_STEP2;

#if !defined(USE_FIXED_PHYSICS) && !defined(USE_MODIFIED_PHYSICS)
            invDistance = 1.0f / squareDistance;               // 1.0 / (Distance^2)
            force1 = p1_mass * pos.mass[j] * invDistance;
            force2 = force1;
#else
#if defined(USE_MODIFIED_PHYSICS)
            invDistance = 1.0f / squareDistance;               // 1.0 / (Distance^2)
#else
    //USE_FIXED_PHYSICS                                        // this is a shortcut to compute 1.0/(sqrt(sqDist) * sqDist)
            squareDistance *= squareDistance * squareDistance; //    distance ^6
            invDistance  = 1.0f / sqrtf(squareDistance);       //    inverse distance ^3/2
#endif
            force1 = pos.mass[j] * invDistance;
            force2 = p1_mass     * invDistance;
#endif

            // sum of accelerations for p1
            p1_accel_x += dv_x * force1;
            p1_accel_y += dv_y * force1;
            p1_accel_z += dv_z * force1;

            // add acceleration for p2 (with negative sign, as the direction is inverted)
            accel.x[j] -= dv_x * force2;
            accel.y[j] -= dv_y * force2;
            accel.z[j] -= dv_z * force2;

        }

        // write back buffered acceleration of p1
        accel.x[i] += p1_accel_x;
        accel.y[i] += p1_accel_y;
        accel.z[i] += p1_accel_z;

    }

}



void processFramePP(int start, int amount) {
    particle_vectors pos;
    acc_vectors accel;
    particle_t * __restrict__ framebase = state.particleHistory + state.particleCount*state.frame;
    particleDetail_t * __restrict__ framedetail = state.particleDetail;

    int i;
    int particles_max;
    particles_max = state.particleCount;


    // create arrays, aligned to 128 bytes (cache line size)

    MALLOC_ALIGNED( pos.x, sizeof(float)*(particles_max + 128), 128);
    MALLOC_ALIGNED( pos.y, sizeof(float)*(particles_max + 128), 128);
    MALLOC_ALIGNED( pos.z, sizeof(float)*(particles_max + 128), 128);
    MALLOC_ALIGNED( pos.mass, sizeof(float)*(particles_max + 128), 128);
    //memset(pos.x, 0, sizeof(float) * (particles_max + 128));
    //memset(pos.y, 0, sizeof(float) * (particles_max + 128));
    //memset(pos.z, 0, sizeof(float) * (particles_max + 128));
    //memset(pos.mass, 0, sizeof(float) * (particles_max + 128));
    MALLOC_ALIGNED( accel.x, sizeof(float)*(particles_max + 128), 128);
    MALLOC_ALIGNED( accel.y, sizeof(float)*(particles_max + 128), 128);
    MALLOC_ALIGNED( accel.z, sizeof(float)*(particles_max + 128), 128);
    memset(accel.x, 0, sizeof(float) * (particles_max + 128));
    memset(accel.y, 0, sizeof(float) * (particles_max + 128));
    memset(accel.z, 0, sizeof(float) * (particles_max + 128));


    // copy frame data to vector-friendly arrays
    for (i=0; i<particles_max; i++) {
        pos.x[i] = framebase[i].pos[0];
        pos.y[i] = framebase[i].pos[1];
        pos.z[i] = framebase[i].pos[2];
        pos.mass[i] = state.particleDetail[i].mass;
    }


    // calculate new accelerations
    do_processFramePP(pos, accel, start, amount);


    // write back results
    for (i=0; i<particles_max; i++) {
#if !defined(USE_FIXED_PHYSICS) && !defined(USE_MODIFIED_PHYSICS)
        framedetail[i].accel[0] += accel.x[i] * state.g;
        framedetail[i].accel[1] += accel.y[i] * state.g;
        framedetail[i].accel[2] += accel.z[i] * state.g;
#else
        framedetail[i].accel[0] += accel.x[i] * copysignf(state.g, pos.mass[i]);
        framedetail[i].accel[1] += accel.y[i] * copysignf(state.g, pos.mass[i]);
        framedetail[i].accel[2] += accel.z[i] * copysignf(state.g, pos.mass[i]);
#endif
    }


    // clean up
    FREE_ALIGNED(pos.x);
    FREE_ALIGNED(pos.y);
    FREE_ALIGNED(pos.z);
    FREE_ALIGNED(pos.mass);

    FREE_ALIGNED(accel.x);
    FREE_ALIGNED(accel.y);
    FREE_ALIGNED(accel.z);

}
