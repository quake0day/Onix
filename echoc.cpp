//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<netinet/in.h>
#include	<inttypes.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>


using boost::asio::ip::tcp;


enum { max_length = 1024 };

void usage();
char *progname;
char *host = NULL;
char *port = NULL;
int ch;
extern char *optarg;
extern int optind;
int HELP = 0;

int main(int argc, char* argv[])
{

    try
    {
        if ((progname = rindex(argv[0], '/')) == NULL)
            progname = argv[0];
        else
            progname++;



        while ((ch = getopt(argc, argv, "hp:s:")) != -1){
            switch(ch) {
                case 'h':
                    HELP = 1;
                    break;
                case 's':
                    host = optarg;
                    break;
                case 'p':
                    port = optarg;
                    break;
                case '?':
                default:
                    usage();
            }
        }

        if (port == NULL)
            port = "9090";
        if (host == NULL){
            host = "localhost";
        }
        if(HELP == 1){
            usage();
            exit(1);
        }


        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), host, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        boost::asio::connect(s, iterator);

        while(true){
            using namespace std; // For strlen.
            std::cout << "dec_client1$: ";
            char request[max_length];
            std::cin.getline(request, max_length);
            size_t request_length = strlen(request);
            boost::asio::write(s, boost::asio::buffer(request, request_length));

            std::vector<std::string> commands;
            boost::split(commands, request, boost::is_any_of(";"));

            char reply[max_length];
            boost::asio::streambuf response;
            boost::asio::read_until(s, response, "\r\n");
            // Check that response is OK.
            std::istream response_stream(&response);
            //std::string http_version;
            //response_stream >> http_version;
            //std::cout<< http_version <<endl;
            int i = 1;
            while(i < commands.size()){
                std::cout << "dec_client1$: Response From server: ";
                std::string status_message;
                std::getline(response_stream, status_message);
                std::cout<< status_message <<endl;
                i++;
            }

        }	/*
               std::string status_message2;
               std::getline(response_stream, status_message2);
               std::cout<< status_message2 <<endl;
               */
        //size_t reply_length = boost::asio::read(s,
        //   boost::asio::buffer(reply, 20));

        //std::cout.write(reply, reply_length);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

    void
usage()
{
    fprintf(stderr, "usage: %s [-h] [-s server-host] [-p port-number]\n", progname);
    exit(1);
}
