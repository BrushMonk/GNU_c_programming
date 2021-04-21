if we know there are a array and pointer like this code:
```cpp
_Complex double array[10], *ptr;
printf("sizeof array == %zu, sizeof ptr == %zu.\n", sizeof array, sizeof ptr);
```
Then we get output:
```shell
sizeof array==80, sizeof ptr==8.
```


Assignment validity in every couple among a point-to-long, a point-to-double, a pointer-to-array-of-2-doubles, a pointer-to-array-of-3-doubles, a point-to-function and a pointer-to-pointer:
```cpp
double flt_array1[5][3]; double flt_array2[3][2];
long *ptr_to_long; double *ptr_to_dbl;
double *array_of_2_ptrs[2]; double *array_of_3_ptrs[3];
double (*ptr_to_array_of_2_dbls)[2]; double (*ptr_to_array_of_3_dbls)[3];
double **ptr_to_ptr; double (*ptr_to_func)(double, double)={fmin, fmax, pow, atan2, fmod};

ptr_to_dbl=ptr_to_long;/* invalid */
ptr_to_dbl=ptr_to_array_of_3_dbls;/* invalid */
ptr_to_array_of_2_dbls=flt_array2;/* valid */
ptr_to_array_of_3_dbls=flt_array1;/* valid */
ptr_to_array_of_2_dbls=flt_array1;/* invalid */
ptr_to_array_of_3_dbls=flt_array2;/* invalid */
ptr_to_ptr=flt_array1;/* invalid */
ptr_to_ptr=ptr_to_func;/* invalid */
```


Assignment validity in every couple among a point-to-constant, a constant array and a constant pointer:
```cpp
unsigned long Fibonacci[]={0,1,1,2,3,5,8,13,21,34,55,89,144};
const unsigned long locked_Fibonacci[]={1,1,2,3,5,8,13,21,34,55,89,144,233,377};
unsigned long *regular_ptr;
const unsigned long *ptr_to_const={0,1,1,2,3,5,8,13,21,34,55,89,144};
/* it is bad usage that unsigned long *ptr_to_const={0,1,1,2,3,5,8,13,21,34,55,89,144}; */
unsigned long *const fix_ptr=Fibonacci;

ptr_to_const=Fibonacci;/* valid */
ptr_to_const=locked_Fibonacci;/* valid */
ptr_to_const=&Fibonacci[11];/* valid */
ptr_to_const++;/* valid */
*fix_ptr=Fibonacci[2];/* valid */

*ptr_to_const=1;/* invalid */
ptr_to_const[3]=Fibonacci[7];/* invalid */
ptr_to_const[3]=locked_Fibonacci[4];/* invalid */
fix_ptr=&Fibonacci[11];/* invalid */
regular_ptr=Locked_Fibonacci;/* invalid */
Fibonacci=ptr_to_const;/* invalid */
```


the usage of `restrict`
If the compiler knows that there is only one pointer to a memory block, it can produce better optimized code. For instance:
```cpp
void updatePtrs(size_t *ptrA, size_t *ptrB, size_t *val)
{*ptrA += *val;
*ptrB += *val;}
```
In the above code, the pointers ptrA, ptrB, and val might refer to the same memory location.
However, if the restrict keyword is used and the above function is declared as:
```cpp
void updatePtrs(size_t *restrict ptrA, size_t *restrict ptrB, size_t *restrict val);
```
then the compiler is allowed to assume that ptrA, ptrB, and val point to different locations and updating the memory location referenced by one pointer will not affect the memory locations referenced by the other pointers.


a pointer to function:
```cpp
unsigned (*ptr_to_func)(char *, const struct cpu_specs *);
```
a function that returns a pointer:
```cpp
unsigned *func_return_to_ptr(char *, const struct cpu_specs *);
```


a pointer to function is used as a parameter in an another function:
```cpp
#include <stdlib.h>
int compar(const void *a,const void *b)
{
    return (*(int*)a-*(int*)b);
}
int main(void)
{
    size_t len=5;
    int array[len]={5,2,1,3,4};
    qsort(array, len, sizeof(int), compar);
    return 0;
}
```
`compar` is a function name as well as a pointer to this function.


use an function with structure as parameter:
```cpp
struct cpu_specs func_with_stru(struct cpu_specs cpu_stru)
{/* the parameter is a structure */
    cpu_stru.cpu_model="Intel(R) Pentium(R) CPU G3250 @ 3.20GHz";
    cpu_stru.cpu_MHz=3200.000;
    return cpu_stru;
}
int main(void)
{
    struct cpu_specs intel_dual_core={.cpu_MHz=3700.000, .cache_size=512, .clflush_size=64};
    intel_dual_core=func_with_stru(intel_dual_core);
}
```
use and functions with pointer to structure as parameters:
```cpp
void func_with_stru(const struct cpu_specs *ptr_to_cpu_stru)
{/* the parameter is a pointer to structure */
    ptr_to_cpu_stru->cpu_model="Intel(R) Pentium(R) CPU G3250 @ 3.20GHz";
    ptr_to_cpu_stru->cpu_MHz=3200.000;
}
int main(void)
{
    struct cpu_specs intel_dual_core={.cpu_MHz=3700.000, .cache_size=512, .clflush_size=64};
    func_with_stru(&intel_dual_core);
}
```