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
parser.add_argument('--plugins', help="figure out plugins")
parser.add_argument('--qt', help="figure out qt plugins?")
subparsers = parser.add_subparsers(dest='which', help="subcommands")

fixparser = subparsers.add_parser('fix', help="moves non-system exe dependencies to directory -o, fixes dylib paths")
fixparser.add_argument('-x', help="exe path", required=True)
fixparser.add_argument('-o', help="makes a copy of exe to -o")
fixparser.add_argument('-f', help="framework output path", required=True)

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

if (args.which == 'fix'):
    exe = args.x
    out = args.o
    dir = args.f
    deps.copyFixDeps(exe, dir, out)

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