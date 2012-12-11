//
// blocking_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include "asio.hpp"
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <cstddef>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>




/* for test only */
#include <boost/graph/graph_utility.hpp>

using boost::asio::ip::tcp;
using namespace std;
using namespace boost;
const int max_length = 1024;
int LOGGING = 0;
typedef adjacency_matrix<directedS> Graph;
#ifndef HAVE_SOCKLEN_T
typedef unsigned int socklen_t;
typedef unsigned int socket_t;
#endif

typedef boost::shared_ptr<tcp::socket> socket_ptr;
typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
typedef boost::iostreams::stream<Tee> TeeStream;

bool should_we_add(Graph &G, int u, int v);
void print_query_res(int status);
bool check_valid(int u, int v);


void usage();
char *progname;
char *filename = NULL;
char *port = NULL;
int ch;
extern char *optarg;
extern int optind;
int HELP = 0;
void logging(char* filename, string data);

template <typename T>
class List{
    public:
        //构造函数
        List():head(NULL), len(0){}
        //析构函数
        ~List(){
            clear();
        }
        //判断链表是否为空
        bool empty()const{
            return head == NULL;
        }
        //返回链表元素的个数
        int size()const{
            return len;
        }
        //遍历链表
        void travel()const{
            if(empty())
                return ;
            Node* p = head;
            while(p != NULL){
                //cout << p->data << ' ';
                p = p->next;
            }
            //cout << endl;
        }
        //遍历链表并查找元素K
        bool travel_and_find(const T& d)const{
            Node* q = new Node(d);
            if(empty())
                return false;
            Node* p = head;
            while(p != NULL){
                if(p->data == q->data){
                    return true;
                }
                p = p->next;
            }
            return false;
        }

        //向任意位置插入元素
        void insert(const T& d, int pos){
            Node* p = new Node(d);
            Node*& pn = getptr(pos);
            p->next = pn;
            pn = p;
            ++len;
        }
        //从链表头部插入
        List& push_front(const T& d){
            insert(d, 0);
            return *this;
        }
        //从链表尾部插入
        List& push_back(const T& d){
            insert(d, size());
            return *this;
        }
        //返回头部元素
        T front()const{
            if(empty())
                return ;
            return head->data;
        }
        //返回尾部元素
        T back()const{
            if(empty())
                return ;
            Node*& pn = getptr(size()-1);
            return pn->data;
        }
        //删除指定位置元素
        void erase(int pos){
            if(pos < 0 || pos >= size())
                throw "invalid";
            Node*& pn = getptr(pos);
            Node* p = pn;
            pn = pn->next;
            delete p;
            --len;
        }
        //删除表头元素
        void pop_front(){
            erase(0);
        }
        //删除尾部元素
        void pop_back(){
            erase(size()-1);
        }
    private:
        struct Node{
            T data;
            Node* next;
            Node(T d):data(d), next(NULL){}
        };
        Node* head;
        int len;
        //释放动态内存，供析构函数调用
        void clear(){
            if(head == NULL)
                return ;
            while(head != NULL){
                Node* p = head;
                delete p;
                head = head->next;
            }
        }
        //得到指向插入位置的指针
        Node*& getptr(int pos){
            if(pos < 0 || pos > size())
                throw "invalid";
            if(pos == 0)
                return head;
            Node* p = head;
            for(int i=1; i<pos; i++){
                p = p->next;
            }
            return p->next;
        }
};


bool add_edge_in(Graph &G, char x, char y,List<int> &li);
int query_edge(Graph &G, int u, int v, List<int> &li);



//6个顶点的名称属性
List<int> li;
//有向图
typedef adjacency_matrix<directedS> Graph;

Graph g(26);

Graph m(26);
List<int> ml;


