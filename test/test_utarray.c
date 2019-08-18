#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "include/utarray.h"
#include <time.h>
int main(int argc, char const *argv[])
{
    UT_array *arr;
    int i,*p;
    utarray_new(arr,&ut_int_icd);
    
    for(i=0; i < 10; i++) 
        utarray_push_back(arr,&i);

    for(p=(int*)utarray_front(arr);p!=NULL;p=(int*)utarray_next(arr,p)){
         printf("%d\n",*p);
    }
    utarray_free(arr);
    return 0;
}
