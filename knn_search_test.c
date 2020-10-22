
#include <stdio.h>
#include "kdtree.h"
#include <sys/time.h>
#include <math.h>
#include <time.h> 

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
 * IMPORTANT: Must call kdtree_alloc() & kdtree_init() once before using the API. 
 * Example kddtree library usage for adding points and KNN search.
 * KNN Search is called via function kd_tree_knn().
 * In order to cleanup call kdtree_free().
 *
 * File:   crud_test.c
 * Author: anaim (aryan.e.naim@jpl.nasa.gov) 
 * Created on August 4, 2020, 7:45 AM
 */
 

int main(int argc, char **argv) {

    /*assuming 1000 datapoints*/
    int max_rows = 1000;
    /*assuming 3 dimensions*/
    int max_cols = 3;
    kdtree_t* kdtree = NULL;
    /*allocate  heap memory */
    kdtree = kdtree_alloc(max_rows, max_cols);
    assert(kdtree);
    printf("alloc ok \n");
    /*init heap memory*/
    kdtree_init(kdtree);
    printf("init ok \n");
    /*enable extra debugging output */
    kd_tree_is_debug_on(1);
    /*set rebuild thresh hold, to rebalance the tree when tree reaches that 
    threshold. Default is 2 anyways.*/
    kd_tree_set_rebuild_threshold(2);
    /*insert some points...*/
    printf("Adding kd_tree nodes & Search:\n");
    
    float point [] = {-100, -100, -100};

    /*call kd_tree_add_points*/
    kd_tree_node* root = kd_tree_get_root();
    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();

    point[0] = -5;
    point[1] = -5;
    point[2] = -5;
   
    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
  
    point[0] = -4;
    point[1] = -4;
    point[2] = -4;

    kd_tree_add_points(&root,point);
    root = kd_tree_get_root();
   

    point[0] = -3;
    point[1] = -3;
    point[2] = -3;

    kd_tree_add_points(&root,point);
    root = kd_tree_get_root();
   
    
    point[0] = -2;
    point[1] = -2;
    point[2] = -2;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
  
    
    point[0] = -1;
    point[1] = -1;
    point[2] = -1;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
  
    
    point[0] = 0;
    point[1] = 0;
    point[2] = 0;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
 
    
    point[0] = 1;
    point[1] = 1;
    point[2] = 1;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
  
    
    point[0] = 2;
    point[1] = 2;
    point[2] = 2;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
   

    point[0] = 3;
    point[1] = 3;
    point[2] = 3;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    

    point[0] = 4;
    point[1] = 4;
    point[2] = 4;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
  
    point[0] = 5;
    point[1] = 5;
    point[2] = 5;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
   
    int column_dimensions = 3;
    
    /*traverse tree for sanity check & debugging */
    int result_size = kd_tree_in_order_traversal(root, column_dimensions);
    printf("***In order traverse iterative :\n");
    kd_tree_print_data_for_debug(node_knn_result_space, column_dimensions, result_size);

    int max_range = 0; 
    float query_point [] = {0.0f, 0.0f, 0.0f};
    printf("KNN Query for EXISTING key\n");
    printf("knn (k nearest neighbor)\n:");
    printf("%d, nearest neighbors to Key: {%f,%f,%f}:\n", max_range, query_point[0], query_point[1], query_point[2]);

    /*test 0*/
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    /*test 1*/

    max_range = 1;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);
    /*test 2*/

    max_range = 2;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    /*test 3*/

    max_range = 3;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    max_range = 4;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    query_point[0] = 5.1;
    query_point[1] = 5.1;
    query_point[2] = 5.1;

    max_range = 0;

    printf("Query for NON-EXISTING key\n");
    printf("knn (k nearest neighbor)\n:");
    printf("%d, nearest neighbors to Key: {%f,%f,%f}:\n", max_range, query_point[0], query_point[1], query_point[2]);


    /*test 0*/
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    /*test 1*/

    max_range = 1;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);
    /*test 2*/

    max_range = 2;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    /*test 3*/

    max_range = 3;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);


    max_range = 4;
    result_size = kd_tree_knn(root, query_point, max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);


    printf("Query for EXISTING key\n");
    printf("Within certain range/radius - knn (k nearest neighbor)\n:");
    printf("%d, nearest neighbors to Key: {%f,%f,%f}:\n", max_range, query_point[0], query_point[1], query_point[2]);



    query_point[0] = 0;
    query_point[1] = 0;
    query_point[2] = 0;


    /*test 0*/

    max_range = 0;
    result_size = kd_tree_knn_based_on_radius(root,
            query_point,
            max_range);

    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);



    /*test 1*/

    max_range = 1;
    result_size = kd_tree_knn_based_on_radius(root,
            query_point,
            max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);
    /*test 2*/

    max_range = 2;
    result_size = kd_tree_knn_based_on_radius(root,
            query_point,
            max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);

    /*test 3*/

    max_range = 3;
    result_size = kd_tree_knn_based_on_radius(root,
            query_point,
            max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);


    max_range = 4;
    result_size = kd_tree_knn_based_on_radius(root,
            query_point,
            max_range);
    printf("knn results:\n");
    kd_tree_print_data_for_debug(node_knn_result_space, k_dimensions,
            result_size);
    
     kdtree_free(kdtree);
     printf("free ok \n");

}

