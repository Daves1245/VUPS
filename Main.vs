import BigInteger;
import Heap;
import Quicksort;
import Mergesort;
import Collections;
import Set;

typedef bigfunction = () => bool;

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

        Collections<BigInteger> c = new Collections<BigInteger>();
        /*
        BigInteger[] numbers = [new BigInteger(1), new BigInteger(2), new BigInteger(3),
            new BigInteger(4), new BigInteger(5), new BigInteger(6), new BigInteger(7),
            new BigInteger(8), new BigInteger(9), new BigInteger(10)];
        c.shuffle(arr);
        */
        //print(arr);

        Set<BigInteger> set = new Set<BigInteger>();
        set.insert(new BigInteger(10));
        set.insert(new BigInteger(10));
        set.insert(new BigInteger(5));

        
        print(set);
        print(set.size());
        print(set.contains(new BigInteger(6)));
        print(set.contains(<bigfunction>(new BigInteger(5)))());
        
        //print("Ready, set, GO!");
        Quicksort q = new Quicksort();
        Mergesort m = new Mergesort();
        //q.quicksort(arr);
    }
}

Main m = new Main();

export {Main};
