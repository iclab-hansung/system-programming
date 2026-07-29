cd "$(dirname "$0")"

apt update -y
apt upgrade -y

apt install -y git
apt install -y wget
apt install -y gcc
apt install -y htop

git clone https://github.com/sjlee9908/system-programming-example.git
