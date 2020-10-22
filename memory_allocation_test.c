/* Copyright [2019-2020], by the California Institute of Technology. 
 * ALL RIGHTS RESERVED. United States Government Sponsorship acknowledged. 
 * Any commercial use must be negotiated with the Office of Technology Transfer 
 * at the California Institute of Technology.
 * 
 * This software may be subject to U.S. export control laws. By accepting this 
 * software, the user agrees to comply with all applicable U.S. export laws and 
 * regulations. User has the responsibility to obtain export licenses, or other 
 * export authority as may be required before exporting such information to 
 * foreign countries or providing access to foreign persons.

 * 
 * File:   memory_allocation_test.c
 * Author: anaim (aryan.e.naim@jpl.nasa.gov)
 * Created on August 4, 2020, 8:43 AM
 * 
 IMPORTANT: Must call kdtree_alloc() & kdtree_init() once before using the API.
 * In order to cleanup call kdtree_free(). 
 */

#include <stdio.h>
#include "kdtree.h"
#include <sys/time.h>
#include <math.h>
#include <time.h> 

/*
 * Test alloc,init,free multiple times.
 */
int main(int argc, char** argv) {

    int max_rows = 1000;
    int max_cols = 10;

    kdtree_t* kdtree = kdtree_alloc(max_rows, max_cols);
    assert(kdtree);
    printf("alloc ok \n");

    kdtree_init(kdtree);
    assert(kdtree);
    printf("init ok\n");

    kdtree_free(kdtree);
    printf("free ok \n");

    kdtree = kdtree_alloc(max_rows, max_cols);
    assert(kdtree);
    printf("alloc ok \n");

    kdtree_init(kdtree);
    assert(kdtree);
    printf("init ok\n");

    kdtree_init(kdtree);
    assert(kdtree);
    printf("init ok\n");

    kdtree_init(kdtree);
    assert(kdtree);
    printf("init ok\n");

    kdtree_free(kdtree);
    printf("free ok \n");

    return (EXIT_SUCCESS);
}

