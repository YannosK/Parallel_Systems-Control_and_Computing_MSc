# Exercise 4

## Compilation

To compile the program of exercise 2, you can use the following command:

```bash
make DEFINES="-D[RWLOCK_POLICY] LIBS="-lpthread"
```

The `RWLOCK_POLICY` to choose from are the following:

- `DEFAULT`: the implementation of the `pthread` library.
- `READER_PRIORITY_POLICY`: the reader priority policy.
- `WRITER_PRIORITY_POLICY`: the writer priority policy.

In order to clean the binary files, you can use the following command:

```bash
make clean
```

## Scripts

To run the `exec.sh` script install the packages specified in `requirements.txt` and see the help message first:

```bash
python3 ./exec.sh -h
```

You need to create a file `conf.py` with the following content:

```python
font_path = "<path_to_font_file>"
```
