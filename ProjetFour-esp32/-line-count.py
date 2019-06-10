#! /usr/bin/env python
# -*- coding: UTF-8 -*-

#----------------------------------------------------------------------------------------------------------------------*

import sys, os, fnmatch

#----------------------------------------------------------------------------------------------------------------------*

def compterLignesFichier (nomFichier) :
  fd = open (nomFichier, 'r')
  n = 0
  while fd.readline ():
    n += 1
  fd.close ()
  return n

#----------------------------------------------------------------------------------------------------------------------*

def compterLignes (repertoire, extension) :
  totalLineCount = 0
  for root, dirnames, filenames in os.walk (repertoire):
    total = 0
    for filename in fnmatch.filter (filenames, '*.' + extension):
      total += compterLignesFichier (os.path.join (root, filename))
    totalLineCount += total
  return totalLineCount

#----------------------------------------------------------------- Get script absolute path
scriptDir = os.path.dirname (os.path.abspath (sys.argv [0]))
os.chdir (scriptDir)
#----------------------------------------------------------------- Get goal as first argument
totalLineCount = compterLignes (scriptDir, "cpp")
totalLineCount += compterLignes (scriptDir, "h")
totalLineCount += compterLignes (scriptDir, "ino")
print ("Total: " + str (totalLineCount) + " lignes")

#----------------------------------------------------------------------------------------------------------------------*
