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

export {Eq,Pair,Comparable};
