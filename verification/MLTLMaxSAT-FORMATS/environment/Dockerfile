FROM ubuntu:22.04
RUN echo 'APT::Install-Suggests "0";' >> /etc/apt/apt.conf.d/00-docker
RUN echo 'APT::Install-Recommends "0";' >> /etc/apt/apt.conf.d/00-docker
RUN DEBIAN_FRONTEND=noninteractive \
  apt-get update \
  && apt-get install -y python3 \
  && apt-get install -y build-essential \
  && apt-get install -y wget \
  && apt-get install -y git \
  && apt-get install -y openjdk-11-jre \
  && apt-get install -y python3-pip \
  && rm -rf /var/lib/apt/lists/* 

RUN mkdir /home/gokul && \
cd /home/gokul && \
git config --global http.sslverify false && \
wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.22.1/cmake-3.22.1-linux-x86_64.sh && \
yes | bash cmake-3.22.1-linux-x86_64.sh

RUN cd /home/gokul && \ 
git clone https://github.com/gokulhari/MLTLMaxSAT-FORMATS.git && \
cd MLTLMaxSAT-FORMATS && \
mkdir build && \
g++ runner.cpp -o runner && \
cd build && \
ln -s /home/gokul/cmake-3.22.1-linux-x86_64/bin/cmake /bin/cmake && \
cmake .. && \
make -j12 && \
git clone https://github.com/Z3Prover/z3.git  && \
cd z3 && \
git checkout 49ee570b09047cbea94f7d26e3df10c86c9ce596 && \
mkdir build && \
cd build && \
cmake .. -DCMAKE_INSTALL_PREFIX=../../usr && \
make -j12 && \
make install


RUN cd /home/gokul/ && \
wget https://temporallogic.org/research/CAV19/artifact.tar.xz && \
tar -xf artifact.tar.xz

RUN cd /home/gokul/ && \
wget -O nuXmv.tar.gz  "https://nuxmv.fbk.eu/theme/download.php?file=nuXmv-2.0.0-linux64.tar.gz" && \
tar -xf nuXmv.tar.gz

RUN DEBIAN_FRONTEND=noninteractive \
  apt-get update \
  && apt-get install -y zlib1g zlib1g-dev \
  && rm -rf /var/lib/apt/lists/*

RUN cd /home/gokul/artifact/translator/src && \
make run && \
pip install matplotlib