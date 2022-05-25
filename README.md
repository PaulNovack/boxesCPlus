# boxesCPlus

<img src="https://user-images.githubusercontent.com/3844301/170073844-9e893950-2ca9-4a53-a672-c6724f6de4ca.jpeg" alt="drawing" width="250"/>

C++ API micro service for boxes react front end

### Work in Progress......  

Has routes set up to server boxesReact Production build files.  Path are currently hardcoded in the source.  Will change to commandline startup arguments

Log in will automatically create user if username is not take with password entered.

Needs better mutex locking access to arrays of boxes and items and move functions to classes

Most routes for JSON API done.

Serves simple requests in less than 2 ms.

when starting service dbcommection string like 'boxes:boxes@127.0.0.1/boxes' must be passed as commandline argument.

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

GET

http://127.0.0.1:8123/login?username=paulnovack&password=paulnovack

http://127.0.0.1:8123/box - Get all boxes

http://127.0.0.1:8123/box/{id} - Get list of items for a box

http://127.0.0.1:8123/item/{id} - Get an Item

http://127.0.0.1:8123/logout - Logout

PUT

http://127.0.0.1:8123/box/{id} - updates existing box

http://127.0.0.1:8123/item/{id} - updates existing item


POST

http://127.0.0.1:8123/box - returns new box json with auto increment id

http://127.0.0.1:8123/item - returns new item json with auto increment id

DELETE

http://127.0.0.1:8123/box/{id} - deleted box if you have ownership

http://127.0.0.1:8123/item{id} - deletes item if you have ownership

.... other methods need added still

# Performance of get boxes method using apache bench

With authkey to bench with apache bench must add cookie

ab -c50 -n1000000 -C 'authToken=fa37JncCHryDsbzayy4cBWDxS22JjzhM'  http://127.0.0.1:8123/boxes

ab -c50 -n1000000 -C 'authToken=fa37JncCHryDsbzayy4cBWDxS22JjzhM'  http://127.0.0.1:8123/boxes/5


![apacheBenchPerformance](https://user-images.githubusercontent.com/3844301/168474846-f1e2ad35-53c9-4717-8bcb-d0522f2a8b83.png)

Improved caching ab results at commit "74b7ac7"


![improvedCaching](https://user-images.githubusercontent.com/3844301/169076410-a7be2f02-8a12-4e30-a250-9c0bed5f4e88.png)
