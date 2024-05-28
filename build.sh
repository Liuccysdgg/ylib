#!/bin/bash
# 安装软件
sudo apt update
sudo apt install -y g++
sudo apt install -y cmake
sudo apt install -y libmysqlclient-dev
sudo apt install -y libssl-dev
sudo apt install -y xorg libx11-dev libgl1-mesa-dev
sudo apt install -y openssl
sudo apt install -y libboost-dev 
sudo apt install -y libmysqlcppconn-dev
sudo apt install -y libleveldb-dev
sudo apt install -y libsoci-dev
sudo apt install -y unixodbc-dev
sudo apt install -y liblua5.3-dev


# 初始化变量 

src_dir=$(pwd)
cd ..
root_dir=$(pwd)
install_dir=$(pwd)/ylib_linux_build
# 创建必要文件夹
if [ ! -d $install_dir ]; then
	mkdir $install_dir
fi

##########################开始编译#########################

######3rdparty######
cd $src_dir/3rdparty
if [ ! -d "HP-Socket" ]; then
	# 下载HPSOCKET
	git clone -b v5.9.1 https://github.com/ldcsaa/HP-Socket.git
	cd HP-Socket/Linux
	chmod 777 script/*
	echo "Y" | ./script/compile.sh
fi
cd $src_dir/3rdparty
# 安装HPSocket
sudo cp HP-Socket/Linux/lib/hpsocket/x64/*.a /usr/local/lib/
sudo cp HP-Socket/Linux/lib/hpsocket/x64/*.so /lib/x86_64-linux-gnu/
sudo cp HP-Socket/Linux/lib/hpsocket/x64/*.so.5 /lib/x86_64-linux-gnu/
sudo cp -r HP-Socket/Linux/include/* /usr/local/include/
sudo mv /usr/local/include/hpsocket /usr/local/include/HPSocket

# 安装mysql-connector-cpp
#cd $src_dir/3rdparty
#if [ ! -d "mysql-connector-cpp" ]; then
#	git clone https://github.com/mysql/mysql-connector-cpp.git
#	cd mysql-connector-cpp
#	mkdir build
#	cd build
#	cmake ../ -DCMAKE_BUILD_TYPE=Release -DWITH_JDBC=true  
#	make -j8
#	sudo make install
#fi

######核心######
cd $install_dir
cmake $src_dir .
make -j8
sudo make install
sudo cp -r $src_dir/include/* /usr/local/include/
