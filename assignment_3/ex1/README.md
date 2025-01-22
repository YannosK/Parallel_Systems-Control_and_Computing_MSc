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

Also, a file containing all the hosts for the MPI to find should be created.
Name it `machines` and put the IP addresses (or DNS names)
of the machines in the following format:

```bash
<ip_address_1>:<number_of_processes_1>
<ip_address_2>:<number_of_processes_2>
...
<ip_address_n>:<number_of_processes_n>
```
