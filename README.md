# Easy Vector
Easy Vector (ezv) is a small and simple header only C vector type. 
It's designed for situations like options parsing where building up an unknown amount of state is required. 

The neat thing about this vector is that it maintains array semantics by hiding state near the allocated memory. 
Some (optional) macro magic makes it even easier to use.

