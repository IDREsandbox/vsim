import subprocess
import os
import shutil
import json

# code from stack overflow

# returns a generator of dependency paths
def otool(s):
    o = subprocess.Popen(['/usr/bin/otool', '-L', s], stdout=subprocess.PIPE)
    first = True
    dylib = ('.dylib' in s) or ('.so' in s)
    for bline in o.stdout:
        line = bline.decode("utf-8")
        if line[0] != '\t':
            continue
        # ignore the first line, that is the id
        if dylib and first:
            first = False
            continue
        yield line.split(' ', 1)[0][1:]

# returns a dictionary
#   plugins_version '3.6.2'
#   plugins_name 'osgPlugins-3.6.2'
#   plugins_path '/usr/local/opt/open-scene-graph/lib/osgPlugins-3.6.2'
def osgInfo():
    proc = subprocess.Popen(['brew', 'info', '--json=v1', 'open-scene-graph'], stdout=subprocess.PIPE)
    stdout, stderr = proc.communicate()
    result = json.loads(stdout)
    version = result[0]['versions']['stable']

    proc = subprocess.Popen(['brew', '--prefix', 'open-scene-graph'], stdout=subprocess.PIPE)
    prefix, stderr = proc.communicate()
    prefix = prefix.decode('utf-8').rstrip()

    plugins_name = 'osgPlugins-' + version
    plugins_path = prefix + '/lib/' + plugins_name

    return {
        'version' : version,
        'plugins_name' : plugins_name,
        'plugins_path' : plugins_path
    }

# returns a dictionary
#   plugins_path '/usr/local/opt/qt/plugins'
#   image_formats
#   platforms
def qtInfo():
    proc = subprocess.Popen(['brew', '--prefix', 'qt'], stdout=subprocess.PIPE)
    prefix, stderr = proc.communicate()
    prefix = prefix.decode('utf-8').rstrip()

    plugins_path = prefix + '/plugins'
    image_formats = plugins_path + '/imageformats'
    platforms = plugins_path + '/platforms'

    return {
        'plugins_path' : plugins_path,
        'image_formats' : image_formats,
        'platforms' : platforms
    }

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

# returns dep_map: basename -> full path
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

    # path_map: old paths -> new paths
    path_map = {}
    # dep_map: basename -> full path (QtCore -> /Frameworks/QtCore)
    dep_map = {}
    new_set = set()

    # copy everything into the frameworks directory, add to map
    # there are some duplicates (QtCore)
    for old_path in deps:
        new_path = shutil.copy(old_path, framework_dir)
        path_map[old_path] = new_path
        dep_map[os.path.basename(new_path)] = new_path
        new_set.add(new_path)
        os.chmod(new_path, 0o755)

    # go through everything copied, fix up them too
    print("copy fix deps path_map:")
    for x in path_map:
        print(x, " - ", path_map[x])
    
    new_set.add(exe)
    for obj in new_set:
        fixdeps(obj, dep_map)

    print("done w/ obj")
    print(*list(otool(obj)), sep='\n')

    return dep_map

# be careful you don't mix this up
def rmcopytree(src, dst):
    shutil.rmtree(dst, ignore_errors=True)
    shutil.copytree(src, dst)
    return dst

# same as listdir but w/ absolute paths
def listdirabs(dir):
    ls = os.listdir(dir)
    lsa = list(map(lambda name: os.path.join(dir, name), ls))
    return lsa

def osgPluginsDirname():
    return osgInfo()['plugins_name']

# returns a set of absolute dir plugins set(/stuff/osgdb_dot.so, /stuff/osgdb_osg.so)
def copyOSGPlugins(dest_dir):
    osg_info = osgInfo()
    osg_name_list_absolute = listdirabs(osg_info['plugins_path'])

    out = set()
    for plugin in osg_name_list_absolute:
        result = shutil.copy(plugin, dest_dir)
        os.chmod(result, 0o755)
        out.add(os.path.abspath(result))

    return out

def isdylib(path):
    return not os.path.isdir(path) and (path.endswith('.dylib') or path.endswith('.so'))

def copyQtPlugins(dest_dir):
    qt_info = qtInfo()
    qt_plugin_path = qt_info['plugins_path']

    to_copy = set(['platforms', 'imageformats'])
    out_subdirs = []
    for subdir in to_copy:
        src = os.path.join(qt_plugin_path, subdir)
        dst = os.path.join(dest_dir, subdir)
        path = rmcopytree(src, dst)
        out_subdirs.append(path)

    # recursively find all the dylibs in copied plugins subdirs
    libs = set()
    def traverse(dir):
        print("traversing:", dir)
        subs = listdirabs(dir)
        print("subs test", subs)
        for x in subs:
            if isdylib(x):
                libs.add(x)
            elif os.path.isdir(x):
                traverse(x)
    for dir in out_subdirs:
        traverse(dir)

    return libs

def copyPlugins(content_dir):
    osg_info = osgInfo()
    qt_info = qtInfo()

    # VSim.app
    #  /Content/Frameworks
    #  /Content/Plugins
    # so paths have to be @loader_path/../Frameworks/QtCore, something like that

    # /Content/qtPlugins/...
    # /Content/osgPlugins-3.5.6/...

    # copy everything over first,
    # then fix up otool stuff
    # turn these into absolute paths?
    osg_plugins_path = osg_info['plugins_path']
    osg_name_list = os.listdir(osg_plugins_path)
    osg_name_list_absolute = list(map(lambda x: os.path.join(osg_plugins_path, x), osg_name_list))

    for plugin in osg_name_list_absolute:
        shutil
    #shutil.rmtree(osgdir, ignore_errors=True)
    #shutil.copytree(osg_dlls, content_dir + '/osgPlugins')

    print("plugins list dir", os.listdir(osg_plugins_path))
    print(osg_name_list_absolute)

# obj: an executable/dylib to fix
# dep_map: maps dep names (QtCore) to an absolute path (Content/Frameworks/QtCore),
#   set of replacement dylibs
# what if mutiple libs have same name? osg/foo.dylib, qt/foo.dylib ¯\_(ツ)_/¯
def fixdeps(obj, dep_map):
    print("fixing up:", obj)
    deps = list(otool(obj))
    obj_dir = os.path.dirname(obj)
    obj_name = os.path.basename(obj)

    for old_dep_path in deps:
        # QtCore
        dep_name = os.path.basename(dep_path)
        if dep_name not in dep_set:
            continue
        # /Frameworks/QtCore
        dep_path = dep_map[dep_name]

        # change the path
        # relative path to dst, from obj
        new_path = os.path.relpath(dep_path, obj_dir)

        # use @loader_path for libs
        if 'VSim' in obj:
            new_path = os.path.join("@executable_path/", new_path)
        else:
            new_path = os.path.join("@loader_path/", new_path)
            # simplify id, don't need this but helps otool make more sense
            installNameToolId(obj, obj_name)

        print("rel path comp:", new_path, old_dep_path)
        installNameTool(obj, old_dep_path, new_path)

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

