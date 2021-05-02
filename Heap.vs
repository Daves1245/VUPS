import Operators;

class Heap<Element: Comparable> {
    Element[] arr;
    int size;
}

implement Heap<Element> {
    void swap(Element[] arr, int a, int b) {
        Element tmp = arr[a];
        arr[a] = arr[b];
        arr[b] = tmp;
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

    void percolateDown(Element[] arr, int i) {
        int swapi = i;
        if (this.left(i) <= this.size && arr[i].compareTo(arr[this.left(i)]) < 0) {
            swapi = this.left(i);
        }
        if (this.right(i) <= this.size && arr[i].compareTo(arr[this.right(i)]) < 0) {
            swapi = this.right(i);
        }
        if (swapi != i) {
            this.swap(arr, swapi, i);
        }
    }

    void percolateUp(Element[] arr, int i) {
        while (i != 0 && arr[i].compareTo(arr[this.parent(i)]) != 0) {
            this.swap(arr, i, this.parent(i));
            i = this.parent(i);
        }
    }

    void insert(Element[] arr, Element val) {
        if (this.size == arr.size()) {
            arr.resize(2 * arr.size());
        }
        this.size++;
        arr[this.size] = val;
        this.percolateUp(arr, this.size);
    }

    Element pop(Element[] arr) {
        Element ret = arr[0];
        this.swap(arr, 0, this.size);
        this.size--;
        this.percolateDown(arr, 0);
        return ret;
    }

    void build_heap(Element[] arr) {
        this.size = arr.size();
        for (int i = this.size; i >= 0; i--) {
            this.percolateDown(arr, i);
        }
    }

    void heapsort(Element[] arr) {
        this.build_heap(arr);
        for (int i = arr.size(); i >= 0; i--) {
            this.swap(arr, i, 0);
            this.size--;
            this.percolateDown(arr, 0);
        }
    }
}

export {Heap};