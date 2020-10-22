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
 */

#include "kdtree.h"
/*macros are related to fast median algorithm, see kth_smallest()*/
#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }
#define median(a,n) kth_smallest(a,n,(((n)&1)?((n)/2):(((n)/2)-1)))
int is_debug_run = 0; 
/*=============================================================================
variables -kdtree  
==============================================================================*/
/*kd tree heap */
kdtree_t* self = NULL;  
kdtree_t*  kd_tree_processing_space = NULL; 
/*kd tree leaf nodes heap*/
kd_tree_node* node_space = NULL;
kd_tree_node* node_processing_space= NULL;
kd_tree_node* node_knn_result_space= NULL;
kd_tree_node* batch_node_processing_space = NULL; 
/*median calculation heap*/
float* columns_median_space= NULL;
float* columns_median_processing_space=NULL;
/*Structure of a stack of nodes type kd_tree_stack_node.*/
typedef struct kd_tree_stack_node 
{ 
  kd_tree_node* t; 
  struct kd_tree_stack_node* next; 
} kd_tree_stack_node;
/*stack heap*/
kd_tree_stack_node* stack_processing_space; 
/*elem_type is related to fast median algorithm, see kth_smallest()*/
typedef float elem_type ;

/*=============================================================================
Global variables declared
==============================================================================*/
/*kd-tree related*/
int max_data_dimensions = 1;
int k_dimensions = 1;
/*default heap_size (aka row size is 10,000 nodes)*/
int heap_size = 10000; 
/* stack related*/
int snode_processing_heap_tail_index = 0; 
int snode_processing_heap_head_index  = 0; 

/*=============================================================================
Declarations
==============================================================================*/
/*tree*/
kdtree_t* kd_tree_alloc_tree_space();
kdtree_t* kd_tree_get_tree_space();
void kd_tree_free_tree_space(kdtree_t* tree);
void kd_tree_init_tree_space(kdtree_t* tree);
/*tree processing heap space*/
kdtree_t*  kd_tree_alloc_tree_processing_space();
kdtree_t*  kd_tree_get_processing_space();
void kd_tree_init_kdtree_processing_space(kdtree_t* tree);
void kd_tree_free_kdtree_processing_space(kdtree_t* tree);
/*tree internals heap space*/
struct kdtree_internals* kd_tree_alloc_internals(); 
void kd_tree_init_tree_internals(kdtree_t* tree);
void kd_tree_free_internals(kdtree_t* tree);
/*leaf nodes*/
void kd_tree_alloc_node_space_heap(int rows,int max_dimensions);
kd_tree_node*
get_pre_allocated_kd_tree_node_heap();
void kd_tree_init_node_heap(kd_tree_node** nodes, int max_dimensions);
void kd_tree_free_node_space(kd_tree_node** nodes);
/*leaf nodes processing*/
void kd_tree_alloc_node_processing_space_heap(int rows,int max_dimensions);
kd_tree_node*
get_pre_allocated_processing_heap();
void kd_tree_init_node_processing_space_heap(kd_tree_node** nodes, 
        int max_dimensions);
void kd_tree_free_node_processing_space_heap(kd_tree_node** nodes);
/*knn result space*/
void kd_tree_alloc_node_knn_result_heap();
kd_tree_node*
get_knn_result_heap();
void kd_tree_init_node_knn_result_heap(kd_tree_node** nodes, 
        int max_dimensions);
void kd_tree_free_node_knn_result_heap(kd_tree_node** nodes);
/*batch processing space*/
void kd_tree_alloc_batch_processing_heap();
kd_tree_node*
get_kd_tree_batch_processing_heap();
void kd_tree_init_batch_processing_heap(kd_tree_node** nodes, 
        int max_dimensions);
void kd_tree_free_batch_processing_heap(kd_tree_node** nodes);
/*median heap space*/
void kd_tree_alloc_columns_median_heap(int k_dimensions);
void kd_tree_init_columns_median_heap(float** medians, int k_dimensions);
float* kd_tree_get_columns_median_heap();
void kd_tree_free_columns_median_space(float** medians);
/*median processing space */
float* kd_tree_alloc_columns_median_processing_space(int rows);
void kd_tree_init_columns_median_processing_space(float** medians, int rows);
float* kd_tree_get_columns_median_processing_space();
void kd_tree_free_columns_median_processing_space(float** medians);
/*Minimal Stack function prototypes, push, pop, empty*/
void kd_tree_stack_push(kd_tree_stack_node** top_ref, kd_tree_node *t); 
kd_tree_node  *kd_tree_stack_pop(kd_tree_stack_node** top_ref); 
int kd_tree_stack_is_stack_empty(kd_tree_stack_node *top); 
/*other functions, setters & getters*/
/*kd_tree_set_root is mutator*/
void kd_tree_set_root(kd_tree_node* root);
void kd_tree_set_k_dimensions(int max_dimensions);
int kd_tree_get_k_dimensions();
void kd_tree_set_rows_size(int rows);
int kd_tree_get_rows_size();
int kd_tree_get_processing_size();
void kd_tree_set_column_median(float median, int column_index);
float kd_tree_get_column_median(int column_index);
void kd_tree_set_column_median_processing_space(float median, int column_index);
float kd_tree_get_column_median_processing_space(int column_index);
int get_column_vector_from_matrix (float * info, int row_length, 
int  column_length, int column_dimension, float * out_put_array);
kd_tree_node* 
kd_tree_new_node (const float data[],const int k_dimensions,const int copying);
/*mutator*/
int
kd_tree_rebuild (kd_tree_node* root,const int k_dimensions);
void
kd_tree_set_previous_tree_size (const int size);
void
kd_tree_set_previous_tree_size (const int size);
float kd_tree_get_previous_tree_size();
void
set_current_number_of_kd_tree_nodes(int number_of_nodes);
int
kd_tree_get_current_number_of_kd_tree_nodes();
void
kd_tree_increment_current_number_of_kd_tree_nodes();

/*=============================================================================
Function       
Description:
Notes:          Used for debugging.   
==========================================================*/
void
kd_tree_decrement_current_number_of_kd_tree_nodes ();
/*mutator*/
void kd_tree_add_record(kd_tree_node** root, const float key [], int depth,
        const int k_dimensions, const int copying, 
        const float rebuild_threshold);
/*mutator*/
int kd_tree_update_record(kd_tree_node* root, const float target_data [],  
        const float new_data [],const int k_dimensions);
int kd_tree_search_helper(kd_tree_node* root, const float data[], int depth, 
        const int k_dimensions);
float kd_tree_n_dimensional_euclidean(const float values_1 [], 
        const float values_2 [], const int k_dimensions);
int kd_tree_knn_helper(kd_tree_node* const root,const float data_point[],
               const int  k_dimensions,
               int number_of_nearest_neighbors);
int kd_tree_delete_data_point_helper(kd_tree_node* root, 
        float const  data_point [], 
        int depth,
        const int k_dimensions);
int
kd_tree_knn_based_on_radius_helper (kd_tree_node* root, 
                    const float data_point[], 
                    const int k_dimensions, 
                    float range_from_data_point);
void
set_current_number_of_kd_tree_nodes(int number_of_nodes);
void kd_tree_decrement_current_number_of_kd_tree_nodes();
void
kd_tree_increment_current_number_of_kd_tree_nodes();
/*function related to sorting,finding median of arrays*/
void insertion_sort_based_on_distance(kd_tree_node* arr[], int n);
void insertion_sort_based_on_distance2(kd_tree_node* arr, int n);
float regular_median (float values [] , int n);
elem_type kth_smallest(elem_type a[], int n, int k); 
void insertion_sort(float  * arr, int n);
/*isEmpty node*/
int is_empty_node(kd_tree_node* node, int number_of_dimensions);

/*=============================================================================
Implementations -kdtree  
==============================================================================*/
int kdtree_build_index( kdtree_t* self,
                        float* dataset,
                             int rows,
                             int cols);

/*=============================================================================
Function        kdtree_free_index
Description:    
==========================================================*/
void kdtree_free_index(kdtree_t* self);

/*return tree*/
kdtree_t* kd_tree_get_kd_tree()
{
    if (NULL==self)
    {
     self = (kdtree_t*) calloc (1, sizeof (kdtree_t));
     assert(self);
    }
     return self;
}

int kd_tree_is_debug_on()
{
    if (NULL!=self)
    {
    return self->is_debug_run;
    }
    else
    {
     return 0;     
    }
}

void kd_tree_set_debug_on(int on)
{
    if (NULL!=self)
    {
    self->is_debug_run = on;
    }
}

void kd_tree_set_k_dimensions(int max_dimensions)
{
    k_dimensions = max_dimensions;
}

int kd_tree_get_k_dimensions()
{
    return k_dimensions; 
}

void kd_tree_set_rows_size(int rows)
{
    heap_size = rows; 
}
int kd_tree_get_rows_size()
{
    return heap_size; 
}
int kd_tree_get_processing_size()
{
    return  kd_tree_get_rows_size() *2; 
}

