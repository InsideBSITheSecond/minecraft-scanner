FROM debian:latest

# Download latest listing of available packages:
RUN apt-get -y update
# Upgrade already installed packages:
RUN apt-get -y upgrade
# Install a new package:
#RUN apt-get install openssh-server -y
RUN apt-get install git cmake -y

#RUN mkdir /var/run/sshd
# Set root password for SSH access (change 'your_password' to your desired password)
#RUN echo 'root:password' | chpasswd
#RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
#RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

RUN git clone https://github.com/InsideBSITheSecond/minecraft-scanner.git
WORKDIR "/minecraft-scanner"
RUN git submodule update --init --recursive
RUN mkdir build
WORKDIR "/build"
RUN cmake -S ..
RUN make -j16

#EXPOSE 22
#CMD ["/usr/sbin/sshd", "-D"]
