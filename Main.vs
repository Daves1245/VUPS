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
        BigInteger[] arr = [50];
        for (int i = 1; i < 10000; i++) {
            arr.push(new BigInteger(10000 - i));
        }
        print("Ready, set, GO!");
        heap.heapsort(arr);
        //Quicksort q = new Quicksort();
        //Mergesort m = new Mergesort();
        //print(arr); q.quicksort(arr);
    }
}

Main m = new Main();

export {Main};