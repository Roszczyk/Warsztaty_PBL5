import sys
import os
from testrunner import run 

def testfunc(child):
    child.sendline("")
    child.expect_exact(">")

    print("Benchmark was successful")

if __name__ == "__main__":
    sys.exit(run(testfunc))
