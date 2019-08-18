#!/bin/bash
for((i=8080;i<=8120;i++));
    do
        kill -9 $(lsof -i :$i)
    done