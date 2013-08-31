from ctypes import *
lgh = cdll.LoadLibrary("./libsock.so")
lgh.myprint()

def mycallback(result):
    print result
    return 0

MYFUNC = CFUNCTYPE(c_int, c_int)
myfunc = MYFUNC(mycallback)

lgh.myloop(20, myfunc)

