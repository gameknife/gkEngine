cd ../global_task/
source set_global_env.sh

rm `find $GKENGINE_HOME/../INTERMEDIATE -type f -name "*"`
rmdir -p $GKENGINE_HOME/../INTERMEDIATE

rm `find $GKENGINE_HOME/.. -type f -name "*.pdb"`
rm `find $GKENGINE_HOME/.. -type f -name "*.ilk"`
rm `find $GKENGINE_HOME/.. -type f -name "*.user"`
rm `find $GKENGINE_HOME/.. -type f -name "*.ipch"`
rm `find $GKENGINE_HOME/.. -type f -name "*.sdf"`
rm `find $GKENGINE_HOME/.. -type f -name "*.aps"`
rmdir -p $GKENGINE_HOME/../code/Lib
# pause