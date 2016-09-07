#!/bin/bash

#if [ $# -lt 2 ]; then
#    echo "error... need args"
#    exit 1
#fi

#if [ $1 = "nginx" ]; then
#    npid=`ps -aux | grep $1 | grep -v grep | head -n 1 | awk '{print $2}'`
#    if [ $npid ]; then
#        sudo kill -9 $npid
#    echo $npid
#    fi
#    sudo /usr/local/nginx/sbin/$1
#    echo $npid
#fi

#if [ $2 = "upload" ]; then
#    upid=`ps -aux | grep $2 | grep -v grep | head -n 1 | awk '{print $2}'`
#    if [ $upid ]; then
#        kill -9 $upid
#    echo $upid
#    fi
#    spawn-fcgi -a 127.0.0.1 -p 8081 -f ./$2
#    echo $upid
#fi
    upid=`ps -aux | grep "upload_cgi" | grep -v grep | head -n 1 | awk '{print $2}'`
    kill -9 $upid
    spawn-fcgi -a 127.0.0.1 -p 8012 -f ./upload_cgi
    sudo /usr/local/nginx/sbin/nginx -s reload
    sudo /usr/bin/fdfs_trackerd ./conf/tracker.conf restart
    sudo /usr/bin/fdfs_storaged ./conf/storage.conf restart
