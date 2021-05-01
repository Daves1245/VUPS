class Mergesort {
    void mergesort(int[] arr);
}

implement Mergesort {
    void merge(int[] arr, int l, int m, int r) {
        int a = 0;
        int b = 0;
        int c = l;
        int n1 = m - l + 1;
        int n2 = r - m;
        int[] left = [];
        int[] right = [];
        
        for (int i = 0; i < n1; i++) {
            left.push(arr[l + i]);
        }
        for (int i = 0; i < n2; i++) {
            right.push(arr[m + 1 + i]);
        }

        while (a < n1 && b < n2) {
            if (left[a] <= right[b]) {
                arr[c] = left[a];
                a++;
            } else {
                arr[c] = right[b];
                b++;
            }
            c++;
        }
        while (a < n1) {
            arr[c] = left[a];
            a++;
            c++;
        }
        while (b < n2) {
            arr[c] = right[b];
            b++;
            c++;
        }
    }

    void _mergesort(int[] arr, int l, int r) {
        if (l < r) {
            int m = l + (r - l) / 2;
            this._mergesort(arr, l, m);
            this._mergesort(arr, m + 1, r);
            this.merge(arr, l, m, r);
        }
    }

    void mergesort(int[] arr) {
        this._mergesort(arr, 0, arr.size() - 1);
    }
}

export {Mergesort};