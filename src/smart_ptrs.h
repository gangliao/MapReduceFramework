/**
    MIT License
    Copyright (c) 2016 Gang Liao <gangliao@gatech.edu>
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
   all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE
    SOFTWARE.
*/

#pragma once

#include <memory>
#include <type_traits>

using namespace std;

/**
 *  Create make_unique interface.
 */

/// For general pointer
template <class T, typename... Args>
inline typename enable_if<!is_array<T>::value, unique_ptr<T>>::type make_unique(
    Args&&... args) {
  return unique_ptr<T>(new T(forward<Args>(args)...));
}

/// For dynamic allocated array
template <class T>
inline typename enable_if<is_array<T>::value && extent<T>::value == 0,
                          unique_ptr<T>>::type
make_unique(size_t size) {
  using U = typename remove_extent<T>::type;
  return unique_ptr<T>(new U[size]());
}

/// Filter fixed-size array
template <class T, typename... Args>
inline typename enable_if<extent<T>::value != 0, void>::type make_unique(
    Args&&... args) = delete;
