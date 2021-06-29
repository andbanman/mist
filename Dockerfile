FROM ubuntu:latest

RUN apt-get -y update
RUN apt-get -y install libboost-test1.71-dev
RUN apt-get -y install libboost-serialization1.71-dev
RUN apt-get -y install libboost-python1.71-dev
RUN apt-get -y install libboost-filesystem1.71-dev
RUN apt-get -y install libboost-program-options1.71-dev
RUN apt-get -y install libboost-numpy1.71-dev
RUN apt-get -y install python3-numpy
RUN apt-get -y install python3-dev
RUN apt-get -y install cmake
RUN apt-get -y install g++
RUN apt-get -y install gdb

#
# Mount source at /mist
#

WORKDIR /mist/build
CMD cmake -B /mist/build -DBuildPython:BOOL=YES -DBuildTest:BOOL=YES -DCMAKE_BUILD_TYPE=Release /mist && \
    cmake --build /mist/build --config Release

ENV PYTHONPATH=/mist/build/lib
