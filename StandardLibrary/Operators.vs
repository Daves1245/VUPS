trait Eq {
    bool is_equal(Eq e);
}
class Pair<U: any, V: any> {
    U first;
    V second;
    init(U first, V second);
}
implement Pair<U, V> {
    init(U first, V second) {
        this.first = first;
        this.second = second;
    }
}

trait Comparable {
    int compareTo(Comparable e);
}


implement Eq on int {
    bool is_equal(Eq other) {
        if (other is int) {
            int v = <int>other;
            return this == v;
        } else {
            return false;
        }
    }
}


export {Eq,Pair,Comparable};
