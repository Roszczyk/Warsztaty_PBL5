#!/usr/bin/env python3

import sys
import os
from testrunner import run

def testfunc(child):
    child.sendline("pair_brackets \(a+b\)")
    child.expect_exact("GOOD")
    child.sendline("pair_brackets \((a+b\)")
    child.expect_exact("BAD")
    child.sendline("pair_brackets \(a+b\))")
    child.expect_exact("BAD")
    child.sendline("pair_brackets \{a+b\}")
    child.expect_exact("GOOD")
    child.sendline("pair_brackets \{(a+b\)}")
    child.expect_exact("GOOD")
    child.sendline("pair_brackets \{a+b\)")
    child.expect_exact("BAD")
    child.sendline("pair_brackets \[a+b")
    child.expect_exact("BAD")
    child.sendline("pair_brackets \({{{a+b\})")
    child.expect_exact("BAD")
    child.sendline("pair_brackets a+b")
    child.expect_exact("GOOD")
    child.sendline("pair_brackets {{a+b}")
    child.expect_exact("BAD")

    print("Benchmark was successful")

if __name__ == "__main__":
    sys.exit(run(testfunc))