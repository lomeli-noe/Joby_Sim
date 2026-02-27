#pragma once
#include <cstddef>

namespace Joby {

template <typename T, size_t SIZE>
class MessageBuffer {
private:
    T m_data[SIZE];
    size_t m_head = 0; // Write here
    size_t m_tail = 0; // Read here
    size_t m_count = 0;

public:
    bool push(const T& item) {
        if (isFull()) {
           m_tail = (m_tail + 1) % SIZE;
           m_count--; 
        }
        m_data[m_head % SIZE] = item;
        m_head = (m_head + 1) % SIZE;
        m_count++;
        return true;
    }

    T pop() {
        if (isEmpty()) return T(); // Return default if empty
        T res = m_data[m_tail % SIZE];
        m_tail = (m_tail + 1) % SIZE;
        m_count--;
        return res;
    }

    bool isFull() const { return m_count == SIZE; }
    bool isEmpty() const { return m_count == 0; }
};
}