<<<<<<< HEAD
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
=======
# CS144
学习著名课程CS144时的代码和心得
>>>>>>> main
