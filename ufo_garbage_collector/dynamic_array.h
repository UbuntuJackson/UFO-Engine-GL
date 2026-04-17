#pragma once
#include <exception>
#include <stdexcept>
#include <string>
#include <bits/stl_iterator_base_funcs.h>

namespace ufo::gc{

template<typename tType>
class DynamicArray{
private:
    int m_size;
    int m_capacity;
    tType* m_array;
public:

    //Too platform specific
    typedef __gnu_cxx::__normal_iterator<tType*, DynamicArray<tType>> iterator;
    typedef __gnu_cxx::__normal_iterator<const tType*, DynamicArray<tType>> const_iterator;

    DynamicArray():
    m_size{0},
    //capacity needs to be incremented somehow, pretty sure
    m_capacity{1},
    m_array{new tType[m_capacity]}
    {

    }

    DynamicArray(const DynamicArray<tType>& _rhs):
    m_size{_rhs.size()},
    m_capacity{_rhs.capacity()},
    m_array{new int[m_capacity]}
    {
        for(int i = 0; i < _rhs.size(); i++){
            m_array[i] = _rhs.m_array[i];
        }
    }

    ~DynamicArray(){
        delete[] m_array;
    }

    void push_back(tType _value){
        //Console::PrintLine("size",m_size,"capactity", m_capacity);

        if(m_size < m_capacity){
            m_array[m_size] = _value;
            ++m_size;
        }
        else{
            m_capacity*=2;
            tType* new_array = new tType[m_capacity];

            for(int i = 0; i<m_size; i++){
                new_array[i] = m_array[i];
            }

            delete []m_array;
            m_array = new_array;

            //Same as in if(){...}
            m_array[m_size] = _value;
            ++m_size;
        }

    }

    void clear() {
        delete[] m_array;
        m_size = 0;
        m_capacity = 0;
    }

    int size() const {
        return m_size;
    }

    bool empty() const {
        return m_size == 0;
    }

    bool capacity() const {
        return m_capacity;
    }

    void pop_back(){
        if(m_size == 0){
            throw std::runtime_error("Too few elements to pop");
        }

        --m_size;
    }

    void shrink_to_fit(){
        m_capacity = m_size;
        tType* new_array = new tType[m_capacity];

        for(int i = 0; i < m_size; i++){
            new_array[i] = m_array[i];
        }

        delete[] m_array;
        m_array = new_array;

    }

    tType& operator[](int _index){
        return m_array[_index];
    }

    bool operator==(const DynamicArray<tType>& _rhs) const {
        if(m_size != _rhs.size()) return false;
        else{
            for(int i = 0; i < m_size; i++){
                if(m_array[i] != _rhs.m_array[i]){
                    return false;
                }
            }
        }
    }

    bool operator!=(const DynamicArray<tType>& _rhs) const {
        return !(*this == _rhs);
    }

    const_iterator cbegin(){
        return const_iterator(&m_array[0]);
    }

    const_iterator cend(){
        return const_iterator(&m_array[m_size]);
    }

    iterator begin(){
        return iterator(&m_array[0]);
    }

    iterator end(){
        return iterator(&m_array[m_size]);
    }

    //GC mark phase
    void Mark(){
        for(int i = 0; i<m_size; i++){
            if(m_array[i] != nullptr) m_array[i]->Mark();
        }
    }

};

}
