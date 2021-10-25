import Hashable;
import Operators;

class Set<Key: Hash + Eq> {
    init();
    bool contains(Key k);
    void insert(Key k);
    void remove(Key k);
    int size();
}

implement Set<Key> {
    // Set of key/values
    Key[][] buckets;
    // Refers to buckets._size()
    int capacity;
    // Refers to # of elements in HashMap
    int _size;

    int size() {
        return this._size;
    }

    init() {
        this._size = 0;
        this.capacity = 25;
        this.buckets = [];
        this.buckets.resize(25);
        for(int i = 0; i < 25; i++) {
            this.buckets[i] = [];
        }
    }

    bool contains(Key k) {
        int hash = k.hash();
        Key[] bucket = this.buckets[hash % this.capacity];
        for(int i = 0; i < bucket.size(); i++) {
            if (k.is_equal(bucket[i])) {
                return true;
            }
        }
        return false;
    }

    void insert(Key k) {
        int hash = k.hash();
        Key[] bucket = this.buckets[hash % this.capacity];
        int found = -1;
        for(int i = 0; i < bucket.size(); i++) {
            if (k.is_equal(bucket[i])) {
                // If key is found, Overwrite the value at that location and then return
                return;
            }
        }
        // If key is not found, add to the linked list
        this._size++;
        bucket.push(k);
    }

    void remove(Key k) {
        int hash = k.hash();
        Key[] bucket = this.buckets[hash % this.capacity];
        int found = -1;
        for(int i = 0; i < bucket.size(); i++) {
            if (k.is_equal(bucket[i])) {
                // If found, remove the value at that location and then return
                this._size--;
                bucket.remove(i);
                return;
            }
        }
        // If not found, there's nothing to do
    }
}

export {Set};
