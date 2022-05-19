# boxesCPlus
C++ API micro service for boxes react front end

### Work in Progress......  

Needs mutex locking access to arrays of boxes and items 

Needs routes added to match golang version of:

	r.HandleFunc("/box", postBox).Methods("POST") -- new
	r.HandleFunc("/box", putBox).Methods("PUT") -- update
	r.HandleFunc("/box/{box_id}", deleteBox).Methods("DELETE") -- delete
	r.HandleFunc("/item", postItem).Methods("POST")  -- new
	r.HandleFunc("/item/{item_id}", putItem).Methods("PUT") -- update
	r.HandleFunc("/items/{box_id}", getItems).Methods("GET") -- all items in a box
	r.HandleFunc("/item/{item_id}", getItems).Methods("GET") -- single item
	r.HandleFunc("/item/{item_id}", deleteItem).Methods("DELETE") -- delete

Serves simple requests in less than 2 ms.

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


You should then be able to access: 

http://127.0.0.1:8123/login?username=paulnovack&password=paulnovack

http://127.0.0.1:8123/box

http://127.0.0.1:8123/box/5

http://127.0.0.1:8123/item/5

http://127.0.0.1:8123/logout

.... other methods need added still

# Performance of get boxes method using apache bench

With authkey to bench with apache bench must add cookie

ab -c50 -n1000000 -C 'authToken=fa37JncCHryDsbzayy4cBWDxS22JjzhM'  http://127.0.0.1:8123/boxes

ab -c50 -n1000000 -C 'authToken=fa37JncCHryDsbzayy4cBWDxS22JjzhM'  http://127.0.0.1:8123/boxes/5


![apacheBenchPerformance](https://user-images.githubusercontent.com/3844301/168474846-f1e2ad35-53c9-4717-8bcb-d0522f2a8b83.png)

Improved caching ab results at commit "74b7ac7"


![improvedCaching](https://user-images.githubusercontent.com/3844301/169076410-a7be2f02-8a12-4e30-a250-9c0bed5f4e88.png)
