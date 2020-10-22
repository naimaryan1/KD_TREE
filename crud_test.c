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
 * File:   kdtree.c
 * Author: anaim (aryan.e.naim@jpl.nasa.gov) 
 *
 * Example of libray usage  and test of add,search(single node_, delete, update 
 * and traverse. 
 * 
 * IMPORTANT: Must call kdtree_alloc() & kdtree_init() once before using the API.
 * 
 * In order to add points call kd_tree_add_points()
 * In order to update a point call kd_tree_update_point()
 * In order to search for a single point call kd_tree_search_data_point()
 * In order to delete a point call kd_tree_delete_data_point()
 * In order to traverse the entire tree for debugging call 
 * kd_tree_in_order_traversal() the call kd_tree_print_data_for_debug(). 
 * In order to cleanup call kdtree_free().
 */

int main(int argc, char **argv) {

    /*assuming 3 dimensions*/
    int max_rows = 1000;
    /*assuming 1000 datapoints*/
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
    
//  float point [] = {-100, -100, -100};
    float point [] ={-53.148998,0.000000,9.000000};
    /*call kd_tree_add_points*/
    kd_tree_node* root = kd_tree_get_root();
    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n ", point[0], point[1], point[2]);
    }

    point[0] = 7.999700;
    point[1] = 0.069812;
    point[2] = 8.000000;
    
    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found\n ", point[0], point[1], point[2]);
    }

    point[0] =7.998780;
    point[1] = 0.139619;
    point[2] = 8.000000;
    

    kd_tree_add_points(&root,point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found\n ", point[0], point[1], point[2]);
    }

    point[0] = 7.997260;
    point[1] = 0.209416;
    point[2] = 8.000000;

    kd_tree_add_points(&root,point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", point[0], point[1], point[2]);
    }

    point[0] = 7.995130;
    point[1] = 0.279196;
    point[2] = 8.000000;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n ", point[0], point[1], point[2]);
    }


    point[0] = 7.992390;
    point[1] = 0.348955;
    point[2] = 8.000000;
    
    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", point[0], point[1], point[2]);
    }


    point[0] = 8.987670;
    point[1] = 0.471024;
    point[2] = 9.000000;
    

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n ", point[0], point[1], point[2]);
    }

    point[0] = 8.983210;
    point[1] = 0.549437;
    point[2] = 9.000000;
    
    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n ", point[0], point[1], point[2]);
    }


    point[0] = 7.980510;
    point[1] =  0.558052;
    point[2] =  8.000000;
    

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n ", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n ", point[0], point[1], point[2]);
    }


    point[0] = 3;
    point[1] = 3;
    point[2] = 3;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", point[0], point[1], point[2]);
    }


    point[0] = 4;
    point[1] = 4;
    point[2] = 4;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", point[0], point[1], point[2]);
    }

    point[0] = 5;
    point[1] = 5;
    point[2] = 5;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found!\n", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", point[0], point[1], point[2]);
    }

    point[0] = 100;
    point[1] = 100;
    point[2] = 100;

    kd_tree_add_points(&root, point);
    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, point)) {

        printf("%f,%f,%f Found\n", point[0], point[1], point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", point[0], point[1], point[2]);
    }


    printf("Search of NON-existent data:\n");

    float query_point [] = {10, 10, 10};

    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {
        printf("%f,%f,%f Found\n! ", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found\n! ",
                query_point[0], query_point[1], query_point[2]);
    }

    /*-20,-20,-20 is not added  so it should not be found */

    query_point [0] = -20;
    query_point [1] = -20;
    query_point [2] = -20;

    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {
        printf("%f,%f,%f Found\n", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n",
                query_point[0], query_point[1], query_point[2]);
    }

    query_point [0] = -101;
    query_point [1] = -101;
    query_point [2] = -101;

    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {
        printf("%f,%f,%f Found\n", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n",
                query_point[0], query_point[1], query_point[2]);
    }

    query_point [0] = -1000;
    query_point [1] = -1000;
    query_point [2] = -1000;


    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {
        printf("%f,%f,%f Found\n", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n",
                query_point[0], query_point[1], query_point[2]);
    }


    /*After delete & change - can we find previously found nodes? */

    int column_dimensions = 3;


    int result_size = kd_tree_in_order_traversal(root, column_dimensions);
    printf("***In order traverse iterative :\n");
    kd_tree_print_data_for_debug(node_knn_result_space, column_dimensions, result_size);


    /*delete nodes  {-1,-1,-1}*/
    query_point [0] = 1000;
    query_point [1] = 1000;
    query_point [2] = 1000;


    printf("Delete {%f,%f,%f} \n! ", query_point[0], query_point[1], query_point[2]);
    kd_tree_delete_data_point(root, query_point);

    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {

        printf("%f,%f,%f Found\n", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", query_point[0], query_point[1], query_point[2]);
    }


    result_size = kd_tree_in_order_traversal(root, column_dimensions);
    printf("***In order traverse iterative :\n");
    kd_tree_print_data_for_debug(node_knn_result_space, column_dimensions, result_size);

    result_size = kd_tree_in_order_traversal(root, column_dimensions);
    printf("***In order traverse iterative :\n");
    kd_tree_print_data_for_debug(node_knn_result_space, column_dimensions, result_size);
    
                
    query_point [0] = 0;
    query_point [1] = 0;
    query_point [2] = 0;


    printf("Delete {%f,%f,%f} \n! ", query_point[0], query_point[1], query_point[2]);
    kd_tree_delete_data_point(root, query_point);

    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {

        printf("%f,%f,%f Found\n", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", query_point[0], query_point[1], query_point[2]);
    }
    printf("current_number_of_kd_tree_nodes=%d\n:", kd_tree_get_current_number_of_kd_tree_nodes());

    query_point [0] = -1;
    query_point [1] = -1;
    query_point [2] = -1;


    printf("Delete {%f,%f,%f} \n! ", query_point[0], query_point[1], query_point[2]);
    kd_tree_delete_data_point(root, query_point);

    root = kd_tree_get_root();
    if (kd_tree_search_data_point(root, query_point)) {

        printf("%f,%f,%f Found\n", query_point[0], query_point[1], query_point[2]);
    } else {
        printf("%f,%f,%f Not Found!\n", query_point[0], query_point[1], query_point[2]);
    }

    printf("current_number_of_kd_tree_nodes=%d\n:", kd_tree_get_current_number_of_kd_tree_nodes());
    result_size = kd_tree_in_order_traversal(root, column_dimensions);
    printf("In order traverse iterative :\n");
    kd_tree_print_data_for_debug(node_knn_result_space, column_dimensions, result_size);


    printf("\n");

    /*start update*/
    printf("Attempting to update:\n");
    query_point [0] = 100;
    query_point [1] = 100;
    query_point [2] = 100;

    float new_data [] = {200, 200, 200};


    printf("update {%f,%f,%f} to {%f,%f,%f}", query_point[0],
            query_point[1],
            query_point[2],
            new_data[0],
            new_data[1],
            new_data[2]);

    int flag = kd_tree_update_point(root, query_point, new_data);

    root = kd_tree_get_root();
    if (flag) {

        printf("Update was successful\n");
    } else {
        printf("Update was NOT successful!\n");
    }

    query_point [0] = -100;
    query_point [1] = -100;
    query_point [2] = -100;

    new_data[0] = -200;
    new_data[1] = -200;
    new_data[2] = -200;

    printf("update {%f,%f,%f} to {%f,%f,%f}", query_point[0],
            query_point[1],
            query_point[2],
            new_data[0],
            new_data[1],
            new_data[2]);

    flag = kd_tree_update_point(root, query_point, new_data);

    root = kd_tree_get_root();
    if (flag) {

        printf("Update was successful\n");
    } else {
        printf("Update was NOT successful!\n");
    }

    printf("current_number_of_kd_tree_nodes=%d\n:", kd_tree_get_current_number_of_kd_tree_nodes());
    result_size = kd_tree_in_order_traversal(root, column_dimensions);
    printf("In order traverse iterative :\n");
    kd_tree_print_data_for_debug(node_knn_result_space, column_dimensions, result_size);

    /*end update*/

       kdtree_free(kdtree);
        printf("free ok \n");

}

