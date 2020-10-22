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
 * Batch test for points insertion of nodes & timing. 
 * IMPORTANT: Must call kdtree_alloc() & kdtree_init() once before using the API. 
 * 
 * File:   batch_test.c
 * Created on August 4, 2020, 8:39 AM
 * File:   kdtree.c
 * Author: anaim (aryan.e.naim@jpl.nasa.gov) 
 */

#include <stdio.h>
#include "kdtree.h"
#include <sys/time.h>
#include <math.h>
#include <time.h> 

int main(int argc, char** argv) {

    /*test adding 20000 points for 5 iterations and gather avg timings*/
    int max_rows = 20000;
    int max_cols = 3;

    kdtree_t* kdtree  = kdtree_alloc(max_rows, max_cols);
    assert(kdtree);
    printf("alloc ok \n");

    int counter = 0;
    int number_of_test = 5; 
    double total_time_taken = 0.0;
    kd_tree_node* root; 
    while (counter < number_of_test) {
        kdtree_init(kdtree);
        /*extra debug is on!*/
        kd_tree_set_debug_on(1);
        kd_tree_set_rebuild_threshold(2);
        printf("init ok, counter:%d\n", counter);

        int i = 0;
        float point[3];
        printf("inserting:\n");
        clock_t add_start_time = clock();
        //insert
        for (; i < max_rows; i++) {
            point[0] = i;
            point[1] = i;
            point[2] = i;
            root = kd_tree_get_root(); 
            kd_tree_add_points(&root, point);


        }//end insert 
        printf("***inserted, %d nodes\n", kd_tree_get_current_number_of_kd_tree_nodes());

        clock_t end_start_time = clock();
        clock_t delta_time = end_start_time - add_start_time;
        double time_taken = ((double) delta_time) / CLOCKS_PER_SEC; // in seconds

        total_time_taken = total_time_taken + time_taken;
        counter++;
        printf("iteration %d, %f seconds to execute \n", counter, time_taken);
        printf("insertion done!:\n");


        //start search 
        i = 0;
        int found_counter = 0;
        for (; i < max_rows; i++) {
            point[0] = i;
            point[1] = i;
            point[2] = i;
            if (kd_tree_search_data_point(kd_tree_get_root(), point)) {
                found_counter++;
            } else {
                printf("point not found {%f,%f,%f}\n", point[0], point[1], point[2]);
            }


        }//end search  

        printf("***found_counter!:%d\n", found_counter);
        
    }
    
    printf("\n");
    printf("average runtime for insertion of %d nodes, tested over iterations=%d, secs=%f\n", max_rows, counter, (total_time_taken / counter));
    /*clean up*/
    kdtree_free(kdtree);
    printf("free ok \n");


    return 0;
}

