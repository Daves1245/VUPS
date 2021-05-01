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
        int[] arr = [50];
        for (int i = 1; i < 100000; i++) {
            arr.push(100000 - i);
        }
        Quicksort q = new Quicksort();
        //Mergesort m = new Mergesort();
        //print(arr);
        print("ready, set, GO!");
        q.quicksort(arr);
    }
}

Main m = new Main();

export {Main};