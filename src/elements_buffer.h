#pragma once

#include <stddef.h>
#include <algorithm>


namespace hwmx {
    template<typename T> void construct(T* p, const T& rhs) { new (p) T(rhs); }
    template<typename T> void destroy(T* p) noexcept { p->~T(); }

    template<typename FwdIter>
    void destroy(FwdIter first, FwdIter last) noexcept {
        while (first != last) {
            destroy(&*first);
            first++;
        }
    }


    template<typename T>
    class MemoryBuf {
    protected:
        T* data;
        size_t size, used = 0;

        MemoryBuf(size_t sz = 0) : 
            data(
                (sz == 0) ? 
                nullptr :
                static_cast<T*>(::operator new(sizeof(T) * sz))
            ),
            size(sz) {}

        MemoryBuf(const MemoryBuf& rhs) = delete;
        MemoryBuf& operator=(const MemoryBuf& rhs) = delete;

        MemoryBuf(MemoryBuf&& rhs) noexcept: 
            data(rhs.data),
            size(rhs.size),
            used(rhs.used) 
        {
            rhs.data = nullptr;
            rhs.size = 0; rhs.used = 0;
        }

        MemoryBuf& operator=(MemoryBuf&& rhs) noexcept { swap(rhs); }

        void swap(MemoryBuf& rhs) noexcept {
            std::swap(data, rhs.data);
            std::swap(size, rhs.size);
            std::swap(used, rhs.used);
        }

        ~MemoryBuf() {
            destroy(data, data + used);
            ::operator delete(data);
        }
    };


    template<typename T>
    class ElementsBuf: private MemoryBuf<T> {
    protected:
        using MemoryBuf<T>::data;
        using MemoryBuf<T>::size;
        using MemoryBuf<T>::used;

        ElementsBuf(size_t size, T val = T{}) : MemoryBuf<T>(size) {
            while (used < size) {
                construct(data + used, val);
                used += 1;
            }
        }

        template<typename IT>
        ElementsBuf(size_t size, const IT& first, const IT& second) : MemoryBuf<T>(size) {
            for (auto cfirst = first; cfirst != second; cfirst++) {
                construct<T>(data + used, *cfirst);
                used += 1;
            }
        }

        ElementsBuf(const ElementsBuf& rhs) : MemoryBuf<T>(rhs.size) {
            while (used < rhs.used) {
                construct(data + used, rhs.data[used]);
                used += 1;
            }
        }

        ElementsBuf& operator=(const ElementsBuf& rhs) {
            ElementsBuf tmp{ rhs };

            swap(tmp);
            return *this;
        }

        ElementsBuf(ElementsBuf&& rhs) noexcept : MemoryBuf<T>(std::move(rhs)) {}

        ElementsBuf& operator=(ElementsBuf&& rhs) noexcept { swap(rhs); }

        void swap(ElementsBuf& rhs) noexcept { MemoryBuf<T>::swap(rhs); }
    };
}