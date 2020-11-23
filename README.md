# Easy Vector (EV)
Easy Vector is a small and simple header only C vector type. 
It's designed for situations like options parsing where building up an unknown amount of state is required. 

The neat thing about this vector is that it maintains array semantics by hiding state near the allocated memory. 
Some (optional) macro magic makes it even easier to use.

## Quick Start

Getting started with easy Vector is ... easy. 
The follwoing demo allocates a new vector and pushes the value of "1" into it.

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