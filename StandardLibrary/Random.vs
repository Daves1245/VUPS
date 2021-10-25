class InternalRandom {
    init();
    int iseed;
    int rand();
}

implement InternalRandom {
    init() {
        this.iseed = <int>time();
    }
    int rand() {
        this.iseed = this.iseed * 172760327 + 110012327;
        return this.iseed;
    }
}

InternalRandom Random = new InternalRandom();

export {Random};