/*mutator*/
void kd_tree_set_root(kd_tree_node* root)
{
 /*DONT check/block this function using  kd_tree_allow_update flag, because
    this function is used by kd_tree_rebuild(0 that will also block the 
  * rebuild!*/
    if (NULL != kd_tree_get_kd_tree()) {
        kd_tree_get_kd_tree()->_root = root;
    } else {
        printf("kd_tree_set_root(), kd_tree is NULL,call init. ");
    }
}

kd_tree_node* kd_tree_get_root()
{
  return kd_tree_get_kd_tree()->_root;
}


void
kd_tree_set_previous_tree_size (const int size)
{
    if (NULL!=kd_tree_get_kd_tree())
    {
        kd_tree_get_kd_tree()->_internals->previous_tree_size = size;
    }
}

float kd_tree_get_previous_tree_size() {
    if (NULL != kd_tree_get_kd_tree()) {
        return kd_tree_get_kd_tree()->_internals->previous_tree_size;

    } else {
        return -1;
    }
}

void
set_current_number_of_kd_tree_nodes(int number_of_nodes)
{
     if (NULL!=self)
     {
    self->_internals->current_number_of_kd_tree_nodes=number_of_nodes;
     }
}


int
kd_tree_get_current_number_of_kd_tree_nodes()
{
    if (NULL!=self)
    {
    return self->_internals->current_number_of_kd_tree_nodes;
    }
    else
    {
        return -1; 
    }
}

void
kd_tree_increment_current_number_of_kd_tree_nodes()
{
    if (NULL!=kd_tree_get_kd_tree())
    {
    kd_tree_get_kd_tree()->_internals->current_number_of_kd_tree_nodes++;
    }
}


void
kd_tree_decrement_current_number_of_kd_tree_nodes ()
{
    if (NULL!=kd_tree_get_kd_tree() &&  
       kd_tree_get_kd_tree()->_internals->current_number_of_kd_tree_nodes>0)
    {
    kd_tree_get_kd_tree()->_internals->current_number_of_kd_tree_nodes--;
    }
    
}


/*=============================================================================
Function        set_rebuild_threshold
Description:    setter for  rebuild_threshold. By default is 1. 
==========================================================*/
void kd_tree_set_rebuild_threshold(const float threshold)
{
    if (NULL!=kd_tree_get_kd_tree())
    {
     kd_tree_get_kd_tree()->_internals->rebuild_threshold = threshold;
    }
    
}

/*=============================================================================
Function        get_rebuild_threshold
Description:    getter for rebuild_threshold By default it returns 1. 
==========================================================*/
float kd_tree_get_rebuild_threshold() {
    if (NULL != kd_tree_get_kd_tree()) {

        return kd_tree_get_kd_tree()->_internals->rebuild_threshold;
    } else {
        return -1;
    }
}
/*=============================================================================
Function        new_node
Description:    given data create a noe & return it. 
==========================================================*/
kd_tree_node*
kd_tree_new_node(const float data[], const int k_dimensions, const int copying) 
{
    kd_tree_node* new_node = NULL; 
    /*DONT check/block this function using  kd_tree_allow_update flag, because
     this function is used by kd_tree_rebuild(0 that will also block the rebuild!*/

    /*if we are constructing the kd-tree */
    if (!copying) {
        new_node = get_pre_allocated_kd_tree_node_heap();

        kd_tree_increment_current_number_of_kd_tree_nodes();

    }        /*else if we are creating a temporary copy */
    else {
        new_node = get_pre_allocated_processing_heap();
    }
    /*copy data*/
     int i = 0;
           for (; i < k_dimensions; i++) {
               
        new_node->dataset[i] = data[i];
    }
 
     return new_node; 
   
}
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
==========================================================*/
/*mutator*/
void kd_tree_add_points(kd_tree_node** root, const float data []) {

    if (NULL != kd_tree_get_kd_tree()) {
        if (kd_tree_get_kd_tree()->_internals->kd_tree_allow_update) {

           kd_tree_add_record(root,
                    data,
                    0,
                    kd_tree_get_k_dimensions(),
                    0,
                    kd_tree_get_rebuild_threshold());
        


        } else {
            printf("kd_tree_add_points(),"
                    " kd_tree is locked for rebuild!");
        }
    } else {
        printf("kd_tree_add_points(),"
                " kd_tree is NULL call init");
    }
}

/*=============================================================================
Function        kd_tree_rebuild 
Description:    
                IF threshold is reached
                    THEN
 *                      1) lock all mutator operations allow only READ_ONLY
 *                      2)Similar to FLANN's implementation split based on 
 *                      statistic (median or variance). 
 *                      of specific column/dimension. 
 *                          a)traverse current tree & get back a vector
 *                          b)extract column wise values 
 *                          c)sort (only using text book median)
 *                          d)calc median & set column 
 *                      3) reset  previous tree memory (actually reset heap)
 *                      4) REBUILD 
 *                      5) Unlock tree update operations. DONE.
References:             based on FLANN implementation.
 *                      FLANN’s implementation of k-d trees uses a simple
strategy for re-balancing the trees: It reconstructs all trees
each time the dataset doubles in size from the initial dataset
(i.e., the first batch). Therefore, the k-d trees can become
unbalanced as new data is loaded but eventually will be
reconstructed. When loading a large dataset progressively,
even if the incoming distribution matches the current k-d
tree structure, FLANN will always reconstruct its k-d trees
when the dataset doubles in size. According to:
 * 
J. Jo, J. Seo, and J.-D. Fekete, “PANENE: A Progressive Algorithm for Indexing 
 * and Querying Approximate k-Nearest Neighbors,” IEEE Transactions on 
 * Visualization and Computer Graphics, pp. 15, 2019.
 */

/*mutator*/
int
kd_tree_rebuild(kd_tree_node* root, const int k_dimensions) {
    /*DEBUGGING*/
    kd_tree_get_kd_tree()->_internals->rebuild_counter++; 
    if (kd_tree_is_debug_on())
    {
        printf("REBUILD #,%d=\n", 
                kd_tree_get_kd_tree()->_internals->rebuild_counter);
    }


    int result_size = 0;
    float* info = NULL;
    kd_tree_node* current = NULL;
    kd_tree_node* inorder_traversed = NULL;
    if (NULL != root) {
        current = root;
        /*1)lock all mutator operations allow only*/
        kd_tree_get_kd_tree()->_internals->kd_tree_allow_update = 0;
        /*2)Similar to nanoFlann or flann implementation, split using median 
        a)traverse current tree & get back a vector*/      
        /*START TRAVERSE*/
        int heap_index = 0;
        heap_index = kd_tree_in_order_traversal(current ,k_dimensions);
        /* END TRAVERSAL*/
        result_size = heap_index;
        inorder_traversed = node_knn_result_space;
        /*END TRAVERSE*/
        /*b)extract column wise values*/
        if (result_size > 0) {
            /* START - calculate column medians */
            kd_tree_init_columns_median_processing_space(
                    &columns_median_processing_space, kd_tree_get_rows_size());
           
            int column = 0;
            float median_val2;
            for (; column < k_dimensions; column++) {
                /*printf ("column {");*/
                int row = 0;
                for (; row < result_size; row++) {

                    if (NULL != inorder_traversed[row].dataset) {
                        info = (float *) inorder_traversed[row].dataset;

                        if (NULL != info && info[0] != FLT_MAX) {

                            columns_median_processing_space[row] = 
                            *(info + column);
                            
                        }
                    }
                }/*end row loop*/
                /*printf ("}\n");*/
                /*sort (only using text book median)*/
                /*used before regular median 
                 * insertion_sort(columns_median_processing_space, 
                 * result_size);*/
                /*DEBUGGING printf ("result_size=%d:\n", result_size);*/
                /*step 3- start column median calculation*/
                median_val2 = median(columns_median_processing_space,
                        result_size);
                kd_tree_set_column_median(median_val2,column);
                /*printf ("median_val2=%f:\n", median_val2);*/

            }/*end column median calculation loop */       
    /*4) batch delete entire node_space all tree nodes except the tree.
     Note: we reset the median before delete so we cannot use the APIs,
     delete function, because a new median means we CANNOT find nodes, 
     that were based on the old median. Nor do we want to delete 1 by 1, 
     because thats slow, therefore  we just reset the heap. */      
    int i = 0;
    int c=0;
    for (; i <  kd_tree_get_rows_size(); i++)
    {
         /*reset  values */
        if (NULL!=node_space[i].dataset)
        {
            c=0;
            for (; c<kd_tree_get_k_dimensions(); c++)
            {
                node_space[i].dataset[c] =  FLT_MAX;
            }
        }   
        
        node_space[i].distance_to_neighbor= FLT_MAX;
        node_space[i].left = NULL;
        node_space[i].right=NULL;
        node_space[i].parent= NULL; 
        
    }
    set_current_number_of_kd_tree_nodes(0);
  
    /*printf("DEBUGGING dimension %d, regular_median=%f, median_wirth=%f:\n", 
     column, median_val , median_val2);*/
            /*4) REBUILD kd-tree loop. Add points back based on new medians*/
            i = 0;
            for (; i < result_size; i++) {
                if (
                        NULL != inorder_traversed[i].dataset) {
                    info = (float *) inorder_traversed[i].dataset;

                    if (NULL != info) {

                        kd_tree_add_record(&root,
                                inorder_traversed[i].dataset,
                                0,
                                kd_tree_get_k_dimensions(),
                                0,
                                kd_tree_get_rebuild_threshold());
                    }
                }

            }/*rebuild loop */
       
        }//end if result > 0 
        /*5) Unlock tree update operations. DONE.*/
        kd_tree_get_kd_tree()->_internals->kd_tree_allow_update = 1;
    }//end if root is NOT null 
    return result_size;
}
/*=============================================================================
Function        kd_tree_add_record
Description:    build a kd tree
References:     1) https://www.sanfoundry.com/java-program-find-nearest-
 *                neighbour-using-k-d-tree-search/
                2) https://www.cs.cmu.edu/~ckingsf/bioinfo-lectures/kdtrees.pdf
                3) https://www.geeksforgeeks.org/k-dimensional-tree/
                4) https://github.com/jtsiomb/kdtree 
                5) https://www.tutorialspoint.com/data_structures_algorithms/
 *                 tree_traversal_in_c.htm
Inputs:         tree * root - The  pointer to the root of the kd-tree
 *              float data [] - use data with k_dimensions.
 *              int depth - Current dimension used for building the kd-tree
 *              int k_dimensions - number of columns in the dataset 
 *              (number  of  features).
 *              int copying - Either 0(false) or 1(true). By default 0 is used 
 *              to insert a new record. This function is also used to return
 *              internally new kd-tree used for traversal operations. 
Output:         Returns a pointer to new created or updated kd-tree type tree. 
TODO:           Implement iterative version. 
==========================================================*/

