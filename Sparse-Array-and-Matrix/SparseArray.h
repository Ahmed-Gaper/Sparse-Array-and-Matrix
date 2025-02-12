#ifndef SPARSE_ARRAY_H
#define SPARSE_ARRAY_H

#include <iostream>
#include <stdexcept>
#include <utility>

template <typename T>
class SparseArray {
private:
    struct ArrayNode {
        T value;
        int index;
        ArrayNode* next;
        ArrayNode* previous;
        ArrayNode(const T& value, int index)
            : value(value), index(index), next(nullptr), previous(nullptr) {}
    };

    int capacity_;  
    int length_;    
    ArrayNode* head;
    ArrayNode* tail;

    void insert(const T& value, int index) {
        if (value == T()) return;
        if (index >= capacity_)
            throw std::out_of_range("Index exceeds sparse array capacity");
        
        ArrayNode* newElement = new ArrayNode(value, index);
        length_++;

        if (!head) {
            head = tail = newElement;
            return;
        }

        ArrayNode* cur = head;
        while (cur && cur->index < index)
            cur = cur->next;

        if (cur == head) {
            newElement->next = head;
            head->previous = newElement;
            head = newElement;
        } else if (!cur) {
            tail->next = newElement;
            newElement->previous = tail;
            tail = newElement;
        } else {
            ArrayNode* prev = cur->previous;
            prev->next = newElement;
            newElement->previous = prev;
            newElement->next = cur;
            cur->previous = newElement;
        }
    }

    void removeNode(ArrayNode* node) {
        if (!node) return;

        if (node == head) {
            head = node->next;
            if (head) head->previous = nullptr;
            else tail = nullptr;
        } else if (node == tail) {
            tail = node->previous;
            if (tail) tail->next = nullptr;
            else head = nullptr;
        } else {
            node->previous->next = node->next;
            node->next->previous = node->previous;
        }
        delete node;
        length_--;
    }

public:
    explicit SparseArray(int capacity) 
        : capacity_(capacity), length_(0), head(nullptr), tail(nullptr) {}

    ~SparseArray() {
        clear();
    }

    SparseArray(const SparseArray& other) 
    : capacity_(other.capacity_), length_(0), head(nullptr), tail(nullptr) {
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert((*it).second, (*it).first);
        }
    }

    SparseArray& operator=(const SparseArray& other) {
        if (this != &other) {
            if (capacity_ != other.capacity_)
                throw std::invalid_argument("Capacity mismatch in assignment");
            clear();
            for (auto it = other.begin(); it != other.end(); ++it) {
                insert((*it).second, (*it).first);
            }
        }
        return *this;
    }

    SparseArray(SparseArray&& other) noexcept 
        : capacity_(other.capacity_), length_(other.length_), 
          head(other.head), tail(other.tail) {
        other.head = nullptr;
        other.tail = nullptr;
        other.length_ = 0;
    }

    SparseArray& operator=(SparseArray&& other) noexcept {
        if (this != &other) {
            clear();
            capacity_ = other.capacity_;
            length_ = other.length_;
            head = other.head;
            tail = other.tail;
            other.head = nullptr;
            other.tail = nullptr;
            other.length_ = 0;
        }
        return *this;
    }

    T get(int index) const {
        if (index >= capacity_)
            throw std::out_of_range("Index exceeds sparse array capacity");
        for (ArrayNode* cur = head; cur; cur = cur->next) {
            if (cur->index == index)
                return cur->value;
            else if (cur->index > index)
                break;
        }
        return T();
    }

    void set(int index, const T& value) {
        if (index >= capacity_)
            throw std::out_of_range("Index exceeds sparse array capacity");
        
        ArrayNode* cur = head;
        while (cur && cur->index < index)
            cur = cur->next;

        if (cur && cur->index == index) {
            if (value == T())
                removeNode(cur);
            else
                cur->value = value;
        } else {
            if (value != T())
                insert(value, index);
        }
    }

    int capacity() const { return capacity_; }
    int nonZeroCount() const { return length_; }

    void clear() {
        ArrayNode* cur = head;
        while (cur) {
            ArrayNode* temp = cur;
            cur = cur->next;
            delete temp;
        }
        head = tail = nullptr;
        length_ = 0;
    }

    class ConstIterator {
        const ArrayNode* current;
    public:
        ConstIterator(const ArrayNode* node) : current(node) {}

        ConstIterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const {
            return current == other.current;
        }

        bool operator!=(const ConstIterator& other) const {
            return !(*this == other);
        }

        std::pair<int, T> operator*() const {
            return {current->index, current->value};
        }
    };

    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }

    void print(std::ostream& os = std::cout) const {
        ArrayNode* cur = head;
        for (int i = 0; i < capacity_; ++i) {
            if (cur && cur->index == i) {
                os << cur->value << " ";
                cur = cur->next;
            } else {
                os << T() << " ";
            }
        }
        os << '\n';
    }

    void printNonZero(std::ostream& os = std::cout) const {
        for (ArrayNode* cur = head; cur; cur = cur->next) {
            os << cur->value << " ";
        }
        os << '\n';
    }

    friend std::ostream& operator<<(std::ostream& os, const SparseArray<T>& arr) {
        arr.print(os);
        return os;
    }

    class Proxy {
        SparseArray<T>& array;
        int index;
    public:
        Proxy(SparseArray<T>& array, int index) : array(array), index(index) {}

        operator T() const {
            return array.get(index);
        }

        Proxy& operator=(const T& value) {
            array.set(index, value);
            return *this;
        }

        Proxy& operator=(const Proxy& other) {
            return *this = static_cast<T>(other);
        }
    };

    Proxy operator[](int index) {
        if (index < 0 || index >= capacity_)
            throw std::out_of_range("Index out of bounds");
        return Proxy(*this, index);
    }

    T operator[](int index) const {
        return get(index);
    }
};

#endif