import subprocess
import time

ip = '127.0.0.1'
port = 9095
pid = 1234

# Get base address of process
cmd = 'interface {} {} info process {}'.format(ip, port, pid)
base = subprocess.run(cmd, capture_output=True, text=True).stdout.strip('\n')

# Get bytes at that address
cmd = 'interface {} {} memory process {} read {} 16'.format(ip, port, pid, base)
bytes = subprocess.run(cmd, capture_output=True, text=True).stdout.strip('\n')

# Show bytes
print(bytes)

# Nop some bytes
cmd = 'interface {} {} memory process {} write {} 9090'.format(ip, port, pid, base)
subprocess.run(cmd, capture_output=True, text=True)

# Get bytes at that address again
cmd = 'interface {} {} memory process {} read {} 16'.format(ip, port, pid, base)
bytes = subprocess.run(cmd, capture_output=True, text=True).stdout.strip('\n')

# Show bytes
print(bytes)