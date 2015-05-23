#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "atom.h"
#include "bin-tree.h"
#include "lightning.h"
#include "mem.h"

struct node {
    const char *key;
    void *value;
    struct node *L;
    struct node *R;
};

struct BinTree {
    struct node *root;
    CmpFunction cmp;
};

static int
BinTree_insert_under(BinTree tree, struct node *root, struct node *n);

static void *
BinTree_lookup_under(BinTree tree, struct node *root, const char *key);

static void
BinTree_free_under(struct node *root);

BinTree
BinTree_init(CmpFunction cmp)
{
    BinTree tree;
    NEW(tree);
    tree->root = NULL;
    tree->cmp = cmp == NULL ? (CmpFunction) strcmp : cmp;
    return tree;
}

int
BinTree_insert(BinTree tree, const char *key, void *value)
{
    assert(tree);
    int cmp;
    struct node *n;
    NEW(n);
    n->key = Atom_string(key);
    n->value = value;
    n->L = n->R = NULL;

    if (tree->root == NULL) {
        tree->root = n;
    } else {
        cmp = tree->cmp(key, tree->root->key);
        if (cmp < 0) {
            if (tree->root->L == NULL) {
                tree->root->L = n;
                return 0;
            } else {
                BinTree_insert_under(tree, tree->root->L, n);
            }
        } else if (cmp > 0) {
            if (tree->root->R == NULL) {
                tree->root->R = n;
                return 0;
            } else {
                BinTree_insert_under(tree, tree->root->R, n);
            }
        } else {
            /* duplicate */
        }
    }
    return 0;
}

static int
BinTree_insert_under(BinTree tree, struct node *root, struct node *n)
{
    int cmp = tree->cmp(n->key, root->key);
    if (cmp < 0) {
        if (root->L == NULL) {
            root->L = n;
            return 0;
        } else {
            return BinTree_insert_under(tree, root->L, n);
        }
    } else if (cmp > 0) {
        if (root->R == NULL) {
            root->R = n;
            return 0;
        } else {
            return BinTree_insert_under(tree, root->R, n);
        }
    } else {
        /* duplicate */
        return 0;
    }
}

void *
BinTree_lookup(BinTree tree, const char *key)
{
    assert(tree);
    int cmp;
    if (tree->root == NULL) {
        return NULL;
    } else {
        cmp = tree->cmp(key, tree->root->key);
        if (cmp == 0) {
            return tree->root->value;
        } else if (cmp < 0) {
            if (tree->root->L == NULL) {
                return NULL;
            } else {
                return BinTree_lookup_under(tree, tree->root->L, key);
            }
        } else {
            if (tree->root->R == NULL) {
                return NULL;
            } else {
                return BinTree_lookup_under(tree, tree->root->R, key);
            }
        }
    }
}

static void *
BinTree_lookup_under(BinTree tree, struct node *root, const char *key)
{
    int cmp = tree->cmp(key, root->key);
    if (cmp == 0) {
        return root->value;
    } else if (cmp < 0) {
        if (root->L == NULL) {
            return NULL;
        } else {
            return BinTree_lookup_under(tree, root->L, key);
        }
    } else {
        if (root->R == NULL) {
            return NULL;
        } else {
            return BinTree_lookup_under(tree, root->R, key);
        }
    }
}

/* We may want to pass in a function pointer
   to free the nodes */
void
BinTree_free(BinTree *tree)
{
    assert(tree && *tree);
    struct node *root = (*tree)->root;
    if (root != NULL) {
        BinTree_free_under(root);
    } else {
        /* null tree */
    }
    FREE(*tree);
}

static void
BinTree_free_under(struct node *root)
{
    if (root->L != NULL) {
        BinTree_free_under(root->L);
    }
    if (root->R != NULL) {
        BinTree_free_under(root->R);
    }
    FREE(root);
}
