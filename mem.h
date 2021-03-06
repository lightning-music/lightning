#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

extern void *
lightning_mem_alloc(long nbytes,
          const char * file,
          int line);

extern void *
lightning_mem_calloc(int count,
           long nbytes,
           const char * file,
           int line);

extern void
lightning_mem_free(void * ptr,
         const char * file,
         int line);

extern void *
lightning_mem_resize(void * ptr,
           long nbytes,
           const char * file,
           int line);

#define ALLOC(nbytes) \
    lightning_mem_alloc((nbytes), __FILE__, __LINE__)

#define CALLOC(count, nbytes) \
    lightning_mem_calloc((count), (nbytes), __FILE__, __LINE__)

#define NEW(p) ((p) = ALLOC((long) sizeof *(p)))
#define NEW0(p) ((p) = CALLOC(1, (long) sizeof *(p)))
#define FREE(p) ((void) (lightning_mem_free((p), __FILE__, __LINE__), (p) = 0))
#define RESIZE(p, nbytes) ((p) = lightning_mem_resize((p), (nbytes), __FILE__, __LINE__))

#endif // MEM_H_INCLUDED