/*mutator*/
void
kd_tree_add_record(kd_tree_node** root, const float key [], int depth,
        const int k_dimensions,
        const int copying, const float rebuild_threshold) {
    /*DONT check/block this function using  kd_tree_allow_update flag, because
    this function is used by kd_tree_rebuild, that will also block the rebuild!*/
    int current_number_of_kd_tree_nodes_val = 
    kd_tree_get_current_number_of_kd_tree_nodes();
    int kd_tree_get_previous_tree_size_val = kd_tree_get_previous_tree_size();
    int rebuild_threshold_val = kd_tree_get_rebuild_threshold();
    float current_ratio = 0.0f;
    float median = 0.0;
    size_t cd = 0;
    /*if we are in the middle of rebuilding dont trigger the rebuild logic again
    that will cause a unexpected behavior*/
    if (kd_tree_get_kd_tree()->_internals->kd_tree_allow_update != 0) {
        //guarding against division by 0 
        if (kd_tree_get_previous_tree_size_val != 0) {
            current_ratio = current_number_of_kd_tree_nodes_val /
                    kd_tree_get_previous_tree_size_val;
            /*rebuild thresh hold has been reached?*/
            if (current_ratio > rebuild_threshold_val) {
                /*call rebuild procedure & send a copy of root for the rebuild*/
                kd_tree_init_node_processing_space_heap(&node_processing_space, 
                        kd_tree_get_k_dimensions());
                /*kd_tree_rebuild will 1st lock all write operations*/
                kd_tree_rebuild(kd_tree_get_root(), k_dimensions);
                kd_tree_set_previous_tree_size(kd_tree_get_current_number_of_kd_tree_nodes());

            }
        } else {
            kd_tree_set_previous_tree_size(kd_tree_get_current_number_of_kd_tree_nodes());
        }
    }
    /*start recursive insert*/
     if (is_empty_node(*root, k_dimensions)) {
        *root = kd_tree_new_node(key, k_dimensions, copying);
        /*was the root set before*/
        if (is_empty_node(kd_tree_get_root(), k_dimensions)) {
            kd_tree_set_root(*root);
        }
    }
    else {
        /* Calculate current dimension (cd) of comparison */
        cd = depth % k_dimensions;
        /*determine current dimension/*/
        /*by using modula operator we can cycle through all dimensions */
        /* and decide the left or right subtree*/
        median = kd_tree_get_column_median(cd);

        if (key[cd] < median) {

           kd_tree_add_record( &(*root)->left, key, depth + 1,
                    k_dimensions,
                    copying, rebuild_threshold);
        } else {


            kd_tree_add_record(&(*root)->right, key, depth + 1,
                    k_dimensions,
                    copying, rebuild_threshold);
        }
    }//end else
    /*end recursive insert*/

}

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
        const float new_data [])
{
 return kd_tree_update_record(root, target_data,  
        new_data,kd_tree_get_k_dimensions());
}

/*=============================================================================
Function        kd_tree_update_record
Description:    Internal API called by friendly kd_tree_update_record. 
 *              which builds a kd-tree. This utilized     
Inputs:         tree * root - The  pointer to the root of the kd-tree
 *              float target_data [] - data to be found and updated 
 *              float new_data [] - that will replace target_data []
Output:         Returns a int flag acting as boolean 0 if update failed or 1 
 *              if operation was successful.     
==========================================================*/
/*mutator*/
int kd_tree_update_record(kd_tree_node* root, const float target_data [],  
        const float new_data [],const int k_dimensions)
{
   int flag = 0;  
   flag = kd_tree_delete_data_point_helper(root, target_data,0, k_dimensions);
   if (flag)
   {
         kd_tree_add_record(&root,
                    new_data,
                    0,
                     k_dimensions,
                    0,
                    kd_tree_get_rebuild_threshold());
   }
   return flag; 
}

/*===========================================================================
Function:       in_order_stack_traversal
Description:    Returns number of  tree nodes that were traversed and set in 
 *              output parameter output_flatten_kd_tree.  
Inputs:         kd_tree_node *root. 
 *              int number_dimensions.
Outputs:        In order array representing all kdtree nodes & returns number               
 *              nodes in the flatten_kd_tree.
References: 
 1) https://tutorialspoint.dev/data-structure/
 2) https://prismoskills.appspot.com/lessons/
 Binary_Trees/Traversal_without_recursion.jsp
 3) MIT Algorithms cormen
 4) Algorithms in C Robert Sedgewick 
==========================================================*/
int
kd_tree_in_order_traversal (kd_tree_node *root, int number_dimensions)
{
 
    /* set current to root*/
    kd_tree_node* curr = root; 
    int heap_index= 0; 
    
    if (!is_empty_node(curr,number_dimensions))
    {
          kd_tree_init_node_knn_result_heap(&node_knn_result_space,
          kd_tree_get_k_dimensions());
     
    while (!is_empty_node(curr,number_dimensions))
    {
        if (!is_empty_node(curr->left,number_dimensions)) 
        {
            /*find inorder predecessor of left subtree*/
            kd_tree_node* pre = curr->left;
            while (!is_empty_node(pre->right,number_dimensions) && 
                  !kd_tree_points_equal(pre->right->dataset,
                  curr->dataset,number_dimensions))
            {
                pre = pre->right;
            }
            if (is_empty_node(pre->right,number_dimensions)) 
            {
                /*Threaded property for this node was not set. Set it now*/
                pre->right = curr;
                curr = curr->left; // Thread created. Switch to this subtree now
            } else 
            {
                /* This means pre.right = curr
                 Which implies that thread for this was set already, 
                So we do not need to traverse this subtree again*/
                pre->right = NULL; // reset the thread we created earlier
                 if (!is_empty_node(curr,number_dimensions)) 
                 {
                    memcpy(node_knn_result_space[heap_index].dataset, 
                            curr->dataset,
                            sizeof (float)*k_dimensions);
                    heap_index++; 
                 }
                curr = curr->right;
            }
        } else 
        {
               if (!is_empty_node(curr,number_dimensions)) 
                 {                  
                    memcpy(node_knn_result_space[heap_index].dataset, 
                            curr->dataset,
                            sizeof (float)*k_dimensions);
            heap_index++; 
               }
            curr = curr->right; 
        }
    }
    
    }
       return heap_index; 
}


/*=============================================================================
Function:       kd_tree_search_data_point
Description:    User friendly wrapper.  
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
int kd_tree_search_data_point(kd_tree_node* root, const float data[])
{
  return kd_tree_search_helper (root, data, 0,kd_tree_get_k_dimensions() );
}
/*=============================================================================
Function:       search_tree
Description:    traverses the tree by either going left if the key is larger
 * than current node's info or going left if the key is smaller than current 
 * nodes value. Ultimately reaching the key if it exists. This traversal is 
 * O(log n) and considered optimal. 
Input:          tree * root - root of of the kd-tree used to start traversal
 *              float data[] - query point used as search parameter.
 *              int depth - Current dimension used for building the kd-tree
 *              int k_dimensions - number of columns in the dataset              
Output:         Returns  0 (false) with NOT found. Returns 1 (true) if found.
=============================================================================*/
int
kd_tree_search_helper(kd_tree_node* root, const float data[], int depth, 
        const int k_dimensions) {
    /*flag false*/
    int flag = 0;
    size_t cd = 0;
    float median = 0.0f;
    if (!is_empty_node(root,k_dimensions)) {
        kd_tree_node* current = root;
        //while loop to find target node for deletion. 
        while (NULL != current) {
            //if found then break;  
            if (kd_tree_points_equal(current->dataset, data, k_dimensions)) {
                /*flag true*/
                flag = 1;
                break;
            }
            else

             /* Calculate current dimension (cd) of comparison */
             cd = depth % k_dimensions;
            /*determine current dimension/*/
            /*by using modula operator we can cycle through all dimensions */
            /* and decide the left or right subtree*/
            median = kd_tree_get_column_median(cd);
            /*printf("kd_tree_search_helper(), median=%f\n",median);*/

            if (data[cd] < median) {
                current = current->left;
            } else {
                current = current->right;
            }
            
            depth++;

        }//end traverse while 

    }//end if root is NOT 
    return  flag; 
}

