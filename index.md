## Files (index)

### 01_language_core
- `static_cast.cpp` — `static_cast` behavior / compile-time rules
- `functional_cast.cpp` — functional-style cast experiment
- `implicit_conversion.cpp` — implicit conversion cases
- `converting_constructor.cpp` — converting ctor + `explicit`
- `default_arg_overload_resolution.cpp` — default args vs overload resolution
- `volatile.cpp` — `volatile` meaning & optimizer interaction
- `using_declaration_access_control.cpp` — `using` and access control edge cases

### 02_oop
- `virtual_function.cpp` — vtable / override dispatch basics
- `friend_patterns.cpp` — friend usage patterns
- `access_protected_member.cpp` — protected access rules in practice

### 03_templates
- `template_specialization.cpp` — specialization / partial specialization examples
- `template_class_divide.h` + `template_class_divide_impl.h` — header/impl split for templates

### 04_functions
- `functor_basics.cpp` — function objects fundamentals
- `functor_in_for_each.cpp` — functor used in algorithms (`for_each`, etc.)
- `lambda_unique_closure_type.cpp` — closure type uniqueness / capture behavior

### 05_memory
- `new_delete.cpp` — operator new/delete behavior
- `std_allocator_basic_usage.cpp` — allocator basic usage notes
- `type_punning.cpp` — type punning / aliasing pitfalls

### 06_exceptions
- `exception_propagation.cpp` — propagation rules
- `stack_unwinding.cpp` — unwinding & destructor calls
- `destructor_throw.cpp` — throwing from destructors (terminate / noexcept)

### 07_optimization
- `RVO.cpp` — RVO experiment
- `RVO_vs_move.cpp` — RVO vs move interaction
- `TCO.cpp` — tail call optimization experiment

### 08_iterators
- `iterator_category.cpp` — iterator categories & dispatch patterns

### 09_concurrency
- `std_atomic.cpp` — atomic basics / memory ordering entry point
