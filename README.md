# cpp-concepts

A collection of small, focused C++ programs exploring core language concepts.  
Each file isolates one topic so you can observe how the language + compiler actually behave.  
(This repo is for learning by experiment, not as a tutorial.)
 
Still updating...

## Structure (topics)

- `00_setup/`  
  build/run scripts, misc utilities

- `01_language_core/`  
  casts, implicit conversions, overload resolution, `volatile`, `using` access rules

- `02_oop/`  
  virtual functions, access control, `friend` patterns

- `03_templates/`  
  specialization, header/impl split experiments

- `04_functions/`  
  functors, lambdas, closure types, algorithm interaction

- `05_memory/`  
  `new/delete`, allocators, type punning

- `06_exceptions/`  
  exception propagation, stack unwinding, throwing in destructors

- `07_optimization/`  
  RVO / move interactions, TCO experiments

- `08_iterators/`  
  iterator category / traits-related experiments

- `09_concurrency/`  
  atomics basics


## How to run

Examples:
```bash
# build & run a single file
g++ -std=c++98 -Wall -Wextra -Werror 01_language_core/implicit_conversion.cpp && ./a.out

# or use your script if you keep one
./00_setup/run.sh
```

## Related Repositories
- [cpp-containers](https://github.com/Yonaim/cpp-containers)
- [cpp-custom-allocators](https://github.com/Yonaim/cpp-custom-allocators)
