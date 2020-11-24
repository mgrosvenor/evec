# Easy Vector (EV)
Easy Vector is a small and simple header only C vector type.
It's designed for situations like options parsing where building up an unknown amount of state is required.

It has a number of features:
- **Easy**: EV is designed to make the code as easy to use a possible.
            Helps you to focus on solving your problem, not on learning yet another thing.  
- **Simple**: The entire code base is only a few hundered lines, contained in a single header file.
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
   This will automatically allocate a new vector structure if none exits.
2. Retrieve the number of items in the vector using `evcnt()`.
   This is useful to iterate over the final vector.
3. Free the structure when done using `evfree()`.

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

#define EV_FALL
#include "evec.h"


int main(int argc, char** argv)
{
    void\* a = evinisz(128);
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

#define EV_FALL
#include "evec.h"

int compare(void\* lhs, void\* rhs)
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


## Detailed Documentation
### Build Macros

** Hard Exit ** <br/>
By default EV will "fail hard and early".
This means that `exit()` will be called on all errors.
This default behaviour can be overridden by defining `EV_HARD_EXIT` as `0` e.g.

~~~C
#define EV_HARD_EXIT 0
~~~

<hr/>

** Initial Slot Count ** <br/>
By default EV will allocate 8 slots in the vector.
This can be overridden by defining the `EV_INIT_COUNT` value. e.g  

~~~C
#define EV_GROWTH_FACTOR 3
~~~

<hr/>


** Growth Factor ** <br/>
By default EV grow the vector by a factor of 2 each time it runs out of slots.
For example, if there are 8 slots, EV will grow the vector to 16.  
EV can be made to grow faster  the `EV_INIT_COUNT` value. e.g  

~~~C
#define EV_INIT_COUNT 64
~~~

<hr/>

** Function Availability ** <br/>

The following functions are included in all builds:
- Initialisation functions - `evinit()`,`evinisz()`,`evini()`
- Push functions - `evpsh()`,`evpush()`
- Count and index functions - `evcnt()`, `evidx()`
- Memory free - `evfree()`

Beyond those basic functions, other advanced functions require specific inclusion in the build by defining the following:
- `EV_FALL` - All advanced functions are included
- `EV_FPOP` - Pop function to remove an item from the tail
- `EV_FEDL` - Delete function to remove an item from anywhere
- `EV_FMEMSZ` - Memory sizing functions including `evvsz()`, `evvmem()`, `evomem()`, `evtmem()`
- `EV_FSORT` - Sort function to sort the vector contents


### Initialisation
These functions allocate a new vector and initialise it.
Typically it is not necessary to call them directly because the various push functions will do this for you.
But it may be that you know ahead of time how many slots you need, or that you need to be more specific about object sizes.
In these cases the init functions will be useful.


**void\* evinit(type)**  <br/>
Easy allocate a new vector, based on type information.
<table>
<tr><td> **type**: 		</td><td> A fully specified C type. <td></tr>
<tr><td> **return**: 	</td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**void\* evinisz(size_t sz)**  <br/>
Easy allocate a new vector with slot sizes as given.
<table>
<tr><td> **slt_size**:  </td><td> The size of each slot in the vector typically the size of the type that is being stored.<td></tr>
<tr><td> **return**: 	</td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**void\* evini(size_t slt_size, size_t count)**  <br/>
Allocate a new vector and initialise it.
<table>
<tr><td> **type**: 		</td><td> The size of each slot in the vector typically the size of the type that is being stored.<td></tr>
<tr><td> **count**:     </td><td> The number of initial elements (of size slt_size) to be allocated. This should be set to the lower bound of the expected number of items (which could be zero). </td></tr>
<tr><td> **return**: 	</td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>


### Push

Functions to add a new data onto the back of the vector.   

**void\* evpsh(vec, obj)**  <br/>

Easy push a new value onto the tail of a vector.
If the vector is NULL, memory will be automatically allocated for INIT_COUNT elements, based on the object size as returned by sizeof(obj).
If the memory backing the vector is too small, memory will be reallocated to grow the vector by the the GROWTH_FACTOR. e.g. 16B with a GROWTH_FACTOR=2 will grow to 32B.

The reason for this wrapper macro is to make it easy to push literal values.

<table>
<tr><td> **vec**:       </td><td> Pointer to type of object that is (or will become) the vector, eg. int* for a vector of ints.<td></tr>
<tr><td> **obj**:       </td><td> The value to push into the vector. <td></tr>
<tr><td> **return**:    </td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
<tr><td> **quirks**:    </td><td> This macro works fine for all literals expect string literals, for which you probably want to store the char* pointer, but this doesn't really exist.
                                  For these types, you'll need to use the explicit evpush function. </td></tr>
</table>
<hr/>

**void\* evpush(void\* vec, void\* obj, size_t obj_size)**  <br/>
Push a new value onto the vector tail.
The if the vector is NULL, memory will be automatically allocated for INIT_COUNT elements, based on the object size supplied.

If the memory backing the vector is too small, memory will be reallocated to grow the vector by the the GROWTH_FACTOR. e.g. 16B with a GROWTH_FACTOR=2 will grow to 32B.
<table>
<tr><td> **vec**:       </td><td> Pointer to type of object that is (or will become) the vector, eg. int* for a vector of ints.<td></tr>
<tr><td> **obj**:       </td><td> Pointer to the value to push into the vector. <td></tr>
<tr><td> **obj_size**:  </td><td> The size of the value to be pushed into the vector. <td></tr>
<tr><td> **return**:    </td><td> A pointer to the memory region, or NULL. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>

### Count and Index
These two simple functions help to navigate around the vector once created.

**size_t evcnt(void\* vec)**  <br/>
Get the number of items in the vector.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **return**:    </td><td> The number of objects in the vector. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**void\* evidx(void\* vec, size_t idx)**  <br/>
Return a the pointer to the slot at a given index.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector.<td></tr>
<tr><td> **idx**:       </td><td> The index value. Cannot be <0 or greater than the object count. <td></tr>
<tr><td> **return**:    </td><td> Pointer to the value at the given index <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>


### Free
EV allocates memory for the underlying array, as well as accounting.
At some point this memory should be freed.

**void\* evfree(void\* vec)** <br/>
Free the memory used to hold the vector and its accounting.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **obj**:       </td><td> The value to push into the vector. <td></tr>
<tr><td> **return**:    </td><td> NULL. Use `vec = evfree(vec)` to ensure there are no dangling pointers.
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>

### Pop and Delete
Functions to remove items from the vector.

**void evpop(void\* vec)**  <br/>
Remove the last value from the vector tail.

**Note:** To use this function `EV_FPOP` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **return**:    </td><td> None. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**void\* evdel(void\* vec, size_t idx)**  <br/>
Remove a value from the vector at the given index

**Note:** To use this function `EV_FDEL` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector.<td></tr>
<tr><td> **idx**:       </td><td> The index value. Cannot be <0 or greater than the object count. <td></tr>
<tr><td> **return**:    </td><td> None <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>

### Memory sizing functions
It can be useful to know how much memory is available or in current use.

**size_t evvcnt(void\* vec)**  <br/>
Get the number of slots in the vector

**Note 1:** This function does not return the number of used slots (ie the number of objects) in the vector.
If you are looking for this functionality, please use `evcnt()` function described above. <br/>
**Note 2:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **return**:    </td><td> The number of slots in the vector. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**size_t size_t evvmem(void* vec)**  <br/>
Get the amount of memory currently used to store the vector including unused slots.

**Note:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **return**:    </td><td> The amount of memory currently used to store the vector. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**size_t size_t evomem(void\* vec)**  <br/>
Get the amount of memory currently used to store objects in the vector

**Note:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **return**:    </td><td> The amount of memory currently used to store objects in the vector. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

**size_t evtmem(void\* vec)**  <br/>
 Get the total memory used by the vector including including accounting overheads.

**Note:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **return**:    </td><td> The total amount of memory consumed by the vector. <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>


### Sorting

**void evsort(void\* vec, int (\*compar)(const void\* a, const void\* b))**  <br/>
Sort the elements of the vector in place

**Note:** To use this function `EV_FSORT` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the vector<td></tr>
<tr><td> **compar**:    </td><td> Function pointer which implements the comparison function.
                                  This function returns +ve if a > b, -ve if a < b and 0 if a==b. </td></tr>
<tr><td> **return**:    </td><td> None. The vector will be sorted in place if this function succeeds <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>

### Copying

**void\* evcpy(void\* src)**  <br/>
Make a new copy of the vector, including allocating underlying memory.

**Note:** To use this function `EV_FCOPY` or `EV_FALL` must be defined.

<table>
<tr><td> **vec**:       </td><td> Pointer to the sourcevector<td></tr>
<tr><td> **return**:    </td><td> A new vector with the same contents as the source <td></tr>
<tr><td> **failure**:   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); <td></tr>
</table>
<hr/>
