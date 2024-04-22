""" This module defines the paths that GPUVerify will use
    to run the various tools that GPUVerify Depends on.

    These paths must be absolute paths.

    Please note that this version of gvfindtools is for deployment ONLY.
    Only modify this is you know what you are doing.
"""
import sys
import os

#Paths, use init() to set them.
bugleSrcDir = None
bugleBinDir = None
libclcSrcDir = None
libclcInstallDir = None
llvmSrcDir = None
llvmBinDir = None
llvmLibDir = None
gpuVerifyBinDir = None
z3SrcDir = None
z3BinDir = None
cvc4SrcDir=None
cvc4BinDir=None
defaultSolver=None

# deploy.py will do a substitution for True
useMono = True if 'True' == 'True' else False

def init(pathPrefix):
  """Modify the modules variables by adding a path prefix"""

  global bugleSrcDir, bugleBinDir, libclcSrcDir, libclcInstallDir
  global llvmSrcDir, llvmBinDir, llvmLibDir
  global gpuVerifyBinDir
  global z3SrcDir, z3BinDir, cvc4SrcDir, cvc4BinDir
  global defaultSolver
  # The path to the Bugle Source directory.
  # The include-blang/ folder should be there
  bugleSrcDir = pathPrefix + os.sep + "bugle"

  # The Path to the directory where the "bugle" executable can be found.
  bugleBinDir = pathPrefix + os.sep + "bin"

  # The path to the libclc Source directory. Not used in the deployed setting
  libclcInstallDir = pathPrefix + os.sep + "libclc"

  # The path to the directory where libclc can be found.
  # The include/ and lib/clc/ folders should be there
  libclcInstallDir = pathPrefix + os.sep + "libclc"

  # The path to the llvm Source directory. Not used in the deployed setting
  llvmSrcDir = pathPrefix + os.sep + "llvm"

  # The path to the directory containing the llvm binaries.
  # llvm-nm, clang and opt should be there
  llvmBinDir = pathPrefix + os.sep + "bin"

  # The path containing the llvm libraries
  llvmLibDir = pathPrefix + os.sep + "lib"

  # The path to the directory containing the GPUVerify binaries.
  # GPUVerifyVCGen.exe, GPUVerifyCruncher.exe and GPUVerifyBoogieDriver.exe should be there
  gpuVerifyBinDir = pathPrefix + os.sep + "bin"

  # The path to the z3 Source directory. Not used in the deployed setting
  z3SrcDir = pathPrefix + os.sep + "z3"

  # The path to the directory containing z3.exe
  z3BinDir = pathPrefix + os.sep + "bin"

  # The path to the cvc4 Source directory. Not used in the deployed setting
  cvc4SrcDir = pathPrefix + os.sep + "cvc4"

  # The path to the directory containing cvc4.exe
  cvc4BinDir = pathPrefix + os.sep + "bin"

  # Default solver should be one of ['z3','cvc4']
  defaultSolver = 'z3'
