 /*Copyright 2020, by the California Institute of Technology. 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * 
 * File:   memory_allocation_test.c
 * Author: anaim (aryan.e.naim@jpl.nasa.gov)
 * Created on August 4, 2020, 8:43 AM
 * 
 *IMPORTANT: Must call kdtree_alloc() & kdtree_init() once before using the API.
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

