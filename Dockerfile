# I use arch, btw
FROM archlinux:latest

# Install stuff
RUN pacman --noconfirm -Syu git cmake make gcc clang

# clone repo
RUN git clone https://github.com/InsideBSITheSecond/minecraft-scanner.git
WORKDIR "/minecraft-scanner"

# compile
RUN git submodule update --init --recursive
RUN mkdir build
WORKDIR "/minecraft-scanner/build"
RUN cmake -S ..
RUN make