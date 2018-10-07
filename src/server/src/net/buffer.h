/*
 * Copyright (C) 2018
 */

#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include <endian.h>

#include <cassert>
#include <cstring>

namespace net
{
	class Buffer
	{
		public:
  static const size_t kCheap_prepend = 8;
  static const size_t kInitial_size = 1024;

  explicit Buffer(size_t initial_size = kInitial_size)
    : buffer_(kCheap_prepend + initial_size),
      reader_index_(kCheap_prepend),
      writer_index_(kCheap_prepend)
  {
    assert(readable_bytes() == 0);
    assert(writable_bytes() == initial_size);
    assert(prependable_bytes() == kCheap_prepend);
  }

  // implicit copy-ctor, move-ctor, dtor and assignment are fine
  // NOTE: implicit move-ctor is added in g++ 4.6

  void swap(Buffer& rhs)
  {
    buffer_.swap(rhs.buffer_);
    std::swap(reader_index_, rhs.reader_index_);
    std::swap(writer_index_, rhs.writer_index_);
  }

  size_t readable_bytes() const
  { return writer_index_ - reader_index_; }

  size_t writable_bytes() const
  { return buffer_.size() - writer_index_; }

  size_t prependable_bytes() const
  { return reader_index_; }

  const char* peek() const
  { return begin() + reader_index_; }

  const char* findCRLF() const
  {
    // FIXME: replace with memmem()?
    const char* crlf = std::search(peek(), begin_write(), kCRLF, kCRLF+2);
    return crlf == begin_write() ? NULL : crlf;
  }

  const char* findCRLF(const char* start) const
  {
    assert(peek() <= start);
    assert(start <= begin_write());
    // FIXME: replace with memmem()?
    const char* crlf = std::search(start, begin_write(), kCRLF, kCRLF+2);
    return crlf == begin_write() ? NULL : crlf;
  }

  const char* findEOL() const
  {
    const void* eol = memchr(peek(), '\n', readable_bytes());
    return static_cast<const char*>(eol);
  }

  const char* findEOL(const char* start) const
  {
    assert(peek() <= start);
    assert(start <= begin_write());
    const void* eol = memchr(start, '\n', begin_write() - start);
    return static_cast<const char*>(eol);
  }

  // retrieve returns void, to prevent
  // string str(retrieve(readable_bytes()), readable_bytes());
  // the evaluation of two functions are unspecified
  void retrieve(size_t len)
  {
    assert(len <= readable_bytes());
    if (len < readable_bytes())
    {
      reader_index_ += len;
    }
    else
    {
      retrieve_all();
    }
  }

  void retrieve_until(const char* end)
  {
    assert(peek() <= end);
    assert(end <= begin_write());
    retrieve(end - peek());
  }

  void retrieveInt64()
  {
    retrieve(sizeof(int64_t));
  }

  void retrieveInt32()
  {
    retrieve(sizeof(int32_t));
  }

  void retrieveInt16()
  {
    retrieve(sizeof(int16_t));
  }

  void retrieveInt8()
  {
    retrieve(sizeof(int8_t));
  }

  void retrieve_all()
  {
    reader_index_ = kCheap_prepend;
    writer_index_ = kCheap_prepend;
  }

  std::string retrieve_all_as_string()
  {
    return retrieve_as_string(readable_bytes());
  }

