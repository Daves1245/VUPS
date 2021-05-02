import Random;
import Operators;

class Collections<Element: Eq> {
    init();
    Random r;
    void shuffle(Element[] arr);
}

implement Collections<Element> {
    init() {
        this.r = new Random();
    }
    void shuffle(Element[] arr) {
        Element[] tmp = [];
        for (int i = 0; i < arr.size(); i = i) {
            bool unused = true;
            int ri = this.r.rand() % arr.size();
            for (int j = 0; j < tmp.size(); j++) {
                if (tmp[j].is_equal(arr[ri])) {
                    unused = false;
                    break;
                }
            }
            
            if (unused) {
                tmp[i] = arr[ri];
                i++;
            }
        }

        arr = tmp;
    }
}

export {Collections};