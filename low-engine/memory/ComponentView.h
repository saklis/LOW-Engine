#pragma once

#include <cstddef>

namespace LowEngine::Memory {
    template<typename T>
    class ComponentView {
    public:
        ComponentView(void* data, size_t count)
            : _data(reinterpret_cast<T*>(data)), _count(count) {
        }

        T* begin() { return _data; }
        T* end() { return _data + _count; }
        const T* begin() const { return _data; }
        const T* end() const { return _data + _count; }

    protected:
        T* _data;
        size_t _count;
    };
}
