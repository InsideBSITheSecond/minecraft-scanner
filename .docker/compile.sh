sudo docker rmi my_ssh_image --force
sudo docker build -t my_ssh_image .
#sudo docker images
sudo docker run -d -p 2222:22 --name my_ssh_container my_ssh_image
#sudo docker port my_ssh_container
sudo docker inspect --format='{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_ssh_container