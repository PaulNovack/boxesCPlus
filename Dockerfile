# our local base image
FROM ubuntu:20.04
MAINTAINER Paul Novack <paul.novack@gmail.com>

RUN apt-get update && apt-get install g++ -y 
RUN apt-get install rsync zip openssh-server make -y
RUN apt-get install libmysqlcppconn-dev libboost-all-dev gcc -y
RUN apt-get install build-essential -y
RUN apt-get install gcc-multilib net-tools vim curl -y

LABEL description="Ubuntu 20.04 for boxes development and deployment" 
WORKDIR /app
COPY . .
COPY /dist/Debug/GNU-Linux/libmysqlcppconn8.so /usr/lib/libmysqlcppconn8.so
COPY /dist/Debug/GNU-Linux/libserved.so /usr/local/lib/libserved.so
CMD ["cp","/usr/local/lib/libmysqlcppconn8.so","/usr/local/lib/libmysqlcppconn8.so.2"]                                                  
RUN ldconfig                  
#RUN /app/dist/Debug/GNU-Linux/boxescplus

EXPOSE 8123
