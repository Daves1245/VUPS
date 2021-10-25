import Operators;

class PriorityQueue<Element: Comparable> {
    init();
    Element pop();
    void push(Element e);
    int size();
}

implement PriorityQueue<Element> {
    Element[] arr;
    int _size;

    init() {
        this.arr = [];
        this.arr.resize(1);
        this._size = 0;
    }

    int size() {
        return this._size;
    }

    void swap(int a, int b) {
        Element tmp = this.arr[a];
        this.arr[a] = this.arr[b];
        this.arr[b] = tmp;
    }

    int parent(int i) {
        return i / 2;
    }
    int left(int i) {
        return 2 * i;
    }
    int right(int i) {
        return 2 * i + 1;
    }

    void percolateDown(int i) {
        int swapi = i;
        if (this.left(i) <= this.size() && this.arr[i].compareTo(this.arr[this.left(i)]) < 0) {
            swapi = this.left(i);
        }
        if (this.right(i) <= this.size() && this.arr[i].compareTo(this.arr[this.right(i)]) < 0) {
            swapi = this.right(i);
        }
        if (swapi != i) {
            this.swap(swapi, i);
        }
    }

    void percolateUp(int i) {
        while (i > 1 && this.arr[i].compareTo(this.arr[this.parent(i)]) != 0) {
            this.swap(i, this.parent(i));
            i = this.parent(i);
        }
    }

    void insert(Element val) {
        this._size++;
        if (this.size() == this.arr.size()) {
            this.arr.resize(2 * this.arr.size());
        }
        this.arr[this.size()] = val;
        this.percolateUp(this.size());
    }

    void push(Element e) {
        this.insert(e);
    }

    Element pop() {
        Element ret = this.arr[1];
        this.swap(1, this.size());
        this._size--;
        this.percolateDown(1);
        return ret;
    }
}

export {PriorityQueue};