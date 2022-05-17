# boxesCPlus
C++ API micro service for boxes react front end

Work in Progress......

Needs routes added to match golang version of:

	r.HandleFunc("/boxes", postBox).Methods("POST")
	r.HandleFunc("/boxes", putBox).Methods("PUT")
	r.HandleFunc("/boxes", getAllBoxes).Methods("GET")
	r.HandleFunc("/boxes/{box_id}", deleteBox).Methods("DELETE")
	// Handle Items endpoints
	r.HandleFunc("/items", postItem).Methods("POST")
	r.HandleFunc("/items", putItem).Methods("PUT")
	r.HandleFunc("/items/{box_id}", getItems).Methods("GET")
	r.HandleFunc("/items/{item_id}", deleteItem).Methods("DELETE")

Serves simple requests in less than 2 ms. On test laptop. Uses new mySQL dev API for connection pool and boost beast for http server.

Small list of 8 boxes serves about ~4000 req/sec on i7 Laptop running http server and mysql database.

Work in progress just got connection pool working after changing over to XDev API mySQL Connectivity

Requires libs installed in addition to normal c++ build tools to compile

MySQLConnector C++  uses xDev API https://dev.mysql.com/doc/connector-cpp/8.0/en/

served https://github.com/meltwater/served

libboost https://www.boost.org/  (sudo apt install libboost-all-dev)

## Building
Once you have all dependancies installed from root of project run:

make build or make clean

- or run with docker set up below....

## Load database

You will need to load the database "boxes.sql" in data directory into a mysql instance running on host database named "boxes" and set up user "boxes" and password "boxes" on host.

The container "startServer.sh" is set up to use argument "--add-host=host.docker.internal:host-gateway" so the docker container will reference the database by the dsn "host.docker.internal" if you are running a reasonably up to date docker installation this will work on linux, mac and windows.

## Running with docker

There is a docker file that contains all the set up to enable libraries and copy executable file into the container
- create container run: ./createBoxesDockerContainer.sh
- list the containers get the container id run: docker image list
- In the "startServer.sh" replace the hash with the containerid from above
- Run: startServer.sh

You should then be able to access: http://127.0.0.1:8123/boxes/1

# Performance of get boxes method using apache bench

![apacheBenchPerformance](https://user-images.githubusercontent.com/3844301/168474846-f1e2ad35-53c9-4717-8bcb-d0522f2a8b83.png)
