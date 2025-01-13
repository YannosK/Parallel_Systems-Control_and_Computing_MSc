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

    result = subprocess.run(['make', '-C', '../source/sequential/', 'clean'], text=True)
    if result.returncode != 0:
        print(f"Error: make clean exited with return code {result.returncode}")

    result = subprocess.run(['make', '-C', '../source/sequential/'], text=True)
    if result.returncode != 0:
        print(f"Error: make exited with return code {result.returncode}")

def run_exec(arg):
    '''
    Runs the compiled executable
    passing as input the inserted arguments

    Args:
    - arg: a list of string with the CLI arguments
    '''

    app = ['../source/sequential/build/app'] + arg

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
        else:
            print(result.stdout)

##############################################################
# CLI parser configuration
##############################################################

parser = argparse.ArgumentParser()
parser.add_argument('size', help='Number of rows and columns of the square matrix')

##############################################################
# main program logic
##############################################################

if __name__ == "__main__":

    args = parser.parse_args()

    arg = [value for key, value in vars(args).items()]

    print('\n**************************\nBuild\n**************************\n')
    run_make()

    print('\n**************************\nExecution\n**************************\n')
    run_exec(arg)

    print('\n')

