class Stack<Element: any> {
    init(Element[] arr);
    Element[] arr;
    void push(Element e);
    Element pop();
}

implement Stack<Element> {
    init(Element[] arr) {
        this.arr = arr;
    }

    void push(Element e) {
        this.arr.push(e);
    }

    Element pop() {
        if (this.arr.size() == 0) {
            throw "Stack is empty";
        }
        return this.arr.pop();
    }
}

export {Stack};