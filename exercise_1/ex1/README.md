# Exercise 1

## Compilation

To compile the program of exercise 1, you can use the following command:

```bash
make LIBS="-lpthread" all
```

In order to clean the binary files, you can use the following command:

```bash
make clean
```

## Scripts

To run the `exec.sh` script see the help message first:

```bash
python3 ./exec.sh -h
```

You need to create a file `conf.py` with the following content:

```python
font_path = "<path_to_font_file>"
```