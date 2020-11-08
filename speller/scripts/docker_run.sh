#!/bin/bash

docker run -v $(pwd):/usr/src/app ubuntuc:0.1 bash -c "make build; $1"
