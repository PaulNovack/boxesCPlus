#!/bin/bash
# replace image by getting image with docker image list
docker run -it -p 8123:8123 --add-host=host.docker.internal:host-gateway --name PaulCPP 855405078f8f /app/dist/Debug/GNU-Linux/boxescplus
