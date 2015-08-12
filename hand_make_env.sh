cd exec/tools/global_task

export GKENGINE_HOME=${PWD}'/../../'
DATE=$(date +%Y%m%d)
export BUILD_NUMBER='1.0.0.'$DATE

sh unzip_thirdparty.sh
#call place_thirdparty.bat
#PAUSE