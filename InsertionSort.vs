class InsertionSort {
    void sort(int[] arr, int size);
}

implement InsertionSort {
    void sort(int[] arr, int size) {
        for (int i = 1; i < size; i++) {
            int j = i - 1;
            int key = arr[i];
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }
}

export {InsertionSort};
