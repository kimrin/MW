#!/bin/bash

cmake ../src/jsonnet
cmake --build .
cmake --build . --target install
