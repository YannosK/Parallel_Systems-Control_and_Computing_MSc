#include "files.h"
#include "timer.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SOFTENING 1e-9f
#define DEBUG

/*
 * Each body contains x, y, and z coordinate positions,
 * as well as velocities in the x, y, and z directions.
 */
typedef struct {
    float x, y, z, vx, vy, vz;
} Body;

/*
 * Kernel to calculate the gravitational impact
 * of all bodies in the system on all others.
 *
 * @param p pointer to a buffer that holds all the bodies
 * @param dt minimum time differential
 * @param n the total number of bodies
 */
__global__ void bodyForce(Body *p, float dt, int n) {
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    int stride = blockDim.x * gridDim.x;

    for(int i = index; i < n; i += stride) {
        float Fx = 0.0f;
        float Fy = 0.0f;
        float Fz = 0.0f;

        for(int j = 0; j < n; j++) {
            float dx = p[j].x - p[i].x;
            float dy = p[j].y - p[i].y;
            float dz = p[j].z - p[i].z;
            float distSqr = dx * dx + dy * dy + dz * dz + SOFTENING;
            float invDist = rsqrtf(distSqr);
            float invDist3 = invDist * invDist * invDist;

            Fx += dx * invDist3;
            Fy += dy * invDist3;
            Fz += dz * invDist3;
        }

        p[i].vx += dt * Fx;
        p[i].vy += dt * Fy;
        p[i].vz += dt * Fz;
    }
}

/**
 * Kernel to integrate positions of bodies.
 * This should be done after `bodyForce`.
 *
 * @param p pointer to a buffer that holds all the bodies
 * @param dt minimum time differential
 * @param n the total number of bodies
 */
__global__ void integratePosition(Body *p, float dt, int n) {
    size_t index = threadIdx.x + blockIdx.x * blockDim.x;
    size_t stride = blockDim.x * gridDim.x;

    for(size_t i = index; i < n; i += stride) {
        p[i].x += p[i].vx * dt;
        p[i].y += p[i].vy * dt;
        p[i].z += p[i].vz * dt;
    }
}