/*=============================================================================
Function:       kd_tree_points_equal
Description:    A utility function to determine equality                
 *              in multidimensional space. Returns 1 if                 
 *              true & 0 if false.  
TODO:          change signature to use pointers  
=============================================================================*/
int kd_tree_points_equal(const float point1[],const  float point2[], 
        int k_dimensions)
{
    int flag = 1;
    if (NULL == point1 || NULL == point2)
    {
        flag = 0; 
    }
    else
    {
        float size = k_dimensions;
        /*compare each index*/
        int i = 0;
        for (; i < size; i++)
        {
            if (point1[i] != point2[i])
            {
                /*false*/
                flag = 0;
                break;
            }
        }
    }
    return flag;
}


 /*=============================================================================
Function        n_dimensional_euclidean
Description:    calculate the Euclidean distance in n dimensional space.
Input:          float values_1 [] - float array with k dimensions
  *             float values_2 [] - float array with k dimensions
Output:         Returns distance. 
References:     https://en.wikipedia.org/wiki/Euclidean_distance 
==========================================================*/
float
kd_tree_n_dimensional_euclidean (const float values_1 [],
                                 const float values_2 [],
                                 const int k_dimensions)
{
    float total_distance = 0;
    float distance = 0;
    int i = 0;
    int size = k_dimensions;
    for (; i < size; i++)
    {
        distance = values_1[i] - values_2[i];
        total_distance = total_distance + (distance * distance);
        /*Note: we actually need calculate Euclidean & the "Manhattan
         * distance/Manhattan norm" will NOT be useful since the 
         * knn_based_radious() is radius search, therefore actual distance is 
         needed. 
         */

    }

    return sqrt (total_distance);
}

/*===========================================================================
Function        knn algorithm to find N nearest  neighbors. 
Description:    Given a root to traverse and a data point, this function 
 *              attempts to find nearest neighbors to that data point
 *              within a specific Euclidean distance or radius.
Inputs:         tree * root - tree root which is traversed n order to 
 *              attempt to find the data[].
 *              float data_point[] - query point is as search parameter. 
 *              int depth - current dimension used for comparison. Should be 0. 
 *              int k_dimensions - number of  features 
                int number_of_nearest_neighbors - desired number of results
 *              int sort_result - should results be sorted. 0  is false any
 *              other number is true.  
Outputs:        int - number of neighbors found. 
 *              tree** n_nearests - pointer to array of points of type
 *              tree that represent the nearest neighbors.
References:     Foundations of Multidimensional and Metric Data Structures
 *              By Hanan Samet Chapter 4 
Notes:          This implementation is O(N) running time. Although the tree
 *              is traversed  using kd-tree which is O(log N) in complexity
 *              we are going through all tree nodes and not actually search but 
 *              calculating all distances away from our point of interest,
 *              hence it becomes O(N) traversal. Use friendly wrapper to knn().
==========================================================*/
int
kd_tree_knn(kd_tree_node* const root, const float data_point[],
            int number_of_nearest_neighbors)
{
    return kd_tree_knn_helper(root,data_point,kd_tree_get_k_dimensions(),
               number_of_nearest_neighbors);
}

/*===========================================================================
Function        knn, knn algorithm  using kd-tree.
Description:    Given a root to traverse and a data point, this function 
 *              attempts to find nearest neighbors to that data point
 *              using Euclidean distance.
Inputs:         
Outputs:
References:     Foundations of Multidimensional and Metric Data Structures
 *              By Hanan Samet Chapter 4 
Notes:          This implementation is O(log N) running time.The tree
 *              is traversed  using kd-tree which is O(log N) in complexity
 *              we are calculating all distances away from our point of interest.
 TODO:          We can further optimize using IncNearest Algorithm 
                Hanan Samet Chapter 4  
==========================================================*/
int kd_tree_knn_helper(kd_tree_node* const root, const float data_point[],
        const int k_dimensions,
        int number_of_nearest_neighbors) {

    int nearest_counter = 0;
    int heap_index = 0;
    kd_tree_node* current = NULL;
    size_t cd = 0;
    int depth = 0;
    float median = 0.0f;
    float distance = 0.0f;
    if (root != NULL) {
        /*init result heap*/
        kd_tree_init_node_knn_result_heap(&node_knn_result_space,
                kd_tree_get_k_dimensions());
        /*set to current pointer*/
        current = root;
        /*while current is NOT null*/
        while (!is_empty_node(current, k_dimensions)) {
                /*CALCULATE DISTANCE*/
                distance=kd_tree_n_dimensional_euclidean(data_point,
                        current->dataset,
                        k_dimensions);
                /*we will simulate a priority queue- therefore if array is NOT full*/
                if (heap_index < kd_tree_get_rows_size()) {
                    /*insert in queue*/
                    /*insert data in heap by copying memory*/
                    memcpy(node_knn_result_space[heap_index].dataset, 
                            current->dataset,
                            sizeof (float)*k_dimensions);
                    node_knn_result_space[heap_index].distance_to_neighbor = 
                            distance;
                    heap_index++;
                } else {
                    /*sort ascending*/
                    insertion_sort_based_on_distance2(node_knn_result_space,
                            nearest_counter);
                    /*is the smallest distance greater than current distance?*/
                    if (node_knn_result_space[0].distance_to_neighbor > 
                            distance) {
                        /*insert in array at the top, index=0*/
                        /*insert data in heap by copying memory*/
                        memcpy(node_knn_result_space[0].dataset, 
                                current->dataset,
                                sizeof (float)*k_dimensions);
                        node_knn_result_space[0].distance_to_neighbor = 
                                distance;

                    }
                }
            
            /* Calculate dimension (cd) of comparison */
            cd = depth % k_dimensions;
            /*using modula operator cycle through all dimensions */
            median = kd_tree_get_column_median(cd);
            //printf("kd_tree_add_record.(), median=%f\n",median);
            /* left or right subtree*/

            if (data_point[cd] < median) {
                current = current->left;

            } else {
                current = current->right;

            }
            depth++;
        }/*end while*/
        /*END TRAVERSE */
        nearest_counter = heap_index;
     
        /*if there are enough tree nodes to warrant a sort */
        if (nearest_counter > 0) {
            /* sort */
            insertion_sort_based_on_distance2(node_knn_result_space,
                    nearest_counter);
        }

    }/*end if inputs are valid */

    /*if actual neighbors was less than requested*/
    if (nearest_counter < number_of_nearest_neighbors) {
        number_of_nearest_neighbors = nearest_counter;
    }

    return number_of_nearest_neighbors;

}
/*===========================================================================
Function        knn, knn algorithm  using kd-tree.
Description:    Given a root to traverse and a data point, this function 
 *              attempts to find nearest neighbors to that data point
 *              within a  Euclidean distance or radius.
Inputs:         
Outputs:
References:     Foundations of Multidimensional and Metric Data Structures
 *              By Hanan Samet Chapter 4 
Notes:          This implementation is O(log N) running time.The tree
 *              is traversed  using kd-tree which is O(log N) in complexity
 *              we are calculating all distances away from our point of interest.
 TODO:          We can further optimize using IncNearest Algorithm 
                Hanan Samet Chapter 4 
==========================================================*/
int
kd_tree_knn_based_on_radius_helper(kd_tree_node* root, const float data_point[],
        const int k_dimensions,
        float range_from_data_point) {
    int nearest_counter = 0;
    int heap_index = 0;
    kd_tree_node* current = NULL;
    size_t cd = 0;
    int depth = 0;
    float median = 0.0f;
    float distance = 0.0f;
    if (root != NULL) {
        /*init result heap*/
        kd_tree_init_node_knn_result_heap(&node_knn_result_space,
                kd_tree_get_k_dimensions());
        /*set to current pointer*/
        current = root;
        /*while current is NOT null*/
        while (!is_empty_node(current, k_dimensions)) {
            /*CALCULATE DISTANCE*/
            distance = kd_tree_n_dimensional_euclidean(data_point,
                    current->dataset,
                    k_dimensions);
            if (distance <= range_from_data_point) {
                  /*we will simulate a priority queue- therefore if array is NOT full*/
                if (heap_index < kd_tree_get_rows_size()) {
                    /*insert in queue*/
                    /*insert data in heap by copying memory*/
                    memcpy(node_knn_result_space[heap_index].dataset, 
                            current->dataset,
                            sizeof (float)*k_dimensions);
                    node_knn_result_space[heap_index].distance_to_neighbor = 
                            distance;
                    heap_index++;
                } else {
                    /*sort ascending*/
                    insertion_sort_based_on_distance2(node_knn_result_space,
                            nearest_counter);
                    /*is the smallest distance greater than current distance?*/
                    if (node_knn_result_space[0].distance_to_neighbor > 
                            distance) {
                        /*insert in array at the top, index=0*/
                        /*insert data in heap by copying memory*/
                        memcpy(node_knn_result_space[0].dataset, 
                               current->dataset,
                               sizeof (float)*k_dimensions);
                        node_knn_result_space[0].distance_to_neighbor = 
                        distance;

                    }
                }
            }
            /* Calculate dimension (cd) of comparison */
            cd = depth % k_dimensions;
            /*using modula operator cycle through all dimensions */
            median = kd_tree_get_column_median(cd);
            //printf("kd_tree_add_record.(), median=%f\n",median);
            /* left or right subtree*/

            if (data_point[cd] < median) {
                current = current->left;

            } else {
                current = current->right;

            }
            depth++;
        }/*end while*/
        /*END TRAVERSE */
        nearest_counter = heap_index;

        /*if there are enough tree nodes to warrant a sort */
        if (nearest_counter > 0) {
            /* sort */
            insertion_sort_based_on_distance2(node_knn_result_space,
                    nearest_counter);
        }

    }/*end if inputs are valid */
    return nearest_counter;
}/*end function */


