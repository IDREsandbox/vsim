import subprocess
import os
import shutil
import json

def otool(s):
    """returns a generator of dependency paths
    credits: stack overflow
    """
    o = subprocess.Popen(['/usr/bin/otool', '-L', s], stdout=subprocess.PIPE)
    # for some dylibs the first line is the id, for osg .so is isn't...
    # so what do we do
    # if the name contains itsef then it's probably an id and skip it
    name = os.path.basename(s)

    dylib = ('.dylib' in s) or ('.so' in s)
    for bline in o.stdout:
        line = bline.decode("utf-8")
        if line[0] != '\t':
            continue
        dep = line.split(' ', 1)[0][1:]
        # ignore ids, self-references
        if dep.endswith(name):
            continue
        yield dep

def excludeSystem(libs):
    return exclude(libs, ["/System/Library", "/usr/lib/system"])

def installNameTool(exe, old, new):
    subprocess.Popen(['/usr/bin/install_name_tool', '-change', old, new, exe])

def installNameToolId(dylib, id):
    subprocess.Popen(['/usr/bin/install_name_tool', '-id', id, dylib])

def rmcopytree(src, dst):
    """be careful you don't mix up the args, might accidentally delete src"""
    shutil.rmtree(dst, ignore_errors=True)
    shutil.copytree(src, dst)
    return dst

def listdirabs(dir):
    """same as listdir but w/ absolute paths"""
    ls = os.listdir(dir)
    lsa = list(map(lambda name: os.path.join(dir, name), ls))
    return lsa

def sizeof(path):
    try:
        info = os.stat(path)
        return info.st_size
    except:
        return 0

def scriptDir():
    return os.path.dirname(os.path.realpath(__file__))


def isdylib(path):
    return not os.path.isdir(path) and (path.endswith('.dylib') or path.endswith('.so'))

def osgInfo():
    """returns a dictionary
        plugins_version '3.6.2'
        plugins_name 'osgPlugins-3.6.2'
        plugins_path '/usr/local/opt/open-scene-graph/lib/osgPlugins-3.6.2'
    """
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

def osgPluginsDirname():
    return osgInfo()['plugins_name']

def qtInfo():
    """returns a dictionary
        plugins_path '/usr/local/opt/qt/plugins'
    """
    proc = subprocess.Popen(['brew', '--prefix', 'qt'], stdout=subprocess.PIPE)
    prefix, stderr = proc.communicate()
    prefix = prefix.decode('utf-8').rstrip()

    plugins_path = prefix + '/plugins'

    return {
        'plugins_path' : plugins_path,
    }

def exclude(stuff, exclusions):
    """returns a list where exclusions are removed
    uses basic string search
    """
    # any of stuff in x
    def anyIn(stuff, x):
        for i in stuff:
            if i in x:
                return True
        return False

    # filter: keep when no exclusions in x
    return list(filter(lambda x: not anyIn(exclusions, x), stuff))

def allDeps(single=None, multi=None):
    """recursively finds dependencies of a single or multiple exes/dylibs"""
    need = set()
    if single:
        need.add(single)
        print("single?")
    if multi is not None:
        need.update(multi)
    print("\n\n\nall deps need", need, "\n", multi)
    done = set()

    while need:
        needed = set(need)
        need = set()

        for f in needed:
            # print("otool result", otool(f))
            # for x in otool(f):
            #     print("-" + x)
            stuff = list(otool(f))

            #for dep in stuff:
            #    if 'Cellar' in dep:
            #        print("cellar dep from f", f)

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
    if single:
        done.remove(single)
    if multi is not None:
        done.difference_update(multi)
    return sorted(done)

def copyDepsInto(deps, framework_dir):
    """returns a tuple
        name_map : maps base to full path 'QtCore' -> '/.../Frameworks/QtCore
        dep_list : list of all new deps
    filters out system deps
    """
    print("CALLING COPY DEPS INTO", deps, framework_dir)
    deps = exclude(deps, ["/usr/lib", "/System"])

    path_map = {}
    dep_map = {}
    new_set = set()
    for old_path in deps:
        new_path = shutil.copy(old_path, framework_dir)
        print("copying dep", old_path, new_path)
        path_map[old_path] = new_path
        dep_map[os.path.basename(new_path)] = new_path
        new_set.add(new_path)
        os.chmod(new_path, 0o755)

    # go through everything copied, fix up them too
    print("copy fix deps path_map:")
    for x in path_map:
        print(x, " - ", path_map[x])

    return dep_map, list(new_set)

def copyOSGPlugins(dest_dir):
    """
    copies osg plugins into a directory osgPlugins-3.5.6 or the like inside of dest_dir
    returns a set of absolute paths to plugins
    """
    osg_info = osgInfo()
    osg_name_list_absolute = listdirabs(osg_info['plugins_path'])

    osg_dir = os.path.join(dest_dir, osg_info['plugins_name'])
    os.makedirs(osg_dir, exist_ok=True)

    out = set()
    for plugin in osg_name_list_absolute:
        result = shutil.copy(plugin, osg_dir)
        os.chmod(result, 0o755)
        out.add(os.path.abspath(result))

    return out


