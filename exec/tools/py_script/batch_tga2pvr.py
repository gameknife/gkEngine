# -*- coding: cp936 -*-
# batch process script
# by kaimingyi  2012/06/30
import glob
import sys
import os

# root path
str = "..\\engine"
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
        if (os.path.splitext(strThisFile)[1]) == ".tga":
            print '[File]' , strThisFile
            strCmd = "pvrtextool.exe -m -fOGLPVRTC4 -yflip0 -silent -i" + strThisFile
            print '[Execute]' , strCmd
            os.system(strCmd)
    

str = "..\\media"
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
        if (os.path.splitext(strThisFile)[1]) == ".tga":
            print '[File]' , strThisFile
            strCmd = "pvrtextool.exe -m -fOGLPVRTC4 -yflip0 -silent -i" + strThisFile
            print '[Execute]' , strCmd
            os.system(strCmd)
