#include <functional>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    const static inline size_t min_cnt_rows = 10;
    const static inline size_t scale = 4;
    
    typedef std::pair<KeyType, ValueType> node;
    
    size_t cur_capacity;
    size_t cur_size;
    
    Hash hasher;
    
    std::vector<std::vector<node>> table;
    
    void rebuild() {
        if (cur_capacity < cur_size * scale && cur_size * scale < cur_size * scale) return;
        cur_capacity = max(min_cnt_rows, 2 * cur_size);
        std::vector<std::vector<node>> next_table(cur_capacity);
        for (auto it = begin(); it != end(); ++it) {
            size_t ind = hasher(it->first) % cur_capacity;
            next_table[ind].push_back(*it);
        }
        swap(table, next_table);
    }
    
    class iterator {
    private:
        HashMap* hash_table;
        
        size_t row;
        size_t ind;
        
        void find_next_occupied() {
            ++ind;
            while (row < hash_table->cur_capacity && ind == hash_table->table[row].size()) {
                ++row;
                ind = 0;
            }
        }
        
    public:
        
        iterator (HashMap* hash_map, size_t cell_row = 0, size_t cell_ind = 0) {
            hash_table = hash_map;
            row = cell_row;
            ind = cell_ind;
        }
        
        iterator operator++ () {
            find_next_occupied();
            return *this;
        }
        
        iterator operator++ (int) {
            iterator oldIter(hash_table, row, ind);
            find_next_occupied();
            return oldIter;
        }
        
        node& operator* () {
            return *hash_table->table[row][ind];
        }
        
        node* operator-> () {
            return &hash_table->table[row][ind];
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
        
        void find_next_occupied() const {
            ++ind;
            while (row < hash_table->cur_capacity && ind == hash_table->table[row].size()) {
                ++row;
                ind = 0;
            }
        }
        
    public:
        
        const_iterator (const HashMap* hash_map, size_t cell_row = 0, size_t cell_ind = 0) {
            hash_table = hash_map;
            row = cell_row;
            ind = cell_ind;
        }
        
        iterator operator++ () {
            find_next_occupied();
            return *this;
        }
        
        iterator operator++ (int) {
            iterator oldIter(hash_table, row, ind);
            find_next_occupied();
            return oldIter;
        }
        
        const node& operator* () const {
            return *hash_table->table[row][ind];
        }
        
        const node* operator-> () const {
            return &hash_table->table[row][ind];
        }
        
        bool operator== (iterator other) const {
            return other.hash_table == hash_table && other.row == row && other.ind == ind;
        }
        
        bool operator!= (iterator other) const {
            return !(other == *this);
        }
    };
    
public:
    
    HashMap(Hash hash_function = Hash()) : hasher(hash_function), cur_size(0), cur_capacity(min_cnt_rows) {
        table.resize(min_cnt_rows);
    }
    
    template<class Iterator>
    HashMap(Iterator begin, Iterator end, Hash hash_function = Hash()) : hasher(hash_function), cur_size(0), cur_capacity(min_cnt_rows) {
        table.resize(min_cnt_rows);
        for (auto it = begin; it != end; ++it) {
            size_t ind = hasher(it->first) % cur_capacity;
            table[ind].push_back(*it);
            ++cur_size;
        }
    }
    
    HashMap(std::initializer_list<node> list, Hash hash_function = Hash()) : hasher(hash_function), cur_size(0), cur_capacity(min_cnt_rows) {
        table.resize(min_cnt_rows);
        for (auto el : list) {
            size_t ind = hasher(el.first) % cur_capacity;
            table[ind].push_back(el);
            ++cur_size;
        }
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
        return iterator(&this, 0, 0);
    }
    
    iterator end() {
        return iterator(&this, table.size(), 0);
    }
    
    const_iterator begin() const {
        return const_iterator(&this, 0, 0);
    }
    
    const_iterator end() const {
        return const_iterator(&this, table.size(), 0);
    }
    
    void insert(const node& element) {
        size_t ind = hasher(element.first) % cur_capacity;
        for (const auto& el : table[ind]) {
            if (el == element) return;
        }
        table[ind].push_back(element);
        ++cur_size;
        rebuild();
    }
    
    void erase(const KeyType& key) {
        size_t ind = hasher(key) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[i].first == key) {
                table[i].erase(table[i].begin() + i);
                break;
            }
        }
        rebuild();
    }
    
    iterator find(const KeyType& key) {
        size_t ind = hasher(key) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[i].first == key) return iterator(&this, ind, i);
        }
        return end();
    }
    
    const_iterator find(const KeyType& key) const {
        size_t ind = hasher(key) % cur_capacity;
        for (size_t i = 0; i < table[ind].size(); ++i) {
            if (table[i].first == key) return const_iterator(&this, ind, i);
        }
        return end();
    }
    
    //11
    
};

using namespace std;

int main() {
    vector<pair<int, int>> a = {{1, 2}, {3, 4}};
    HashMap<int, int> mp({{1, 2}, {3, 5}});
    
    return 0;
}
