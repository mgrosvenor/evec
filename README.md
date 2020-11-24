# Easy Vector (EV)
Easy Vector is a small and simple header only C vector type. 
It's designed for situations like options parsing where building up an unknown amount of state is required. 

It has a number of features: 
- **Easy**: EV is designed to make the code as easy to use a possible. 
            Helps you to focus on solving your problem, not on learning yet another thing.  
- **Simple**: The entire code base is less than 500 lines including comments. 
  			  Typically only 100 lines of which is actually used.
- **Cheap**: EV has a pay-for-what-you-use model. 
             Most "features" are optional, included only with a #define. There's no cost 
 

## Quick Start

Getting started with Easy Vector is ... easy. 
The following demo allocates a new vector and pushes the value of "1" into it.

~~~C
#include "evec.h"

int* a = NULL;
a = evpsh(a, 1);
~~~

 
Once you have some values in the vector, you may want to iterate over them  

~~~C
for(int a = 0; i < evcnt(a); i++){
    printf("%i, ", a[i]);
}
~~~
  
Eventually, you'll want to clean up the mess afterwards. 

~~~C
a = evfree(a); 
~~~
  
Done. It's that easy!


## Core Functions

There are 4 core functions required to use EV.  
1. Add values into the vector using `evpsh()` functions. 
2. Allocate a new vector structure if none exits using `evini()` functions. 
   Typically this is done implicitly as part of 1 above so there is no need to call this explicitly.   
3. Retrieve the number of items in the vector using `evcnt()`. 
   This is useful to iterate over the final vector. 
4. Free the structure when done using `evfree()`. 

For example: 

~~~C
#include <stdio.h>
#include "evec.h"


int main(int argc, char** argv)
{
    int* a = NULLL
    a = evpsh(a, 2);
    a = evpsh(a, 4);
    a = evpsh(a, 6);
    
    for(int i =0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }  
    
    a = evfree(a);
    return 0;
}
~~~


## Advanced Usage
Beyond these basic functions, EV provides more advanced options. 
These options require compile time `#defines` to include them. 
The simplest way is to use `#define EV_ALLFEATS`. 
More fine grained options are docuemented under in the [build parameters](#build-parameters) section. 

It's not always possible to (or desired) for each vector slot to be sized based on the type information provided by `evpsh()` or `evnit()`.
In these situations, the `evinisz()` may be used to allocate specific slot sizes.  
To iterate over these custom sizes, use `evidx()` which is aware of the slot sizes. 
For example:

~~~C
#include <stdio.h>

#define EV_ALLFEATS
#include "evec.h"


int main(int argc, char** argv)
{
    void* a = evinisz(128);
    a = evpsh(a, "Test");
    a = evpsh(a, "Best");
    a = evpsh(a, "Rest");
    
    for(int i =0; i < evcnt(a); i++){
        char* s = (char*)evidx(a,i);
        printf("%i: %s\n", i, s);
    }  
    
    a = evfree(a);
    return 0;   
}
~~~

For more advanced uses of EV, you may want to remove items from the vector, you can do this with `evpop()` which ejects the last item from the vector.
Alternatively, `evedel()` can be used to remove an item at a given index.  
Finally, `evsort()` can be used to sort items.

~~~C
#include <stdio.h>

#define EV_ALLFEATS
#include "evec.h"

int compare(void* lhs, void* rhs)
{
    int* a = (int*)lhs;
    int* b = (int*)rhs;

    return *a < *b;
}

int main(int argc, char** argv)
{
    int* a = NULLL
    a = evpsh(a, 2);
    a = evpsh(a, 4);
    a = evpsh(a, 6);
    
    a = evpsh(a, 2);
    a = evpsh(a, 4);
    a = evpsh(a, 6);
    
    evsort(a,compare);
        
    //Remove duplicates
    for(int i = 1; i < evcnt(a); i++){
        if(a[i] == a[i-1]){
            evdel(a,i);
        }       
    }  
   
    for(int i =0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }  
      
   
    a = evfree(a);
    return 0;
}
~~~


##Detailed Documentation
###Build Parameters


### Initialisation

*evinit(), evinisz(), evini()* <br/>
These functions allocate a new vector and initialize it. 
Typically it is not necessary to call them directly because the push() functions will do this for you.
But it may be that you know ahead of time how many slots you need, or that you need to be more specific about object sizes. 
In these cases the init functions will be useful. 


####void* evinit(type)
Easy allocate a new vector, based on type information. 
<table>
<tr><td> **type**: 		</td><td> A fully specified C type. <td></tr>
<tr><td> **return**: 	</td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If [EV_HARD_EXIT](#EV_HARD_EXIT) is enabled, this function may cause exit(); <td></tr>
</table>

#### void* evinisz(size_t sz)
Easy allocate a new vector with slot sizes as given. 
<table>
<tr><td> **slt_size**:  </td><td> The size of each slot in the vector typically the size of the type that is being stored.<td></tr>
<tr><td> **return**: 	</td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If [EV_HARD_EXIT](#EV_HARD_EXIT) is enabled, this function may cause exit(); <td></tr>
</table>

#### void* evini(size_t slt_size, size_t count)
Allocate a new vector and initialize it.
<table>
<tr><td> **type**: 		</td><td> The size of each slot in the vector typically the size of the type that is being stored.<td></tr>
<tr><td> **count**:     </td><td> The number of initial elements (of size slt_size) to be allocated. This should be set to the lower bound of the expected number of items (which could be zero). </td></tr>
<tr><td> **return**: 	</td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If [EV_HARD_EXIT](#EV_HARD_EXIT) is enabled, this function may cause exit(); <td></tr>
</table>


### Puhs :  evinit(), evinisz(), evini()


### Macros

There are a number of useful macro functions to help control the compile time behaviour of EV.

#### EV_HARD_EXIT