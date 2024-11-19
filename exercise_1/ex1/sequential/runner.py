import sys
import subprocess

def run_make():
    result  = subprocess.run(['make', 'clean'], capture_output=True, text=True)
    print(result.stdout)
    result  = subprocess.run(['make'], capture_output=True, text=True)
    print(result.stdout)

def run_exec(arg):
    result  = subprocess.run(['./calc_pi_seq', arg], capture_output=True, text=True)
    print(result.stdout)

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print('Usage: python3 runner.py <shotsnum>')
        sys.exit(1)

    shotsnum = sys.argv[1]

    print('\n******************\nBuild\n******************\n')
    run_make()

    print('\n******************\nRun\n******************\n')
    run_exec(shotsnum)