int main(const int argc, const char **argv) {
    /**------------------------------------------------------------------------------------
     * Command line arguments
     ------------------------------------------------------------------------------------*/

    // The assessment will test against both 2<11 and 2<15.
    // Feel free to pass the command line argument 15 when you generate ./nbody
    // report files
    int nBodies = 2 << 11;
    if(argc > 1)
        nBodies = 2 << atoi(argv[1]);

#ifdef DEBUG
    printf("\nProblem size: %d\n", nBodies);
#endif

    /**------------------------------------------------------------------------------------
     * Provided assesments - DO NOT MODIFY
     ------------------------------------------------------------------------------------*/

    // The assessment will pass hidden initialized values to check for
    // correctness. You should not make changes to these files, or else the
    // assessment will not work.
    const char *initialized_values;
    const char *solution_values;

    if(nBodies == 2 << 11) {
        initialized_values = "09-nbody/files/initialized_4096";
        solution_values = "09-nbody/files/solution_4096";
    } else { // nBodies == 2<<15
        initialized_values = "09-nbody/files/initialized_65536";
        solution_values = "09-nbody/files/solution_65536";
    }

    if(argc > 2)
        initialized_values = argv[2];
    if(argc > 3)
        solution_values = argv[3];

    /**------------------------------------------------------------------------------------
     * GPU parameters update
     ------------------------------------------------------------------------------------*/

    cudaDeviceProp props;

    int deviceId;
    int warpSize;
    int maxThreadsPerBlock;
    size_t threadsPerBlock;
    size_t numberOfBlocks;

    cudaGetDevice(&deviceId);
    cudaGetDeviceProperties(&props, deviceId);

    warpSize = props.warpSize;
    maxThreadsPerBlock = props.maxThreadsPerBlock;

#ifdef DEBUG
    printf("Warp size: %d\n", warpSize);
    printf("Max threads per block: %d\n", maxThreadsPerBlock);
#endif

    threadsPerBlock = 4 * warpSize;
    if(threadsPerBlock > maxThreadsPerBlock)
        threadsPerBlock /= 4;

    if(nBodies % threadsPerBlock != 0)
        numberOfBlocks = (nBodies / threadsPerBlock) + 1;
    else
        numberOfBlocks = (nBodies / threadsPerBlock);

#ifdef DEBUG
    printf("\nNumber of blocks: %lu\n", numberOfBlocks);
    printf("Threads per block: %lu\n", threadsPerBlock);
#endif

    /**------------------------------------------------------------------------------------
     * Parameters initialization
     ------------------------------------------------------------------------------------*/

    cudaError_t allocErr, bodyForceErr, integrationErr, asyncErr, fetchErr;

    const float dt = 0.01f; // Time step
    const int nIters = 10;  // Simulation iterations

    int bytes = nBodies * sizeof(Body); // total size of all the bodie in memory
    float *buf; // buffer that holds all the data bodies in memory - element
                // scope access

    allocErr = cudaMallocManaged(&buf, bytes);
    if(allocErr != cudaSuccess) {
        printf(
            "Error in allocation of the bodies buffer: %s\n",
            cudaGetErrorString(allocErr)
        );
        return 1;
    }
    Body *p = (Body *)buf; // buffer that holds all the bodies in memory - body
                           // struct scope access

    // initialization of starting positions and velocities
    read_values_from_file(initialized_values, buf, bytes);

    fetchErr = cudaMemPrefetchAsync(p, bytes, deviceId);
    if(fetchErr != cudaSuccess) {
        printf(
            "Error in unififed memory pre-fetching HtoD: %s\n",
            cudaGetErrorString(fetchErr)
        );
        return 5;
    }

    double totalTime = 0.0;

    /**------------------------------------------------------------------------------------
     * Simulation of `nBodies`
     ------------------------------------------------------------------------------------*/

    /*
     * This simulation will run for `nIters` cycles of time, calculating
     * gravitational interaction amongst bodies, and adjusting their positions
     * to reflect.
     */

    for(int iter = 0; iter < nIters; iter++) {
        StartTimer();

        // compute interbody forces
        bodyForce<<<numberOfBlocks, threadsPerBlock>>>(p, dt, nBodies);
        bodyForceErr = cudaGetLastError();

        // integrate positions based on calculated forces
        integratePosition<<<numberOfBlocks, threadsPerBlock>>>(p, dt, nBodies);
        integrationErr = cudaGetLastError();

        asyncErr = cudaDeviceSynchronize();

        const double tElapsed = GetTimer() / 1000.0;
        totalTime += tElapsed;

        if(asyncErr != cudaSuccess) {
            printf(
                "Asynchronous error in iteration %d: %s\n", iter,
                cudaGetErrorString(asyncErr)
            );
            return 4;
        }
        if(bodyForceErr != cudaSuccess) {
            printf(
                "bodyForce kernel error in iteration %d: %s\n", iter,
                cudaGetErrorString(bodyForceErr)
            );
            return 3;
        }
        if(integrationErr != cudaSuccess) {
            printf(
                "integratePosition kernel error in iteration %d: %s\n", iter,
                cudaGetErrorString(integrationErr)
            );
            return 3;
        }
    }

    /**------------------------------------------------------------------------------------
     * Execution results
     ------------------------------------------------------------------------------------*/

    fetchErr = cudaMemPrefetchAsync(p, bytes, cudaCpuDeviceId);
    if(fetchErr != cudaSuccess) {
        printf(
            "Error in unififed memory pre-fetching DtoH: %s\n",
            cudaGetErrorString(fetchErr)
        );
        return 5;
    }

    double avgTime = totalTime / (double)(nIters);
#ifdef DEBUG
    printf("\nAverage execution time: %lf\n", avgTime);
#endif

    float billionsOfOpsPerSecond = 1e-9 * nBodies * nBodies / avgTime;
    write_values_to_file(solution_values, buf, bytes);

    printf("%0.3f Billion Interactions / second\n", billionsOfOpsPerSecond);

    /**------------------------------------------------------------------------------------
     * Deallocation and termination
     ------------------------------------------------------------------------------------*/

    allocErr = cudaFree(buf);
    if(allocErr != cudaSuccess) {
        printf(
            "Error in de-allocation of the bodies buffer: %s\n",
            cudaGetErrorString(allocErr)
        );
        return 2;
    }

    return 0;
}
