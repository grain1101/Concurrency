template<typename K, typename V>
class HashTable {
public:
    HashTable(int initialSize) {
        m_size = initialSize;
        v.assign(m_size, nullptr);
        mutexes = vector<mutex>(m_size);
    }
    ~HashTable(){
        for(auto n : v) {
            while(n) {
                auto next = n->next;
                delete n;
                n = next;
            }
        }
    }
    V get(const K& key) {
        int k = getHashKey(key);
        HashNode* ret = nullptr;
        {
            lock_guard<mutex> lk(mutexes[k]);
            ret = v[k];
            while(ret && ret->key != key)  ret = ret->next;
        }
        if (!ret) throw out_of_range("hash table does not contain " + to_string(key) + " value");
        return ret->value;
    }
    void put(const K& key, const V& value) {
        int k = getHashKey(key);
        HashNode* newNode = new HashNode(key, value);
        lock_guard<mutex> lk(mutexes[k]);
        newNode->next = v[k];
        v[k] = newNode;
    }
private:
    int m_size;
    std::hash<K> m_hash;
    struct HashNode {
        K key;
        V value;
        HashNode* next; // Separate Chain Hash Table Collision Handling
        HashNode(const K& k, const V& val) : key(k), value(val), next(nullptr) {};
    };
    vector<HashNode*> v;
    vector<mutex> mutexes;
    int getHashKey(const K& key) {
        return m_hash(key) % m_size;
    }
    unsigned int hashString(const string& s) {
        unsigned int key = 33;
        for(unsigned int i = 0; i < s.size(); ++i) {
            key = (1013904223 + key*1664525) % 4294967296;    // % 4294967296 not necessary, but there for the example.
        }
        return key % 100;
    }
};