void session(socket_ptr sock)
{
    try
    {
        for (;;)
        {
            char data[max_length];
			
            boost::system::error_code error;
            size_t length = sock->read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            std::vector<std::string> strs;
            char *charPtr = data;
            string str = charPtr;
            std::vector<std::string> commands;
			if(LOGGING != 1){
				cout<<"dec_server$ ";
			//cout<<"request received from"<< s<< ":"<<endl;
            cout<<str<<endl;
		}
			//logging(filename,s);
			logging(filename,str);
            if (std::string::npos == str.find_first_of(";")){

                boost::asio::streambuf request;
                std::ostream request_stream(&request);	
                request_stream<<"You miss ; please try again\r\n";
                boost::asio::write(*sock, request);			  

            }
            else if (std::string::npos != str.find_first_of(";"))
            {

                boost::split(strs, str, boost::is_any_of(";"));
                //cout<<strs[0]<<endl;
                //boost::asio::write(*sock, boost::asio::buffer(data, length));

                std::vector<string>::iterator it;



                boost::asio::streambuf request;
				boost::asio::streambuf request2;
                std::ostream request_stream(&request);	
                for(it = strs.begin(); it != strs.end()-1; it++)
                {
                    string str = *it;
                    str= trim_left_copy(str);
                    trim_right(str);
                    boost::split(commands, str, boost::is_any_of(" "));
                    string command_begin = commands[0];
                    if(boost::iequals(command_begin, "reset")){
                        bool status;
                        typedef graph_traits<Graph>::vertex_iterator vertex_iter;
                        std::pair<vertex_iter, vertex_iter> vp;
                        for (vp = vertices(g); vp.first != vp.second; ++vp.first){
                            clear_vertex(*vp.first,g);
                        }                    
                        while(li.empty()==false){
                            li.pop_front();
                        }
                        if(li.size() == 0){
                            status = true;
                        }

                        //boost::asio::streambuf request;
                        //std::ostream request_stream(&request);

                        if(status == true){
							if(LOGGING != 1){
                            cout<<"RESET DONE"<<endl;
						}
						logging(filename,"RESET DONE\r\n");
                            request_stream << "RESET DONE\r\n";
							
                        }
                        else if(status == false){
							if(LOGGING != 1){
                            cout<<"RESET FAILED"<<endl;
						}

						logging(filename,"RESET FAILED\r\n");
						
                            request_stream << "RESET FAILED\r\n";
                        }
                        // send(sock,sendBuf, k.size(), 0);
                        // Send the request.
                        boost::asio::write(*sock, request);
                        //boost::asio::write(*sock, boost::asio::buffer(sendBuf, strlen(sendBuf)));
                    }

                    // insert Command
                    else if(boost::iequals(command_begin, "insert")){
                        std::vector<string>::iterator event;
                        bool addAble=true;
                        int status;
                        for(event = commands.begin(); event != commands.end(); event++){
                            string e = *event;
                            std::vector<std::string> nodes;
                            boost::split(nodes, e, boost::is_any_of("->"));
                            //cout<<"SIZE:"<<nodes.size()<<endl;
                            string k;
                            char *sendBuf = NULL;
                            //boost::asio::streambuf request;
                            //std::ostream request_stream(&request);

                            if(nodes.size() == 3){
                                if(check_valid(nodes[0].c_str()[0],nodes[2].c_str()[0]) == false){
                                    continue;
                                }
                                bool isAdd = false;
                                bool isConfilct = false;

                                isAdd = should_we_add(g, nodes[0].c_str()[0], nodes[2].c_str()[0]);
                                isConfilct = ::add_edge_in(m,nodes[0].c_str()[0],nodes[2].c_str()[0],ml);
                                //cout<<isConfilct<<endl;
                                if(isAdd == false){
									if(LOGGING != 1){
                                    cout<<"CONFILICT DETECTED. INSERT FAILED"<<endl;
                                    cout<<nodes[0]<<"->"<<nodes[2]<<" and "<<nodes[2]<<"->"<<nodes[0]<<" cannot be true at the same time"<<endl;
									
								}
									request_stream << "CONFILICT DETECTED. INSERT FAILED \r\n";
                                    request_stream << nodes[0];
                                    request_stream << "->";
                                    request_stream << nodes[2];
                                    request_stream << " and ";
                                    request_stream << nodes[2];
                                    request_stream << "->";
                                    request_stream << nodes[0];
                                    request_stream << " cannot be true at the same time\r\n";
                                    addAble = false;
									string k = "CONFILICT DETECTED. INSERT FAILED\r\n";
									k.append(nodes[0]);
									k.append("->");
									k.append(nodes[2]);
									k.append(" and ");
									k.append(nodes[2]);
									k.append("->");
									k.append(nodes[0]);
									k.append(" cannot be true at the same time\r\n");
									logging(filename,k);
                                }
                                if(isConfilct == false){
                                    request_stream <<"CONFILICT DETECTED. INSERT FAILED\r\n";
                                    request_stream <<"There is a Cycle in your input.\r\n";
                                    addAble = false;
									string k = "CONFILICT DETECTED. INSERT FAILED\r\n";
									k.append("There is a Cycle in your input.\r\n");
									logging(filename,k);

                                }
                            }
                        }
                        typedef graph_traits<Graph>::vertex_iterator vertex_iter;
                        std::pair<vertex_iter, vertex_iter> vp;
                        for (vp = vertices(m); vp.first != vp.second; ++vp.first){
                            clear_vertex(*vp.first,m);
                        }
                        while(ml.empty()==false){
                            ml.pop_front();
                        }
                        if(addAble == true){
                            bool total_status = true;
                            for(event = commands.begin(); event != commands.end(); event++){
                                string e = *event;
                                std::vector<std::string> nodes;
                                boost::split(nodes, e, boost::is_any_of("->"));
                                //cout<<"SIZE:"<<nodes.size()<<endl;

                                if(nodes.size() == 3){
                                    string k;           
                                    char *sendBuf = NULL;
                                    status = ::add_edge_in(g,nodes[0].c_str()[0],nodes[2].c_str()[0],li);
                                }

                            }
                            if(total_status == true){
								if(LOGGING != 1){
                                cout<<"Insert DONE.\r\n"<<endl;
							}
							logging(filename,"INSERT DONE");
							
                                request_stream << "INSERT DONE. \r\n";
                            }
                        }

                        /*
                        //打印图g顶点集合
                        std::cout << "有向图g的各种元素: "<<std::endl;
                        std::cout << "vertex set: ";
                        print_vertices(g, name);
                        std::cout << std::endl;


                        //打印图g边集合
                        std::cout << "edge set: ";
                        print_edges(g, name);
                        std::cout << std::endl;

                        //打印有向图g所有顶点的出边集合
                        std::cout << "out-edges: " << std::endl;
                        print_graph(g, name);
                        std::cout << std::endl;

*/
                    }


                    else if(boost::iequals(command_begin, "query")){
                        if(commands.size() >= 3){
                            int status=true;
                            status = query_edge(g, commands[1].c_str()[0], commands[2].c_str()[0], li);
							if(LOGGING != 1){
                            cout<<"QUERY DONE."<<endl;
							logging(filename,"QUERY DONE");
						}


                            if(status == 0){
                                request_stream << commands[1];
                                request_stream << " Happend before ";
                                request_stream << commands[2];
								string k = commands[1];
								k.append(" happend  before ");
								k.append(commands[2]);
								logging(filename,k);
                            }
                            if(status == 1){
                                //my_id='D';
                                request_stream << commands[2];
                                request_stream << " happend before ";
                                request_stream << commands[1];
								string k = commands[2];
								k.append(" happend  before ");
								k.append(commands[1]);
								logging(filename,k);
                            }
                            if(status == 2){
                                request_stream << commands[1];
                                request_stream <<" concurrent to ";
                                request_stream << commands[2];
								string k = commands[1];
								k.append(" concurrent to ");
								k.append(commands[2]);
								logging(filename,k);
                            }
                            if(status == -1){
                                request_stream <<"Event not found: ";
                                request_stream << commands[1];
								string k = "Event not found: ";
								k.append(commands[1]);
								logging(filename,k);
                            }
                            if(status == -2){
                                request_stream <<"Event not found: ";
                                request_stream <<commands[2];
								string k = "Event not found: ";
								k.append(commands[2]);
								logging(filename,k);
                            }
                            if(status == -3){
                                request_stream <<"Event not found: ";
                                request_stream <<commands[1];
                                request_stream <<",";
                                request_stream <<commands[2];
								string k = "Event not found: ";
								k.append(commands[1]);
								k.append(",");
								k.append(commands[2]);
								logging(filename,k);
                            }
                            if(status == -4){
                                request_stream << "Query the same event: ";
                                request_stream <<commands[1];
								string k = "Query the same event: ";
								k.append(commands[1]);
								logging(filename,k);
								
                            }
                            if(status == -5){
                                request_stream << "Server error...";
								logging(filename,"Server error...");
								
                            }
                            request_stream << "\r\n";

                        }
                    }
                        else{
                            //boost::asio::streambuf request;
                            //std::ostream request_stream(&request);
							if(LOGGING !=1){
                            cout<<"null or invalid command"<<endl;
						}
							//logging(filename,"null or invalid command");
						
                            request_stream <<"null or invalid command\r\n";
                        }
                    }
                    request_stream << "\r\n\r\n";
					/*
					std::istream is(&request);
					std::string line;
					std::getline(is, line);
					while(line.size() > 0){
					logging(filename,line);
					std::getline(is, line);
				}
				*/
                	boost::asio::write(*sock, request);
				
                }
            }
        }
        catch (std::exception& e)
        {
			if(LOGGING != 1){
            std::cerr << "Exception in thread: " << e.what() << "\n";
		}
		logging(filename,"Exception in thread");
        }
    }

    void server(boost::asio::io_service& io_service, short port)
    {
        tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
        for (;;)
        {
            socket_ptr sock(new tcp::socket(io_service));
            a.accept(*sock);
            boost::thread t(boost::bind(session, sock));
        }
    }

    int main(int argc, char* argv[])
    {
        try
        {
			
	        if ((progname = rindex(argv[0], '/')) == NULL)
	            progname = argv[0];
	        else
	            progname++;
			

	        while ((ch = getopt(argc, argv, "hp:I:")) != -1){
	            switch(ch) {
	                case 'h':
	                    HELP = 1;
	                    break;
	                case 'I':
	                    filename = optarg;
						LOGGING = 1;
						//logging(filename,"KKKK");
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
	        if ( filename == NULL && LOGGING == 1){
	            usage();
	        }
	        if(HELP == 1){
	            usage();
	            exit(1);
	        }	
            boost::asio::io_service io_service;

            using namespace std; // For atoi.
			
            server(io_service, atoi(port));
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }

        return 0;
    }



    bool should_we_add(Graph &G, int x, int y){
        int u = (int) x;
        int v = (int) y;
        if(u >= 65 && u <= 90 && v>=65 && v<= 90){
            u = u -65;
            v = v -65;

        }
        if(u == v) // insert same event
        {
            return false;
        }

        std::vector<int> distances(num_vertices(G), 0);

        breadth_first_search(G, vertex(v, G),
                visitor(make_bfs_visitor(record_distances(&distances[0], on_tree_edge()))));

        if(distances[u] != 0){
            // it is reachable, do NOT add the edge
            //cout << "Cycle!" << endl;
            return false;
        }
        return true;
    }

    bool add_edge_in(Graph &G, char x, char y,List<int> &li){
        int u = (int) x;
        int v = (int) y;
        if(u >= 65 && u <= 90 && v>=65 && v<= 90){
            u = u -65;
            v = v -65;
            if(u == v) // insert same event
            {
                return false;
            }
        }
        else{
            return false;
        }
        if(should_we_add(G, u, v) == true){
            add_edge(u,v,G);
            if(li.travel_and_find(u) == false){
                li.push_back(u);
            }
            if(li.travel_and_find(v) == false){
                li.push_back(v);
            }
            return true;
        }
        else{
            return false;
        }
    }

    /* 0:u -> v
     * 1: v -> u
     * 2: u,v concurrent
     * -1: u not found
     * -2: v not found
     * -3: u and v not found
     * -5: occur error
     * -4: query the same thing;
     */
    int query_edge(Graph &G, int x, int y, List<int> &li){
        int u = (int) x;
        int v = (int) y;
        if(u >= 65 && u <= 90 && v>=65 && v<= 90){
            u = u -65;
            v = v -65;
            if(u == v) // insert same event
            {
                return -4;
            }
        }
        bool test_u_v = true;
        bool test_v_u = true;
        int status = -5;
        if(li.travel_and_find(u) == false && li.travel_and_find(v) == false){
            // not found u and v
            status = -3;
            return status;
        }
        if(li.travel_and_find(u) == false){ // not found u
            status = -1;
            return status;
        }
        if(li.travel_and_find(v) == false){ // not found v
            status = -2;
            return status;
        }
        test_u_v = should_we_add(G, u, v);
        test_v_u = should_we_add(G, v, u);
        if(test_u_v == true && test_v_u == true){
            status = 2;
            return status;
        }
        if(test_u_v == true && test_v_u == false){
            status = 0;
            return status;
        }
        if(test_u_v == false && test_v_u == true){
            status = 1;
            return status;
        }
        return status;
    }

    void print_query_res(int status){
        if(status == 0){
            cout<<"u->v"<<endl;
        }
        if(status == 1){
            cout<<"v->u"<<endl;
        }
        if(status == 2){
            cout<<"u | v"<<endl;
        }
        if(status == -1){
            cout<<"u not found"<<endl;
        }
        if(status == -2){
            cout<<"v not found"<<endl;
        }
        if(status == -3){
            cout<<"u, v not found"<<endl;
        }
    }

    bool check_valid(int u, int v)
    {
        if(u >= 65 && u <= 90 && v>=65 && v<= 90){
            return true;
        }
        return false;
    }
	
void
usage()
{
    fprintf(stderr, "usage: %s [-h] [-p port-number] [-I file]\n", progname);
    exit(1);
}

void logging(char* filename, string data){
	if(LOGGING == 1){
	ofstream f1(filename,ios::app);
	if(!f1) 
		return;
	f1<<data<<endl;
	f1.close();
}
}