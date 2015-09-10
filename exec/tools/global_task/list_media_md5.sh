source set_global_env.sh

list_media_md5(){
file=$GKENGINE_HOME/media/$1.7z
hash=`md5 -q $file`
echo $hash
}

list_media_md5 terrian
list_media_md5 textures
list_media_md5 basic

list_media_md5 conf_room
list_media_md5 outdoor
list_media_md5 character
