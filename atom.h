#ifndef ATOM_H_INCLUDED
#define ATOM_H_INCLUDED

/* Atom interface graciously lifted from
   C Interfaces and Implementations
   by David R. Hanson

   His definition of an Atom is:
   > An atom is a pointer to a unique, immutable
   > sequence of zero or more arbitrary bytes.

   > There is only a single occurrence of any atom,
   > which is why it's called an atom. Two atoms are
   > identical if they point to the same location.
   > Comparing two byte sequences for equality
   > by simply comparing pointers is one of the
   > advantages of atoms. Another advantage is that
   > using atoms saves space because there is
   > only one occurrence of each sequence. */

extern int
Atom_length(const char *str);

extern const char *
Atom_new(const char *str, int len);

extern const char *
Atom_string(const char *str);

extern const char *
Atom_int(long n);

extern int
Atom_cmp(const char *x, const char *y);

#endif
