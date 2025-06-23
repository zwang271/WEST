# C2PO

C2PO (Configuration Compiler for Property Organization) is the formula compiler for R2U2.

## Usage

C2PO requires an input file and a file for generating a signal mapping (i.e., to tell which variable each input corresponds to during runtime) in order to generate a specification binary.

To compile a C2PO file, run the `c2po.py` script with a `.csv` or `.map` file as argument. One of the `--booleanizer`/`-bz` or `--atomic-checker`/`-at` flags must be set. For instance, to run an example:

    python c2po.py --booleanizer --trace examples/cav.csv examples/cav.c2po 

The assembled binary is generated at `spec.bin` by default and is ready to be run by a properly configured R2U2 over input data. For full compiler options:

    python c2po.py -h

C2PO requires Python 3.8 or newer.

## C2PO File Format

C2PO files are used as input to C2PO and use C2PO's specification language. They include various sections: 

- **INPUT**: Where input signals and their types are declared
- **FTSPEC**: Where future-time MLTL specifications are defined. These specifications will use SCQs for their memory.
- **PTSPEC**: Where past-time MLTL specifications are defined. The specifications will use box queues for their memory.
- **STRUCT**: Where C-like structs are defined.
- **DEFINE**: Where macros can be defined.
- **ATOMIC**: Where atomics used by the atomic checker are defined. *Must compile with `--atomic-checker`/`-at` flag.*

See `syntax.md` for a formal description of the input file format and `examples/` directory for sample files.

## CSV File Format

A CSV file given to C2PO as input has a `.csv` file extension and requires a header denoted with a '#' character as the first character of the line. For instance:

    # sig0,sig1
    0,1

is a valid csv file.

## Signal Map File Format

A signal map file has a `.map` file extension. Each line of the input file should be of the form `SYMBOL ':' NUMERAL` such that if `SYMBOL` corresponds to a signal identifier in the MLTL file, its signal ID is set to the integer value of `NUMERAL`.

Note that if `SYMBOL` is not present in the MLTL file, the line is ignored.

## Booleanizer

The booleanizer is a general purpose engine that can perform arithmetic, bitwise operations, parameterized set aggregation (`foratleastn`, etc.), and other such capabilities. 

## Atomic Checker

Atomic checkers are the names for the relational expression seen in `examples/atomic_checker.c2po`. They allow for the filtering of signals and then comparing the output of that signal to a constant or other signal. This is particularly useful in hardware, but is available in the software version of R2U2 as well.

