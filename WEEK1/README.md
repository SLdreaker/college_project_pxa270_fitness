# Import python in C
> 
## Python (food.py)
Import Requsest to get the information from website and import BeatifulSoup to reorganize data

## C code (food.c)
include Python.h and use c code to call the python.

## Compile
We compile the c code, which contain the header file (Python.h)


Linux on Ubuntu

 `gcc food.c -o food -I /usr/include/python2.7/ -lpython2.7`

## Reference
[C調用python](http://www.cnblogs.com/bracken/archive/2013/05/23/3094534.html)