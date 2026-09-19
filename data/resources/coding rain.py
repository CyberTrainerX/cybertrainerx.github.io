import random
import time

str1 = "0123456789          "
def genLine(sample,count):
    line = ""
    for i in range(count):
        line += random.choice(sample)
    return line
while True:
    print(genLine(str1,150))
    time.sleep(0.075)
