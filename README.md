# boxesCPlus
C++ API micro service for boxes react front end

Work in Progress........

Serves simple requests in less than 2 ms. On test laptop. Uses new mySQL dev API for connection pool and boost beast for http server.

Small list of 8 boxes serves about 7000 req/sec on i7 Laptop running http server and mysql database.

Work in progress just got connection pool working after changing over to XDev API mySQL Connectivity

Requires libs installed in addition to normal c++ build tools to compile

MySQLConnector C++  uses xDev API https://dev.mysql.com/doc/connector-cpp/8.0/en/

served https://github.com/meltwater/served

libboost https://www.boost.org/  (sudo apt install libboost-all-dev)



## Required libraries (ldd boxescplus)

linux-vdso.so.1 

libpthread.so.0 

libmysqlcppconn8.so.2  -- Included in dist folder for ubuntu 20.04

libserved.so.1.4  -- Included in dist folder for ubuntu 20.04

libstdc++.so.6 

libgcc_s.so.1 

libc.so.6 

/lib64/ld-linux-x86-64.so.2 

libssl.so.1.1 

libcrypto.so.1.1 

libresolv.so.2 

libm.so.6 

libdl.so.2 

Performance of get boxes method using apache bench

![apacheBenchPerformance](https://user-images.githubusercontent.com/3844301/168474846-f1e2ad35-53c9-4717-8bcb-d0522f2a8b83.png)