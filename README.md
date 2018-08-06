## tack

A recreation of [Gary Bernhardt's
Selecta](https://github.com/garybernhardt/selecta) written in C.

## Usage

```
make
cat (ls *.txt | ./tack)
```

See [Selecta's
readme](https://github.com/garybernhardt/selecta/blob/master/README.md) for a
thorough explanation of general use.

## Motivation

Tack's primary purpose is to serve as a learning exercise so I could work with
some data structures and algorithms in C. I don't expect tack to replace
Selecta. That said, I want tack to be usable and useful - I don't consider it to
be a toy project.

There is currently no compelling reason to use tack over Selecta. Tack is faster
than Selecta but not so much that it matters. Unless you're regularly dumping
10s or 100s of thousands of lines of input into Selecta, you won't even notice
the speed difference. The file of test data I use when developing tack contains
50k lines and Selecta works fine with that amount - there is a little lag but
it's not at all problematic. Tack is roughly as fast with 150k lines as Selecta
is with 50k.

On the flip side, tack has zero support for Unicode or multi-byte characters at
all. It very much assumes ASCII. This isn't because I think Unicode is
unimportant - quite the opposite - but for my general usage of a tool like this
ASCII suffices, and in C, dealing with ASCII only is just easier. I may one day
add Unicode support, but until then, if you need Unicode support, you should
stick with Selecta.

## License

BSD 2-clause
