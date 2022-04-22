#include <iostream>
#include "stream_reassembler.hh"
#include "byte_stream.hh"
#include <set>

using namespace std;


struct Node {
    size_t size = 0;
    size_t index = 0;
    std::string data = "";
    bool operator<(const Node &_node) const { return index < _node.index;}
};

int main(void) {
    // StreamReassembler stream(1000);
    // stream.push_substring("b", 1, false);
    // stream.push_substring("d", 3, false);
    // stream.push_substring("c", 2, false);
    // stream.push_substring("a", 0, false);
    // cout << stream.unassembled_bytes() << endl;
    // stream.push_substring("a", 0, false);

    // stream.push_substring("cdefg", 2, false);

    // ByteStream output = stream.stream_out();
    // cout << output.read(4) << endl;
    Node node2 = {1, 1, "b"};
    Node node1 = {2, 2, "cd"};
    if(node1 < node2) cout << "node1!" << endl;
    if(node2 < node1) cout << "node2!" << endl;
}