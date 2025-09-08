# my C utils lib

- `mrs_strings.h`: string manipulation library
- `mrt_test.h`: mini test library 
- `mrl_logger.h`: logging utilities 
- `mrd_debug.h`: memory allocation debugging info. `-DDEBUG` or `#define DEBUG` to enable debug logging
    TODODOODODO keep track of total alloced regions. if some not free, have some debug output.
    keep track of all pointers?

eg: this PTR (id) with this DATA that was allocated HERE, was not freed

### TODO
- [ ] add left rigth comparison for failed tests 
- [ ] malloc calloc realloc free wrappers for debugging memory leaks
- [x] debug maintain only active allocations. eg: freed and not realloced to something not freed

i hope you like UB!


![https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif](https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif)
