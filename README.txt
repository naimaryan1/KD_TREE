1) What is this library?

This is a minimal kd-tree library. 

The kd-tree library builds balanced kd-tree, the kd-tree helps
run efficient KNN (k nearest neighbors) algorithm. Most algorithms are 
iterative and non-recursive.  

This was tested on Linux Ubuntu and Windows 10.


2) Installation:

IMPORTANT: make sure to check out the branch "kd_tree_rebuild_refactor"
navigate to folder: /uprite_gitlab/lwr/c/mem_management 

In order to integrate this library into your project just copy & paste kdtree.h
and kdtree.c files into your project. 

3) Build & test kdtree:

Use Make & at the command prompt type "make clean all". Executable file titled
"test" will be generated. By default the Make file will compile & run a test
file titled "knn_search_test.c". 

4) Other test files:

batch_test.c
crud_test
memory_allocation_test.c

Thats all. 



