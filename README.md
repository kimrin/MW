# MW
experimental garbage of programming languages.

## How to build:

```
before build MW, please checkout llvm/llvm-project repository as llvm-project
that same level of MW directory itself.

in MW/llvm directory:

$ ./install.sh

(only once: install LLVM libs. takes very long time...)

then MW/jsonnet directory, run:

$ ./install.sh

and in MW/bin directory:

$ ./install.sh

(this takes several minutes...)
```

`mw` (body of compiler) will generate in `MW/bin`.

## How to run:

```
in MW/bin directory:

$ ./mw ../src/app/expression.py
```

mw reads expression.py python file, then call first class's constructor and `codegen_jsonnet` method.
`codegen_jsonnet` method returns JSONNET of MW program, and mw attempt to send this JSONNET to jsonnet compilers and will get JSON for compilations of MW compiler itself.

The code parts of using LLVM (compiler itself) is not yet implemented.

