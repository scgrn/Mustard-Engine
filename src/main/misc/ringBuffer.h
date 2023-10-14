/**

zlib License

(C) 2021 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#ifndef AB_RING_BUFFER_H
#define AB_RING_BUFFER_H

#include <memory>
#include <mutex>

namespace AB {

//    adapted from https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
//  https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/circular_buffer.cpp

template <class T>
class RingBuffer {
    public:
        explicit RingBuffer(size_t size) :
            buf_(std::unique_ptr<T[]>(new T[size])),
            max_size_(size) {

        }

        void put(T item) {
            std::lock_guard<std::mutex> lock(mutex_);

            buf_[head_] = item;

            if (full_) {
                tail_ = (tail_ + 1) % max_size_;
            }

            head_ = (head_ + 1) % max_size_;

            full_ = head_ == tail_;
        }

        T get() {
            std::lock_guard<std::mutex> lock(mutex_);

            if (empty()) {
                return T();
            }

            //Read data and advance the tail (we now have a free space)
            auto val = buf_[tail_];
            full_ = false;
            tail_ = (tail_ + 1) % max_size_;

            return val;
        }

        void reset() {
            std::lock_guard<std::mutex> lock(mutex_);
            head_ = tail_;
            full_ = false;
        }

        bool empty() const {
            //if head and tail are equal, we are empty
            return (!full_ && (head_ == tail_));
        }

        bool full() const {
            //If tail is ahead the head by 1, we are full
            return full_;
        }

        size_t capacity() const {
            return max_size_;
        }

        size_t size() const {
            size_t size = max_size_;

            if (!full_) {
                if (head_ >= tail_) {
                    size = head_ - tail_;
                } else {
                    size = max_size_ + head_ - tail_;
                }
            }

            return size;
        }

    private:
        std::mutex mutex_;
        std::unique_ptr<T[]> buf_;
        size_t head_ = 0;
        size_t tail_ = 0;
        const size_t max_size_;
        bool full_ = 0;
};

}

#endif
