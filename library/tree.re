[@text {|Tree - a naive merkle tree implementation .|}];

open Bytes;

/**
[t] represents a merkle tree.
    Each element in the tree (whether it is a node or a leaf) is identified by its hash.
    If modified, the hash will change
 */
type tree =
  | Leaf(bytes)
  | Node(bytes, tree, tree);

type t =
  | HashWithTree(bytes => bytes, tree);

type error =
  | InvalidInputData;

let (>=>) = Result.bind;

let rec inner_size = (t: tree) => {
  switch (t) {
  | Leaf(_) => 1
  | Node(_, left, right) => inner_size(left) + inner_size(right)
  };
};

let size = (t: t) => {
  let HashWithTree(_, tree) = t;

  inner_size(tree);
};

let hash = (t: tree): bytes => {
  switch (t) {
  | Leaf(leaf_hash) => leaf_hash
  | Node(node_hash, _, _) => node_hash
  };
};

let equal = (first_tree: t, second_tree: t) => {
  let HashWithTree(_, first_t) = first_tree;
  let HashWithTree(_, second_t) = second_tree;

  // FIXME we should compare hash functions used
  // to generate the trees, that would be no luck that two hashes
  // from two different hash functions are equal

  Bytes.equal(hash(first_t), hash(second_t));
};
let (===) = equal;

let inner_append =
    (hash_function: bytes => bytes, data: bytes, t: tree)
    : result(tree, error) =>
  if (Bytes.length(data) == 0) {
    Error(InvalidInputData);
  } else {
    let node_hash =
        (hash_function: bytes => bytes, left_hash: bytes, right_hash: bytes) =>
      hash_function(Bytes.cat(left_hash, right_hash)); // Not sure concatenation is the standard way to do this, I've read somewhere about an XOR?

    let insert_to_right =
        (hash_function: bytes => bytes, left_tree: tree, data) => {
      let newLeafHash = hash_function(data);
      Node(
        node_hash(hash_function, hash(left_tree), newLeafHash),
        left_tree,
        Leaf(newLeafHash),
      );
    };

    switch (t) {
    | Leaf(hash) as leaf => Ok(insert_to_right(hash_function, leaf, data))
    | Node(hash, left, right) as left_tree
        when inner_size(left) == inner_size(right) =>
      Ok(insert_to_right(hash_function, left_tree, data))
    | Node(_, left, right) =>
      let new_right_tree = insert_to_right(hash_function, right, data);
      Ok(
        Node(
          node_hash(hash_function, hash(left), hash(new_right_tree)),
          left,
          new_right_tree,
        ),
      );
    };
  };

let append = (data: bytes, t: t): result(t, error) => {
  let HashWithTree(hash_function, tree) = t;

  inner_append(hash_function, data, tree)
  |> Result.map(new_tree => HashWithTree(hash_function, new_tree));
};

let initialize_tree =
    (
      hash_function: bytes => bytes,
      first_element: bytes,
      other_elements: list(bytes),
    )
    : result(t, error) =>
  if (Bytes.length(first_element) == 0) {
    Error(InvalidInputData);
  } else {
    let first_leaf = Ok(Leaf(hash_function(first_element)));

    let addWithHash = (acc: result(tree, error), current: bytes) =>
      acc >=> inner_append(hash_function, current);

    List.fold_left(addWithHash, first_leaf, other_elements)
    |> Result.map(new_tree => HashWithTree(hash_function, new_tree));
  };

let validate_data_with_tree =
    (first_element: bytes, other_elements: list(bytes), tree: t): bool => {
  let HashWithTree(hash_function, _) = tree;

  initialize_tree(hash_function, first_element, other_elements)
  |> Result.map(equal(tree))
  |> Result.value(~default=false);
};
let is_leaf = {
  fun
  | HashWithTree(_, Leaf(_)) => true
  | HashWithTree(_, Node(_, _, _)) => false;
};

let is_node = (tree: t) => !is_leaf(tree);
