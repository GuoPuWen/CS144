#include <iostream>
#include <set>
#include <algorithm>
#include "stream_reassembler.hh"
#include "byte_stream.hh"

using namespace std;


// struct Node {
//     size_t size = 0;
//     size_t index = 0;
//     std::string data = "";
//     // bool operator<=(const Node &_node) const { return index > _node.index;}
//     bool operator<(const Node &_node) const { return index < _node.index;}
// };

// bool Node::operator<(const Node& _node1, const Node &_node2) { return _node1.index - _node2.index;}

int main(void){

    StreamReassembler::Node node1 = {1, 1, "b"};
    StreamReassembler::Node node2 = {2, 2, "cd"};

     StreamReassembler stream(1000);
     stream.mergeNode(node1, node2);
     cout << node1.data << endl;
    
    // cout << node1.index << node1.size << node1.data << endl;
    // Node node2 = {1, 6, "abc"};
    // cout << node2.index << node2.size << node2.data << endl;
    // set<Node> _nodes = {node1,node2};
    // if(node1 < node2) {
    //     cout << "11"<< endl;
    // }
    // auto iter = _nodes.lower_bound(node3);
    // if(iter != _nodes.end()) {
    //     cout << (*iter).data << endl;
    // }
    
    
}

// set::lower_bound/upper_bound
// #include <iostream>
// #include <set>

// int main ()
// {
//   std::set<int> myset;
//   std::set<int>::iterator itlow,itup;

//   for (int i=1; i<10; i++) myset.insert(i*10); // 10 20 30 40 50 60 70 80 90

//   itlow=myset.lower_bound (9);                //       ^
//   itup=myset.upper_bound (60);                 //                   ^
// std::cout << *itlow << endl;
//   myset.erase(itlow,itup);                     // 10 20 70 80 90

//   std::cout << "myset contains:";
//   for (std::set<int>::iterator it=myset.begin(); it!=myset.end(); ++it)
//     std::cout << ' ' << *it;
//   std::cout << '\n';

//   return 0;
// }