#include <iostream>
#include <functional>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    const static inline size_t min_cnt_rows = 10;
    const static inline size_t scale = 4;
    
    typedef std::unique_ptr<std::pair<const KeyType, ValueType>> node;
    
    size_t cur_capacity;
    size_t cur_size;
    
    Hash hasher;
    
    std::vector<std::vector<node>> table;
    
    void rebuild() {
        if (cur_capacity < cur_size * scale && cur_size * scale < cur_size * scale) return;
        cur_capacity = std::max(min_cnt_rows, 2 * cur_size);
        std::vector<std::vector<node>> next_table(cur_capacity);
        for (size_t i = 0; i < table.size(); ++i) {
            for (auto &ptr : table[i]) {
                size_t ind = hasher(ptr->first) % cur_capacity;
                next_table[ind].push_back(std::move(ptr));
            }
        }
        table.swap(next_table);
    }
    
public:
    
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
            ++ind;
            find_next_occupied();
            return oldIter;
        }
        
        std::pair<const KeyType, ValueType>& operator* () {
            return *hash_table->table[row][ind];
        }
        
        std::pair<const KeyType, ValueType>* operator-> () {
            return &*(hash_table->table[row][ind]);
        }
        
        bool operator== (iterator other) {
            return other.hash_table == hash_table && other.row == row && other.ind == ind;
        }
        
        bool operator!= (iterator other) {
            return !(other == *this);
        }
    };
    
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
            ++ind;
            find_next_occupied();
            return oldIter;
        }
        
        const std::pair<const KeyType, ValueType>& operator* () const {
            return *hash_table->table[row][ind];
        }
        
        const std::pair<const KeyType, ValueType>* operator-> () const {
            return &*(hash_table->table[row][ind]);
        }
        
        bool operator== (const_iterator other) const {
            return other.hash_table == hash_table && other.row == row && other.ind == ind;
        }
        
        bool operator!= (const_iterator other) const {
            return !(other == *this);
        }
    };
    
    HashMap(Hash hash_function = Hash()) : hasher(hash_function), cur_size(0), cur_capacity(min_cnt_rows) {
        table.resize(min_cnt_rows);
    }
    
    template<class Iterator>
    HashMap(Iterator begin, Iterator end, Hash hash_function = Hash()) : hasher(hash_function), cur_size(0), cur_capacity(min_cnt_rows) {
        table.resize(min_cnt_rows);
        for (auto it = begin; it != end; ++it) insert(*it);
    }
    
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash hash_function = Hash()) : hasher(hash_function), cur_size(0), cur_capacity(min_cnt_rows) {
        table.resize(min_cnt_rows);
        for (const auto& el : list) insert(el);
    }
    
    HashMap(const HashMap& other) : HashMap(other.hasher) {
        for (const auto& el : other) {
            insert(el);
        }
    }
    
    HashMap operator= (const HashMap& other) {
        hasher = other.hasher;
        table.resize(min_cnt_rows);
        cur_capacity = min_cnt_rows;
        for (const auto& el : other) {
            insert(el);
        }
        return *this;
    }
    
    HashMap operator= (HashMap&& other) {
        hasher = std::move(other.hasher);
        table = std::move(other.table);
        cur_size = other.cur_size;
        cur_capacity = other.cur_capacity;
        return *this;
    }
    
    size_t size() const {
        return cur_size;
    }
    
    bool empty() const {
        return cur_size == 0;
    }
    
    Hash hash_function() const {
        return hasher;
    }
    
    iterator begin() {
        return iterator(this, 0, 0);
    }
    
    iterator end() {
        return iterator(this, table.size(), 0);
    }
    
    const_iterator begin() const {
        return const_iterator(this, 0, 0);
    }
    
    const_iterator end() const {
        return const_iterator(this, table.size(), 0);
    }
    
    void insert(const std::pair<KeyType, ValueType>& element) {
        size_t ind = hasher(element.first) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[ind][i]->first == element.first) return;
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
                break;
            }
        }
        rebuild();
    }
    
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
    
    ValueType& operator[] (const KeyType& key) {
        auto it = find(key);
        if (it == end()) insert({key, ValueType()});
        it = find(key);
        return it->second;
    }
    
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
    
    void clear() {
        for (size_t ind = 0; ind < cur_capacity; ++ind) {
            while (!table[ind].empty()) table.pop_back();
        }
        rebuild();
    }
    
};

