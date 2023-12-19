#!/usr/bin/env python3

import numpy as np 
import sys
import os
# from testrunner import run

OUTSIDE=0
BEFORE=1
AFTER=2

def chooseBrackets():
    testing_brackets=["{", "}", "[", "]", "(", ")"]
    choose_brackets=np.random.randint(0,2)

    return [testing_brackets[choose_brackets*2], testing_brackets[choose_brackets*2+1]]



def genTest():
    testing_positions=[OUTSIDE, BEFORE, AFTER]
    choose_positions=np.random.randint(0,2)

    testing_brackets=chooseBrackets()

    if choose_positions==OUTSIDE:
        string=f"{testing_brackets[0]}StringTest{testing_brackets[1]}"
    if choose_positions==BEFORE:
        string=f"{testing_brackets[0]}{testing_brackets[1]}StringTest"
    if choose_positions==AFTER:
        string=f"StringTest{testing_brackets[0]}{testing_brackets[1]}"

    return string

def genIll():
    correct=genTest()
    option=np.random.randint(0,4)
    usedbr=chooseBrackets()

    if option==0:
        string = correct + usedbr[1]
    if option==1:
        string=correct.lstrip(correct[0])
    if option==2:
        string=correct.rstrip(correct[len(correct)-1])
    if option==3:
        string = correct + usedbr[0]
    if option==4:
        string = usedbr[0] + correct
    
    return string

def testfunc(child):
    for i in range(100):
        child.sendline(f"pair_brackets {genTest()}")
        child.expect_exact("GOOD")
    for i in range(100):
        child.sendline(f"pair_brackets {genIll()}")
        child.expect_exact("BAD")


def testtest():
    for i in range(100):
        print(f"pair_brackets {genTest()}")
    for i in range(100):
        print(f"pair_brackets {genIll()}")


# if __name__ == "__main__":
#     sys.exit(run(testfunc))
        
testtest()