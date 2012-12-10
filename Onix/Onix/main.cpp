//FileName:adj_matrix_demo.cpp
//演示邻接矩阵adjacency_matrix的基本应用

#pragma warning(disable : 4800 4819)
static char svnid[] = "$Id: soc.c 6 2009-07-03 03:18:54Z kensmith $";

#define	BUF_LEN	8192







char *progname;
char buf[BUF_LEN];

void usage();
int setup_client();
int setup_server();





#include	<stdio.h>
#include	<stdlib.h>
//#include	<string.h>
//#include	<ctype.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<netinet/in.h>
#include	<inttypes.h>

#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <functional>
#include <cctype>
#include <locale>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/breadth_first_search.hpp>


/* for test only */
#include <boost/graph/graph_utility.hpp>

int s, sock, ch, server, done, aflg;
ssize_t bytes;
int soctype = SOCK_STREAM;
char *host = NULL;
char *port = NULL;
extern char *optarg;
extern int optind;


//用于print_vertices(), print_edges()和print_graph();
using namespace boost;
using namespace std;

typedef adjacency_matrix<directedS> Graph;
#ifndef HAVE_SOCKLEN_T
typedef unsigned int socklen_t;
typedef unsigned int socket_t;
#endif

bool should_we_add(Graph &G, int u, int v);
void print_query_res(int status);
bool check_valid(int u, int v);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}
// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

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
            std::cout << p->data << ' ';
			p = p->next;
		}
        std::cout << std::endl;
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


const size_t MAX_THREADS = 4;
bool ctrlc_pressed = false;

