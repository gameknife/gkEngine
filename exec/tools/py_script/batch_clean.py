# -*- coding: cp936 -*-
# batch process script
# by kaimingyi  2012/06/30
import glob
import sys
import os

# root path
str = "..\\Engine"
str = os.path.abspath(str)

#files =glob.glob(str)

print 'Start with', str

#for name in files:
#    print name, '\n'

#go through a dir tree
for root, dirs, files in os.walk(str):
#gei into a dir
    print '[Get into]',root
  
    for name in files:
        #every file
        strThisFile = root + '\\' + name

        #if dds file
        if (os.path.splitext(strThisFile)[1]) == ".dds" or (os.path.splitext(strThisFile)[1]) == ".pvr":
            print '[File]' , strThisFile
            #strCmd = "texconv -ft tga -o " + root + " " + strThisFile
            print '[Delete]' , strThisFile
            os.remove(strThisFile)
            
            #os.system(strCmd)
    

str = "..\\Media"
str = os.path.abspath(str)

#files =glob.glob(str)

print 'Start with', str

#for name in files:
#    print name, '\n'

#go through a dir tree
for root, dirs, files in os.walk(str):
#gei into a dir
    print '[Get into]',root
  
    for name in files:
        #every file
        strThisFile = root + '\\' + name

        #if dds file
        if (os.path.splitext(strThisFile)[1]) == ".dds" or (os.path.splitext(strThisFile)[1]) == ".pvr":
            print '[File]' , strThisFile
            #strCmd = "texconv -ft tga -o " + root + " " + strThisFile
            print '[Delete]' , strThisFile
            os.remove(strThisFile)
            
            #os.system(strCmd)
