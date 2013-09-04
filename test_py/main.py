from ctypes import *
lgh = cdll.LoadLibrary("./libsock.so")

def mycallback(result):
    print result
    return 0

MYFUNC = CFUNCTYPE(c_int, c_int)
myfunc = MYFUNC(mycallback)


lgh.init(20, myfunc)

