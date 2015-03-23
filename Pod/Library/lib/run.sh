#!/bin/bash
lipo $1.a -thin armv7 -output ./armv7/$1-armv7.a
lipo $1.a -thin arm64 -output ./arm64/$1-arm64.a
lipo $1.a -thin i386 -output ./i386/$1-i386.a
lipo $1.a -thin x86_64 -output ./x86_64/$1-x86_64.a
