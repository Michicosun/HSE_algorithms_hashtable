#include <iostream>
#include <functional>
#include <vector>
#include <memory>

//Ahahaha, found a translation error in the comments

//Class HashMap. Interface is similar to the unordered_map from stl
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    
    const size_t min_cnt_rows = 10;
    const size_t scale = 4;
    
    typedef std::unique_ptr<std::pair<const KeyType, ValueType>> node;
    
    size_t cur_capacity;
    size_t cur_size;
    
    Hash hasher;
    
    std::vector<std::vector<node>> table;
    
    
    //Rebuilds HashMap if count of element not in range [capacity / scale ... capacity]
    void rebuild() {
        if (cur_capacity < cur_size * scale && cur_size * scale < cur_capacity * scale) return;
        cur_capacity = std::max(min_cnt_rows, 2 * cur_size);
        std::vector<std::vector<node>> next_table(cur_capacity);
        for (size_t i = 0; i < table.size(); ++i) {
            for (auto& el : table[i]) {
                size_t ind = hasher(el->first) % cur_capacity;
                next_table[ind].push_back(std::move(el));
            }
        }
        table.swap(next_table);
    }
    
public:
    
    //Class iterator points to a hash table cell table[row][ind]
    //Can jump to next occupied cell
    class iterator {
    private:
        HashMap* hash_table;
        
        size_t row;
        size_t ind;
        
        void find_next_occupied() {
            while (row < hash_table->cur_capacity && ind == hash_table->table[row].size()) {
                ++row;
                ind = 0;
            }
        }
        
    public:
        
        iterator () {}
        
        iterator (HashMap* hash_map, size_t cell_row = 0, size_t cell_ind = 0) {
            hash_table = hash_map;
            row = cell_row;
            ind = cell_ind;
            find_next_occupied();
        }
        
        iterator operator++ () {
            ++ind;
            find_next_occupied();
            return *this;
        }
        
        iterator operator++ (int) {
            iterator oldIter(hash_table, row, ind);
            ++(*this);
            return oldIter;
        }
        
        std::pair<const KeyType, ValueType>& operator* () {
            return *hash_table->table[row][ind];
        }
        
        std::pair<const KeyType, ValueType>* operator-> () {
            return hash_table->table[row][ind].get();
        }
        
        bool operator== (iterator other) {
            return other.hash_table == hash_table && other.row == row && other.ind == ind;
        }
        
        bool operator!= (iterator other) {
            return !(other == *this);
        }
    };
    
    //Class const_iterator is a const version of iterator
    class const_iterator {
    private:
        const HashMap* hash_table;
        
        size_t row;
        size_t ind;
        
        void find_next_occupied() {
            while (row < hash_table->cur_capacity && ind == hash_table->table[row].size()) {
                ++row;
                ind = 0;
            }
        }
        
    public:
        
        const_iterator () {}
        
        const_iterator (const HashMap* hash_map, size_t cell_row = 0, size_t cell_ind = 0) {
            hash_table = hash_map;
            row = cell_row;
            ind = cell_ind;
            find_next_occupied();
        }
        
        const_iterator operator++ () {
            ++ind;
            find_next_occupied();
            return *this;
        }
        
        const_iterator operator++ (int) {
            const_iterator oldIter(hash_table, row, ind);
            ++(*this);
            return oldIter;
        }
        
        const std::pair<const KeyType, ValueType>& operator* () const {
            return *hash_table->table[row][ind];
        }
        
        const std::pair<const KeyType, ValueType>* operator-> () const {
            return hash_table->table[row][ind].get();
        }
        
        bool operator== (const_iterator other) const {
            return other.hash_table == hash_table && other.row == row && other.ind == ind;
        }
        
        bool operator!= (const_iterator other) const {
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
        return iterator(this, 0, 0);
    }
    
    const_iterator begin() const {
        return const_iterator(this, 0, 0);
    }
    
    // Returns an iterator pointing to the position behind the last element
    iterator end() {
        return iterator(this, table.size(), 0);
    }
    
    const_iterator end() const {
        return const_iterator(this, table.size(), 0);
    }
    
    void insert(const std::pair<KeyType, ValueType>& element) {
        size_t ind = hasher(element.first) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[ind][i]->first == element.first) {
                return;
            }
        }
        table[ind].push_back(node(new std::pair<const KeyType, ValueType>(element)));
        ++cur_size;
        rebuild();
    }
    
    void erase(const KeyType& key) {
        size_t ind = hasher(key) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[ind][i]->first == key) {
                table[ind].erase(table[ind].begin() + i);
                --cur_size;
                break;
            }
        }
        rebuild();
    }
    
    // Returns an iterator to the element if found, otherwise the iterator to the end
    iterator find(const KeyType& key) {
        size_t ind = hasher(key) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[ind][i]->first == key) return iterator(this, ind, i);
        }
        return end();
    }
    
    const_iterator find(const KeyType& key) const {
        size_t ind = hasher(key) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[ind][i]->first == key) return const_iterator(this, ind, i);
        }
        return end();
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
        for (size_t ind = 0; ind < cur_capacity; ++ind) table[ind].clear();
        cur_size = 0;
        rebuild();
    }
    
};

