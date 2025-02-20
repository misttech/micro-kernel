// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FBL_STRING_H_
#define FBL_STRING_H_

#include <zircon/compiler.h>

#include <atomic>
#include <initializer_list>
#include <string>
#include <string_view>

namespace fbl {
namespace tests {
struct StringTestHelper;
}  // namespace tests

// A string with immutable contents.
//
// fbl::String is designed to resemble std::string except that its content
// is immutable.  This makes it easy to share string buffers so that copying
// strings does not incur any allocation cost.
//
// Empty string objects do not incur any allocation.  Non-empty strings are
// stored on the heap.  Note that fbl::String does not have a null state
// distinct from the empty state.
//
// The content of a fbl::String object is always stored with a null terminator
// so that |c_str()| is fast.  However, be aware that the string may also contain
// embedded null characters (this is not checked by the implementation).
class __OWNER(char) String {
 public:
  // Creates an empty string.
  // Does not allocate heap memory.
  String() { InitWithEmpty(); }

  // Creates a copy of another string.
  // Does not allocate heap memory.
  String(const String& other) : data_(other.data_) { AcquireRef(data_); }

  // Move constructs from another string.
  // The other string is set to empty.
  // Does not allocate heap memory.
  String(String&& other) : data_(other.data_) { other.InitWithEmpty(); }

  // Creates a string from the contents of a null-terminated C string.
  // Allocates heap memory only if |data| is non-empty.
  // |data| must not be null.
  String(const char* data) { Init(data, std::string_view(data).size()); }

  // Creates a string from the contents of a character array of given length.
  // Allocates heap memory only if |length| is non-zero.
  // |data| must not be null.
  String(const char* data, size_t length) { Init(data, length); }

  // Creates a string with |count| copies of |ch|.
  // Allocates heap memory only if |count| is non-zero.
  String(size_t count, char ch) { Init(count, ch); }

  // Creates a string from the contents of a string view.
  // Allocates heap memory only if |view.length()| is non-zero.
  String(std::string_view view) : String(view.data(), view.length()) {}

  // Creates a string from the contents of a string.
  // Allocates heap memory only if |str.length()| is non-zero.
  String(const std::string& str) : String(str.data(), str.length()) {}

  // Destroys the string.
  ~String() { ReleaseRef(data_); }

  // Returns a pointer to the null-terminated contents of the string.
  const char* data() const { return data_; }
  const char* c_str() const { return data(); }

  // Returns the length of the string, excluding its null terminator.
  size_t length() const { return *length_field_of(data_); }
  size_t size() const { return length(); }

  // Returns true if the string's length is zero.
  bool empty() const { return length() == 0U; }

  // Character iterators, excluding the null terminator.
  const char* begin() const { return data(); }
  const char* cbegin() const { return data(); }
  const char* end() const { return data() + length(); }
  const char* cend() const { return data() + length(); }

  // Gets the character at the specified index.
  // Position must be greater than or equal to 0 and less than |length()|.
  const char& operator[](size_t pos) const { return data()[pos]; }

  // Performs a lexicographical character by character comparison.
  // Returns a negative value if |*this| comes before |other| in lexicographical order.
  // Returns zero if the strings are equivalent.
  // Returns a positive value if |*this| comes after |other| in lexicographical order.
  int compare(const String& other) const;

  // Sets this string to empty.
  // Does not allocate heap memory.
  void clear();

  // Swaps the contents of this string with another string.
  // Does not allocate heap memory.
  void swap(String& other);

  // Assigns this string to a copy of another string.
  // Does not allocate heap memory.
  String& operator=(const String& other);

  // Move assigns from another string.
  // The other string is set to empty.
  // Does not allocate heap memory.
  String& operator=(String&& other) noexcept;

  // Assigns this string from the contents of a null-terminated C string.
  // Allocates heap memory only if |data| is non-empty.
  // |data| must not be null.
  String& operator=(const char* data) {
    Set(data, std::string_view(data).size());
    return *this;
  }

  // Assigns this string from the contents of a string view.
  // Allocates heap memory only if |view.length()| is non-zero.
  String& operator=(std::string_view view) {
    Set(view.data(), view.length());
    return *this;
  }

  // Assigns this string from the contents of a string.
  // Allocates heap memory only if |str.length()| is non-zero.
  String& operator=(const std::string& str) {
    Set(str.data(), str.length());
    return *this;
  }

  // Create a string view backed by the string.
  // The view does not take ownership of the data so the string
  // must outlast the string view.
  //
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator std::string_view() const { return {data(), length()}; }

  // Concatenates the specified strings.
  static String Concat(std::initializer_list<String> strings);

 private:
  friend struct fbl::tests::StringTestHelper;

  explicit String(char* data, decltype(nullptr) /*overload disambiguation*/) : data_(data) {}

  // Assigns this string from the contents of a character array of given length.
  // Allocates heap memory only if |length| is non-zero.
  // |data| must not be null.
  void Set(const char* data, size_t length);

  // A string buffer consists of a length followed by a reference count
  // followed by a null-terminated string.  To make access faster, we offset
  // the |data_| pointer to point at the first byte of the content instead of
  // at the beginning of the string buffer itself.
  static constexpr size_t kLengthFieldOffset = 0U;
  static constexpr size_t kRefCountFieldOffset = sizeof(size_t);
  static constexpr size_t kDataFieldOffset = sizeof(size_t) + sizeof(std::atomic_uint);

  static size_t* length_field_of(char* data) {
    return reinterpret_cast<size_t*>(data - kDataFieldOffset + kLengthFieldOffset);
  }
  static std::atomic_uint* ref_count_field_of(char* data) {
    return reinterpret_cast<std::atomic_uint*>(data - kDataFieldOffset + kRefCountFieldOffset);
  }
  static constexpr size_t buffer_size(size_t length) { return kDataFieldOffset + length + 1U; }

  // For use by test code only.
  unsigned int ref_count() const {
    return ref_count_field_of(data_)->load(std::memory_order_relaxed);
  }

  // Storage for an empty string.
  struct EmptyBuffer {
    size_t length{0U};
    std::atomic_uint ref_count{1U};
    char nul{0};
  };
  static_assert(offsetof(EmptyBuffer, length) == kLengthFieldOffset, "");
  static_assert(offsetof(EmptyBuffer, ref_count) == kRefCountFieldOffset, "");
  static_assert(offsetof(EmptyBuffer, nul) == kDataFieldOffset, "");

  static EmptyBuffer gEmpty;

  void Init(const char* data, size_t length);
  void Init(size_t count, char ch);
  void InitWithEmpty();

  static char* AllocData(size_t length);
  static char* InitData(void* buffer, size_t length);
  static void AcquireRef(char* data);
  static void ReleaseRef(char* data);

  char* data_;
};

bool operator==(const String& lhs, const String& rhs);

inline bool operator!=(const String& lhs, const String& rhs) { return !(lhs == rhs); }

inline bool operator<(const String& lhs, const String& rhs) { return lhs.compare(rhs) < 0; }

inline bool operator>(const String& lhs, const String& rhs) { return lhs.compare(rhs) > 0; }

inline bool operator<=(const String& lhs, const String& rhs) { return lhs.compare(rhs) <= 0; }

inline bool operator>=(const String& lhs, const String& rhs) { return lhs.compare(rhs) >= 0; }

inline std::ostream& operator<<(std::ostream& stream, const fbl::String& string) {
  return stream << std::string_view(string);
}

}  // namespace fbl

#endif  // FBL_STRING_H_
