import Random;
import Operators;

class Collections<Element: Eq> {
    init();
    void shuffle(Element[] arr);
}

implement Collections<Element> {
    init() {

    }
    void shuffle(Element[] arr) {
        for (int i = 0; i < arr.size() - 1; i++) {
            int ri = i + randi() % (arr.size() - 1 - i);
            
            Element tmp = arr[i];
            arr[i] = arr[ri];
            arr[ri] = tmp;
        }
    }
}

export {Collections};