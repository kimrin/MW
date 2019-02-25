#!/bin/bash

cmake ../../llvm-project/llvm/
cmake --build .
cmake --build . --target install
