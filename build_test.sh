#!/bin/bash

clear
export BUILD_MODE=test
make info all
./test
