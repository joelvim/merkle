open Merkle.Tree;
open Bytes;
open QCheck;

let (>=>) = Result.bind;
let hash = (data: bytes) => data;

let data = (size_gen: Gen.t(int)) =>
  Gen.string_size(size_gen) |> Gen.map(of_string);
let valid_data = data(Gen.int_range(1, 256));

let invalid_data = data(Gen.return(0));

let arbitrary_data = {
  let size_gen =
    Gen.bool
    |> Gen.map(
         fun
         | true => 10
         | false => 0,
       );
  data(size_gen);
};

let size =
  Test.make(
    ~name="Size of the tree should be the same as the input data",
    QCheck.(pair(make(valid_data), list(make(valid_data)))),
    ((head, tail)) => {
      let tree_size =
        initialize_tree(hash, head, tail)
        |> Result.map(size)
        |> Result.value(~default=-1);

      tree_size == List.length(tail) + 1;
    },
  );

let only_leaf =
  Test.make(
    ~name="A tree with only one node should be a leaf",
    QCheck.(make(valid_data)),
    head => {
    initialize_tree(hash, head, [])
    |> Result.map(is_leaf)
    |> Result.value(~default=false)
  });

let two_leaves =
  Test.make(
    ~name="A tree with more than one leaf should be a node",
    QCheck.(pair(make(valid_data), make(valid_data))),
    ((head, other)) => {
    initialize_tree(hash, head, [other])
    |> Result.map(is_node)
    |> Result.value(~default=false)
  });

let invalid_data_first =
  Test.make(
    ~name="It should return an error if head is invalid",
    QCheck.(pair(make(invalid_data), list(make(valid_data)))),
    ((head, tail)) =>
    Result.is_error(initialize_tree(hash, head, tail))
  );

let invalid_data_others =
  Test.make(
    ~name="It should return an error if others contain invalid data",
    QCheck.(
      triple(
        make(valid_data),
        make(invalid_data),
        list(make(arbitrary_data)),
      )
    ),
    ((head, invalid, tail)) =>
    Result.is_error(initialize_tree(hash, head, List.cons(invalid, tail)))
  );

let validate_tree =
  Test.make(
    ~name="It should validate when using same data to build tree and validate",
    QCheck.(pair(make(valid_data), list(make(valid_data)))),
    ((head, tail)) =>
    initialize_tree(hash, head, tail)
    |> Result.map(validate_data_with_tree(head, tail))
    |> Result.value(~default=false)
  );

let different_tree =
  Test.make(
    ~name=
      "It should not validate different data than the one that produced the tree",
    QCheck.(
      quad(
        make(valid_data),
        list(make(valid_data)),
        make(valid_data),
        list(make(valid_data)),
      )
    ),
    ((head, tail, other_head, other_tail)) =>
    initialize_tree(hash, head, tail)
    |> Result.map(validate_data_with_tree(other_head, other_tail))
    |> Result.map(valid => valid == false)
    |> Result.value(~default=false)
  );

let () = {
  let size = [QCheck_alcotest.to_alcotest(size)];

  let initialize =
    List.map(
      QCheck_alcotest.to_alcotest,
      [only_leaf, two_leaves, invalid_data_first, invalid_data_others],
    );

  let validate =
    List.map(QCheck_alcotest.to_alcotest, [validate_tree, different_tree]);

  Alcotest.run(
    "merkle_tree_lib",
    [("initialize", initialize), ("size", size), ("validate", validate)],
  );
};
