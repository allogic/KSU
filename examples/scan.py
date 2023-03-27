import subprocess
import time

ip = '127.0.0.1'
port = 9095
pid = 1234
bytes = 'FF00FF00FF00'

# Reset the scanner first
cmd = 'interface {} {} scan reset'.format(ip, port)
subprocess.run(cmd, capture_output=True, text=True)

# First scan array of bytes (Must be executed only once or right after restting the scanner)
cmd = 'interface {} {} scan aob {} {}'.format(ip, port, pid, bytes)
scans = subprocess.run(cmd, capture_output=True, text=True).stdout.strip('\n').splitlines()

while len(scans) > 10:
  # Next scan for unchanged values till we only have 10 entries left
  cmd = 'interface {} {} scan unchanged'.format(ip, port)
  scans = subprocess.run(cmd, capture_output=True, text=True).stdout.strip('\n').splitlines()

  # Print results
  print('Found {} addresses that match {}'.format(len(scans), bytes))

  # Wait a short period of time so that values in memory can actually change
  time.sleep(5)