int
kd_tree_knn_based_on_radius (kd_tree_node* root, 
                    const float data_point[],
                    float range_from_data_point)
{
    return kd_tree_knn_based_on_radius_helper (root,data_point, 
                    kd_tree_get_k_dimensions(), 
                    range_from_data_point);
}
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
void
kd_tree_delete_data_point(kd_tree_node* root, const float data_point []) {

    if (NULL != kd_tree_get_kd_tree()) {
        if (kd_tree_get_kd_tree()->_internals->kd_tree_allow_update) {
            kd_tree_delete_data_point_helper(root,
                    data_point,
                    0,
                    kd_tree_get_k_dimensions());
        } else {
            printf("kd_tree_delete_point(),"
                    " kd_tree is locked for rebuild!");
        }
    }

}




/*===========================================================================
Function        delete_data_point
Description:    Given a data_point (query point) attempts to delete
 *              and return that point from the kd-tree. After deletion 
 *              subsequent calls to  delete_data_point same data point will 
 *              return NULL. After deletion calls to search will return 0.
 *              Internal API.
Input:          tree * root - tree root which is traversed
                float data_point [] - query point is as search parameter.
Output:         Returns tree pointer to delete tree. 
References:     https://www.geeksforgeeks.org/k-dimensional-tree-set-3-delete/
==========================================================*/
//if (kd_tree_points_equal(current->dataset, data_point, k_dimensions)) 
// is_empty_node(root, k_dimensions))
// Test : https://www.geeksforgeeks.org/binary-search-tree-set-3-iterative-delete/

int kd_tree_delete_data_point_helper(kd_tree_node* root, float const
        data_point [], int depth, const int k_dimensions) {
    /*calculate  current dimension */
    size_t cd = 0;
    float median = 0.0;
    kd_tree_node* current = NULL;
    kd_tree_node* parent = NULL;
    int flag =0; 

    if (!is_empty_node(root, k_dimensions)) {

        current = root;

       //while loop to find target node for deletion. 
        while (NULL != current) {
           //if found then break;  
           if (kd_tree_points_equal(current->dataset, data_point, k_dimensions)) \
                {
                    break; 
                }
           
           else
           {
             parent  = current;
            /* Calculate current dimension (cd) of comparison */
            cd = depth % k_dimensions;
            /*determine current dimension/*/
            /*by using modula operator we can cycle through all dimensions */
            /* and decide the left or right subtree*/
            median = kd_tree_get_column_median(cd);
            /*printf("kd_tree_delete_data_point(), median=%f\n", median);*/
            
            if (data_point[cd] < median) {
                current = current->left;
            } else {
                current = current->right;
            }
            depth++;
            
           }

        }//end while current is NOT null
        
            //if found. attempt to delete based on Hibbard Algorithm 
            if (!is_empty_node(current, k_dimensions)) {
                    //If node has no children
                if (current->left == NULL && current->right == NULL) {
                    if (parent != NULL) {
                        if (parent->left == current) {
                            parent->left = NULL;
                        } else {
                            parent->right = NULL;
                        }
                    }
                   
                    //DELETE START 
                    /*special case current is root*/
                    /*start delete*/
                    if (NULL != current->dataset) {
                        int i = 0;
                        for (; i < k_dimensions; i++) {
                            current ->dataset[i] = FLT_MAX;
                        }
                    }
                    current->distance_to_neighbor = FLT_MAX;
                    current->left = NULL;
                    current->right = NULL;
                    //DELETE END

                } 
                //If current has one child
                else if ((current->left == NULL && current->right != NULL) || 
                        (current->left != NULL && current->right == NULL)) {
                    //If node has a right child 
                    if (current->right != NULL && current->left == NULL) {
                          if (parent != NULL) {
                        if (parent->right == current) {
                            parent->right = current->right;
                        } else if (parent->left == current) {
                            parent->left = current->right;
                        }
                          }
                          else
                          {
                        //else special case this is the root 
                        kd_tree_set_root(current->right); 
                    
                          }
                    } 
                    //If node has a left child
                    if (current->left != NULL && current->right == NULL) {
                          if (parent != NULL) {
                        if (parent->right == current) {
                            parent->right = current->left;
                        } else if (parent->left == current) {
                            parent->left = current->left;
                        }
                          }
                          else
                          {
                          
                        //else special case this is the root 
                        kd_tree_set_root(current->left); 
                   
                          
                          }
                    }
                    //DELETE START 
                    /*special case current is root*/
                    /*start delete*/
                    if (NULL != current ->dataset) {
                        int i = 0;
                        for (; i < k_dimensions; i++) {
                            current ->dataset[i] = FLT_MAX;
                        }
                    }
                    current->distance_to_neighbor = FLT_MAX;
                    current->left = NULL;
                    current->right = NULL;
           
                    /*end delete*/
                    //DELETE END
                  
                }
                    //If current has two children
                else {
                    kd_tree_node* swap_this = current->right;
                    kd_tree_node* swap_this_prev = current;

                    while (swap_this->left != NULL) {
                        swap_this_prev = swap_this;
                        swap_this = swap_this->left;
                    }
                    memcpy(current->dataset, swap_this->dataset,
                            sizeof (float)*k_dimensions);

                    if (swap_this_prev->left == swap_this) {
                        swap_this_prev->left = swap_this->right;
                    } else if (swap_this_prev->right == swap_this) {
                        swap_this_prev->right = swap_this->right;
                    }
                    
                    //DELETE START 
                    /*special case swap_this is root*/
                    /*start delete*/
                    if (NULL != swap_this->dataset) {
                        int i = 0;
                        for (; i < k_dimensions; i++) {
                            swap_this ->dataset[i] = FLT_MAX;
                        }
                    }
                    swap_this->distance_to_neighbor = FLT_MAX;
                    swap_this->left = NULL;
                    swap_this->right = NULL;
                    /*end delete*/
                 
                }
                flag = 1;
                /* decrement total nodes count */
                kd_tree_decrement_current_number_of_kd_tree_nodes();
            }


    }
    
    return flag; 

}//end function 
    

/*=============================================================================
Implementations - memory management  
==============================================================================*/
kdtree_t * kdtree_alloc(int max_rows, int max_cols) {

    if (max_rows>0 && max_cols>0)
    {
    /*tree*/
    self = kd_tree_alloc_tree_space();
    kd_tree_processing_space = kd_tree_alloc_tree_processing_space();
    /*tree internals*/
    if (NULL != self) {
        kd_tree_get_tree_space()->_internals = kd_tree_alloc_internals();
        kd_tree_alloc_tree_processing_space()->_internals =
                kd_tree_alloc_internals();
    }
    /*nodes*/
    kd_tree_alloc_node_space_heap(max_rows,max_cols);
    kd_tree_alloc_node_processing_space_heap(max_rows,max_cols);
    kd_tree_alloc_node_knn_result_heap();
    /*batch processing*/
    kd_tree_alloc_batch_processing_heap();
    /*median*/
    kd_tree_alloc_columns_median_heap(max_cols);
    kd_tree_alloc_columns_median_processing_space(max_rows);
    }
    else
    {
       printf("kdtree_alloc(), Error max_rows and max_columns need to be"
               "greater than 0.\n");
    }
    return self;
}

void kdtree_init(kdtree_t* self) {
    if (NULL!=self)
    {
    /*initially extra debug is off*/
    self->is_debug_run =0;  
    /*tree*/
    kd_tree_init_tree_space(self);
    kd_tree_init_tree_space(kd_tree_get_processing_space());
    /*tree internals*/
    kd_tree_init_tree_internals(self);
    kd_tree_init_tree_internals(kd_tree_get_processing_space());
    /*nodes*/
    kd_tree_init_node_heap(&node_space,
            kd_tree_get_k_dimensions());
    kd_tree_init_node_processing_space_heap(&node_processing_space,
            kd_tree_get_k_dimensions());
    kd_tree_init_node_knn_result_heap(&node_knn_result_space,
            kd_tree_get_k_dimensions());
    /*batch processing*/
    kd_tree_init_batch_processing_heap(&batch_node_processing_space,
            kd_tree_get_k_dimensions());
    /*median*/
    kd_tree_init_columns_median_heap(&columns_median_space,
            kd_tree_get_k_dimensions());
    
    kd_tree_init_columns_median_processing_space(&columns_median_processing_space,
            kd_tree_get_rows_size());
    }
    else
    {
        printf("kdtree_init(), Error, Alloc was not successful, "
        "therefore init will NOT run!");
    }
    
}

