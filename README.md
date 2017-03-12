# Brainfuck Compiler

A [brainfuck](http://esolangs.org/wiki/Brainfuck) compiler written in C.

## Usage

You can use it to compile to assembly. This can then be assembled using `gcc`, and executed directly.

	$ make
    $ ./bfc examples/hello.b hello.s
    $ gcc -o hello hello.s
    $ ./hello
    Hello World!

## Platforms

Targets x86_64 assembly on linux.

## Limitations

The compiler uses a fixed-size array of 4000 elements for the cells.

## Optimizations

This is not (yet) an optimizing compiler.
