//FileName:adj_matrix_demo.cpp
//演示邻接矩阵adjacency_matrix的基本应用

#pragma warning(disable : 4800 4819)

#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/breadth_first_search.hpp>
/* for test only */
//#include <boost/graph/graph_utility.hpp>


//用于print_vertices(), print_edges()和print_graph();
using namespace boost;
using namespace std;
typedef adjacency_matrix<directedS> Graph;
bool should_we_add(Graph &G, int u, int v);
void print_query_res(int status);

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
			cout << p->data << ' ';
			p = p->next;
		}
		cout << endl;
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


bool add_edge_in(Graph &G, int u, int v,List<int> &li);
int query_edge(Graph &G, int u, int v, List<int> &li);

int main()
{
	enum { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y ,Z, TOTAL };
	//枚举常量，A…F用作顶点描述器
    
    /* For test only
	//根据枚举量的性质，N == 6
	const char* name = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
     */
	//6个顶点的名称属性
    List<int> li;
    
	//有向图
	typedef adjacency_matrix<directedS> Graph;
    
	Graph g(TOTAL);
	//构造有N（N=6）个顶点的图
	//添加边到无向图g中
    add_edge_in(g,B,C,li);
    add_edge_in(g,B,F,li);
    add_edge_in(g,C,A,li);
    add_edge_in(g,F,A,li);
    add_edge_in(g,D,K,li);
    add_edge_in(g,A,K,li);
    add_edge_in(g,F,C,li);
    
    cout<<"TEST"<<endl;
    
    print_query_res(query_edge(g, B, C, li));
    print_query_res(query_edge(g, B, F, li));
    print_query_res(query_edge(g, F, B, li));
    print_query_res(query_edge(g, G, M, li));
    print_query_res(query_edge(g, B, K, li));
    print_query_res(query_edge(g, K, B, li));
    print_query_res(query_edge(g, A, D, li));
    print_query_res(query_edge(g, G, A, li));
    print_query_res(query_edge(g, A, G, li));



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


bool should_we_add(Graph &G, int u, int v){
    
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

bool add_edge_in(Graph &G, int u, int v,List<int> &li){
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
 */
int query_edge(Graph &G, int u, int v, List<int> &li){
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


