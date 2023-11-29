#pragma once

#include <iostream>
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

        virtual ~MemoryBuf() {
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

        ElementsBuf(size_t size, const T& val) : MemoryBuf<T>(size) {
            while (used < size) {
                construct(data + used, val);
                used += 1;
            }
        }

        template<typename IT>
        ElementsBuf(size_t size, const IT& first) : MemoryBuf<T>(size) {
            auto cfirst = first;
            for (int i = 0; i < size; i++) {
                if (i > 0) cfirst++;
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


    template<typename T>
    class ControlBlock {
    private:
        // T* data; ???
        size_t counter;

    public:
        ControlBlock(): counter(1) {}
        size_t change(int value) { counter += value; return counter; }
        size_t get_counter() { return counter; }
    };


    template<typename T>
    class SharedMemoryBuf {
    private:
        ControlBlock<T>* control_block_ptr;

    protected:
        T* data;
        size_t size, used = 0;

        SharedMemoryBuf(size_t sz = 0):
            control_block_ptr(
                (sz == 0) ? nullptr : new ControlBlock<T>{}
            ),
            data(
                (sz == 0) ? 
                nullptr :
                static_cast<T*>(::operator new(sizeof(T) * sz))
            ),
            size(sz) {}

        SharedMemoryBuf(const SharedMemoryBuf& rhs):
            control_block_ptr(rhs.control_block_ptr),
            data(rhs.data),
            size(rhs.size),
            used(rhs.used)
        {
            if (control_block_ptr)
                control_block_ptr->change(1);
        }
        SharedMemoryBuf& operator=(const SharedMemoryBuf& rhs) {
            SharedMemoryBuf tmp{rhs};

            swap(tmp);
            return *this;
        }

        SharedMemoryBuf(SharedMemoryBuf&& rhs) noexcept:
            control_block_ptr(rhs.control_block_ptr),
            data(rhs.data),
            size(rhs.size),
            used(rhs.used) 
        {
            rhs.control_block_ptr = nullptr;
            rhs.data = nullptr;
            rhs.size = 0; rhs.used = 0;
        }

        SharedMemoryBuf& operator=(SharedMemoryBuf&& rhs) noexcept { swap(rhs); }

        void swap(SharedMemoryBuf& rhs) noexcept {
            std::swap(control_block_ptr, rhs.control_block_ptr);
            std::swap(data, rhs.data);
            std::swap(size, rhs.size);
            std::swap(used, rhs.used);
        }

        size_t get_counter() noexcept { return control_block_ptr->get_counter(); }

        virtual ~SharedMemoryBuf() {
            if (!control_block_ptr)
                return;

            if (control_block_ptr->change(-1) == 0) {
                delete control_block_ptr;
                destroy(data, data + used);
                ::operator delete(data);
            }
        }
    };


    template<typename T>
    class SharedElementsBuf: private SharedMemoryBuf<T> {
    protected:
        using SharedMemoryBuf<T>::data;
        using SharedMemoryBuf<T>::size;
        using SharedMemoryBuf<T>::used;

        using SharedMemoryBuf<T>::get_counter;

        SharedElementsBuf(size_t size, const T& val) : SharedMemoryBuf<T>(size) {
            while (used < size) {
                construct(data + used, val);
                used += 1;
            }
        }

        template<typename IT>
        SharedElementsBuf(size_t size, const IT& first) : SharedMemoryBuf<T>(size) {
            auto cfirst = first;
            for (int i = 0; i < size; i++) {
                if (i > 0) cfirst++;
                construct<T>(data + used, *cfirst);
                used += 1;
            }
        }

        SharedElementsBuf(const SharedElementsBuf& rhs) : SharedMemoryBuf<T>(rhs) {}

        SharedElementsBuf& operator=(const SharedElementsBuf& rhs) {
            SharedElementsBuf tmp{ rhs };

            swap(tmp);
            return *this;
        }

        SharedElementsBuf(SharedElementsBuf&& rhs) noexcept : SharedMemoryBuf<T>(std::move(rhs)) {}

        SharedElementsBuf& operator=(SharedElementsBuf&& rhs) noexcept { swap(rhs); }

        void swap(SharedElementsBuf& rhs) noexcept { SharedMemoryBuf<T>::swap(rhs); }

        void cow() {
            if (get_counter() == 1)
                return;

            SharedElementsBuf new_buffer(size, data);
            swap(new_buffer);
        }
    };

    template<typename T, bool is_lazy>
    using ElementsBuf_ = std::conditional_t<
        is_lazy,
        SharedElementsBuf<T>,
        ElementsBuf<T>
    >;
}