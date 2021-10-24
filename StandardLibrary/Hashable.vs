trait Hash {
    int hash();
}

implement Hash on int {
    int hash() {
        int ret = 357232049 * this + 1037126747;
        if (ret < 0) {
            ret = -ret;
        }
        return ret;
    }
}

export {Hash};