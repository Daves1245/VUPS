class StringUtil {
    int find(string source, string key);
    int findOffset(string source, string key, int offset);
    int findLast(string source, string key);
    bool contains(string source, string key);
    string[] split(string source, string delimeter);
}

implement StringUtil {
    int find(string source, string key) {
        return this.findOffset(source, key, 0);
    }

    int findOffset(string source, string key, int offset) {
        int max = offset + source.size() - key.size();
        for (int i = 0; i < max; i++) {
        // TODO != operator should accept char parameters
            if (source[i] != key[0]) {
                while (++i <= source.size() && source[i] != key[0]);
            }
            int j;
            for (j = i; j < max; j++) {
                if (source[j] != key[j]) {
                    break;
                }
            }
            if (j < i + key.size()) {
                continue;
            } else {
                return j;
            }
        }
        return -1;
    }

    // TODO
    int findLast(string source, string str) {

    }

    bool contains(string source, string str) {
        return find(source, str) != -1;
    }

    string[] split(string source, string delimeter) {
        string[] ret = [];
        int offset = 0;
        int index;
        while ((index = this.findOffset(source, delimeter, offset)) != -1) {
        // TODO fix
            ret.push(new string(source.substring(index, delimeter.size())));
        }

        return ret;
    }
}

export {StringUtil};