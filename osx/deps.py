import subprocess
import os

# code from stack overflow

# returns a generator of dependency paths
def otool(s):
    o = subprocess.Popen(['/usr/bin/otool', '-L', s], stdout=subprocess.PIPE)
    for bline in o.stdout:
        line = bline.decode("utf-8")
        if line[0] == '\t':
            yield line.split(' ', 1)[0][1:]

def allDeps(app):
    need = set([app])
    done = set()

    while need:
        needed = set(need)
        need = set()

        for f in needed:
            # print("otool result", otool(f))
            # for x in otool(f):
            #     print("-" + x)
            stuff = list(otool(f))

            # filter out everything that doesn't exist?
            # @loader_path
            # for line in stuff:
            #     if (not os.path.exists(line)):
            #         print("this guy is broken?", f, line)

            # multiple QtCore dependencies...
            # which belongs to who?
            for line in stuff:
                if "QtCore" in line:
                    print("qt", f, line)

            stuff = filter(os.path.exists, stuff)
            need.update(stuff)

        #print("otool result")
        done.update(needed)
        need.difference_update(done)

    return sorted(done)

def sizeof(path):
    try:
        info = os.stat(path)
        return info.st_size
    except:
        return 0

def scriptDir():
    return os.path.dirname(os.path.realpath(__file__))

deps = allDeps(scriptDir() + "/../build/VSim")


for l in deps:
    print(l)

def printSummary(d):
    nbytes = sum(map(sizeof, d))
    print("total size:", nbytes, nbytes / 1000000, "mb")
    print("count", len(d))

printSummary(deps)

# filter out system/library, count them

system_deps = list(filter(lambda x: "/System/Library/" in x, deps))
printSummary(system_deps)


# sizes
#print(allDeps("../build/VSim"))