# Easy Vector (EV)
Easy Vector is a small and simple header only C vector type.
It's designed for situations like options parsing where building up an unknown amount of state is required.

It has a number of features:
- **Easy**: EV is designed to make the code as easy to use a possible.
            Helps you to focus on solving your problem, not on learning yet another thing.
- **Simple**: The entire code base is only a few hundered lines, contained in a single header file.
- **Cheap**: EV has a pay-for-what-you-use model.
             Most "features" are optional, included only with a `#define`.
             This means that there's no cost for functions that you don't need.

EV is licensed under a BSD 3-Clause license to make it simple and easy to integrate into any open or closed source project.
See [Licensing](#licensing) section for more details.

EV should be compatible with any sensible compiler on any reasonable OS.
It has been tested on Linux (x86) (Ubuntu 20.04) with GCC (9.3) compiler and on
MacOS X (x86) with Clang compiler (11.0.3)

For release notes/change log please see the [Release Notes](#release-notes) section.

## Getting Started

Getting started with Easy Vector is ... easy.
You can use EV in just 4 simple steps

### Step 1 - Copy the header file
Copy the `evec.h` header file from [here](https://github.com/mgrosvenor/evec/raw/main/evec.h) and save it into your project. Include it in your code like this:

~~~C
#include "evec.h"
~~~

EV supports a number of build time options.
See [Build Time Options](#build-time-options) for more details.  

### Step 2 - Push values into vector

EV tries to automatically allocate a new vector structure and sensibly push values into it.
For example:

~~~C
int* a = NULL;
evpsh(a, 1);
~~~

There are lots of options for ways allocate and push values into EV.
For more details see [Core Functions](#core-functions) section.


### Step 3 - Access vector values

Once you have some values in the vector, you will eventually want to access them:  

~~~C
eveach(ai, a){
    printf("%i\n", *ai);
}
~~~


**One quick note:** EV does not guarantee that values are stable (ie stored in the same location in memory).
If you need to access values, consistently across pushes, please use the `evidx()` function.
More details can be found in the [Core Functions](#core-functions) section.  

### Step 4 - Free resources

Eventually, you'll want to clean up the mess afterwards.

~~~C
a = evfree(a);
~~~

Done. It's that easy!


## Core Functions

There are 3 core functions required to use EV.
1. Add values into the vector using `evpsh()` functions.
   This will automatically allocate a new vector structure if none exists.
2. Iterate over the vector using `eveach()`.
3. Free the structure when done using `evfree()`.

For example:

~~~C
#include <stdio.h>
#include "evec.h"


int main(int argc, char** argv)
{
    int* a = NULL;
    evpsh(a, 2);
    evpsh(a, 4);
    evpsh(a, 6);

    eveach(a,ai){
        printf("%i\n", *ai);
    }  

    a = evfree(a);
    return 0;
}
~~~
## Extended Core Functions

Beyond the basic usage, EV provides a few extra core functions for accessing the vector.

Many operations on a vector care about only the first or the last element.
For these operations, the `evhead()` and `evtail()` functions are provided.

`evhead()` returns a pointer to the first element in the vector, but, it also resets the internal iterator state.
After a call to `evhead()`, you can also call `evnext()` to get the next item.
This be used to manually implement the `eveach()` iteration loop above. Eg:

~~~C
#include <stdio.h>
#include "evec.h"


int main(int argc, char** argv)
{
int* a = NULL;
evpsh(a, 2);
evpsh(a, 4);
evpsh(a, 6);

    for(int* ai = evhead(a); ai != NULL; ai = evnext(a))
        printf("%i\n", *ai);
    }  

    a = evfree(a);
    return 0;
}
~~~

To find out how many items are in the vector, use `evcnt()`. 

If you are using a simple datatype like `int` or `char`, then you can simple access items in the vector using array/pointer notation, e.g `a[i]`. 
Do keep in mind that this kind of access is only valid until the next EV operation which causes a memory reallocation, (eg `evpsh()` may do this).
If you want to have consistent access to items, or for more complicated data types (e.g `struct`s) you can use `evidx()` to obtain a pointer to the object at the given index.  

The following example has the same functionality as the previous example, but uses these extended functions.
It has the neat property that you also have access to the iterator index value.  

~~~C
#include <stdio.h>
#include "evec.h"


int main(int argc, char** argv)
{
    int* a = NULL;
    evpsh(a, 2);
    evpsh(a, 4);
    evpsh(a, 6);

    for(int i = 0; i < evcnt(a); i++){
        printf("%i: %i\n", i, *(int*)evidx(a, i));
    }  

    a = evfree(a);
    return 0;
}
~~~



## Advanced Usage
Beyond these basic functions, EV provides more advanced options.
These options require compile time `#defines` to include them.
The simplest way is to use `#define EV_FALL`.
More fine grained options are docuemented under in the [build macros](#build-macros) section.

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
    void* a = evinisz(128);
    evpsh(a, "Test");
    evpsh(a, "Best");
    evpsh(a, "Rest");

    for(int i = 0; i < evcnt(a); i++){
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

int compare(const void* lhs, const void* rhs)
{
    int* a = (int*)lhs;
    int* b = (int*)rhs;

    return *b - *a;
}

int main(int argc, char** argv)
{
    int* a = NULL;
    evpsh(a, 2);
    evpsh(a, 4);
    evpsh(a, 6);

    evpsh(a, 2);
    evpsh(a, 4);
    evpsh(a, 6);

    evsort(a,compare);

    //Remove duplicates
    for(int i = 1; i < evcnt(a); i++){
        if(a[i] == a[i-1]){
            evdel(a,i);
            i--;
        }       
    }  

    for(int i = 0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }        

    a = evfree(a);
    return 0;
}
~~~


### Build Time Options

**Hard Exit** <br/>
By default EV will "fail hard and early".
This means that `exit()` will be called on all errors.
This default behavior can be overridden by defining `EV_HARD_EXIT` as `0` .

**Note**: This must be done before the "evec.h" header is included. e.g.

~~~C
#define EV_HARD_EXIT 0
#include "evec.h"
~~~

<hr/>

**Initial Slot Count** <br/>
By default EV will allocate 8 slots in the vector.
This can be overridden by defining the `EV_INIT_COUNT` value.

**Note**: This must be done before the "evec.h" header is included. e.g.

~~~C
#define EV_INIT_COUNT 64
#include "evec.h"
~~~

<hr/>

**Growth Factor** <br/>
By default EV grow the vector by a factor of 2 each time it runs out of slots.
For example, if there are 64 slots, EV will grow the vector to 128.
EV can be made to grow faster by setting the `EV_GROWTH_FACTOR` value.

**Note**: This must be done before the "evec.h" header is included. e.g.

~~~C
#define EV_GROWTH_FACTOR 3
#include "evec.h"
~~~

<hr/>

**Pedantic Error Checking** <br/>
By default EV will apply reasonably pedantic error checking.
For example, checking in most functions that the vector supplied is not null.
You may want to avoid these checks if you trust your code.
EV can be made to avoid pedantic by setting `EV_PEDANTIC` to 0.

**Note**: This must be done before the "evec.h" header is included. e.g.

~~~C
#define EV_PEDANTIC 0
#include "evec.h"
~~~

<hr/>

**Multiple Compilation Units (.c files)**<br/>
You may want to use EV in multiple C files across your project.
If you do this, you may get an error something like:

```
duplicate symbol '_evpush' in:
    /var/folders/sy/v5c7yz1j1mn0jhd1bcc6b1nr0000gn/T/test-6a1637.o
    /var/folders/sy/v5c7yz1j1mn0jhd1bcc6b1nr0000gn/T/test2-8b2b8d.o
```

This is because C function definitions are global, so the compiler is seeing the EV functions defined more than once.  This can easily be resolved by telling EV only to include the function declarations (but not definitions).
You can do this with the following

```C
//This is test1.c
#include "evec.h"
```

```C
//This is test2.c
#define EV_HONLY
#include "evec.h"
```

```C
//This is test3.c
#define EV_HONLY
#include "evec.h"
```



**Function Availability** <br/>

The following functions are included in all builds:
- Initialisation functions - `evinit()`,`evinisz()`,`evini()`
- Push functions - `evpsh()`,`evpush()`
- Iteration and access functions - `eveach()`, `evcnt()`, `evidx()`, `evhead()`, `evnext()`, `evtail()` 
- Memory free - `evfree()`

Beyond those basic functions, other advanced functions require specific inclusion in the build by defining the following:
- `EV_FPOP` - Pop function to remove an item from the tail
- `EV_FEDL` - Delete function to remove an item from anywhere
- `EV_FMEMSZ` - Memory sizing functions including `evvsz()`, `evvmem()`, `evomem()`, `evtmem()`
- `EV_FSORT` - Sort function to sort the vector contents
- `EV_FCOPY` - Funciton to copy one EV vector and make a new one
- `EV_FALL` - All above functions are included

## Detailed Documentation
### Initialisation
These functions allocate a new vector and initialise it.
Typically it is not necessary to call them directly because the various push functions will do this for you.
But it may be that you know ahead of time how many slots you need, or that you need to be more specific about object sizes.
In these cases the init functions will be useful.


**void\* evinit(type)**  <br/>
Easy allocate a new vector, based on type information.
<table>
<tr><td> type 		</td><td> A fully specified C type. </td></tr>
<tr><td> return 	</td><td> A pointer to the memory region, or NULL. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**void\* evinisz(size_t sz)**  <br/>
Easy allocate a new vector with slot sizes as given.
<table>
<tr><td> slt_size  </td><td> The size of each slot in the vector typically the size of the type that is being stored.</td></tr>
<tr><td> return 	</td><td> A pointer to the memory region, or NULL. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**void\* evini(size_t slt_size, size_t count)**  <br/>
Allocate a new vector and initialise it.
<table>
<tr><td> type 		</td><td> The size of each slot in the vector typically the size of the type that is being stored.</td></tr>
<tr><td> count     </td><td> The number of initial elements (of size slt_size) to be allocated. This should be set to the lower bound of the expected number of items (which could be zero). </td></tr>
<tr><td> return 	</td><td> A pointer to the memory region, or NULL. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>


### Push

Functions to add a new data onto the back of the vector.   

**evpsh(vec, obj)**  <br/>

Easy push a new value onto the tail of a vector.
If the vector is NULL, memory will be automatically allocated for INIT_COUNT elements, based on the object size as returned by sizeof(obj).
If the memory backing the vector is too small, memory will be reallocated to grow the vector by the the GROWTH_FACTOR. e.g. 16B with a GROWTH_FACTOR=2 will grow to 32B.

The reason for this wrapper macro is to make it easy to push literal values.

<table>
<tr><td> vec       </td><td> Pointer to type of object that is (or will become) the vector, eg. int* for a vector of ints.</td></tr>
<tr><td> obj       </td><td> The value to push into the vector. </td></tr>
<tr><td> return    </td><td> A pointer to the memory region, or NULL. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
<tr><td> quirks    </td><td> This macro works fine for all literals expect string literals, for which you probably want to store the char* pointer, but this doesn't really exist.
                                  For these types, you'll need to use the explicit evpush function. </td></tr>
</table>
<hr/>

**void\* evpush(void\* vec, void\* obj, size_t obj_size)**  <br/>
Push a new value onto the vector tail.
The if the vector is NULL, memory will be automatically allocated for INIT_COUNT elements, based on the object size supplied.

If the memory backing the vector is too small, memory will be reallocated to grow the vector by the the GROWTH_FACTOR. e.g. 16B with a GROWTH_FACTOR=2 will grow to 32B.
<table>
<tr><td> vec       </td><td> Pointer to type of object that is (or will become) the vector, eg. int* for a vector of ints.</td></tr>
<tr><td> obj       </td><td> Pointer to the value to push into the vector. </td></tr>
<tr><td> obj_size  </td><td> The size of the value to be pushed into the vector. </td></tr>
<tr><td> return    </td><td> A pointer to the memory region, or NULL. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

### Access and Iteration 
These functions help to navigate around the vector once created.

**eveach(var, vector){...}** <br/>
Macro to help iterate over each element of the `vector`, putting a pointer to the element in `var`.
This macro is equivalent to

```C
 for(typeof(vec) var = evhead(vec); var; var = evnext(vec))
```

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> var       </td><td> Variable name for the iterator </td></tr>
<tr><td> return    </td><td> This macro has no return value, it is desigted to help iterate over the vector. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>


**size_t evcnt(void\* vec)**  <br/>
Get the number of items in the vector.
If the vector is NULL, or empty, return 0.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> return    </td><td> The number of objects in the vector. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**void\* evidx(void\* vec, size_t idx)**  <br/>
Return a the pointer to the slot at a given index.

**Note** this pointer is only valid until the next vector operation.
A vector operation (such as a `push()`) may cause a memory reallocation which can make this pointer undefined.


<table>
<tr><td> vec       </td><td> Pointer to the vector.</td></tr>
<tr><td> idx       </td><td> The index value. Cannot be <0 or greater than the object count. </td></tr>
<tr><td> return    </td><td> Pointer to the value at the given index </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

**void\* evhead(void\* vec, size_t idx)**  <br/>
Return a the pointer to the first slot in the vector.

**Note** this pointer is only valid until the next vector operation.
A vector operation (such as a `push()`) may cause a memory reallocation which can make this pointer undefined.


<table>
<tr><td> vec       </td><td> Pointer to the vector.</td></tr>
<tr><td> return    </td><td> Pointer to the value at the given index </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

**void\* evnext(void\* vec, size_t idx)**  <br/>
Return a the pointer next value after the head (if `evhead()` was last called ), or next value after the last call to `evnext()`. 
It is invalid to call `evnext()` without first calling `evhead()`. 
When there are no more elements in the vector, `evnext()` returns NULL;

**Note** this pointer is only valid until the next vector operation.
A vector operation (such as a `push()`) may cause a memory reallocation which can make this pointer undefined.


<table>
<tr><td> vec       </td><td> Pointer to the vector.</td></tr>
<tr><td> return    </td><td> Pointer to the next value in the vector, or NULL if there are none. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

**void\* evtail(void\* vec, size_t idx)**  <br/>
Return a the pointer to the last occupied slot in the vector.

**Note** this pointer is only valid until the next vector operation. 
A vector operation (such as a `push()`) may cause a memory reallocation which can make this pointer undefined. 

<table>
<tr><td> vec       </td><td> Pointer to the vector.</td></tr>
<tr><td> return    </td><td> Pointer to the last occupied slot in the vector </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

### Free
EV allocates memory for the underlying array, as well as accounting.
At some point this memory should be freed.

**void\* evfree(void\* vec)** <br/>
Free the memory used to hold the vector and its accounting.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> obj       </td><td> The value to push into the vector. </td></tr>
<tr><td> return    </td><td> NULL. Use `vec = evfree(vec)` to ensure there are no dangling pointers.
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

### Pop and Delete
Functions to remove items from the vector.

**void evpop(void\* vec)**  <br/>
Remove the last value from the vector tail.

**Note:** To use this function `EV_FPOP` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> return    </td><td> None. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**void\* evdel(void\* vec, size_t idx)**  <br/>
Remove a value from the vector at the given index.

**Note:** To use this function `EV_FDEL` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector.</td></tr>
<tr><td> idx       </td><td> The index value. Cannot be <0 or greater than the object count. </td></tr>
<tr><td> return    </td><td> None </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>

### Memory sizing functions
It can be useful to know how much memory is available or in current use.

**size_t evvcnt(void\* vec)**  <br/>
Get the number of slots in the vector

**Note 1:** This function does not return the number of used slots (ie the number of objects) in the vector.
If you are looking for this functionality, please use `evcnt()` function described above. <br/>
**Note 2:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> return    </td><td> The number of slots in the vector. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**size_t size_t evvmem(void* vec)**  <br/>
Get the amount of memory currently used to store the vector including unused slots.

**Note:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> return    </td><td> The amount of memory currently used to store the vector. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**size_t size_t evomem(void\* vec)**  <br/>
Get the amount of memory currently used to store objects in the vector

**Note:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> return    </td><td> The amount of memory currently used to store objects in the vector. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

**size_t evtmem(void\* vec)**  <br/>
 Get the total memory used by the vector including including accounting overheads.

**Note:** To use this function `EV_FMEMSZ` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> return    </td><td> The total amount of memory consumed by the vector. </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>


### Sorting

**void evsort(void\* vec, int (\*compar)(const void\* a, const void\* b))**  <br/>
Sort the elements of the vector in place

**Note:** To use this function `EV_FSORT` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the vector</td></tr>
<tr><td> compar    </td><td> Function pointer which implements the comparison function.
                             This function returns +ve if a > b, -ve if a < b and 0 if a==b. </td></tr>
<tr><td> return    </td><td> None. The vector will be sorted in place if this function succeeds </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>

### Copying

**void\* evcpy(void\* src)**  <br/>
Create a new vector and copy the contents of the source vector into it.

**Note:** To use this function `EV_FCOPY` or `EV_FALL` must be defined.

<table>
<tr><td> vec       </td><td> Pointer to the source vector</td></tr>
<tr><td> return    </td><td> A new vector with the same contents as the source </td></tr>
<tr><td> failure   </td><td> If EV_HARD_EXIT is enabled, this function may cause exit(); </td></tr>
</table>
<hr/>


## Release notes
**25 Nov 2020** - V1.0 <br/>
- Initial release.

<hr/>
<br/>

**4 Jan 2021** - V1.2 <br/>
* Allow evcnt() on null vector (returns 0)
* More pedantic header checking.
* Code formatting cleanup

**29 Nov 2020** - V1.1 <br/>
- Improved quick start documentation with step by step guide.
- Added release notes to documentation
- Added multiple compilation unit support with `EV_HONLY` define.
- Added `test2.c` to show off multiple compilation unit support.  
- Made pedantic checking optional with `EV_PEDATNIC` define.
- Made debug printing optional with `EV_DEBUG` define.

**25 Nove 2020** - V1.0 <br/>
* Initial release of Easy Vector (EV)

<hr/>
<br/>


## Licensing

Copyright (c) 2020, Matthew P. Grosvenor
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
