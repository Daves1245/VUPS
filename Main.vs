import BigInteger;
import Heap;
import Quicksort;
import Mergesort;

class Main {
    init();
}

implement Main {
    init() {
        Heap<BigInteger> heap = new Heap<BigInteger>();
        int[] arr = [10, 9, 8, 7, 6, 5, 4, 3, 2, 1];
        Quicksort q = new Quicksort();
        Mergesort m = new Mergesort();
        //q.quicksort(arr, 9);
        m.mergesort(arr);
        for (int i = 0; i < 10; i++) {
            raw_print(arr[i], " ");
        }
    }
}

Main m = new Main();

export {Main};