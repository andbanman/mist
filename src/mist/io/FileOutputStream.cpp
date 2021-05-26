#include "cerrno"
#include "cstring"
#include "iostream"

#include "io/FileOutputStream.hpp"
#include <exception>

using namespace mist;
using namespace mist::io;

void FileOutputStream::init() {
    if (!file || !file->is_open())
        throw FileOutputStreamException("init", "Could not open file '" + filename + "' for wrtiting: " + std::strerror(errno));
    if (!buffer)
        throw FileOutputStreamException("init", std::string("Could not allocate write buffer: ") + std::strerror(errno));
    if (!double_strbuf.size())
        throw FileOutputStreamException("init", std::string("Could not allocate double-to-string buffer: ") + std::strerror(errno));
}

//XXX: speed of string conversion has large performance impact
// snprintf with pre-allocated buffer faster than sstream and boost::lexical_cast
inline std::string double_to_string_fast(FileOutputStream::buffer_type buff, double v) {
    std::string ret; // declare return to encourage RVO
    snprintf(buff.data(), DOUBLE_BUFFER_MAX_SIZE-1, "%g", v);
    buff.data()[DOUBLE_BUFFER_MAX_SIZE-1] = '\0';
    ret = buff.data();
    return ret;
}

void FileOutputStream::buffered_write(std::string const& ss) {
    auto len = ss.length();
    if (buffer_cur_size + len >= buffer_max_size) {
        std::unique_lock<mutex_type> lock(*this->m.get());
        file->write(buffer->data(), buffer_cur_size);
        file->write(ss.data(), len);
        buffer_cur_size = 0;
    } else {
        std::copy(ss.c_str(), ss.c_str()+len, buffer->data() + buffer_cur_size);
        buffer_cur_size += len;
   }
}

void FileOutputStream::direct_write(std::string const& ss) {
    std::unique_lock<mutex_type> lock(*this->m.get());
    file->write(ss.data(), ss.size());
}

//TODO constructor delegation
FileOutputStream::FileOutputStream(std::string const& filename) :
    OutputStream(mutex_ptr(new mutex_type)),
    file(file_ptr(new file_type(filename))),
    buffer(buffer_ptr(new buffer_type(BUFFER_MAX_SIZE_DEFAULT))),
    double_strbuf(DOUBLE_BUFFER_MAX_SIZE),
    buffer_max_size(BUFFER_MAX_SIZE_DEFAULT),
    buffer_cur_size(0),
    filename(filename),
    header("")
{
    init();
}

FileOutputStream::FileOutputStream(std::string const& filename, size_type buffer_max_size) :
    OutputStream(mutex_ptr(new mutex_type)),
    file(file_ptr(new file_type(filename))),
    buffer(buffer_ptr(new buffer_type(buffer_max_size))),
    double_strbuf(DOUBLE_BUFFER_MAX_SIZE),
    buffer_max_size(BUFFER_MAX_SIZE_DEFAULT),
    buffer_cur_size(0),
    filename(filename),
    header("")
{
    init();
}

FileOutputStream::FileOutputStream(FileOutputStream const& other) :
    OutputStream(other.m),
    file(other.file),
    buffer(buffer_ptr(new buffer_type(other.buffer_max_size))),
    double_strbuf(other.double_strbuf.size()),
    buffer_max_size(other.buffer_max_size),
    buffer_cur_size(0),
    filename(other.filename),
    header("")
{
    init();
}

FileOutputStream::FileOutputStream(std::string const& filename, std::string const& header) :
    OutputStream(mutex_ptr(new mutex_type)),
    file(file_ptr(new file_type(filename))),
    buffer(buffer_ptr(new buffer_type(BUFFER_MAX_SIZE_DEFAULT))),
    double_strbuf(DOUBLE_BUFFER_MAX_SIZE),
    buffer_max_size(BUFFER_MAX_SIZE_DEFAULT),
    buffer_cur_size(0),
    filename(filename),
    header(header)
{
    init();
    direct_write(header + "\n");
}

FileOutputStream::FileOutputStream(std::string const& filename, std::string const& header, size_type buffer_max_size) :
    OutputStream(mutex_ptr(new mutex_type)),
    file(file_ptr(new file_type(filename))),
    buffer(buffer_ptr(new buffer_type(buffer_max_size))),
    double_strbuf(DOUBLE_BUFFER_MAX_SIZE),
    buffer_max_size(BUFFER_MAX_SIZE_DEFAULT),
    buffer_cur_size(0),
    filename(filename),
    header(header)
{
    init();
    direct_write(header + "\n");
}

FileOutputStream::~FileOutputStream() {
    if (buffer_cur_size) {
        try {
            std::unique_lock<mutex_type> lock(*this->m.get());
        } catch(std::exception &e) {
            std::cerr << "FileOutputStream::~FileOutputStream: Error: Could not acquire lock, file '" + filename + "' may be incomplete.\n";
            return;
        }
        try {
            file->write(buffer->data(), buffer_cur_size);
        } catch (std::exception &e) {
            std::cerr << "FileOutputStream::~FileOutputStream: Error: Could not write out buffers, file '" + filename + "' may be incomplete.\n";
        }
    }
    if (file.use_count() == 1) {
        try {
            file->flush();
            file->close();
        } catch (std::exception &e) {
            std::cerr << "FileOutputStream::~FileOutputStream: Error closing file '" + filename + "'.\n";
        }
    }
}

void FileOutputStream::push(tuple_type const& tuple, result_type const& result) {
    std::string ss = "";
    for (auto t : tuple)
        ss += std::to_string(t) + ",";
    for (auto it = result.begin(); it < result.end() - 1; it++)
        ss += double_to_string_fast(double_strbuf, *it) + ",";
    ss += double_to_string_fast(double_strbuf, result.back()) + "\n";
    this->buffered_write(ss);
}

std::string FileOutputStream::get_filename() {
    return this->filename;
}
