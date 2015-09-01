cd ../global_task/
source set_global_env.sh

find $GKENGINE_HOME/engine/ -type f -name "*.dds" -delete
find $GKENGINE_HOME/engine/ -type f -name "*.pvr" -delete
find $GKENGINE_HOME/engine/ -type f -name "*.atc" -delete
# pause