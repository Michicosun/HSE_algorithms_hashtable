#include <iostream>
#include <functional>
#include <vector>
#include <list>
#include <memory>

//CHANGE_2 : rewrote the inner vector to std::list

//CHANGE_1 : Ahahaha, found a translation error in the comments

//Class HashMap. Interface is similar to the unordered_map from stl
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    
    typedef std::unique_ptr<std::pair<const KeyType, ValueType>> node;
    typedef typename std::list<node>::iterator list_iterator;
    typedef typename std::list<node>::const_iterator const_list_iterator;
    
    const static size_t min_cnt_rows;
    const static size_t scale;
    const static size_t change_capacity;
    
    size_t cur_capacity;
    size_t cur_size;
    
    Hash hasher;
    
    std::vector<std::list<node>> table;
    
    //Rebuilds HashMap if count of element not in range [capacity / scale... capacity]
    void rebuild() {
        if (cur_capacity < cur_size * scale && cur_size * scale < cur_capacity * scale) return;
        cur_capacity = std::max(min_cnt_rows, change_capacity * cur_size);
        std::vector<std::list<node>> next_table(cur_capacity);
        for (auto& row : table) {
            for (auto& el : row) {
                size_t ind = hasher(el->first) % cur_capacity;
                next_table[ind].push_front(move(el));
            }
        }
        swap(table, next_table);
    }
    
