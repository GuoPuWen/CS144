#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : 
   _nodes({}), _unassembled_byte(0), _head_index(0), _end_flag(false), _output(capacity), _capacity(capacity)
 {}

long StreamReassembler::mergeNode(Node &node1, const Node &node2) {
    Node x = node1.index < node2.index ? node1 : node2;
    Node y = node2.index > node1.index ? node2 : node1;
    if(x.index + x.size < y.index) {
        return -1;
    }else if(y.index + y.size <= x.index + x.size) {
        node1 = x;
        return y.size;
    }else {
        size_t offset = x.index + x.size - y.index;
        x.data += y.data.substr(offset);
        x.size += y.size - offset;
        node1 = x;
        return offset;
    }
} 

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    DUMMY_CODE(data, index, eof);
    _end_flag = _end_flag | eof;
    if(data.size() > _capacity) _end_flag = false;
    if( index + data.size() <= _head_index) {
        if(_end_flag) {
            _output.end_input();
        }
    }
    //index 超出capacity
    // if() return;

    //========= 组装Node 节点 =========
    Node node;
    //index 在当前_head_index后面 丢弃
    if(index + data.length() <= _head_index) {
        if(_end_flag && empty()) {
            _output.end_input();
        }
        return;
    }
    //index 在_head_index 前面(=顺序到达 > 超前到达)
    else if(index >= _head_index) {
        node.data = data;
        node.index = index;
        node.size = data.length();
    }
    //当前index 有部分重复 需要丢弃部分   index + data.length() > _head_index
    else {
        size_t offset = _head_index - index;
        node.data = data.substr(offset);
        node.index = _head_index;
        node.size = data.length() - offset;
    }
    _unassembled_byte += node.size;

    //========= 开始合并到set中 合并后面 =========
    long merge_size = 0;
    auto iter = _nodes.lower_bound(node);
    while(iter != _nodes.end() && (merge_size = mergeNode(node, *iter)) >= 0) {
        _unassembled_byte -= merge_size;
        //从set中移除到被合并的
        _nodes.erase(iter);
        iter = _nodes.lower_bound(node);
    }
    if(iter != _nodes.begin()) {
        iter--;
        while((merge_size = mergeNode(node, *iter)) >= 0) {
            _unassembled_byte -= merge_size;
            //从set中移除到被合并的
            _nodes.erase(iter);
            iter = _nodes.lower_bound(node);
            if(iter == _nodes.begin()) {        //注意这里判断
                break;
            }
            iter--;
        }
    }


    _nodes.insert(node);
    
    //========= 组装输出流 =========
    if(!_nodes.empty() && _nodes.begin()->index == _head_index) {
        const Node temp = *_nodes.begin();
        size_t write_sizes = _output.write(temp.data);

        _head_index += write_sizes;
        _unassembled_byte -= write_sizes;
        _nodes.erase(_nodes.begin());
    }   

    if(empty() && _end_flag) {
        _output.end_input();
    }

}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_byte; }

bool StreamReassembler::empty() const { return _unassembled_byte == 0; }