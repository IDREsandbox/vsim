import sys
import os
import argparse
import json
import subprocess

import deps

parser = argparse.ArgumentParser(description="Build VSim.app for osx")
parser.add_argument('--otool', help="runs otool")
parser.add_argument('--otoolr', help="runs recursive otool")
parser.add_argument('--exclusion', help="excludes system libs", action='store_true')
parser.add_argument('--osg', help="figure out osg plugins")
parser.add_argument('--osg2', help="test osg plugins")
parser.add_argument('--plugins', help="figure out plugins")
parser.add_argument('--qt', help="figure out qt plugins?")
subparsers = parser.add_subparsers(dest='which', help="subcommands")

prepparser = subparsers.add_parser('prep', help="builds the .app tree structure")
prepparser.add_argument('-x', help="exe path", required=True)
prepparser.add_argument('-o', help="output app dir", required=True)

buildparser = subparsers.add_parser('build', help="builds the .app")
buildparser.add_argument('-x', help="exe path", required=True)
buildparser.add_argument('-o', help="output app dir", required=True)

copyparser = subparsers.add_parser('update', help="copies over the executable into the .app, for faster testing")
copyparser.add_argument('-x', help="exe path", required=True)
copyparser.add_argument('-o', help="output app dir", required=True)

args = parser.parse_args()

# examples:
# testing
# python3 test_deps.py --otoolr VSim
# building
# python3 test_deps.py fix -x ../build2/VSim -o VSim -f Frameworks

# how to test breaking stuff

# /usr/lib (not system) => 
# /usr/local/opt => /usr/local/optfoo
# /usr/local/Cellar

def printSummary(d):
    nbytes = sum(map(deps.sizeof, d))
    print("total size:", nbytes, nbytes / 1000000, "mb")
    print("count", len(d))

exe_path = deps.scriptDir() + "/../build2/VSim"

if (args.otool):
    obj = args.otool
    result = list(deps.otool(obj))
    print(*result, sep='\n')
    printSummary(result)

if (args.otoolr):
    obj = args.otoolr
    result = list(deps.allDeps(obj))

    if args.exclusion:
        result = deps.exclude(result, ["/System/Library", "/usr/lib/system"])

    print(*result, sep='\n')
    printSummary(result)

if (args.which == 'prep'):
    exe = args.x
    out = args.o
    deps.prepAppDir(out, exe)

if (args.which == 'build'):
    exe = args.x
    out = args.o
    deps.doEverything(out, exe)

if (args.which == 'update'):
    exe = args.x
    app = args.o
    deps.replaceExe(app, exe)

if (args.plugins):
    dir = args.plugins
    deps.copyFixPlugins(dir)
    # copy /usr/local/Cellar/qt/5.11.2/plugins to Plugins
    #   /imageformats
    #   /platforms
    # copy /usr/local/Cellar/open-scene-graph/3.6.2/lib/osgPlugins-3.6.2 to Plugins
    #  uhhh they're .so, is that OK?

    # we're going to do all this copying, then we have to fix up them too

    # brew --prefix qt
    # brew --prefix open-scene-graph

    # osg: we're going to have to parse `brew info open-scene-graph` for the plugins version?
    # brew info --json=v1 open-scene-graph
    #   then stuff[0][versions][stable] -> '3.6.2'
    #   then osgPlugins-3.6.2

if (args.osg):
    dir = args.osg
    print(deps.osgInfo())
    libs = deps.copyOSGPlugins(dir)
    print("osg plugins:", *libs, sep='\n')

if (args.qt):
    dir = args.qt
    print(deps.qtInfo())
    libs = deps.copyQtPlugins(dir)
    print("qt plugins results:", *libs, sep='\n')

if (args.osg2):
    app = args.osg2
    vsim = os.path.join(app, "Contents", "MacOS", "VSim")

    plugins_dir = os.path.join(app, "Contents", "Plugins", deps.osgInfo()['plugins_name'])
    osg_plugins = deps.listdirabs(plugins_dir)

    all_deps = deps.allDeps(multi=(osg_plugins + [vsim]))

    print(*all_deps, sep='\n')