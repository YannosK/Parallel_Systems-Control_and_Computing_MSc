import argparse
import subprocess

##############################################################
# Function definitions
##############################################################

def run_make():
    '''
    Builds the program application
    using the provided Makefile
    '''

    result = subprocess.run(['make', '-C', '../source/parallel/', 'clean'], text=True)
    if result.returncode != 0:
        print(f"Error: make clean exited with return code {result.returncode}")

    result = subprocess.run(['make', '-C', '../source/parallel/'], text=True)
    if result.returncode != 0:
        print(f"Error: make exited with return code {result.returncode}")

def run_exec(n, arg):
    '''
    Runs the compiled executable
    passing as input the inserted arguments

    Args:
    - n : the number of processes
    - arg: a list of string with the CLI arguments
    '''

    app = ['mpiexec', '-n', str(n), '../source/parallel/build/app'] + arg

    result = subprocess.run(app, text=True, capture_output=True)

    if result.stderr:
        print('Error message in the execution:')
        print(result.stderr)
    else:
        if result.returncode != 0:
            print(f"Error: Program exited with return code {result.returncode}")
            if result.returncode == -11:
                print(f"Segmentation Fault")
            elif result.returncode == 1:
                print(f"Wrong number of arguments")
            elif result.returncode == 2:
                print(f"Wrong arguments: too small problem and too many processes")
            elif result.returncode == 3:
                print(f"Wrong arguments: problem size must be divisible by the number of processes")
        else:
            print(result.stdout)

##############################################################
# CLI parser configuration
##############################################################

parser = argparse.ArgumentParser()
parser.add_argument('processcount', help='The number of processes')
parser.add_argument('size', help='Number of rows and columns of the square matrix - problem size')

##############################################################
# main program logic
##############################################################

if __name__ == "__main__":

    args = parser.parse_args()

    if int(args.processcount) > int(args.size):
        raise AssertionError("Processes should be less than the problem size")
    else:
        if int(args.size) % int(args.processcount) != 0:
            raise AssertionError("Processes should be divisible from the problem size")
    
    arg = [value for key, value in vars(args).items() if key not in ['processcount'] and value is not None]

    print('\n**************************\nBuild\n**************************\n')
    run_make()

    print('\n**************************\nExecution\n**************************\n')
    run_exec(args.processcount ,arg)

    print('\n')

