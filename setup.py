# Copyright (c) 2023 Vector Informatik GmbH
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

from setuptools import setup, dist
import shutil
import os
import sys
import glob
import errno
from pathlib import Path
from distutils import util
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir
import pybind11_stubgen

moduleName = 'pybtf'

# workaround for https://bugs.python.org/issue38633
orig_copyxattr = shutil._copyxattr
def patched_copyxattr(src, dst, *, follow_symlinks=True):
	try:
		orig_copyxattr(src, dst, follow_symlinks=follow_symlinks)
	except OSError as ex:
		if ex.errno != errno.EACCES: raise
shutil._copyxattr = patched_copyxattr

# change search path of pypirc file to the root of this repo
from distutils.command.register import register as register_orig
from distutils.command.upload import upload as upload_orig
class register(register_orig):
    def _get_rc_file(self):
        return os.path.join(os.path.abspath(os.path.dirname(__file__)), 'pypirc')
class upload(upload_orig):
    def _get_rc_file(self):
        return os.path.join(os.path.abspath(os.path.dirname(__file__)), 'pypirc')


cmakeBuildDir = ''
libraryPostfix = ''
if os.name == 'nt':
    cmakeBuildDir = os.path.join(os.path.abspath(os.path.dirname(__file__)), 'build', 'src', 'pybindings', 'Debug')
    libraryPostfix = 'cp*-win_amd64.pyd'
else:
    cmakeBuildDir = os.path.join(os.path.abspath(os.path.dirname(__file__)), 'build', 'src', 'pybindings')
    libraryPostfix = 'cpython-*-x86_64-linux-gnu.so'

class BinaryDistribution(dist.Distribution):
    def has_ext_modules(foo):
        return True

def copyFile(inFile, outDir):
    print("CopyFile: " + inFile + " to " + outDir)
    couldCopyAtLeastOneFile=False
    for file in glob.glob(inFile):
        shutil.copyfile(file, os.path.join(outDir, os.path.basename(file)))
        couldCopyAtLeastOneFile = True
    return couldCopyAtLeastOneFile

def createModule():
    dest = os.path.join(os.path.abspath(os.getcwd()), moduleName)
    if os.path.abspath(os.path.dirname(__file__)) != os.path.abspath(os.getcwd()):
        src = os.path.join(os.path.abspath(os.path.dirname(__file__)), moduleName)
        print('Copy ' + moduleName + ' into ' + dest)
        if os.path.exists(dest):
            shutil.rmtree(dest)
        shutil.copytree(src=src, dst=dest)
    success = copyFile(os.path.join(cmakeBuildDir, 'pybtf.' + libraryPostfix), dest)
    if not success:
        print('Could not copy file: pybtf')
        exit(1)

def generateStubs():
    sys.path.insert(0, os.path.abspath(os.getcwd()))
    module = os.path.join(os.path.abspath(os.getcwd()), moduleName)
    pybind11_stubgen.main(args=[moduleName, '-o', module])
    # copy pybtf-stubs/pybtf to pybtfstubs
    stubpath = os.path.join(module, moduleName + '-stubs')
    stubpybtfpath = os.path.join(stubpath, moduleName)
    deststubpath = os.path.join(module, moduleName + 'stubs')
    shutil.copytree(src=stubpybtfpath, dst=deststubpath)
    shutil.rmtree(stubpath)
    # inject import of submodules
    pyipath = os.path.join(deststubpath, "__init__.pyi")
    with open(pyipath, "a") as pyifile:
        pyifile.write("from . import helper\n")
    # get all pyi files
    files = ['']
    for file in Path(module).rglob('*.pyi'):
        files.append(os.path.relpath(file, module))
    return files

createModule()
stub_files = generateStubs()

setup(
    name=moduleName,
    author="Julian Maidl",
    author_email="support@vector.com",
    description="BTF python tools",
    long_description="",
    zip_safe=False,
    packages=[moduleName, moduleName + '.pybtfstubs', moduleName + '.pybtfstubs.helper'],
    package_data={moduleName: ['pybtf.' + libraryPostfix] + stub_files},
    include_package_data=True,
    distclass=BinaryDistribution,
    setuptools_git_versioning={
       "template": "{tag}",
       "dev_template": "{tag}",
       "dirty_template": "{tag}",
    },
    setup_requires=['setuptools-git-versioning'],
    cmdclass={
        'register': register,
        'upload': upload,
    },
)