void kdtree_free(kdtree_t* self) {
    /*median*/
    kd_tree_free_columns_median_processing_space(&columns_median_processing_space);
    kd_tree_free_columns_median_space(&columns_median_space); 
//    if (columns_median_space)
//    {
//    free (columns_median_space);
//    }
    /*nodes*/
    kd_tree_free_node_space(&node_space);
    kd_tree_free_node_processing_space_heap(&node_processing_space);
    kd_tree_free_node_knn_result_heap(&node_knn_result_space);
    kd_tree_free_batch_processing_heap(&batch_node_processing_space);
    /*tree internals*/
    kd_tree_free_internals(self);
    kd_tree_free_internals(kd_tree_get_processing_space());
    /*tree*/
    kd_tree_free_tree_space(self);
    kd_tree_free_tree_space(kd_tree_get_processing_space());
}

/*=============================================================================
Function        kd_tree_alloc_internals
Description:    
==========================================================*/
struct kdtree_internals* kd_tree_alloc_internals(void)
{
    kdtree_internals* internals = calloc(1, sizeof (internals));
    return internals;
}
 


/*Implementations - init internals*/
void kd_tree_init_tree_internals(kdtree_t* tree) {
    if (NULL != tree && NULL != tree->_internals) {
        tree->_internals->current_number_of_kd_tree_nodes = 0;
        tree->_internals->kd_tree_allow_update = 1;
        tree->_internals->previous_tree_size = 0;
        tree->_internals->rebuild_threshold = REBUILD_THRESHOLD;
        tree->_internals->rebuild_counter = 0; 
    }
}

/*free*/
void kd_tree_free_internals(kdtree_t* tree) {
    if (NULL != tree && NULL != tree->_internals) {
        free(tree->_internals);
    }
}


/*Implementations - kdtree_t heap*/
/*alloc*/
/*=============================================================================
Function        kd_tree_alloc_tree_space
Description:    

==========================================================*/
kdtree_t*
kd_tree_alloc_tree_space()
{
    if (NULL== self)
    {
     self = (kdtree_t*) calloc (1, sizeof (kdtree_t));
    }
     return self;
}

/*init*/
void
kd_tree_init_tree_space(kdtree_t* self)
{
    if (NULL!= self)
    {
     self->_internals = kd_tree_alloc_internals();
    }
   
}

/*free*/
void kd_tree_free_tree_space(kdtree_t* self)
{
     /*reset kd_tree*/
     free(self);
     self=NULL;
}

/*get*/
kdtree_t* kd_tree_get_tree_space()
{
return self; 
}

/*Implementations - kdtree_t processing heap*/

/*=============================================================================
Function        kdtree_t_alloc_tree_processing_space
Description:    

==========================================================*/
/*alloc*/
kdtree_t* kd_tree_alloc_tree_processing_space()
{
     if (NULL== kd_tree_processing_space)
    {
    kd_tree_processing_space = (kdtree_t*) calloc (1, sizeof (kdtree_t));
    
    }
     return kd_tree_processing_space;
}

/*init*/
void kd_tree_init_kdtree_processing_space(kdtree_t* tree)
{
    if (NULL!=tree)
    {
       tree->_internals =  kd_tree_alloc_internals();
       tree->_root = NULL; 
    }

}
/*free*/
void kd_tree_free_kdtree_processing_space(kdtree_t* tree)
{
    free(tree);
    tree = NULL; 
}

/*get*/
kdtree_t*  kd_tree_get_processing_space()
{
    return kd_tree_processing_space;
}

/*Implementations - node_space heap*/
/*=============================================================================
Function        kd_tree_alloc_node_space_heap 
Description:    pre-allocates fixed heap size  KD_TREE_HEAP_SIZE for kd tree  
 *              data structure.  This method MUST be called before using this
 *              library. 
==========================================================*/
void kd_tree_alloc_node_space_heap (int rows, int max_dimensions)
{
    node_space =(kd_tree_node*) calloc(rows,
            sizeof (kd_tree_node));
    
    kd_tree_set_k_dimensions (max_dimensions);
    kd_tree_set_rows_size(rows);
    int i = 0;
    for (; i < rows; i++)
    {
        node_space[i].dataset= (float*) calloc (max_dimensions, sizeof(float)); 
        node_space[i].left = NULL; 
        node_space[i].right = NULL; 
        node_space[i].parent = NULL; 
    }
}
/*=============================================================================
Function        get_pre_allocated_kd_heap
Description:    Returns available static memory to new_node() function. This
 *              memory represents the kd tree. 
 *              The first fit algorithm ensures this function returns next
 *              available block from tree_space.
 *              Must call init_heap() ONLY once before using this 
 *              function. 
References:     Mastering the FreeRTOS™ Real Time Kernel page page 29 to 32. 
==========================================================*/
kd_tree_node*
get_pre_allocated_kd_tree_node_heap() {
    int i = 0;
    int c = 0;
    int success = 0;
    while (i < kd_tree_get_rows_size() && success!=1) {
        if (NULL != node_space[i].dataset) {
            c = 0;
            //inner loop for data_set 
            for (; c < kd_tree_get_k_dimensions(); c++) {
                /*Note:empty memory is set to  FLT_MAX not NULL*/
                if (node_space[i].dataset[c] ==  FLT_MAX) {
                    success = 1;
                    break;
                }
                else
                {
                    i++;
                }
            }//end inner 
            
        }
       
        
        
    }//end while 
    if (!success) {
        printf("Error, get_pre_allocated_kd_tree_node_heap() No more heap!");
    }
    
    return node_space+i;

}


/*=============================================================================
Function        init_kd_tree_node_heap
Description:    initializes the values nodes in kd_tree. Clears the kd tree. 

==========================================================*/
void kd_tree_init_node_heap(kd_tree_node** nodes, int max_dimensions)
{
    if (NULL!=nodes && NULL!=(*nodes))
    {
    kd_tree_init_tree_internals(kd_tree_alloc_tree_space());
    kd_tree_set_k_dimensions (max_dimensions);
    
    int i = 0;
    int c=0;
    for (; i <  kd_tree_get_rows_size(); i++)
    {
         /*reset  values */
       
        if (NULL!=(*nodes)[i].dataset)
        {
            c=0;
            for (; c<kd_tree_get_k_dimensions(); c++)
            {
                (*nodes)[i].dataset[c] =  FLT_MAX;
            }
        }   

        (*nodes)[i].distance_to_neighbor= FLT_MAX;
        (*nodes)[i].left = NULL;
        (*nodes)[i].right = NULL;
        (*nodes)[i].parent = NULL; 
        
        
    }

      set_current_number_of_kd_tree_nodes(0);
      kd_tree_set_previous_tree_size(0);
    }
  
}

void kd_tree_init_node_processing_space_heap(kd_tree_node** nodes, 
        int max_dimensions) {
    if (NULL != nodes && NULL != (*nodes)) {

        int rows = kd_tree_get_rows_size() * 2;
        int i = 0;
        int c = 0;
        for (; i < rows; i++) {
            /*reset  values */
            if (NULL != (*nodes)[i].dataset) {
                c = 0;
                for (; c < kd_tree_get_k_dimensions(); c++) {
                    (*nodes)[i].dataset[c] = FLT_MAX;
                }
            }
            (*nodes)[i].distance_to_neighbor= FLT_MAX;
            (*nodes)[i].left = NULL;
            (*nodes)[i].right = NULL;
            (*nodes)[i].parent = NULL; 


        }
    }

}

/*=============================================================================
Function       free_kd_tree_node_space
Description:   frees all kd_tree_node_space memory. After this call init
 *              MUST BE CALLED.  
Notes:
==========================================================*/
void kd_tree_free_node_space(kd_tree_node** nodes) {
    if (NULL != nodes && NULL != (*nodes)) {
        int i = 0;
        for (; i < kd_tree_get_rows_size(); i++) {

            free((*nodes)[i].dataset);
            (*nodes)[i].dataset = NULL;
            (*nodes)[i].left = NULL;
            (*nodes)[i].right = NULL;
            (*nodes)[i].parent = NULL; 

        }

        free(*nodes);
        *nodes = NULL;
    }
}


