import Operators;
import Hashable;

class BigInteger {
    // For now, just a wrapper around int
    int val;
    init(int val);
}

implement BigInteger {
    init(int val) {
        this.val = val;
    }
}

// You can implement traits on classes that have already
// been declared and/or implemented! Very extensible
implement Comparable on BigInteger {
    int compareTo(Comparable i) {
        return this.val - (<BigInteger>i).val;
    }
}

implement Eq on BigInteger {
    bool is_equal(Eq e) {
        return this.val == (<BigInteger>e).val;
    }
}

implement Hash on BigInteger {
    int hash() {
        return this.val * 314159;
    }
}

export {BigInteger};