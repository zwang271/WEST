# Examples

```{toctree}
:caption: Examples
:maxdepth: 1
:titlesonly:
:glob:
Examples/*
```

The `Examples` directory holds a set of example mltl and csv files to be run using the
R2U2 tool suite.

For each example, first run the mltl file through the compiler then run the
R2U2 executable with the generated binary files.

To compile the first example:
python r2u2/compiler/r2u2prep.py r2u2/docs/Examples/ex01.mltl r2u2/docs/Examples/ex01.csv

Then to run R2U2:
/path/to/r2u2 r2u2/tools/gen_files/binary_files r2u2/Documentation/Examples/ex01.csv

This pattern should follow for the rest of the examples unless otherwise stated.