/*=============================================================================
Function        kd_tree_init_node_knn_result_heap
Description:    initializes the values nodes in kd_tree. Clears the kd tree. 

==========================================================*/
void kd_tree_init_node_knn_result_heap(kd_tree_node** nodes, int max_dimensions)
{
    if (NULL!=nodes && NULL!=(*nodes))
    {
    int i = 0;
    int c=0;
    for (; i <  kd_tree_get_rows_size(); i++)
    {
         /*reset  values */
        if (NULL!=(*nodes)[i].dataset)
        {
            c=0;
            for (; c<kd_tree_get_k_dimensions(); c++)
            {
                (*nodes)[i].dataset[c] =  FLT_MAX;
            }
        }        
        (*nodes)[i].distance_to_neighbor= FLT_MAX;
        
        
    }
    
    }
   
}
/*=============================================================================
Function       free_kd_tree_node_space
Description:   frees all kd_tree_node_space memory. After this call init
 *              MUST BE CALLED.  
Notes:
==========================================================*/
void kd_tree_free_node_knn_result_heap(kd_tree_node** nodes) {
    if (NULL != nodes && NULL != (*nodes)) {
        int i = 0;
        for (; i < kd_tree_get_rows_size(); i++) {

            free((*nodes)[i].dataset);
            (*nodes)[i].dataset = NULL;
            (*nodes)[i].left = NULL;
            (*nodes)[i].right = NULL;
            (*nodes)[i].parent = NULL; 

        }

        free(*nodes);
        *nodes = NULL;
    }
}


/*Implementations - node_processing_space heap*/
/*=============================================================================
Function        alloc_kd_tree_node_processing_space_heap
Description:    pre-allocates fixed processing heap size  KD_TREE_HEAP_SIZE for
 *              kd tree data structure.  This method MUST be called before using 
 *              this library. This is temporary heap used  by kd tree functions,
 *              in order to, copy, add , rebuild, and return nearest 
 *              neighbors. 
==========================================================*/
void
kd_tree_alloc_node_processing_space_heap(int rows, int max_dimensions) {
    
    //NOTE: processing heap size is rows*2
    rows = rows * 2; 
    node_processing_space = (kd_tree_node*) calloc(rows,
            sizeof (kd_tree_node));
    int i = 0;
    for (; i < rows; i++) {
       
         node_processing_space[i].dataset= (float*) calloc (max_dimensions, 
                 sizeof(float)); 
         node_processing_space[i].left = NULL; 
         node_processing_space[i].right = NULL;
         node_processing_space[i].parent = NULL; 
       
    }

}


kd_tree_node*
get_pre_allocated_processing_heap()
{
    int rows = kd_tree_get_rows_size() *2; 
    int i = 0;
    int c = 0;
    int success = 0;
    for (; i <  rows && success!=1; i++) {
        if (NULL != node_processing_space[i].dataset) {
            c = 0;
            for (; c < kd_tree_get_k_dimensions(); c++) {
                /*Note:empty memory is set to  FLT_MAX not NULL*/
                if (node_processing_space[i].dataset[c] ==  FLT_MAX) {
                    success = 1;
                    break;
                }
            }
        }
    }
    if (!success) {
        printf("Error:get_pre_allocated_processing_heap() No more heap!");
    }

    return &node_processing_space[i];
}
/*=============================================================================
Function       kd_tree_free_node_processing_space_heap
Description:   frees all node_processing_space_heap memory. After this call init
 *              MUST BE CALLED.  
Notes:
==========================================================*/
void kd_tree_free_node_processing_space_heap(kd_tree_node** nodes) {
    if (NULL != nodes && NULL != (*nodes)) {
        int rows = kd_tree_get_rows_size() *2;
        int i = 0;
        for (; i < rows; i++) {

            free((*nodes)[i].dataset);
            (*nodes)[i].dataset = NULL;
            (*nodes)[i].left = NULL;
            (*nodes)[i].right = NULL;
            (*nodes)[i].parent = NULL; 

        }

        free(*nodes);
        *nodes = NULL;
    }
}


/*knn result space*/
void kd_tree_alloc_node_knn_result_heap()
{
   
    int rows = kd_tree_get_rows_size(); 
    node_knn_result_space = (kd_tree_node*) calloc(rows,
            sizeof (kd_tree_node));
    int i = 0;
    for (; i < rows; i++) {
       
         node_knn_result_space[i].dataset= (float*) calloc (rows, 
                 sizeof(float)); 
         node_knn_result_space[i].left = NULL; 
         node_knn_result_space[i].right = NULL;
         node_knn_result_space[i].parent = NULL; 
       
    }
 
}
kd_tree_node*
get_knn_result_heap()
{
    
    int i = 0;
    int c = 0;
    int success = 0;
    while (i < kd_tree_get_rows_size() && success!=1) {
        if (NULL != node_knn_result_space[i].dataset) {
            c = 0;
            //inner loop for data_set 
            for (; c < kd_tree_get_k_dimensions(); c++) {
                /*Note:empty memory is set to  FLT_MAX not NULL*/
                if (node_space[i].dataset[c] ==  FLT_MAX) {
                    success = 1;
                    break;
                }
                else
                {
                    i++;
                }
            }//end inner 
            
        }
      
    }//end while 
    if (!success) {
        printf("Error:get_pre_allocated_kd_tree_node_heap() No more heap!");
    }
   
    return node_knn_result_space+i; 
}

/*Implementations batch processing heap */
/*alloc*/
void kd_tree_alloc_batch_processing_heap()
{
   
    int rows = kd_tree_get_rows_size(); 
    batch_node_processing_space = (kd_tree_node*) calloc(rows,
            sizeof (kd_tree_node));
    int i = 0;
    for (; i < rows; i++) {
       
         batch_node_processing_space[i].dataset= (float*) calloc (rows, 
                 sizeof(float)); 
         batch_node_processing_space[i].left = NULL; 
         batch_node_processing_space[i].right = NULL;
         batch_node_processing_space[i].parent = NULL; 
       
    }
}
/*get*/
kd_tree_node*
get_kd_tree_batch_processing_heap()
{
    int rows = kd_tree_get_rows_size();
    int i = 0;
    int c = 0;
    int success = 0;
    for (; i <  rows && success!=1; i++) {
        if (NULL != batch_node_processing_space[i].dataset) {
            c = 0;
            for (; c < kd_tree_get_k_dimensions(); c++) {
                /*Note:empty memory is set to  FLT_MAX not NULL*/
                if (batch_node_processing_space[i].dataset[c] ==  FLT_MAX) {
                    success = 1;
                    break;
                }
            }
        }
    }
    if (!success) {
        printf("Error:kd_tree_alloc_batch_processing_heap() No more heap!");
    }

    return &batch_node_processing_space[i];
}
/*init*/
void kd_tree_init_batch_processing_heap(kd_tree_node** nodes, int max_dimensions)
{
       if (NULL!=nodes && NULL!=(*nodes))
    {
    int i = 0;
    int c=0;
    for (; i <  kd_tree_get_rows_size(); i++)
    {
         /*reset  values */
        if (NULL!=(*nodes)[i].dataset)
        {
            c=0;
            for (; c<max_dimensions; c++)
            {
                (*nodes)[i].dataset[c] =  FLT_MAX;
            }
        }        
        (*nodes)[i].distance_to_neighbor= FLT_MAX;
        
        
    }
    
    }
}
/*free*/
void kd_tree_free_batch_processing_heap(kd_tree_node** nodes)
{
    free(*nodes);
    *nodes = NULL; 
}


/*Implementations - median heap*/
/*=============================================================================
Function        init_column_statistics_heap
Description:     
==========================================================*/
void kd_tree_alloc_columns_median_heap(int k_dimensions)
{
    /*the column median heap is the same size as the feature dimensions*/
    columns_median_space =(float*)calloc( k_dimensions,
                                             sizeof(float));
}

/*=============================================================================
Function         reset_entire_column_statistics_heap
Description:     
==========================================================*/
void kd_tree_init_columns_median_heap(float** medians, int k_dimensions) {
    if (NULL != medians && NULL!=(*medians)) {
        int i = 0;
        for (; i < k_dimensions; i++) {
            /*reset*/
            (*medians)[i] = FLT_MAX;

        }
    }
}

/*=============================================================================
Function         kd_tree_get_columns_median_heap
Description:     
==========================================================*/
float* kd_tree_get_columns_median_heap()
{
    return columns_median_space; 
}


/*=============================================================================
Function        set_column_median
Description:     
==========================================================*/
void kd_tree_set_column_median(float median, int column_index)
{
    /*DONT lock this operation with  kd_tree_allow_update
    the column median heap is the same size as the feature dimensions*/
    if  (column_index<=kd_tree_get_k_dimensions())
    {
    columns_median_space[column_index] = median;
    }
                                            
}
/*=============================================================================
Function         get_column_median
Description:     
==========================================================*/
float kd_tree_get_column_median(int column_index) {
    return columns_median_space[column_index];
    //return 0; 
}

/*=============================================================================
Function        kd_tree_free_node_processing_space
Description:    

==========================================================*/
/*free*/
void kd_tree_free_columns_median_space(float** medians)
{
    free (*medians);
    *medians = NULL; 
}

/*Implementations -columns_median_processing_space heap*/
/*alloc*/
/*=============================================================================
Function        kd_tree_alloc_columns_median_processing_space
Description:     
==========================================================*/
float* kd_tree_alloc_columns_median_processing_space(int rows)
{
    if (NULL==columns_median_processing_space)
    {
        columns_median_processing_space = (float*) 
                calloc (rows, sizeof(float));
    }
    return columns_median_processing_space;
}