def copyQtPlugins(dest_dir):
    """
    copies qt plugins (platforms, imageformats) into dest_dir
    returns a set of absolute paths of all plugins
    """
    qt_info = qtInfo()
    qt_plugin_path = qt_info['plugins_path']

    to_copy = set(['platforms', 'imageformats', 'styles'])
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
        pass
        #shutil
    #shutil.rmtree(osgdir, ignore_errors=True)
    #shutil.copytree(osg_dlls, content_dir + '/osgPlugins')

    print("plugins list dir", os.listdir(osg_plugins_path))
    print(osg_name_list_absolute)

def fixDeps(obj, dep_map, fix_id=True, exe=False):
    """
    obj: an executable/dylib to fix
    dep_map: maps dep names (QtCore) to an absolute path (Content/Frameworks/QtCore),
      set of replacement dylibs
    fix_id: edit the dylib id to point to itself (this is the first line of otool)
    exe: use @executable_path

    TODO: what if mutiple libs have same name? osg/foo.dylib, qt/foo.dylib ¯\_(ツ)_/¯
    """
    print("fixing up:", obj)
    deps = list(otool(obj))
    obj_dir = os.path.dirname(obj)
    obj_name = os.path.basename(obj)

    if fix_id:
        # simplify id, don't need this but helps otool make more sense
        installNameToolId(obj, obj_name)

    for old_dep_path in deps:
        # QtCore
        dep_name = os.path.basename(old_dep_path)
        if dep_name not in dep_map:
            continue
        # /Frameworks/QtCore
        dep_path = dep_map[dep_name]

        # change the path
        # relative path to dst, from obj
        new_path = os.path.relpath(dep_path, obj_dir)

        prefix = "@executable_path" if exe else "@loader_path"
        new_path = os.path.join(prefix, new_path)

        #print("rel path comp:", new_path, old_dep_path)
        installNameTool(obj, old_dep_path, new_path)

def copyAppExe(app, exe):
    """replaces exe in bundle, returns full path"""
    dir = os.path.join(app, "Contents", "MacOS")
    path = shutil.copy(exe, dir)
    return path

def remove2(path):
    """non-throwing remove"""
    try:
        os.remove(path)
    except:
        pass

def prepAppDir(dst, exe):
    plist_path = os.path.join(dst, "Contents/Info.plist")

    print("exe:", exe)
    #print("bundle template:", bundle_template)

    # basic tree
    os.makedirs(dst, exist_ok=True)

    #rmcopytree(bundle_template, dst)

    for subdir in ["Frameworks", "Plugins", "Resources", "MacOS"]:
        os.makedirs(os.path.join(dst, "Contents", subdir), exist_ok=True)

    root = os.path.join(scriptDir(), "../")

    resources = os.path.join(dst, "Contents", "Resources")
    # copy assets
    assets_dst = os.path.join(resources, "assets")
    if not os.path.exists(assets_dst):
        shutil.copytree(os.path.join(root, "assets"), assets_dst)

    # copy icon
    shutil.copy(os.path.join(root, "assets", "vsim.icns"), assets_dst)

    # copy conf
    shutil.copy(os.path.join(root, "osx", "qt.conf"), resources)

    # copy VSim in there to replace the dummy
    # fix Info.plist
    copyAppExe(dst, exe)

    # remove VSim_bundle if it exists
    # remove2(os.path.join(dst, "Contents", "MacOS", "VSim_bundle"))

    # replace Info.plist
    shutil.copyfile(os.path.join(
        scriptDir(), "Info_template.plist"),
        os.path.join(dst, "Contents", "Info.plist"))

    # rewrite Info.plist
    # with open(plist_path, 'r') as plist :
    #     plist_data = plist.read()

    # plist_data = plist_data.replace('VSim_bundle', 'VSim')
    # """
	# <key>NSPrincipalClass</key>
	# <string>NSApplication</string>
	# <key>NSHighResolutionCapable</key>
	# <string>True</string>
    # """

    # with open(plist_path, 'w') as plist:
    #     plist.write(plist_data)

def scanDeps(app):
    """returns a map (basename -> full path) of dylibs in .app/.../Frameworks"""
    dep_map = {}
    frameworks = os.path.join(app, "Contents", "Frameworks")
    dylibs = os.listdir(frameworks)
    for base in dylibs:
        dep_map[base] = os.path.join(frameworks, base)
    return dep_map

def replaceExe(app, exe):
    vsim = copyAppExe(app, exe)
    dep_map = scanDeps(app)
    fixDeps(vsim, dep_map, fix_id=False, exe=True)

def doEverything(app, exe):
    prepAppDir(app, exe)
    vsim = os.path.join(app, "Contents", "MacOS", "VSim")
    plugins_dir = os.path.join(app, "Contents", "Plugins")
    frameworks_dir = os.path.join(app, "Contents", "Frameworks")

    qt_plugins = copyQtPlugins(plugins_dir)
    osg_plugins = copyOSGPlugins(plugins_dir)
    all_plugins = list(qt_plugins) + list(osg_plugins)

    old_deps = allDeps(multi=(all_plugins + [vsim]))
    print("all old deps", old_deps)
    dep_map, dep_list = copyDepsInto(old_deps, frameworks_dir)

    # simplify deps map

    # fix deps, plugins
    for obj in dep_list + all_plugins:
        fixDeps(obj, dep_map, fix_id=True, exe=False)
    # fix VSim
    fixDeps(vsim, dep_map, fix_id=False, exe=True)
