#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>


#ifndef BPTREE_HPP
#define BPTREE_HPP


template<
        class Key,
        class T,
        class Compare = std::less<Key>
> class Bptree{

private:

public:
    typedef pair<const Key, T> value_type;

    class iterator {
    private:

    public:
        iterator() {
        }
        iterator(const iterator &other) {
        }

        iterator operator++(int) {}

        iterator & operator++() {}

        iterator operator--(int) {}

        iterator & operator--() {}

        value_type & operator*() const {}
        bool operator==(const iterator &rhs) const {}

        bool operator!=(const iterator &rhs) const {}

        value_type* operator->() const noexcept {}
    };

    /**
     * TODO two constructors
     */
    Bptree() {}
    Bptree(const Bptree &other) {}
    /**
     * TODO assignment operator
     */
    Bptree & operator=(const Bptree &other) {}
    /**
     * TODO Destructors
     */
    ~Bptree() {}
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T & at(const Key &key) {}
    const T & at(const Key &key) const {}
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T & operator[](const Key &key) {}
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T & operator[](const Key &key) const {}
    /**
     * return a iterator to the beginning
     */
    iterator begin() {}

    iterator end() {}

    bool empty() const {}

    size_t size() const {}

    void clear() {}

    pair<iterator, bool> insert(const value_type &value) {}

    void erase(iterator pos) {}

    size_t count(const Key &key) const {}

    iterator find(const Key &key) {}
};

#endif
