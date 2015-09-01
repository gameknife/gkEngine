cd ../global_task/
source set_global_env.sh

#find $GKENGINE_HOME/../INTERMEDIATE/ -type f -name "*" -delete
rm -rf $GKENGINE_HOME/../INTERMEDIATE

find $GKENGINE_HOME/../ -type f -name "*.pdb" -delete
find $GKENGINE_HOME/../ -type f -name "*.ilk" -delete
find $GKENGINE_HOME/../ -type f -name "*.user" -delete
find $GKENGINE_HOME/../ -type f -name "*.ipch" -delete
find $GKENGINE_HOME/../ -type f -name "*.sdf" -delete
find $GKENGINE_HOME/../ -type f -name "*.aps" -delete

rm -rf $GKENGINE_HOME/../code/Lib
# pause