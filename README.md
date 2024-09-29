# Oblivion

Cache oblivious ordered list implementation.

## Build and Run

### Requirements

To build and run the project, ensure you have the following installed:

- **Compiler**: `clang++`, `gcc` or `cl`
- **Build System**: `CMake` and `make`

### Building the Project

First generate all configuration and build files with:

```bash
cmake .
```

Then, to build all executables and libraries, run the following command in the project directory:

```bash
make
```

### Running the Executable

To run the command-line interface (CLI), use one of the following command:

```bash
make && ./bin/oblivion-cli
```

## Usage

You can run the command line interface provided using the following syntax:

```
oblivion-cli [option] [filename]
```

### Options

- `-f [filename]` : Specify a file containing the operations to be processed.
- `-i` : Iteractive mode (accept operations from the STDIN).
- `-h` : Display help information about the available options and usage.

### Examples

1. Using a file:
```bash
$ oblivion-cli -f test.txt
```
This will generate a file named `test.txt.out` with the results of the execution.

2. Using standard input:
```bash
$ oblivion-cli -i
INC 5
INC 6
INC 0
IMP
SUC 5
```

3. Displaying help:
```bash
oblivion-cli -h
```