  std::string retrieve_as_string(size_t len)
  {
    assert(len <= readable_bytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
  }

  std::string toStringPiece() const
  {
    return std::string(peek(), static_cast<int>(readable_bytes()));
  }

  void append(const std::string& str)
  {
    append(str.data(), str.size());
  }

  void append(const char* /*restrict*/ data, size_t len)
  {
    ensureWritableBytes(len);
    std::copy(data, data+len, begin_write());
    hasWritten(len);
  }

  void append(const void* /*restrict*/ data, size_t len)
  {
    append(static_cast<const char*>(data), len);
  }

  void ensureWritableBytes(size_t len)
  {
    if (writable_bytes() < len)
    {
      makeSpace(len);
    }
    assert(writable_bytes() >= len);
  }

  char* begin_write()
  { return begin() + writer_index_; }

  const char* begin_write() const
  { return begin() + writer_index_; }

  void hasWritten(size_t len)
  {
    assert(len <= writable_bytes());
    writer_index_ += len;
  }

  void unwrite(size_t len)
  {
    assert(len <= readable_bytes());
    writer_index_ -= len;
  }

  ///
  /// Append int64_t using network endian
  ///
  void appendInt64(int64_t x)
  {
    int64_t be64 = htobe64(x);
    append(&be64, sizeof be64);
  }

  ///
  /// Append int32_t using network endian
  ///
  void appendInt32(int32_t x)
  {
    int32_t be32 = htobe32(x);
    append(&be32, sizeof be32);
  }

  void appendInt16(int16_t x)
  {
    int16_t be16 = htobe16(x);
    append(&be16, sizeof be16);
  }

  void appendInt8(int8_t x)
  {
    append(&x, sizeof x);
  }

  ///
  /// Read int64_t from network endian
  ///
  /// Require: buf->readable_bytes() >= sizeof(int32_t)
  int64_t readInt64()
  {
    int64_t result = peekInt64();
    retrieveInt64();
    return result;
  }

  ///
  /// Read int32_t from network endian
  ///
  /// Require: buf->readable_bytes() >= sizeof(int32_t)
  int32_t readInt32()
  {
    int32_t result = peekInt32();
    retrieveInt32();
    return result;
  }

  int16_t readInt16()
  {
    int16_t result = peekInt16();
    retrieveInt16();
    return result;
  }

  int8_t readInt8()
  {
    int8_t result = peekInt8();
    retrieveInt8();
    return result;
  }

  ///
  /// Peek int64_t from network endian
  ///
  /// Require: buf->readable_bytes() >= sizeof(int64_t)
  int64_t peekInt64() const
  {
    assert(readable_bytes() >= sizeof(int64_t));
    int64_t be64 = 0;
    ::memcpy(&be64, peek(), sizeof be64);
    return be64toh(be64);
  }

  ///
  /// Peek int32_t from network endian
  ///
  /// Require: buf->readable_bytes() >= sizeof(int32_t)
  int32_t peekInt32() const
  {
    assert(readable_bytes() >= sizeof(int32_t));
    int32_t be32 = 0;
    ::memcpy(&be32, peek(), sizeof be32);
    return be32toh(be32);
  }

  int16_t peekInt16() const
  {
    assert(readable_bytes() >= sizeof(int16_t));
    int16_t be16 = 0;
    ::memcpy(&be16, peek(), sizeof be16);
    return be16toh(be16);
  }

  int8_t peekInt8() const
  {
    assert(readable_bytes() >= sizeof(int8_t));
    int8_t x = *peek();
    return x;
  }

  ///
  /// Prepend int64_t using network endian
  ///
  void prependInt64(int64_t x)
  {
    int64_t be64 = htobe64(x);
    prepend(&be64, sizeof be64);
  }

  ///
  /// Prepend int32_t using network endian
  ///
  void prependInt32(int32_t x)
  {
    int32_t be32 = htobe32(x);
    prepend(&be32, sizeof be32);
  }

  void prependInt16(int16_t x)
  {
    int16_t be16 = htobe16(x);
    prepend(&be16, sizeof be16);
  }

  void prependInt8(int8_t x)
  {
    prepend(&x, sizeof x);
  }

  void prepend(const void* /*restrict*/ data, size_t len)
  {
    assert(len <= prependable_bytes());
    reader_index_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d+len, begin()+reader_index_);
  }

  void shrink(size_t reserve)
  {
    // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
    Buffer other;
    other.ensureWritableBytes(readable_bytes()+reserve);
    other.append(toStringPiece());
    swap(other);
  }

  size_t internalCapacity() const
  {
    return buffer_.capacity();
  }

  /// Read data directly into buffer.
  ///
  /// It may implement with readv(2)
  /// @return result of read(2), @c errno is saved
ssize_t readFd(int fd, int* savedErrno);
		private:
  char* begin()
  { return &*buffer_.begin(); }

  const char* begin() const
  { return &*buffer_.begin(); }

  void makeSpace(size_t len)
  {
    if (writable_bytes() + prependable_bytes() < len + kCheap_prepend)
    {
      // FIXME: move readable data
      buffer_.resize(writer_index_+len);
    }
    else
    {
      // move readable data to the front, make space inside buffer
      assert(kCheap_prepend < reader_index_);
      size_t readable = readable_bytes();
      std::copy(begin()+reader_index_,
                begin()+writer_index_,
                begin()+kCheap_prepend);
      reader_index_ = kCheap_prepend;
      writer_index_ = reader_index_ + readable;
      assert(readable == readable_bytes());
    }
}

  std::vector<char> buffer_;
  size_t reader_index_;
  size_t writer_index_;

static const char kCRLF[];
	};
}

#endif
