# Merkle tree library

This project implements a simple merkle tree library in ReasonML.

## How to build an run the tests?

This project is managed by [esy](https://esy.sh/) build tool.

To build the project, run the following commands

```sh

esy install
esy pesy
esy test

```

## Choices made

### Hashing function
I chose not to rely on a given hash function.
As the purpose of the test was not to evaluate cryptography knowledge and method,
I tried to remain generic against hashing.
It allows switching hashing function, and testing with any dumping hashing function.
In my case, the identity function :)

### Tree initilization

I consider initializing a merkle tree with an empty dataset is a non sense. 
So I prevent it by forcing the API user to provide at least one data as input.
I would have loved to have an API with a single non empty list parameter, 
but I don't know OCaml ecosystem well enough to use the right type.
So I did it simply : two params, the head of the list, and the tail of the list that can be empty.

### Empty chunks

Same as for the dataset, I consider an empty chunk of bytes is not a valid input.
However, I do not know if it is possible by type to enforce non emptyness of a `bytes` value.
So I return an error if an empty chunk is received.

### Error handling

OCaml provides various ways to handle errors (exceptions, result type, even option if you want).
I like strongly typed APIs, and I like knowing that a method can fail by reading it's type.
My Java/Scala experience taught me that exceptions are error 
prone and often leads to porrly handled or forgotten exceptions.
So I prefer using the result type way of doing things,
event if it can have a performance impact., but in a first time, 

### Performance

Regarding the tree algorithm, I am not good at writing high performance program.
I prefer writing a correct program, easy to reason about and maintain 
than a perfectly optimized one that could fail in some cases that where not anticipated.

I consider that an algorithm can be optimized if needed, but optimization should not be blind,
and must be done in the context of a real use case scenario, to properly identify the performance
bottlenecks.