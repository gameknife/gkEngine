# shadercache folder

mkdir_ifnotexsit(){
if [ ! -d "$1" ]; then
mkdir $1
fi
}

mkdir_ifnotexsit ./code/thirdparty
mkdir_ifnotexsit ./exec/media
mkdir_ifnotexsit ./exec/media/config
mkdir_ifnotexsit ./exec/paks

cp ./exec/tools/default_cfg/startup.cfg ./exec/media/config

#PAUSE

