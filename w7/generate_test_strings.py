import numpy as np 


def generate():
    test_strings=[]
    testing_brackets=["{", "}", "[", "]", "(", ")"]
    for b in range(3):
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
                string = string+f"{testing_brackets[b*2]}"

            string=string+"test"

            for i in range(after):
                string = string+f"{testing_brackets[b*2+1]}"

            test_strings.append(string)
    
    return test_strings

gen=generate()
for i in range(len(gen)):
    print(gen[i])