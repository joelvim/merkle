/** The tree module type */
type t;

/** Type holding library errors */
type error;

/**
Initialize a tree from a list of bytes.

It takes a hash function so it can support many hashing algorithms.

It takes a non empty list (represented by two params, the head and the tail)
because there would be no sense to build a merkle tree on an empty dataset.

It returns a result that can return an error if the input
list contains invalid data. We consider an empty byte is invalid,
as the purpose of a merkle tree is ensuring data integrity,
if there is no data, you cannot ensure its integrity.
 */
let initialize_tree:
  (bytes => bytes, bytes, list(bytes)) => result(t, error);

/** return the size of a tree */
let size: t => int;

/** Check if two merkle trees are equal */
let equal: (t, t) => bool;

/** Same as [equal] */
let (===): (t, t) => bool;

/** Append some data to an existing merkle tree */
let append: (bytes, t) => result(t, error);

/**
Validate a dataset against a given tree

Same as initialize, we consider there is no meaning to validate an empty dataset
as it could not have been used to generate the tree.
*/
let validate_data_with_tree: (bytes, list(bytes), t) => bool;

/** Check if a tree is a leaf (contains only one element) */
let is_leaf: t => bool;

/** Check if a tree is a node */
let is_node: t => bool;
