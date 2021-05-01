import Operators;

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

export {BigInteger};