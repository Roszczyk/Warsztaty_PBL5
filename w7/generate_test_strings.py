import numpy as np 

testing_brackets=["{", "}", "[", "]", "(", ")"]
good=[]
for j in range(10):
    before=np.random.randint(0,5)
    after=np.random.randint(0,5)

    if before == after:
        good.append("good")
    else:
        good.append("bad")

    string="pair_brackets "

    for i in range(before):
        string = string+"{"

    string=string+"test"

    for i in range(after):
        string = string+"}"

    print(string)

print (good)