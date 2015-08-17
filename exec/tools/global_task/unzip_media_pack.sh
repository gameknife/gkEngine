source set_global_env.sh

mkdir $GKENGINE_HOME/media
cd $GKENGINE_HOME/media

$GKENGINE_HOME/tools/7zr x $GKENGINE_HOME/media/media.7z

del $GKENGINE_HOME/media/media.7z
