# Easy Vector (EV)
Easy Vector is a small and simple header only C vector type. 
It's designed for situations like options parsing where building up an unknown amount of state is required. 

The neat thing about this vector is that it maintains array semantics by hiding state near the allocated memory. 
Some (optional) macro magic makes it even easier to use.

## Quick Start

Getting started with Easy Vector is ... easy. 
The following demo allocates a new vector and pushes the value of "1" into it.

~~~C
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

 
## Detailed Documentation

### Initialisation:  evinit(), evinisz(), evini()

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