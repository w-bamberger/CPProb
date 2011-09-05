
/**
 *
 * @page Notes
 *
 * @section structure-probability-model Structure of a probability model
 *
 * This section considers probability models of the kind that a random
 * variable (y) has a distribution (f_y) assigned. This distribution
 * in turn depends on another random variable (x), which also has a
 * distribution (x) assigned. So we have
 *
 * x ~ f_x()@n
 * y ~ f_y(x)
 *
 * To represent this in C++, several strategies come to mind.
 *
 * 1. Introduce random variable nodes and distribution nodes.
 *    A random variable node always points to a distribution node.
 *    A distribution node in turn points to several parent nodes.
 *    Store a list of all random variable nodes and a list of all
 *    distribution nodes using variants.
 *
 * 2. A node contains a variant for a random variable and a variant
 *    for a distribution. (This needs many type conversions.)
 *
 * 3. A node type is defined by the combination of a random variable
 *    and a distribution.
 */
