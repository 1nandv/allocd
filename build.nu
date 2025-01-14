#!/usr/bin/env nu

let cc = "clang"
let flags = [-Wall -Wextra -g]

def "main naive" {
    let nflags = [-O0 -shared -fPIC]
    exec $cc ...$flags ...$nflags ./naive.c -o ./naive.so
}

def main [p: string] {
    # todo
}
