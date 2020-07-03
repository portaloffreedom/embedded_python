from time import time, ctime

print('Today is', ctime(time()))


def hello():
    print('hello world')
    return 0


def multiply(a,b):
    print("Will compute", a, "times", b)
    c = 0
    for i in range(0, a):
        c = c + b
    return c
