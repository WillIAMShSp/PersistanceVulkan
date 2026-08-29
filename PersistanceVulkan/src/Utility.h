#pragma once

#include <vector>
#include <array>

namespace PersistanceUtils 
{

    /**
     * @brief This is a helper class that allows you to view into an array like a lightweight std::span pre C++ 20
     * 
     * @tparam T The class of the array.
     */
    template<typename T> 
    class ArrayView {
        public:
        
        ArrayView(T* data, size_t size) :m_data(data), m_size(size){}
        ArrayView(std::vector<T>& data) :m_data(data.data()), m_size(data.size()){}
        template<std::size_t N>
        ArrayView(std::array<T, N>& data) :m_data(data.data()), m_size(data.size()) {}

        T& operator[](uint32_t index) { return m_data[index];}
        void operator=(std::vector<T> data) { m_data = data.data(); m_size = data.size();}
        template<std::size_t N>
        void operator=(std::array<T, N> data) {m_data = data.data(); m_size = data.size();}
        T& operator=(std::nullptr_t) {return *this;}

        ArrayView<T> subView(uint32_t index, size_t size) {return ArrayView<T>(m_data+index, size);}
        
        size_t size() {return m_size;}

        bool empty() {return m_size == 0;}
        
        T* data() { return m_data;}
        
        private:
        
        T* m_data = nullptr;
        size_t m_size = 0;
        
    };
}