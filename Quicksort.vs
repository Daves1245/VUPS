class Quicksort {
    void quicksort(int[] arr, int len);
}

implement Quicksort {
    void swap(int[] arr, int a, int b) {
        int tmp = arr[a];
        arr[a] = arr[b];
        arr[b] = tmp;
    }


    int partition(int[] arr, int l, int h) {
        int p = arr[h];
        int ei = l - 1;

        for (int i = l; i < h; i++) {
            if (arr[i] <= p) {
                ei++;
                this.swap(arr, ei, i);
            }
        }
        this.swap(arr, ei + 1, h);
        return ei + 1;
    }

    void __quick_sort(int[] arr, int l, int h){
        if (l < h) {
            int pi = this.partition(arr, l, h);
            this.__quick_sort(arr, l, pi - 1);
            this.__quick_sort(arr, pi + 1, h);
        }
    }

    void quicksort(int[] arr, int len) {
        this.__quick_sort(arr, 0, len);
    }
}

export {Quicksort};
