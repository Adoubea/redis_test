#!/bin/bash

sudo /usr/local/nginx/sbin/nginx

sudo fdfs_trackerd ./conf/tracker.conf

sudo fdfs_storaged ./conf/storage.conf
