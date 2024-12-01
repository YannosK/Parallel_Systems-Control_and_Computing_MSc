import subprocess
import configparser
import os

def getSystemName():
    result = subprocess.run(['uname', '-a'], capture_output=True, text=True)
    return result.stdout.strip()

def getCpuName():
    result = subprocess.run(
        ['bash', '-c', "lscpu | grep 'Model name' | sed 's/Model name:\\s*//'"],
        capture_output=True, text=True
    )
    return result.stdout.strip()

def getNumCores():
    return os.cpu_count()

def getOsVersion():
    with open('/etc/os-release') as f:
        for line in f:
            if line.startswith('PRETTY_NAME'):
                return line.split('=')[1].strip().strip('"')
    return None

def getGccVersion():
    result = subprocess.run(['gcc', '--version'], capture_output=True, text=True)
    if result.returncode == 0:
        return result.stdout.splitlines()[0]  # First line contains the version info
    return None

def getCacheLineSize():
    result = subprocess.run(['getconf', 'LEVEL1_DCACHE_LINESIZE'], capture_output=True, text=True)
    return result.stdout

if __name__ == "__main__":
    config = configparser.ConfigParser()

    config['SystemInfo'] = {
        'SystemName': getSystemName(),
        'CPUName': getCpuName(),
        'NumberOfCores': getNumCores(),
        'OSVersion': getOsVersion(),
        'gccversion' : getGccVersion(),
        'cachelinelength' : getCacheLineSize()
    }

    with open('system.ini', 'w') as configfile:
        config.write(configfile)
