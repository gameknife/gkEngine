cd ../global_task/
source set_global_env.sh

cd ../

#sh sgx_tga2pvr_engine.sh
#sh sgx_tga2pvr_media.sh

$GKENGINE_HOME/tools/gkPak -i "media/level" -o "paks/level.gpk" -l 5 -p 1
$GKENGINE_HOME/tools/gkPak -i "media/objects" -o "paks/objects.gpk" -l 5 -p 1
$GKENGINE_HOME/tools/gkPak -i "media/terrian" -o "paks/terrian.gpk" -l 5 -p 1
$GKENGINE_HOME/tools/gkPak -i "media/textures" -o "paks/textures.gpk" -l 5 -p 1
$GKENGINE_HOME/tools/gkPak -i "engine" -o "paks/engine.gpk" -l 5 -p 1
$GKENGINE_HOME/tools/gkPak -i "media/config" -o "paks/config.gpk" -l 5 -p 1
$GKENGINE_HOME/tools/gkPak -i "media/fonts" -o "paks/fonts.gpk" -l 5 -p 1
