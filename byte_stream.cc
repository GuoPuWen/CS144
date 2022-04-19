#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

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

size_t ByteStream::remaining_capacity() const { return _capacity - (_h - _l); }
