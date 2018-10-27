import subprocess
import os
import shutil

# code from stack overflow

# returns a generator of dependency paths
def otool(s):
    o = subprocess.Popen(['/usr/bin/otool', '-L', s], stdout=subprocess.PIPE)
    first = True
    dylib = '.dylib' in s
    for bline in o.stdout:
        line = bline.decode("utf-8")
        if line[0] == '\t':
            if dylib and first:
                first = False
                continue
        yield line.split(' ', 1)[0][1:]
    # ignore the first line, that is the id

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
            # for line in stuff:
            #     if "QtCore" in line:
            #         print("qt", f, line)

            stuff = filter(os.path.exists, stuff)
            need.update(stuff)

        #print("otool result")
        done.update(needed)
        need.difference_update(done)

    # remove the app
    done.remove(app)
    return sorted(done)

def sizeof(path):
    try:
        info = os.stat(path)
        return info.st_size
    except:
        return 0

def scriptDir():
    return os.path.dirname(os.path.realpath(__file__))

# 
def copyFixDeps(exe, framework_dir, exe_copy=None):
    if exe_copy:
        print("copying exe, running chmod")
        shutil.copyfile(exe, exe_copy)
        exe = exe_copy
        os.chmod(exe, 0o755)

    deps = allDeps(exe)
    #deps = excludeSystem(deps)

    deps = exclude(deps, ["/usr/lib", "/System"])

    # ignore
    # /usr/lib/...

    print("all deps:", *deps, sep='\n')

    # map: old paths -> new paths
    path_map = {}
    new_set = set()

    # copy everything into the frameworks directory, add to map
    # there are some duplicates (QtCore)
    for old_path in deps:
        new_path = shutil.copy(old_path, framework_dir)
        path_map[old_path] = new_path
        new_set.add(new_path)
        os.chmod(new_path, 0o755)

    # go through everything copied, fix up them too
    print("copy fix deps path_map:")
    for x in path_map:
        print(x, " - ", path_map[x])
    
    new_set.add(exe)
    for obj in new_set:
        print("fixing up:", obj)
        deps = list(otool(obj))

        for dep in deps:
            if dep in path_map:
                # change the path
                # relative path to dst, from obj
                new_path = os.path.relpath(path_map[dep], os.path.dirname(obj))

                # use @loader_path for libs
                if 'VSim' in obj:
                    new_path = "@executable_path/" + new_path
                else:
                    new_path = "@loader_path/" + new_path
                    # simplify id?
                    installNameToolId(obj, os.path.basename(obj))

                # if 'QtCore' in obj:
                print("rel path comp:", new_path, dep)

                installNameTool(obj, dep, new_path)

    print("done w/ obj")
    print(*list(otool(obj)), sep='\n')

    return path_map

# returns a list where exclusions are removed
# uses basic string search
def exclude(stuff, exclusions):
    # any of stuff in x
    def anyIn(stuff, x):
        for i in stuff:
            if i in x:
                return True
        return False

    # filter: keep when no exclusions in x
    return list(filter(lambda x: not anyIn(exclusions, x), stuff))

def excludeSystem(libs):
    return exclude(libs, ["/System/Library", "/usr/lib/system"])

def installNameTool(exe, old, new):
    subprocess.Popen(['/usr/bin/install_name_tool', '-change', old, new, exe])

def installNameToolId(dylib, id):
    subprocess.Popen(['/usr/bin/install_name_tool', '-id', id, dylib])

# 1. get all dylib dependencies
# 2. get a rewrite path, (@x -> x), and rewrite mappings


# sizes
#print(allDeps("../build/VSim"))

# install_name_tool -change (old) (new) (exe/dylib)
# install_name_tool -change 
#   /the/old/path/to/the_library_name.dylib 
#   "@executable_path/../Frameworks/the_library_name.dylib"
#   ./MyProgram.app/Contents/MacOS/MyProgram

