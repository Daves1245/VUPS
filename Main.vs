import BigInteger;
import Heap;
import Quicksort;
import Mergesort;
import Collections;

class Main {
    init();
}

implement Main {
    init() {
    /*
        Heap<BigInteger> heap = new Heap<BigInteger>();
        BigInteger[] arr = [50];
        for (int i = 1; i < 10000; i++) {
            arr.push(new BigInteger(10000 - i));
        }
        */
        Collections<BigInteger> c = new Collections<BigInteger>();
        BigInteger[] arr = [];/*[new BigInteger(1), new BigInteger(2), new BigInteger(3),
            new BigInteger(4), new BigInteger(5), new BigInteger(6), new BigInteger(7),
            new BigInteger(8), new BigInteger(9), new BigInteger(10)]; */
        c.shuffle(arr);
        //print(arr);

        //print("Ready, set, GO!");
        //heap.heapsort(arr);
        //Quicksort q = new Quicksort();
        //Mergesort m = new Mergesort();
        //print(arr); q.quicksort(arr);
    }
}

Main m = new Main();

export {Main};