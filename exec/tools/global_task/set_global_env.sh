# root path setted
#set GKENGINE_HOME=%cd%\..\..\
#echo root path setted: %GKENGINE_HOME%

export GKENGINE_HOME=${PWD}'/../../'
# root version setted
#set BUILD_NUMBER=1.0.0.%date:~5,2%%date:~8,2%
#echo root version setted: %BUILD_NUMBER%

DATE=$(date +%Y%m%d)
export BUILD_NUMBER='1.0.0.'$DATE
