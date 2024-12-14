import argparse
import subprocess

def run_make():

    result = subprocess.run(['make', 'clean'], text=True)
    if result.returncode != 0:
        print(f"Error: make clean exited with return code {result.returncode}")

    result = subprocess.run(['make'], text=True)
    if result.returncode != 0:
        print(f"Error: make exited with return code {result.returncode}")

def run_schedule_variable_export(schedule : str):
    command = ['export'] + ['OMP_SCHEDULE=\"' + schedule +'\"']
    result = subprocess.run(command, text=True)
    print(command)

def run_exec(arg):

    app = ['./build/app'] + arg

    result = subprocess.run(app, text=True)

    if result.returncode != 0:
        print(f"Error: Program exited with return code {result.returncode}")
        if result.returncode == -11:
            print(f"Segmentation Fault")
        elif result.returncode == 1:
            print(f"Wrong number of arguments")
        elif result.returncode == 2:
            print(f"Ran out of heap memory")

parser = argparse.ArgumentParser()
parser.add_argument('threadcount')
parser.add_argument('iterations')
parser.add_argument('-s', '--schedule', action='store', help='insert OpenMP scheduling', type=str)

if __name__ == "__main__":

    args = parser.parse_args()
    arg = [value for key, value in vars(args).items() if key not in ['schedule'] and value is not None]
    print(arg)

    print('\n**************************\nBuild\n**************************\n')
    run_make()

    if args.schedule:
        print('\n**************************\nScheduling\n**************************\n')
        run_schedule_variable_export(args.schedule)

    print('\n**************************\nExecution\n**************************\n')
    run_exec(arg)

    print('\n')

