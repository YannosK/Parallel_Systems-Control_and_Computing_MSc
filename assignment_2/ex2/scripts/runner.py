import argparse
import subprocess
import os

def run_make():

    result = subprocess.run(['make', '-C', '../', 'clean'], text=True)
    if result.returncode != 0:
        print(f"Error: make clean exited with return code {result.returncode}")

    result = subprocess.run(['make', '-C', '../'], text=True)
    if result.returncode != 0:
        print(f"Error: make exited with return code {result.returncode}")

def run_schedule_variable_export(schedule : str):

    os.environ['OMP_SCHEDULE'] = schedule

    envar = os.environ['OMP_SCHEDULE']
    if schedule != envar:
        print('OMP_SCHEDULE environmental variable not set')
        exit(2)
    else:
        print(f'Environmental variable OMP_SCHEDULE set to {envar}')

def run_exec(arg):

    app = ['../build/app'] + arg

    result = subprocess.run(app, text=True, capture_output=True)

    if result.stderr:
        print('Error message in the execution, probable due to wrong OMP_SCHEDULE value')
        print('Reseting to \"static\" and running again\n')
        run_schedule_variable_export('static')
        run_exec(arg)
    else:
        if result.returncode != 0:
            print(f"Error: Program exited with return code {result.returncode}")
            if result.returncode == -11:
                print(f"Segmentation Fault")
            elif result.returncode == 1:
                print(f"Wrong number of arguments")
            elif result.returncode == 2:
                print(f"Ran out of heap memory")
        else:
            print(result.stdout)

parser = argparse.ArgumentParser()
parser.add_argument('threadcount', help='The number of threads - relevant only for parallel execution, else just insert 1')
parser.add_argument('iterations', help='The size of the system')
parser.add_argument('method', help='The method for back-substitution must either be \'rows\' or \'columns\'')
parser.add_argument('execution', help='Wether the \'sequential\' or \'parallel\' execution is used')
parser.add_argument('-s', '--schedule', action='store', help='insert OpenMP scheduling by specifying the system variable value OMP_SCHEDULE', type=str)

if __name__ == "__main__":

    args = parser.parse_args()
    arg = [value for key, value in vars(args).items() if key not in ['schedule'] and value is not None]

    print('\n**************************\nBuild\n**************************\n')
    run_make()

    if args.schedule:
        print('\n**************************\nScheduling\n**************************\n')
        run_schedule_variable_export(args.schedule)

    print('\n**************************\nExecution\n**************************\n')
    run_exec(arg)

    print('\n')

