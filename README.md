本仓库为cs144 lab实验代码，同时对各个lab的内容理解。cs144是讲解TCP/IP协议栈的，在这些lab中最后将会使用C++循序渐进的搭建出整个TCP/IP协议栈，以及实现路由IP及ARP协议，非常的硬核。目前已经做到lab0、lab1 

# lab0

lab0需要做的就两个部分，webget和ByteStream。在开头部分，介绍了一些使用C++的语言规范

- Use the language documentation at https://en.cppreference.com as a resource.
- Never use malloc() or free().
- Never use new or delete.
- Essentially never use raw pointers (*), and use “smart” pointers (unique ptr or shared ptr) only when necessary. (You will not need to use these in CS144.)
- Avoid templates, threads, locks, and virtual functions. (You will not need to use these in CS144.)
- Avoid C-style strings (char *str) or string functions (strlen(), strcpy()). These are pretty error-prone. Use a std::string instead.
- Never use C-style casts (e.g., (FILE *)x). Use a C++ static cast if you have to (you generally will not need this in CS144).
- Prefer passing function arguments by const reference (e.g.: const Address & address).
- Make every variable const unless it needs to be mutated.
- Make every method const unless it needs to mutate the object.
- Avoid global variables, and give every variable the smallest scope possible.
- Before handing in an assignment, please run make format to normalize the coding style.

### webget

如它的名字一样，webget获取到一个网页，使用LInux自带的connect函数(lab0中对其做了一个封装)获取到指定host的网页内容，lab0中提出需要注意的：

- Please note that in HTTP, each line must be ended with “\r\n” (it’s not sufficientto use just “\n” or endl).

- Don’t forget to include the “Connection: close” line in your client’s request. This tells the server that it shouldn’t wait around for your client to send any more requests after this one. Instead, the server will send one reply and then will immediately end its outgoing bytestream (the one from the server’s socket to your socket). You’ll discover that your incoming byte stream has ended because your socket will reach “EOF” (end of file) when you have read the entire byte stream coming from the server. That’s how your client will know that the server has finished its reply.

- Make sure to read and print all the output from the server until the socket reaches “EOF” (end of file)—a single call to read is not enough.

  • We expect you’ll need to write about ten lines of code.

代码如下：

```c
void get_URL(const string &host, const string &path) {


    TCPSocket socket1;
    socket1.connect(Address(host, "http"));
    socket1.write("GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n\r\n");
    socket1.shutdown(SHUT_WR);
    while(!socket1.eof())
        std::cout << socket1.read();
    socket1.close();

    cerr << "Function called: get_URL(" << host << ", " << path << ").\n";
    cerr << "Warning: get_URL() has not been implemented yet.\n";
}
```

### An in-memory reliable byte stream

本节需要做的是做一个ByteStram，其实就是说使用一个数据结构做一个缓冲区，说到这大家应该明白了。这里就是数据结构的选取，可以使用deque双端队列。我采取的办法是使用string并且使用双指针的方式，这样效率更高

`ByteStream.hh`

```c
#ifndef SPONGE_LIBSPONGE_BYTE_STREAM_HH
#define SPONGE_LIBSPONGE_BYTE_STREAM_HH

#include <string>

class ByteStream {
  private:
    std::string _output;
    size_t _l, _h;
    size_t _capacity;
    bool _end;

    bool _error{};  //!< Flag indicating that the stream suffered an error.

  public:
    //! Construct a stream with room for `capacity` bytes.
    ByteStream(const size_t capacity);
    size_t write(const std::string &data);
    //! \returns the number of additional bytes that the stream has space for
    size_t remaining_capacity() const;

    //! Signal that the byte stream has reached its ending
    void end_input();

    //! Indicate that the stream suffered an error.
    void set_error() { _error = true; }
    //!@}

    //! \name "Output" interface for the reader
    //!@{

    //! Peek at next "len" bytes of the stream
    //! \returns a string
    std::string peek_output(const size_t len) const;

    //! Remove bytes from the buffer
    void pop_output(const size_t len);

    //! Read (i.e., copy and then pop) the next "len" bytes of the stream
    //! \returns a string
    std::string read(const size_t len);

    //! \returns `true` if the stream input has ended
    bool input_ended() const;

    //! \returns `true` if the stream has suffered an error
    bool error() const { return _error; }

    //! \returns the maximum amount that can currently be read from the stream
    size_t buffer_size() const;

    //! \returns `true` if the buffer is empty
    bool buffer_empty() const;

    //! \returns `true` if the output has reached the ending
    bool eof() const;
    //!@}

    //! \name General accounting
    //!@{

    //! Total number of bytes written
    size_t bytes_written() const;

    //! Total number of bytes popped
    size_t bytes_read() const;
    //!@}
};

#endif  // SPONGE_LIBSPONGE_BYTE_STREAM_HH
```

`byte_stream.cc`

