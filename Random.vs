class Random {
    init();
    int iseed;
    int rand();
}

implement Random {
    init() {
        this.iseed = <int>time();
    }
    int rand() {
        this.iseed = this.iseed * 172760327 + 110012327;
        return this.iseed;
    }
}

export {Random};