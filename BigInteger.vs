import Operators;
import Hashable;

class BigInteger {
    int val; // For now, just a wrapper around int to have trait comparable
    init(int val);
}

implement BigInteger {
    init(int val) {
        this.val = val;
    }
}

implement Comparable on BigInteger {
    int compareTo(Comparable i) {
        return this.val - (<BigInteger>i).val;
    }
}

implement Eq on BigInteger {
    bool is_equal(Eq e) {
        return this.val == <int>((<BigInteger>e).val);
    }
}

implement Hash on BigInteger {
    int hash() {
        return this.val * 314159;
    }
}

export {BigInteger};