FROM ubuntu:xenial

# update and install dependencies
RUN apt-get update
RUN apt-get install -y \
        software-properties-common \
        wget
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get update
RUN apt-get install -y \
        make \
        git \
        curl \
        vim \
        vim-gnome
RUN apt-get install -y \
        gcc-4.9 g++-4.9 gcc-4.9-base \
        gcc-4.8 g++-4.8 gcc-4.8-base \
        gcc-4.7 g++-4.7 gcc-4.7-base \
        gcc-4.6 g++-4.6 gcc-4.6-base
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 100
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100

RUN mkdir app

WORKDIR /app

ADD . /app

RUN make example

CMD ./example