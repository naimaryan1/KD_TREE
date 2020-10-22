#include <stdio.h>
#include "kdtree.h"

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
 * Loads 3d points from a file for batch insertion into a kdtree. 
 * 
 * NOTE/TODO:This the upload script has CSV parsing bug as 10/22/2020, 
 * instead to test batch insertion refer to batch_test.c
 */

void kd_tree_batch_load_points(char* file_path,
        int expected_rows,
        int k_dimensions);

int main(int argc, char** argv) {
    int max_rows = 10000;
    int max_cols = 3;
    kdtree_t* kdtree = kdtree_alloc(max_rows, max_cols);
    assert(kdtree);
    printf("alloc ok \n");
    kdtree_init(kdtree);
    /*extra debug is on!*/
    kd_tree_set_debug_on(0);
    kd_tree_set_rebuild_threshold(2);
    printf("init ok\n");
    argv[1] = "/home/anaim/uprite_gitlab/lwr/c/mem_management/dat_avz/001/scan1.dat";
    kd_tree_batch_load_points(argv[1], max_rows, max_cols);
    printf("traverse\n");
    int result_size = kd_tree_in_order_traversal(kd_tree_get_root(), max_cols);
    kd_tree_print_data_for_debug(kd_tree_get_root(), max_cols, result_size);
    kdtree_free(kdtree);
    printf("free ok \n");

}
// Scan all points from file
void kd_tree_batch_load_points(char* file_path,
        int expected_rows,
        int k_dimensions) {
    FILE* infile;
    infile = fopen(file_path, "r");
    const char* delimiter = " ";
    long fl_size;
    char* buffer;
    size_t res;
    int rows_read = 0;
    int rows_inserted = 0;
    if (infile) {

fseek (infile , 0 , SEEK_END);
fl_size = ftell (infile);
rewind (infile);
buffer = (char*) malloc (sizeof(char)*fl_size);
if (buffer == NULL) {
fputs ("Memory error",stderr);
exit (2);
}
res = fread (buffer,1,fl_size,infile);
if (res != fl_size) {
fputs ("Reading error",stderr);
exit (3);
}

char * strtok_res;
strtok_res = strtok(buffer, delimiter); // Change the delimiter 
int data_row = 0;
while (strtok_res != NULL)
{
    if  (rows_read!=0)
    {
        int i =0;  
       while (strtok_res != NULL && i<k_dimensions)
       {
        
           batch_node_processing_space[data_row].dataset[i] = atof(strtok_res);
 printf ("%f,", atof(strtok_res));
strtok_res = strtok (NULL, delimiter); // Change the delimiter  
i++;
       }
         printf ("\n");
data_row++;
    }
rows_read++;

}
         
        //start build kd- tree
        kd_tree_node* root = kd_tree_get_root();  
        while (rows_inserted < rows_read) {
            //printf("inserted:\n");
            if (rows_inserted < 10) {
                kd_tree_print_data_for_debug(&batch_node_processing_space[rows_inserted],
                        k_dimensions, 1);
            }
            kd_tree_add_points(&root, batch_node_processing_space[rows_inserted].dataset);
            rows_inserted++;
        }
        
    }
    
     else {
        printf("Error, kd_tree_batch_load_points.Cant open the file %s", file_path);
    }
        //end build kd- tree 
        printf("rows read=%d\n", rows_read);
        printf("rows inserted=%d\n", rows_inserted);
        
        fclose (infile);
free (buffer);
 
    }

    
    

