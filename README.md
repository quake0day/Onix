Onix
====
![Mou icon](http://img.fun.wayi.com.tw/attachment/201009/6/94074_1283770758vmBb.jpg)


## Overview
**Onix**, A distributed event coordination system (DEC) system in C/C++ on a UNIX-based platform.

For CSE 521 Third Project

**Due Date: Dec 12 2012**

## Usage
It's simple to compile this project, just type 

	make

and it will automatically generate two executable files. One is dec_server and the other one is dec_client.

To start dec_server. Type

	dec_server [-h] [-p port-number] [-I file]

To start dec_client. Type

	dec_client [-h] [-s server-host][-p port-number] 

To Clean the generated files, type

	make clean

## More..

This program has been fully tested. The entire programming requirements are implemented.

**This program can be compiled on timberlake.cse.buffalo.edu.**
For some computer which do not have the C++ boost library, it may occure some errors. However, Prof.Kosar emailed me and said the standard environment is timberlake.cse.buffalo.edu.

## Team Member
Si Chen