int main(int argc, char *argv[])
{
    enum { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y ,Z, TOTAL };
	//枚举常量，A…F用作顶点描述器
    
    //For test only
    //根据枚举量的性质，N == 6
    const char* name = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
	//6个顶点的名称属性
    List<int> li;
    
	//有向图
	typedef adjacency_matrix<directedS> Graph;
    
	Graph g(TOTAL);
    
    Graph m(TOTAL);
    List<int> ml;
    
    
    std::vector<std::string> strs;
    std::vector<std::string> commands;
    fd_set ready;
	struct sockaddr_in msgfrom;
	socket_t msgsize;
	union {
		uint32_t addr;
		char bytes[4];
	} fromaddr;
    
	if ((progname = rindex(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;
	while ((ch = getopt(argc, argv, "adsp:h:")) != -1)
		switch(ch) {
			case 'a':
				aflg++;		/* print address in output */
				break;
			case 'd':
				soctype = SOCK_DGRAM;
				break;
			case 's':
				server = 1;
				break;
			case 'p':
				port = optarg;
				break;
			case 'h':
				host = optarg;
				break;
			case '?':
			default:
				usage();
		}
	argc -= optind;
	if (argc != 0)
		usage();
	if (!server && (host == NULL || port == NULL))
		usage();
	if (server && host != NULL)
		usage();
    /*
     * Create socket on local host.
     */
	if ((s = socket(AF_INET, soctype, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	if (!server)
		sock = setup_client();
	else
		sock = setup_server();
    /*
     * Set up select(2) on both socket and terminal, anything that comes
     * in on socket goes to terminal, anything that gets typed on terminal
     * goes out socket...
     */
	while (!done) {
		FD_ZERO(&ready);
		FD_SET(sock, &ready);
		FD_SET(fileno(stdin), &ready);
		if (select((sock + 1), &ready, 0, 0, 0) < 0) {
			perror("select");
			exit(1);
		}
        int my_id = 'A';
        int my_net_id = htonl(my_id);
		if (FD_ISSET(fileno(stdin), &ready)) {
			if ((bytes = ::read(fileno(stdin), buf, BUF_LEN)) <= 0)
				done++;
			///send(sock, buf, bytes, 0);
            send(sock,(const char*)&my_net_id, 4, 0);
		}
		msgsize = sizeof(msgfrom);
		if (FD_ISSET(sock, &ready)) {
			if ((bytes = ::recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr *)&msgfrom, &msgsize)) <= 0) {
				done++;
			} else if (aflg) {
				fromaddr.addr = ntohl(msgfrom.sin_addr.s_addr);
				fprintf(stderr, "Request received from %d.%d.%d.%d: ", 0xff & (unsigned int)fromaddr.bytes[0],
                        0xff & (unsigned int)fromaddr.bytes[1],
                        0xff & (unsigned int)fromaddr.bytes[2],
                        0xff & (unsigned int)fromaddr.bytes[3]);
                std::cout<<std::endl;
			}
			//write(fileno(stdout), buf, bytes);
            char *charPtr = buf;
            std::string str = charPtr;
            
            std::cout<<str<<std::endl;
            if (std::string::npos == str.find_first_of(";")){
                //std::ostream request_stream(&request);
                //request_stream<<"You miss ; please try again\r\n";
                cout<<"No ;"<<endl;
            }
            else if (std::string::npos != str.find_first_of(";"))
            {

            //boost::split(strs, str, boost::is_any_of(";"));
            std::vector<std::string> strs = split(str, ';');
            //cout<<strs[0]<<endl;
            std::vector<string>::iterator it;
            for(it = strs.begin(); it != strs.end(); it++)
            {
                string str = *it;
                trim(str);
                //trim_right(str);
                std::vector<std::string> commands = split(str, ' ');
                //boost::split(commands, str, boost::is_any_of(" "));
                //boost::trim(commands[0]);
                cout<<commands.size()<<endl;
                if(commands.size() == 0){
                    continue;
                }
                string command_begin = commands[0];
                string reset = "reset";
                string insert = "insert";
                string query = "query";
                if(reset == command_begin){
                    
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
                    string k;
                    char *sendBuf = NULL;
                    if(status == true){
                        cout<<"RESET DONE"<<endl;
                        k="RESET DONE";
                        sendBuf = (char *)k.c_str();
                    }
                    else if(status == false){
                        cout<<"RESET FAILED"<<endl;
                        k="RESET FAILED";
                        sendBuf = (char *)k.c_str();
                    }
                    k = k.append("\n");
                    send(sock,sendBuf, k.size(), 0);
                }
                // insert Command
                else if(command_begin == insert){
                    std::vector<string>::iterator event;
                    bool addAble=true;
                    int status;
                    for(event = commands.begin(); event != commands.end(); event++){
                        string e = *event;
                        //std::vector<std::string> nodes;
                        std::vector<std::string> nodes = split(e, '>');
                        //boost::split(nodes, e, boost::is_any_of("->"));
                        //cout<<"SIZE:"<<nodes.size()<<endl;
                        string k;
                        char *sendBuf = NULL;
                        cout<< nodes[0].c_str()[0] <<":" <<nodes[1].c_str()[0] << endl;
                        if(nodes.size() == 2){
                            if(check_valid(nodes[0].c_str()[0],nodes[1].c_str()[0]) == false){
                                continue;
                            }
                            bool isAdd = false;
                            bool isConfilct = false;
                            
                            isAdd = should_we_add(g, nodes[0].c_str()[0], nodes[1].c_str()[0]);
                            isConfilct = ::add_edge_in(m,nodes[0].c_str()[0],nodes[1].c_str()[0],ml);
                            //cout<<isConfilct<<endl;
                            if(isAdd == false){
                                cout<<"CONFILICT DETECTED. INSERT FAILED"<<endl;
                                cout<<nodes[0].c_str()[0]<<"->"<<nodes[1].c_str()[0]<<" and "<<nodes[1].c_str()[0]<<"->"<<nodes[0].c_str()[0]<<" cannot be true at the same time"<<endl;
                                k = "CONFILICT DETECTED. INSERT FAILED\n";
                                k.append(nodes[0]);
                                k.append("->");
                                k.append(nodes[1]);
                                k.append(" and ");
                                k.append(nodes[1]);
                                k.append("->");
                                k.append(nodes[0]);
                                k.append(" cannot be true at the same time");
                                sendBuf = (char*)k.c_str();
                                k = k.append("\n");
                                //int my_net_id = htonl(my_id);
                                //send(sock,(const char*)&my_net_id, 4, 0);
                                send(sock,sendBuf, k.size(), 0);
                                //remove_edge
                                addAble = false;
                            }
                            if(isConfilct == false){
                                cout<<"CONFILICT DETECTED. INSERT FAILED"<<endl;
                                cout<<"There is a cycle in this input"<<endl;
                                k = "CONFILICT DETECTED. INSERT FAILED\n";
                                k.append("There is a Cycle in your input.");
                                sendBuf = (char*)k.c_str();
                                k = k.append("\n");
                                //int my_net_id = htonl(my_id);
                                //send(sock,(const char*)&my_net_id, 4, 0);
                                send(sock,sendBuf, k.size(), 0);
                                addAble = false;
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
                            //std::vector<std::string> nodes;
                            std::vector<std::string> nodes = split(e, '->');
                            //boost::split(nodes, e, boost::is_any_of("->"));
                            //cout<<"SIZE:"<<nodes.size()<<endl;
                            
                            if(nodes.size() == 2){
                                string k;
                                
                                char *sendBuf = NULL;
                                cout<<nodes[0].c_str()[0]<<":"<<nodes[1].c_str()[0]<<endl;
                                status = ::add_edge_in(g,nodes[0].c_str()[0],nodes[1].c_str()[0],li);
                                if(status == false){
                                    cout<<"CONFILICT DETECTED. INSERT FAILED"<<endl;
                                    cout<<nodes[0]<<"->"<<nodes[1]<<" and "<<nodes[1]<<"->"<<nodes[0]<<" cannot be true at the same time"<<endl;
                                    k = "CONFILICT DETECTED. INSERT FAILED\n";
                                    k.append(nodes[0]);
                                    k.append("->");
                                    k.append(nodes[1]);
                                    k.append(" and ");
                                    k.append(nodes[1]);
                                    k.append("->");
                                    k.append(nodes[0]);
                                    k.append(" cannot be true at the same time");
                                    sendBuf = (char*)k.c_str();
                                    k = k.append("\n");
                                    //int my_net_id = htonl(my_id);
                                    //send(sock,(const char*)&my_net_id, 4, 0);
                                    send(sock,sendBuf, k.size(), 0);
                                    //remove_edge
                                    total_status = false;
                                }
                            }
                            
                        }
                        string k;
                        char *sendBuf = NULL;
                        if(total_status == true){
                            cout<<"Insert DONE."<<endl;
                            k = "INSERT DONE";
                            sendBuf = (char*)k.c_str();
                            k = k.append("\n");
                            //int my_net_id = htonl(my_id);
                            //send(sock,(const char*)&my_net_id, 4, 0);
                            send(sock,sendBuf, k.size(), 0);
                            
                        }
                    }
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
                }
                else if(command_begin == query){

                    if(commands.size() >= 3){
                        int status=true;
                        int my_id,my_net_id;

                        status = query_edge(g, commands[1].c_str()[0], commands[2].c_str()[0], li);
                        cout<<"QUERY DONE."<<endl;
                        string k;
                        char *sendBuf = NULL;
                        if(status == 0){
                            k = commands[1].append(" happend before ");
                            k = k.append(commands[2]);
                            sendBuf = (char*)k.c_str();
                            
                        }
                        if(status == 1){
                            //my_id='D';
                            k = commands[2].append(" happend before ");
                            k = k.append(commands[1]);
                            sendBuf = (char*)k.c_str();
                        }
                        if(status == 2){
                            k = commands[1].append(" concurrent to ");
                            k = k.append(commands[2]);
                            sendBuf = (char*)k.c_str();
                        }
                        if(status == -1){
                            k = "Event not found: ";
                            k = k.append(commands[1]);
                            sendBuf = (char*)k.c_str();
                        }
                        if(status == -2){
                            k = "Event not found: ";
                            k = k.append(commands[2]);
                            sendBuf = (char*)k.c_str();
                        }
                        if(status == -3){
                            k = "Event not found: ";
                            k = k.append(commands[1]);
                            k = k.append(",");
                            k = k.append(commands[2]);
                            sendBuf = (char*)k.c_str();
                        }
                        if(status == -4){
                            k = "Query the same event: ";
                            k = k.append(commands[1]);
                            sendBuf = (char*)k.c_str();
                        }
                        if(status == -5){
                            k = "Server error...";
                            sendBuf = (char*)k.c_str();
                            my_net_id = htonl(my_id);
                        }
                        k = k.append("\n");
                        send(sock,sendBuf, k.size(), 0);
                        //send(sock,(const char*)&my_net_id, 4, 0);
                    }
                }
                else{
                    cout<<"null or invalid command"<<endl;
                }
                //transform(str.begin(),str.end(), str.begin(), tupper);
            }/**/
        }

        }
	}
	//return(0);
    
    
	//构造有N（N=6）个顶点的图
	//添加边到无向图g中
    add_edge_in(g,B,C,li);
    add_edge_in(g,B,F,li);
    add_edge_in(g,C,A,li);
    add_edge_in(g,F,A,li);
    add_edge_in(g,D,K,li);
    add_edge_in(g,A,K,li);
    add_edge_in(g,F,C,li);
    
    std::cout<<"TEST"<<std::endl;
    
    print_query_res(query_edge(g, B, C, li));
    print_query_res(query_edge(g, B, F, li));
    print_query_res(query_edge(g, F, B, li));
    print_query_res(query_edge(g, G, M, li));
    print_query_res(query_edge(g, B, K, li));
    print_query_res(query_edge(g, K, B, li));
    print_query_res(query_edge(g, A, D, li));
    print_query_res(query_edge(g, G, A, li));
    print_query_res(query_edge(g, A, G, li));
    
    
    
    
    
    
    
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
        std::cout<<"u->v"<<std::endl;
    }
    if(status == 1){
        std::cout<<"v->u"<<std::endl;
    }
    if(status == 2){
        std::cout<<"u | v"<<std::endl;
    }
    if(status == -1){
        std::cout<<"u not found"<<std::endl;
    }
    if(status == -2){
        std::cout<<"v not found"<<std::endl;
    }
    if(status == -3){
        std::cout<<"u, v not found"<<std::endl;
    }
}


int
setup_server() {
	struct sockaddr_in serv, remote;
	struct servent *se;
	int newsock;
    socklen_t len;
    
	len = sizeof(remote);
	memset((void *)&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	if (port == NULL)
		serv.sin_port = htons(0);
	else if (isdigit(*port))
		serv.sin_port = htons(atoi(port));
	else {
		if ((se = getservbyname(port, (char *)NULL)) < (struct servent *) 0) {
			perror(port);
			exit(1);
		}
		serv.sin_port = se->s_port;
	}
    
	if (::bind(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
		perror("bind");
		exit(1);
	}
    
    
    
	if (getsockname(s, (struct sockaddr *) &remote, &len) < 0) {
		perror("getsockname");
		exit(1);
	}
	fprintf(stderr, "Port number is %d\n", ntohs(remote.sin_port));
	listen(s, 1);
	newsock = s;
	if (soctype == SOCK_STREAM) {
		fprintf(stderr, "Entering accept() waiting for connection.\n");
		newsock = accept(s, (struct sockaddr *) &remote, &len);
	}
	return(newsock);
}


/*
 * setup_client() - set up socket for the mode of soc running as a
 *		client connecting to a port on a remote machine.
 */

int
setup_client() {
    
	struct hostent *hp, *gethostbyname();
	struct sockaddr_in serv;
	struct servent *se;
    
    /*
     * Look up name of remote machine, getting its address.
     */
	if ((hp = ::gethostbyname(host)) == NULL) {
		fprintf(stderr, "%s: %s unknown host\n", progname, host);
		exit(1);
	}
    /*
     * Set up the information needed for the socket to be bound to a socket on
     * a remote host.  Needs address family to use, the address of the remote
     * host (obtained above), and the port on the remote host to connect to.
     */
	serv.sin_family = AF_INET;
	memcpy(&serv.sin_addr, hp->h_addr, hp->h_length);
	if (isdigit(*port))
		serv.sin_port = htons(atoi(port));
	else {
		if ((se = getservbyname(port, (char *)NULL)) < (struct servent *) 0) {
			perror(port);
			exit(1);
		}
		serv.sin_port = se->s_port;
	}
    /*
     * Try to connect the sockets...
     */
	if (connect(s, (struct sockaddr *) &serv, sizeof(serv)) < 0) {
		perror("connect");
		exit(1);
	} else
		fprintf(stderr, "Connected...\n");
	return(s);
}

bool check_valid(int u, int v)
{
    if(u >= 65 && u <= 90 && v>=65 && v<= 90){
        return true;
    }
    return false;
}


/*
 * usage - print usage string and exit
 */

void
usage()
{
	fprintf(stderr, "usage: %s -h host -p port\n", progname);
	fprintf(stderr, "usage: %s -s [-p port]\n", progname);
	exit(1);
}
