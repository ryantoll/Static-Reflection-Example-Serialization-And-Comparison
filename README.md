# Static-Reflection-Example-Serialization-And-Comparison

This is an example of static reflection in C++17 meant to demonstrate broad techniques and concepts.
I used an approach similar to this in order to build a replacement for an in-house serialization/deserialization library, which had appreciable runtime overhead.
The actual production code could accomodate arbitrarily complex types in large code base.
This was then combined with the Curiously Recurring Template Pattern (CRTP) to allow consumers to easily "mix-in" this functionality into a class with only modest setup.
Further functionality could then be created with this infrastructure in place.
Similar techniques were extended using the Barton–Nackman Trick to inject free functions for binary operator== and operator!= using a similar CRTP mix-in methodology.

One can see that there is no runtime space required to add this extra capability -- not even a v-table.
This approach also avoids gratuitous costly runtime operations such as (additional) heap allocations (above and beyond generating the final output).
The compiler has sufficient information at compile time to create a full plan for serialization based around straightforward pointer arithmetic.
The compiler may even be able to remove the metadata from the final build once the instructions are generated (depending on how the code is arranged).
Certain operations, such as equality, are quite amenable to being done entirely at compile-time as shown.

A big key here is the use of std::tuple as an array-like pseudo-container.
This allows the grouping of heterogeneous types for the metadata.
I need to track both the parent object type as well as the type of each individual element.
The std::tuple object preserves the type information as well as being constexpr capable in C++17.
One could investigate further refinement by starting with Boost MP11.
https://www.boost.org/doc/libs/1_87_0/libs/mp11/doc/html/simple_cxx11_metaprogramming.html
https://www.youtube.com/watch?v=yriNqhv-oM0


Not demonstrated here is the issue of inheritance.
Mixing in member functions creates an ambiguity with the serialization operations.
One solution is to manually replicate the interface for relevant classes, then create a concept of serialization to allow the serialization ecosystem to generically accept any such object.
(C++17 can approximately replicate C++20 concepts by testing for all of the necessary interfaces.)
Alternatively, these could be made into free functions as was done with the lexographical equality checks.
The later technique eliminates ambiguity by having the fully-derived type in the function argument list, which allows for clear overload resolution.