/*get*/
/*=============================================================================
Function       kd_tree_get_columns_median_processing_space
Description:     
==========================================================*/
float* kd_tree_get_columns_median_processing_space()
{
    return columns_median_processing_space; 
}
/*init*/
void kd_tree_init_columns_median_processing_space(float** medians, int rows) {
    if (NULL != medians && NULL!=(*medians)) {
        int i = 0;
        for (; i < rows; i++) {
            /*reset*/
            (*medians)[i] = FLT_MAX;

        }
    }
}
/*free*/
void kd_tree_free_columns_median_processing_space(float** medians)
{
    free(*medians);
    *medians = NULL; 
}

/*Implementations*/
/*=============================================================================
Function:       get_column_vector_from_matrix
Description:    this function points to an arbitrary multidimensional array of  
 *              dimensions m(rows) by  n(columns) and extracts a single 
 *              dimension with all rows. This attempts to extract column vector 
 *              given a m(rows) by n(columns) matrix. A pointer to extracted 
 *              column is returned. 
Inputs:         info - pointer to multidimensional array
 *              row_length - number rows in a m * n  multidimensional array were
 *              number of rows is m. This is zero indexed 
 *              column_length - number of columns in m * n multidimensional 
 *              array were number of columns is n. This is zero indexed.
 *              column_dimension - the specific dimension that will be extracted 
 *              out_put_array - the output array that will have numeric values
 *              that were extracted based on column_dimension value.
 Output:        int 1 is returned if (out_put_array) has valid values else
 *              0 is returned if (out_put_array) does not have valid values.
 *              If 0 is returned you cannot use out_put_array values. 
=============================================================================*/
int
get_column_vector_from_matrix (float * info, int row_length,
                               int column_length, int column_dimension, 
                               float *out_put_array)
{
    int valid_output = 0;
    int row_i = 0;
    float value = 0;
    if (info != NULL && out_put_array != NULL && column_dimension < 
            column_length)
    {
        /*extract data only along column_dimension*/
        for (; row_i < row_length; row_i++)
        {

            value = *(info + (row_i * column_length + column_dimension));
            /*(out_put_array + row_i) = value;*/
            out_put_array[row_i] = value;
           
        } 
        valid_output = 1;
    }
    else
    {
        valid_output = 0;
        printf("kdtree, Error, get_column_vector_from_matrix column_dimension"
                "is greater than total column_length!"); 
    }
    return valid_output;
}

/*===========================================================================
Function        find_median
Description:    Returns the middle number in a given sequence sorted numbers, 
 *              taken as the average of the two middle numbers when the sequence 
 *              has an even number of numbers.
 TODO: Current median algorithm relies on at best O(N log N) sorting. This a
 * basic algorithm for finding  the median & thus not efficient. 
==========================================================*/
float
regular_median (float values [], int n)
{
    /*DONT lock this operation with the kd_tree_allow_update flag*/
    float median = 0;

    /*if number of elements are even*/
    float quotient = 0.0;
    int mid = 0;
    int low_index = 0;
    int high_index = 0;
    if (n % 2 == 0)
    {
        mid = n / 2;
        low_index = mid - 1;
        high_index = mid;
        median = (values[low_index] + values[high_index]) / 2;
        /*DEBUGGING printf("low_index=%d,high_index=%d,"
        "values[low_index]=%f,values[high_index]=%f",
        low_index,high_index,
        values[low_index],
        values[high_index]);*/
    }
    else
    {
        quotient = n / 2;
        mid = (int) ceil (quotient);
        median = values[mid];
    }
    return median;
}

/*---------------------------------------------------------------------------
   Function :   kth_smallest()
   In       :   array of elements, # of elements in the array, rank k
   Out      :   one element
   Job      :   find the kth smallest element in the array
   Notice   :   use the median() macro defined below to get the median. 

                Reference:

                  Author: Wirth, Niklaus 
                   Title: Algorithms + data structures = programs 
               Publisher: Englewood Cliffs: Prentice-Hall, 1976 
    Physical description: 366 p. 
                  Series: Prentice-Hall Series in Automatic Computation 

 ---------------------------------------------------------------------------*/
elem_type kth_smallest(elem_type a[], int n, int k)
{
    int i,j,l,m ;
    register elem_type x ;

    l=0 ; m=n-1 ;
    while (l<m) {
        x=a[k] ;
        i=l ;
        j=m ;
        do {
            while (a[i]<x) i++ ;
            while (x<a[j]) j-- ;
            if (i<=j) {
                ELEM_SWAP(a[i],a[j]) ;
                i++ ; j-- ;
            }
        } while (i<=j) ;
        if (j<k) l=i ;
        if (k<i) m=j ;
    }
    return a[k] ;
}


/*===========================================================================
Function        insertion_sort_based_on_distance
Description:    iterative insertion sort. 
 * Inputs:      tree ** arr - pointers array to be sorted. 
 *              int n - number of elements to sort 
References:     https://www.geeksforgeeks.org/iterative-quick-sort/
 *           https://www.doc.ic.ac.uk/~mjw03/PersonalWebpage/pdfs/quicksort.pdf
 *              https://www.geeksforgeeks.org/quick-sort/.
==========================================================*/
void insertion_sort_based_on_distance(kd_tree_node* arr[], int n)
{
    /*DONT lock this operation with the kd_tree_allow_update flag
	Start from second element (element at index 0 
	is already sorted)*/
    int i = 1;
	for (; i < n; i++) 
	{
		kd_tree_node value = *arr[i];
		int j = i;
             
		/*Find the index j within the sorted subset arr[0..i-1]
		where element arr[i] belongs*/
        kd_tree_node  pre_val = *arr[j - 1];
		while (j > 0 && pre_val.distance_to_neighbor > value.distance_to_neighbor) 
		{
			*arr[j] = *arr[j - 1];
			j--;
            if (j > 0)
            {
            pre_val = *arr[j - 1];
            }
		}

		/*Note that subarray arr[j..i-1] is shifted to
		the right by one position i.e. arr[j+1..i]*/
		*arr[j] = value;
	}
}

/*===========================================================================
Function        insertion_sort_based_on_distance
Description:    iterative insertion sort. 
 * Inputs:      tree ** arr - pointers array to be sorted. 
 *              int n - number of elements to sort 
References:     https://www.geeksforgeeks.org/iterative-quick-sort/
 *             https://www.doc.ic.ac.uk/~mjw03/PersonalWebpage/pdfs/quicksort.pdf
 *              https://www.geeksforgeeks.org/quick-sort/.
==========================================================*/
void insertion_sort_based_on_distance2(kd_tree_node* arr, int n)
{
    /*DONT lock this operation with the kd_tree_allow_update flag
	Start from second element (element at index 0 
	is already sorted)*/
    int i = 1;

	for (; i < n; i++) 
	{
		kd_tree_node value = arr[i];
		int j = i;
             
		/*Find the index j within the sorted subset arr[0..i-1]
		where element arr[i] belongs*/
        kd_tree_node  pre_val = arr[j - 1];
		while (j > 0 && pre_val.distance_to_neighbor > 
                        value.distance_to_neighbor) 
		{
			arr[j] = arr[j - 1];
			j--;
            if (j > 0)
            {
            pre_val = arr[j - 1];
            }
		}

		/*Note that subarray arr[j..i-1] is shifted to
		the right by one position i.e. arr[j+1..i]*/
		
		arr[j] = value;
	}
}



/*perform iterative insertion sort on arr[]
references: https://www.techiedelight.com/insertion-sort-iterative-recursive/
*/
void insertion_sort(float  * arr, int n)
{
    int i = 1;
	for (; i < n; i++) 
	{
        /*TODO: this is NULL*/
		float value = *(arr + i);
		int j = i;
        float  pre_val = *(arr+j - 1);
		while (j > 0 && pre_val> value) 
		{
			*(arr+j) = *(arr+j - 1);
			j--;
            if (j > 0)
            {
            pre_val = *(arr+j - 1);
            }
		}

		/*Note that subarray arr[j..i-1] is shifted to
		the right by one position i.e. arr[j+1..i]*/
		
		*(arr+j) = value;
	}
}

void
kd_tree_print_data_for_debug(kd_tree_node* data, const int k_dimensions,
                      const int result_size)
{

    float distance = 0.0f;
    if (NULL != data && result_size > 0) {
        int i = 0;
        int c = 0;
        kd_tree_node n;
        for (; i < result_size; i++) {
   
                n = data[i]; 
                printf("{");
                c = 0;
                for (; c < k_dimensions; c++) {
                    if (NULL != n.dataset) {

                        printf("%f,", n.dataset[c]);

                    }


                }/*end for */
                printf("}\n");
                if (n.distance_to_neighbor > FLT_MAX) {
                    distance = (float) n.distance_to_neighbor;
                    printf("distance_to_neighbor=%f\n", distance);
                }
            

        }
    } else {
        printf("Result size=%d\n", result_size);
    }
}


/*helper*/
int is_empty_node(kd_tree_node* node, int number_of_dimensions) {
    int flag = 1;
    if (NULL != node && NULL != node->dataset) {

        int i = 0;
        for (; i < number_of_dimensions; i++) {
            if (node->dataset[i] != FLT_MAX)
                flag = 0;
            break;
        }

    }
    return flag;
}
