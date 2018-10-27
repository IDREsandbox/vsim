#sooooo I have no idea how this works

# maybe we should start with a shell script?

# arguments
# -h: help
# -c: cmake build dir
# -x: VSim executable (optional? can we query this through cmake somehow?)
# -b: VSim_bundle.app
# -s: VSim source dir? (optionl? it's just ../)

# typical w/ xcode generator, for testing
# build_osx.py -c ../xbuild -x ../xbuild/Release/VSim -b ../xbuild/Release/VSim_bundle.app

import sys
import os
import subprocess
import shutil
import errno
import argparse

parser = argparse.ArgumentParser(description="Build VSim.app for osx")
parser.add_argument("-c", help="cmake build dir", required=True)
parser.add_argument("-x", help="VSim executable", required=True)
parser.add_argument("-b", help="VSim_bundle.app to copy", required=True)
args = parser.parse_args()

print("hello world! (build_osx.py)", sys.argv)

build_dir = args.c
exe = args.x
bundle_template = args.b
bundle = os.path.join(build_dir, "VSim.app")
plist_path = os.path.join(bundle, "Contents/Info.plist")

print("build dir:", build_dir)
print("exe:", exe)
print("bundle template:", bundle_template)

# copy bundle template to make VSim.app
# copy if doesn't exist
if os.path.exists(bundle):
    shutil.rmtree(bundle)
shutil.copytree(bundle_template, bundle)

# now we have to fix
# copy VSim in there
# fix Info.plist
shutil.copy(exe, os.path.join(bundle, "Contents/MacOS"))

# rewrite Info.plist
with open(plist_path, 'r') as plist :
  plist_data = plist.read()
plist_data = plist_data.replace('VSim_bundle', 'VSim')
with open(plist_path, 'w') as plist:
  plist.write(plist_data)

# fix frameworks, copy frameworks into Contents/Frameworks

# copy osg plugins, fix them up too

# 