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
 * File:   kdtree.h
 * Author: anaim (aryan.e.naim@jpl.nasa.gov) 
 */

#ifndef KDTREE_H_
#define KDTREE_H_

#if defined(__cplusplus)
extern "C"
{
#endif

#include "assert.h"
#include "math.h"
#include "limits.h"
#include "stdbool.h"
#include "float.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
    
/*Rebuild the kd-tree every time the rebuild_threshold is crossed
default every time tre size doubles, hence 2. For tree size n there will be
about ln(n) rebuilds or about natural log of n.*/
#define REBUILD_THRESHOLD 2.0f
/*START-Representation of a kd tree-START*/
/*variables internal to kdtree_t*/
/*private member of kd-tree*/
typedef struct kdtree_internals
{
int kd_tree_allow_update;
/*variables*/
int current_number_of_kd_tree_nodes; 
/*Rebuild the kd-tree every time the rebuild_threshold is crossed
default every time tre size doubles, hence 2.*/
float rebuild_threshold;
/*previous tree rebuild's node count*/ 
int previous_tree_size;
/*the number of times the tree was rebuilt. Used for debugging.*/
int rebuild_counter;
} kdtree_internals;

/*kd_tree_node is single kdtree_t leaf*/
    typedef struct kd_tree_node
    {   
        struct kd_tree_node* left; 
        struct kd_tree_node* right; 
        struct kd_tree_node* parent; 
        float* dataset;  
        float distance_to_neighbor;
    } kd_tree_node;

    /*tree*/
    typedef struct {
        /*in order to  keep track of the kd-tree root*/
        kd_tree_node* _root;
        /*pointer to internal variables struct*/
        struct kdtree_internals* _internals;
        /*if this is true, then code will output more debug info while running.
         Errors will however be reported regardless of this flag*/
        int is_debug_run;

    } kdtree_t; 

/*declare variables*/
extern kdtree_t* self;
extern kdtree_t* kd_tree_processing_space;
extern kd_tree_node* node_space; 
extern kd_tree_node* node_processing_space;
extern kd_tree_node* node_knn_result_space;
extern kd_tree_node* batch_node_processing_space;
extern float* columns_median_space;
extern float* columns_median_processing_space; 
extern int k_dimensions; 



/*=============================================================================
Function       is_debug_on
Description:   Enables more debug output while the application is running. 
Output:        int
==========================================================*/
int kd_tree_is_debug_on();
/*=============================================================================
Function       is_debug_on
Description:   set if debug is on(1) or off(0).
Output:        void
==========================================================*/
void kd_tree_set_debug_on(int on); 
/*=============================================================================
Function       set_k_dimensions
Description:   Internal function used  to set the dimensionality of data or 
 *             also known as number of  features.
Input:         int max_dimensions - number of  features. 
Output:        Void
==========================================================*/
void kd_tree_set_k_dimensions(int max_dimensions);
/*=============================================================================
Function       set_k_dimensions
Description:   Internal function used  to get the dimensionality of data or 
 *             also known as number of  features. This is set by 
 *             init_heap(int max_dimensions) via the user. 
Output:        number of  features. 
==========================================================*/
int kd_tree_get_k_dimensions();


/*=============================================================================
Function       kd_tree_get_kd_tree
Description:   kd_tree_get_kd_tree returns reference to the tree & accesd
 *             its root type kdtree_t. 
==========================================================*/
kdtree_t* kd_tree_get_kd_tree();


/*=============================================================================
Function        kd_tree_get_root
Description:    get current kd-tree root. Used by rebuild procedure, traversal,
		etc.  
==========================================================*/
kd_tree_node* kd_tree_get_root();

/*=============================================================================
Function        set_rebuild_threshold
Description:    setter for  rebuild_threshold. By default is 1. 
==========================================================*/
void kd_tree_set_rebuild_threshold(const float threshold);

/*=============================================================================
Function        get_rebuild_threshold
Description:    getter for rebuild_threshold By default it returns 1. 
==========================================================*/
float kd_tree_get_rebuild_threshold();  


/*=============================================================================
Function        kd_tree_add_points 
Description:    User friendly wrapper for internal kd_tree_add_record function  
 *              which builds a kd-tree. 
References:     1) https://www.sanfoundry.com/java-program-find-nearest-
 *              2) https://www.cs.cmu.edu/~ckingsf/bioinfo-lectures/kdtrees.pdf
                3) https://www.geeksforgeeks.org/k-dimensional-tree/
                4) https://github.com/jtsiomb/kdtree
 *              5) https://www.cs.ubc.ca/research/flann/    
Inputs:         tree * root - The  pointer to the root of the kd-tree
 *              float data [] - use data with k_dimensions.
 *              k_dimensions - number of columns in the dataset 
 *              (number  of  features).
Output:         Returns a pointer to new created or updated kd-tree type tree.
TODO:           Use rebuild threshhold.Must be greater than 1        
==========================================================*/
/*mutator*/
void
kd_tree_add_points(kd_tree_node** root,const float data []);

/*===========================================================================
Function        delete_data_point
Description:    Given a data_point (query point) attempts to delete
 *              and return that point from the kd-tree. After deletion 
 *              subsequent calls to  delete_data_point same data point will 
 *              return NULL. After deletion calls to search will return 0.
 *              User API.
Input:          tree * root - tree root which is traversed
                float data_point [] - query point is as search parameter.
Output:         Returns tree pointer to delete tree. 
References:     https://www.geeksforgeeks.org/k-dimensional-tree-set-3-delete/
==========================================================*/
/*mutator*/
void kd_tree_delete_data_point(kd_tree_node* root,const float data_point []);


/*=============================================================================
Function        kd_tree_update_point 
Description:    User friendly wrapper for internal kd_tree_update_record 
 *              function  
 *              which builds a kd-tree.    
Inputs:         tree * root - The  pointer to the root of the kd-tree
 *              float target_data [] - data to be found and updated 
 *              float new_data [] - that will replace target_data []
Output:         Returns a int flag acting as boolean 0 if update failed or 1 
 *              if operation was successful.      
==========================================================*/
/*mutator*/
int
kd_tree_update_point(kd_tree_node* root, const float target_data [],  
        const float new_data []);


/*=============================================================================
Function:       kd_tree_search_single_node
Description:    
                Traverses the tree for the existence of a single node.
 *              This not a KNN search for KNN search call the function 
 *              kd_tree_knn.
 *              The search is done by either going left if the key is larger 
 *              than current node's info or going left if the key is smaller 
 *              than current nodes value. Ultimately reaching the key if it exists. 
 *              This traversal is O(log n) and considered optimal. 
 *              Wrapper around search(0 function which  is an internal API 
 *              function. User API.
Notes:          User friendly wrapper around search. This is a recursive
 *              function. 
Inputs:         tree * root - tree root which is traversed n order to 
 *              attempt to find the data[].
 *              float data[] - query point used as search parameter. 
Output:         Returns  0 (false) with NOT found. Returns 1 (true) if found. 
=============================================================================*/
int kd_tree_search_data_point(kd_tree_node* root,  const float data[]);

/*===========================================================================
Function        knn algorithm to find N nearest  neighbors. Internal API. 
Description:    Given a root to traverse and a data point, this function 
 *              attempts to find nearest neighbors to that data point
 *              using Euclidean distance.
Inputs:         tree * root - tree root which is traversed n order to 
 *              attempt to find the data[].
 *              float data_point[] - query point is as search parameter. 
 *              int k_dimensions - number of  features 
                int number_of_nearest_neighbors - desired number of results
 *              int sort_result - should results be sorted. 0 false any other
 *              number is true. 
Outputs:        int - number of neighbors found. 
 *              tree** n_nearests - pointer to array of points of type
 *              tree that represent the nearest neighbors.
References:     Samet, H., 2006. Foundations Of Multidimensional And 
 *              Metric Data Structures. San Francisco: Morgan Kaufmann, 
 *              pp.1-37,50-57,485-508.
Notes:          This implementation is  O(log N) running time. The tree
 *              is traversed  using kd-tree which is O(log N) in complexity
 *              and calculating all distances away from our query point. 
 *              Use friendly wrapper to knn().
==========================================================*/
int
kd_tree_knn(kd_tree_node* const root, const float data_point[],
        int number_of_nearest_neighbors);

/*===========================================================================
Function        knn algorithm to find N nearest  neighbors. 
Description:    Given a root to traverse and a data point, this function 
 *              attempts to find nearest neighbors to that data point
 *              within a specific Euclidean distance or radius. User API.  
Inputs:         tree * root - tree root which is traversed in order to 
 *              attempt to find the data[].
 *              float data_point[] - query point is as search parameter. 
                int number_of_nearest_neighbors - desired number of results
Outputs:        int - number of neighbors found. 
 *              tree** n_nearests - pointer to array of points of type
 *              tree that represent the nearest neighbors.
References:     Samet, H., 2006. Foundations Of Multidimensional And 
 *              Metric Data Structures. San Francisco: Morgan Kaufmann, 
 *              pp.1-37,50-57,485-508.
Notes:          This implementation is  O(log N) running time. The tree
 *              is traversed  using kd-tree which is O(log N) in complexity
 *              and calculating all distances away from our query point. 
 *              Use friendly wrapper to knn().
==========================================================*/
int
kd_tree_knn_based_on_radius (kd_tree_node* root, 
                    const float data_point[],
                    float range_from_data_point);

/*===========================================================================
Function        get_current_number_of_kd_tree_nodes
Description:    Returns current number  of tree nodes. 
==========================================================*/
int kd_tree_get_current_number_of_kd_tree_nodes();

/*===========================================================================
Function:       in_order_stack_traversal
Description:    Returns number of  tree nodes that were traversed and set in 
 *              output parameter output_flatten_kd_tree.  
Inputs:         kd_tree_node *root. 
 *              int number_dimensions.
Outputs:        In order array representing all kdtree nodes & returns number               
 *              nodes in the flatten_kd_tree.
References: 
 1)https://tutorialspoint.dev/data-structure/
 * 2) https://prismoskills.appspot.com/lessons/Binary_Trees/Traversal_without_recursion.jsp
 * 3) MIT Algorithms cormen
 * 4) Algorithms in C Robert Sedgewick 
==========================================================*/
int
kd_tree_in_order_traversal (kd_tree_node *root, int number_dimensions);

/*=============================================================================
Function:       kd_tree_points_equal
Description:    A utility function to determine equality                
 *              in multidimensional space. Returns 1 if                 
 *              true & 0 if false.   
=============================================================================*/
int kd_tree_points_equal(const float point1[],const  float point2[], 
        int k_dimensions);

/*END-Representation of a kd tree-END*/

/*START-memory managment-START*/
/*=============================================================================
Function        kdtree_alloc
Description:    Allocates all memory for kdtree library
==========================================================*/
kdtree_t* kdtree_alloc(int max_rows, int max_cols);

/*=============================================================================
Function        kdtree_init
Description:    init all memory for kdtree library
==========================================================*/
void kdtree_init(kdtree_t* self);

/*=============================================================================
Function        kdtree_free
Description:    frees all memory for kdtree library
==========================================================*/
void kdtree_free(kdtree_t* self);
/*END-memory management-END*/

void
kd_tree_print_data_for_debug(kd_tree_node* data, const int k_dimensions,
                      const int result_size);
/*
 * FLANN style convenience wrappers


int kdtree_build_index( kdtree_t* self,
                        float* dataset,
                             int rows,
                             int cols);

int kdtree_find_nearest_neighbors_index(kdtree_t* self,
                                        float* dataset,
                                        int rows,
                                        int cols,
                                        float* testset,
                                        int trows,
                                        int* indices,
                                        float* dists,
                                        int nn);

int kdtree_radius_search(kdtree_t* self, 
                         float* query, // query point
                         int* indices, // array for storing the indices found (will be modified) 
                         float* dists, // similar, but for storing distances 
                         int max_nn,  // size of arrays indices and dists
                         float radius);// search radius (squared radius for euclidian metric) 

*/

#if defined __cplusplus
}
#endif
#endif
