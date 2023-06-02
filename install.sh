sudo apt install build-essential libopencv-dev 
sudo apt install imagemagick ffmpeg
sudo apt install npm

npm install

cd granimate-cc
./make.sh
cd ..

mkdir public/mp4

sudo snap install --classic certbot
sudo ln -s /snap/bin/certbot /usr/bin/certbot
sudo certbot certonly --standalone