```c
#include "byte_stream.hh"


template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _output(""), _l(0), _h(0), _capacity(capacity), _end(false) {
    DUMMY_CODE(capacity);
}

size_t ByteStream::write(const string &data) {
    DUMMY_CODE(data);
    size_t _len = std::min(data.length(), _capacity - (_h - _l));
    _output += data.substr(0, _len);
    _h += _len;
    return _len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    DUMMY_CODE(len);
    size_t _len = std::min(len, _h - _l);
    return _output.substr(_l, _len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    DUMMY_CODE(len);
    _l += std::min(len, _h - _l);
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    DUMMY_CODE(len);
    size_t _len = std::min(len, _h - _l);
    size_t _L = _l;
    _l += _len;
    return _output.substr(_L, _len);
}

void ByteStream::end_input() { _end = true; }

bool ByteStream::input_ended() const { return _end; }

size_t ByteStream::buffer_size() const { return _h - _l; }

bool ByteStream::buffer_empty() const { return _h == _l; }

bool ByteStream::eof() const { return _end && _h == _l; }

size_t ByteStream::bytes_written() const { return _h; }

size_t ByteStream::bytes_read() const { return _l; }

size_t ByteStream::remaining_capacity() const { return _capacity - (_h - _l); 
}

```

### 测试

![image-20220420111239371](http://cdn.noteblogs.cn/image-20220420111239371.png)

# lab1

从lab1开始要自己实现一个TCP/IP，分别有如下几个模块：

![image-20220422222916592](http://cdn.noteblogs.cn/image-20220422222916592.png)

在lab1阶段，需要我们做的是SteamReassembler，就是一个流重组器，在不可靠的网络传输中，对于到达TCPReceiver来说，这些数据报可能会出现乱序、重复、丢失，而SteamReassembler需要做的就是将这些数据报重新整成有序的不重复的字节流并且写入到lab0中已经实现过的ByteStream中，需要实现push_substring方法

lab1中也提到了capacity的概念，按照理解capacity应该是两者之和：

- 已经在_output中的数据，即已经是重组好排好序的数据
- 为重组的数据

### 思路

我使用一个struct存储一个碎片的data、index、length并且重写<运算符，那么我们需要做的就是将这些乱序、重复的碎片整成有序的数据，使用set存储这些碎片，并且写merge_node方法用于将set中的两个Node合并起来，具体看代码：

```c
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
```

### 如何调试

本lab最重要的就是如何调试，使用lab实验中已经写好的lab测试框架可以，具体只需要找到对应test文件夹的源代码，然后使用gdb进行调试即可。当然我是自己写了一些代码去进行自己的调试：

```c
#include <iostream>
#include "stream_reassembler.hh"
#include "byte_stream.hh"
#include <set>

using namespace std;

struct Node {
    size_t size = 0;
    size_t index = 0;
    std::string data = "";
    bool operator<(const Node &_node) const { return index - _node.index;}
};

int main(void) {
    StreamReassembler stream(1000);
    stream.push_substring("b", 1, false);
    stream.push_substring("d", 3, false);
    stream.push_substring("c", 2, false);
    stream.push_substring("a", 0, false);
    cout << stream.unassembled_bytes() << endl;
    // stream.push_substring("a", 0, false);
    // stream.push_substring("cdefg", 2, false);

    ByteStream output = stream.stream_out();
    cout << output.read(4) << endl;
}
```

```c
g++ MyTest.cc -o MyTest stream_reassembler.cc byte_stream.cc
gdb MyTest
```

### 一个bug

在这次代码编写中，我找了比较久的一个bug是出现在struct结构运算符的重载上(不过按照C++的编码习惯这里应该是需要写一个class类去表示的)，具体问题是我原本结构体是这样写的

```c
#include <iostream>
#include "stream_reassembler.hh"
#include "byte_stream.hh"
#include <set>

using namespace std;


struct Node {
    size_t size = 0;
    size_t index = 0;
    std::string data = "";
    bool operator<(const Node &_node) const { return index - _node.index;}
};

int main(void) {
    Node node2 = {1, 1, "b"};
    Node node1 = {2, 2, "cd"};
    if(node1 < node2) cout << "node1!" << endl;
    if(node2 < node1) cout << "node2!" << endl;
}
```

输出：

```c
cs144@cs144vm:~/sponge/libsponge$ ./MyTest 
node1!
node2!
```

这就超级奇怪了，在结构体中使用重载运算符没有起作用吗，后来发现是 index - _node.index出了问题，如果使用<则不会出现问题

```c
#include <iostream>
#include "stream_reassembler.hh"
#include "byte_stream.hh"
#include <set>

using namespace std;

struct Node {
    size_t size = 0;
    size_t index = 0;
    std::string data = "";
    bool operator<(const Node &_node) const { return index < _node.index;}	//看这里
};

int main(void) {
    Node node2 = {1, 1, "b"};
    Node node1 = {2, 2, "cd"};
    if(node1 < node2) cout << "node1!" << endl;
    if(node2 < node1) cout << "node2!" << endl;
}
```

输出：

```c
cs144@cs144vm:~/sponge/libsponge$ ./MyTest 
node2!
```

### 测试

![image-20220422230029041](http://cdn.noteblogs.cn/image-20220422230029041.png)

14没有过可能是网络的问题

