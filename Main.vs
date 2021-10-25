import BigInteger;
import PriorityQueue;
import Quicksort;
import Mergesort;
import Collections;
import Set;

class Main {
    init();
}

implement Main {
    init() {
        // ===========
        // BigInteger
        // ===========
        BigInteger[] arr = [];
        for (int i = 1; i < 10000; i++) {
            arr.push(new BigInteger(i));
        }

        // TODO: Static methods so that you can do
        // Collections<BigInteger>::shuffle
        // rather than instantiate a class for it
        Collections<BigInteger> c = new Collections<BigInteger>();
        c.shuffle(arr);

        // Make a "PriorityQueue" Heap,
        // which requires traits Comparable and Eq
        // to be implemented
        PriorityQueue<BigInteger> heap = new PriorityQueue<BigInteger>();
        heap.insert(arr[0]);
        heap.insert(arr[1]);
        heap.insert(arr[2]);
        // heap.pop() should be the smallest of arr[0],arr[1],arr[2]
        print(arr[0], arr[1], arr[2], heap.pop());

        // ===========
        // int
        // ===========
        Set<int> set = new Set<int>();
        set.insert(10);
        set.insert(10);
        set.insert(5);

        
        print(set);
        print(set.size());
        print(set.contains(6));
        print(set.contains(5));
        
        //print("Ready, set, GO!");
        Quicksort q = new Quicksort();
        Mergesort m = new Mergesort();
        //q.quicksort(arr);

        print(1 & 0);
        print(1 | 0);
        print(1 ^ 0);
        print(~1);
    }
}

Main m = new Main();

export {Main};
