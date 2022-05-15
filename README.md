# boxesCPlus
C++ API micro service for boxes react front end

Work in Progress........

Serves simple requests in less than 2 ms. On test laptop. Uses new mySQL dev API for connection pool and boost beast for http server.

Small list of 8 boxes serves about 7000 req/sec on i7 Laptop running http server and mysql database.

Work in progress just got connection pool working after changing over to XDev API mySQL Connectivity

## Required (ldd boxescplus)

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

