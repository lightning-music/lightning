#ifndef BINTREE_H_INCLUDED
#define BINTREE_H_INCLUDED

#include "lightning.h"
#include <stddef.h>

/* It is a checked runtime error to pass a NULL BinTree to any
   function in this interface */

typedef struct BinTree *BinTree;

/**
 * Initialize a new binary tree
 */
BinTree
BinTree_init(CmpFunction cmp);

/**
 * Insert a new node into the tree
 * Returns 0 on success, nonzero on failure
 */
int
BinTree_insert(BinTree tree, const char *key, void *value);

/**
 * Lookup a node by key
 * Return NULL if not found, return the value of the node
 * if it was found
 */
void *
BinTree_lookup(BinTree tree, const char *key);
               
/**
 * Free all the tree's nodes and the tree structure itself
 */
void
BinTree_free(BinTree *tree);

#endif // BTREE_H_INCLUDED
