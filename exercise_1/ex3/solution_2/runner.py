import sys
import subprocess

def get_cacheline_size():
    result = subprocess.run(['getconf', 'LEVEL1_DCACHE_LINESIZE'], capture_output=True)
    return int(result.stdout)

def run_make():

    result = subprocess.run(['make', 'clean'], text=True)
    if result.returncode != 0:
        print(f"Error: make clean exited with return code {result.returncode}")

    result = subprocess.run(['make'], text=True)
    if result.returncode != 0:
        print(f"Error: make exited with return code {result.returncode}")

def run_exec(arg):

    app = ['./app'] + arg

    result = subprocess.run(app, text=True)

    if result.returncode != 0:
        print(f"Error: Program exited with return code {result.returncode}")
        if result.returncode == -11:
            print(f"Segmentation Fault")
        elif result.returncode == 1:
            print(f"Wrong number of arguments")
        elif result.returncode == 2:
            print(f"Ran out of heap memory")

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print('Usage: python3 runner.py <threadcount> <iterations>')
        sys.exit(1)

    cache_line_size = get_cacheline_size()

    arg = [sys.argv[1], sys.argv[2], str(cache_line_size)]

    print()

    print('\n**************************\nBuild\n**************************\n')
    run_make()

    print('\n**************************\nExecution\n**************************\n')
    run_exec(arg)

    print('\n')
