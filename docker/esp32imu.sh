#!/bin/bash 
#-----------------------------------------------------------------------
SCRIPT_PATH=$(dirname $0)
cd $SCRIPT_PATH

SCRIPT_PATH=$PWD

#-----------------------------------------------------------------------
# Подключение библиотеки цветового оформления
source ./lib_coloration.sh
#-----------------------------------------------------------------------
#echo "$USER $SCRIPT_PATH"

PROJECT_NAME="esp32imu"

PROJECT_DIR="/opt/$PROJECT_NAME"

#-----------------------------------------------------------------------
print_header "Остановить контейнеры стека $PROJECT_NAME"

docker stop $(docker ps -q --filter "label=com.docker.compose.project=$PROJECT_NAME")

#-----------------------------------------------------------------------
print_header "Удалить контейнеры стека $PROJECT_NAME"

docker rm $(docker ps -a -q --filter "label=com.docker.compose.project=$PROJECT_NAME")

#-----------------------------------------------------------------------
print_header "Удалить сети стека $PROJECT_NAME"

docker network rm $(docker network ls -q --filter "label=com.docker.compose.project=$PROJECT_NAME")

#-----------------------------------------------------------------------
print_header "Удалить тома"

docker volume rm $(docker volume ls -q --filter "label=com.docker.compose.project=$PROJECT_NAME")

#-----------------------------------------------------------------------
print_header "Удалить старые каталоги "

sudo rm -rf $PROJECT_DIR/{*,.*}

#-----------------------------------------------------------------------
print_header "Создать каталоги: "

setup_dir () {
	mkdir -p $1
	sudo chown $USER:$USER $1
	sudo chmod u=rwx,g=rx,o=rx $1
 	print_step "    - cоздание и настройка [$1] $USER:$USER u=rwx,g=rx,o=rx"
} 
#-----------------------------------------------------------------------
setup_dir $PROJECT_DIR

setup_dir $PROJECT_DIR/nginx
setup_dir $PROJECT_DIR/site

#-----------------------------------------------------------------------
cd $SCRIPT_PATH
#cd ./esp32imu/esp32imu-docs

#print_header "Запуск создания сайта: "
#mkdocs build

#-----------------------------------------------------------------------
cd $SCRIPT_PATH

print_header "Скопировать данные сайта в рабочий каталог: "
cp ./esp32uav-docs/site $PROJECT_DIR/site

#-----------------------------------------------------------------------
cd $SCRIPT_PATH

print_header "Скопировать настройки сайта в рабочий каталог: "
cp ./nginx/default.conf $PROJECT_DIR/nginx/default.conf

#-----------------------------------------------------------------------
print_header "Запуск пересоздания композиции: "

cd $SCRIPT_PATH

docker-compose \
	-p $PROJECT_NAME \
	-f $PROJECT_NAME.yml \
	up -d \
	--build \
#	--force-recreate \
#	--profile full \

#-----------------------------------------------------------------------
