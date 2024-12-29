# Exercise 1

## Compilation

To compile the program of exercise 1, you can use the following command:

```bash
make [DEBUG=1]
```

In order to clean the binary files, you can use the following command:

```bash
make clean
```

## Scripts

To run the `exec.py` script install the packages specified in `requirements.txt` and see the help message first:

```bash
python3 ./exec.py -h
```

To include a font in the figures create an `.env` file with the font path on your system:

```bash
FONT_PATH="<path_to_font_file>"
```

If none is provided, the default matplotlib font will be used.
