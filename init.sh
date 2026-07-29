cd ~

apt update -y
apt upgrade -y

apt install -y git
apt install -y wget
apt install -y gcc
apt install -y htop

# mkdir ~/lib
# cd ~/lib
# git clone https://github.com/pjreddie/darknet.git

mkdir ~/model
wget https://data.pjreddie.com/files/darknet19.weights
wget https://data.pjreddie.com/files/alexnet.weights
wget http://data.pjreddie.com/files/vgg-16.weights
wget https://data.pjreddie.com/files/resnet50.weights
wget https://data.pjreddie.com/files/resnet152.weights
wget https://data.pjreddie.com/files/densenet201.weights


wget https://raw.githubusercontent.com/pjreddie/darknet/blob/master/cfg/darknet19.cfg
wget https://raw.githubusercontent.com/pjreddie/darknet/blob/master/cfg/alexnet.cfg
wget https://raw.githubusercontent.com/pjreddie/darknet/blob/master/cfg/vgg-16.cfg
wget https://raw.githubusercontent.com/pjreddie/darknet/blob/master/cfg/resnet50.cfg
wget https://raw.githubusercontent.com/pjreddie/darknet/blob/master/cfg/resnet152.cfg
wget https://raw.githubusercontent.com/pjreddie/darknet/blob/master/cfg/densenet201.cfg