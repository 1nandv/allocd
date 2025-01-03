#!/usr/bin/env nu

let flags = [-Wall -Wextra -g]
let cc    = "clang"

def main [] {
    exec $cc ...$flags allocd.c -o allocd
}
