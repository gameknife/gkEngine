cd ../global_task/
source set_global_env.sh

rm `find $GKENGINE_HOME/media -type f -name "*.pvr"`
rm `find $GKENGINE_HOME/media -type f -name "*.dds"`
rm `find $GKENGINE_HOME/media -type f -name "*.atc"`

# pause