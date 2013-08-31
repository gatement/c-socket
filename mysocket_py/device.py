#encoding=utf8

from ctypes import *

def make_c_fun(*args):
    return CFUNCTYPE(*args[1:])(args[0])

def onMsgReceived(msg):
    print "App -> Device: %s" % msg
    return "ok"

def getOutputMsg():
    return "Device -> App: Bonjour..."

def log(s):
    print "Log: %s" % s
    return 0

def main():
    mysocket = cdll.LoadLibrary("./libmysocket.so")

    onMsgReceived_c = make_c_fun(onMsgReceived, c_char_p, c_char_p)
    getOutputMsg_c = make_c_fun(getOutputMsg, c_char_p)
    log_c = make_c_fun(log, c_int, c_char_p)

    port  = 5678
    mysocket.startListening(port, onMsgReceived_c, getOutputMsg_c, log_c)


main()