public:
    
    //Class iterator points to a hash table cell table[row][ind]
    //Can jump to next occupied cell
    class iterator {
    private:
        
        std::vector<std::list<node>>* hash_table;
        
        size_t row;
        list_iterator lit;
        
        void find_next_occupied() {
            if (row < hash_table->size()) {
                if (lit == hash_table->at(row).end()) ++row;
                else return;
                while (row < hash_table->size() && hash_table->at(row).empty()) ++row;
            }
            if (row < hash_table->size()) lit = hash_table->at(row).begin();
            else lit = list_iterator();
        }
        
    public:
        
        iterator () {}
        
        iterator (std::vector<std::list<node>>* hash_map, size_t cell_row, list_iterator it) {
            hash_table = hash_map;
            row = cell_row;
            lit = it;
            find_next_occupied();
        }
        
        iterator operator++ () {
            ++lit;
            find_next_occupied();
            return *this;
        }
        
        iterator operator++ (int) {
            iterator oldIter(hash_table, row, lit);
            ++lit;
            find_next_occupied();
            return oldIter;
        }
        
        const size_t getRow() const {
            return row;
        }
        
        const list_iterator getListIterator() const {
            return lit;
        }
        
        std::pair<const KeyType, ValueType>& operator* () {
            return **lit;
        }
        
        std::pair<const KeyType, ValueType>* operator-> () {
            return (*lit).get();
        }
        
        bool operator== (iterator other) {
            return other.hash_table == hash_table && other.row == row && other.lit == lit;
        }
        
        bool operator!= (iterator other) {
            return !(other == *this);
        }
        
    };
    
    //Class const_iterator is a const version of iterator
    class const_iterator {
    private:
        
        const std::vector<std::list<node>>* hash_table;
        
        size_t row;
        const_list_iterator lit;
        
        void find_next_occupied() {
            if (row < hash_table->size()) {
                if (lit == hash_table->at(row).end()) ++row;
                else return;
                while (row < hash_table->size() && hash_table->at(row).empty()) ++row;
            }
            if (row < hash_table->size()) lit = hash_table->at(row).begin();
            else lit = list_iterator();
        }
        
    public:
        
        const_iterator () {}
        
        const_iterator (const std::vector<std::list<node>>* hash_map, size_t cell_row, const_list_iterator it) {
            hash_table = hash_map;
            row = cell_row;
            lit = it;
            find_next_occupied();
        }
        
        const_iterator operator++ () {
            ++lit;
            find_next_occupied();
            return *this;
        }
        
        const_iterator operator++ (int) {
            const_iterator oldIter(hash_table, row, lit);
            ++lit;
            find_next_occupied();
            return oldIter;
        }
        
        const size_t getRow() const {
            return row;
        }
        
        const const_list_iterator getListIterator() const {
            return lit;
        }
        
        std::pair<const KeyType, ValueType>& operator* () {
            return **lit;
        }
        
        std::pair<const KeyType, ValueType>* operator-> () {
            return (*lit).get();
        }
        
        bool operator== (const_iterator other) {
            return other.hash_table == hash_table && other.row == row && other.lit == lit;
        }
        
        bool operator!= (const_iterator other) {
            return !(other == *this);
        }
        
    };
    
    // Constructors
    HashMap() : hasher() {
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
    }
    
    HashMap(const Hash& hash_function) : hasher(hash_function) {
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
    }
    
    template<class Iterator>
    HashMap(Iterator begin, Iterator end) {
        hasher = Hash();
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
        for (; begin != end; ++begin) insert(*begin);
    }
    
    template<class Iterator>
    HashMap(Iterator begin, Iterator end, const Hash& hash_function) : hasher(hash_function) {
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
        for (; begin != end; ++begin) insert(*begin);
    }
    
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list) {
        hasher = Hash();
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
        for (const auto& el : list) insert(el);
    }
    
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, const Hash& hash_function) : hasher(hash_function) {
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
        for (const auto& el : list) insert(el);
    }
    
    HashMap(const HashMap& other) : hasher(other.hasher) {
        cur_size = 0;
        cur_capacity = min_cnt_rows;
        table.resize(min_cnt_rows);
        for (const auto& el : other) insert(el);
    }
    
    // Assignment operators
    HashMap operator= (const HashMap& other) {
        hasher = other.hasher;
        table.resize(min_cnt_rows);
        cur_capacity = min_cnt_rows;
        for (const auto& el : other) insert(el);
        return *this;
    }
    
    HashMap operator= (HashMap&& other) {
        hasher = std::move(other.hasher);
        table = std::move(other.table);
        cur_size = other.cur_size;
        cur_capacity = other.cur_capacity;
        return *this;
    }
    
    // Returns the current count of elements in the table
    size_t size() const {
        return cur_size;
    }
    
    // Returns True if there are no elements in the table
    bool empty() const {
        return cur_size == 0;
    }
    
    // Returns the used hash function
    Hash hash_function() const {
        return hasher;
    }
    
    // Returns an iterator to the first element of the table
    iterator begin() {
        return iterator(&table, 0, table[0].begin());
    }
    
    const_iterator begin() const {
        return const_iterator(&table, 0, table[0].begin());
    }
    
    // Returns an iterator pointing to the position behind the last element
    iterator end() {
        return iterator(&table, table.size(), list_iterator());
    }
    
    const_iterator end() const {
        return const_iterator(&table, table.size(), const_list_iterator());
    }
    
    // Returns an iterator to the element if found, otherwise the iterator to the end
    iterator find(const KeyType& key) {
        size_t ind = hasher(key) % cur_capacity;
        for (auto it = table[ind].begin(); it != table[ind].end(); ++it) {
            if ((*it)->first == key) return iterator(&table, ind, it);
        }
        return end();
    }
    
    const_iterator find(const KeyType& key) const {
        size_t ind = hasher(key) % cur_capacity;
        for (auto it = table[ind].begin(); it != table[ind].end(); ++it) {
            if ((*it)->first == key) {
                return const_iterator(&table, ind, it);
            }
        }
        return end();
    }
    
    void insert(const std::pair<KeyType, ValueType>& element) {
        auto it = find(element.first);
        if (it == end()) {
            size_t ind = hasher(element.first) % cur_capacity;
            table[ind].push_front(node(new std::pair<const KeyType, ValueType>(element)));
            ++cur_size; rebuild();
        }
    }
    
    void erase(const KeyType& key) {
        auto it = find(key);
        if (it != end()) {
            size_t row = it.getRow();
            table[row].erase(it.getListIterator());
            --cur_size; rebuild();
        }
    }
    
    // Сreates an item if it hasn't left it
    ValueType& operator[] (const KeyType& key) {
        auto it = find(key);
        if (it == end()) insert(std::make_pair(key, ValueType()));
        it = find(key);
        return it->second;
    }
    
    // Throws an exception std::out_of_range if no item is found
    ValueType& at(const KeyType& key) {
        auto it = find(key);
        if (it == end()) throw std::out_of_range("");
        else return it->second;
    }
    
    const ValueType& at(const KeyType& key) const {
        auto it = find(key);
        if (it == end()) throw std::out_of_range("");
        else return it->second;
    }
    
    // Deletes all elements in the table
    void clear() {
        for (size_t ind = 0; ind < cur_capacity; ++ind) {
            while (!table[ind].empty()) table[ind].pop_back();
        }
        cur_size = 0;
        rebuild();
    }
    
};

template<class KeyType, class ValueType, class Hash>
const size_t HashMap<KeyType, ValueType, Hash>::min_cnt_rows = 7;

template<class KeyType, class ValueType, class Hash>
const size_t HashMap<KeyType, ValueType, Hash>::scale = 3;

template<class KeyType, class ValueType, class Hash>
const size_t HashMap<KeyType, ValueType, Hash>::change_capacity = 2;

