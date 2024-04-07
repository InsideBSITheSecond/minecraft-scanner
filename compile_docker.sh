# create docker image
sudo docker build -t minecraft-scanner-builder-image .

# run docker image
sudo docker run --name minecraft-scanner-builder-container minecraft-scanner-builder-image

while true; do
    read -p "Do you want to delete the docker image [y/n] ?" yn
    case $yn in
        [Yy]* ) sudo docker stop minecraft-scanner-builder-container; sudo docker rm minecraft-scanner-builder-container; sudo docker rmi minecraft-scanner-builder-image; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done