# Brainfuck Compiler

A [brainfuck](http://esolangs.org/wiki/Brainfuck) compiler written in C.

## Usage
    $ ./bfc --help
    Usage: bfc [OPTION...] infile
    BFC - A brainfuck compiler written in C. Goes directly to ASM

    -o, --output=FILE          Output to FILE instead of default(infile.s)
    -O, --disableOptimization  Disable optimization
    -?, --help                 Give this help list
        --usage                Give a short usage message

You can use it to compile to assembly. This can then be assembled using `gcc`, and executed directly.

    $ make
    $ ./bfc -o hello.s examples/hello.b 
    $ gcc -o hello hello.s
    $ ./hello
    Hello World!

## Platforms

Targets x86_64 assembly on linux.

## Limitations

The compiler uses a fixed-size array of 30,000 elements for the cells. This makes it not technically turing complete.

## Optimizations

### Peephole optimisations

#### Combining Instructions

We combine successive increments/decrements:

```
    Compile             Combine
+-++  =>   Increment  1   =>   Increment 2
           Increment -1
           Increment  1
           Increment  1
```

If increments/decrements cancel out, we remove them entirely.

```
   Compile             Combine
+-   =>   Increment  1    =>   # nothing!
          Increment -1
```

We combine pointer increments:

```
    Compile                   Combine
><>>  =>   PointerIncrement  1   =>   PointerIncrement 2
           PointerIncrement -1
           PointerIncrement  1
           PointerIncrement  1
```

We remove increments if there's a read immediately after:

```
            Combine
Increment 1   =>   Read
Read

```

We remove successive reads:

```
     Combine
Read   =>   Read
Read

```