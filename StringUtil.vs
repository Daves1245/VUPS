import Optional;

class StringUtil {
    int find(string source, string key);
    int findOffset(string source, string key, int offset);
    // int findLast(string source, string key);
    bool contains(string source, string key);
    // string[] split(string source, string delimeter);
    // Optional<int> to_int(string source);
    // Optional<float> to_float(string source);
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
            int j = i;
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
        return this.find(source, str) != -1;
    }

    string[] split(string source, string delimeter) {
        string[] ret = [];
        int offset = 0;
        int index = -1;
        int prev_index = 0;
        
        for (index = this.findOffset(source, delimeter, offset); index != -1; prev_index = index) {
            //ret.push(source.substring(prev_index, index));
        }

        return ret;
    }

    bool isspace(char a) {
        return a == ' ' || a == '\t' || a == '\n';
    }

    Optional<int> to_int(string source) {

    }

    Optional<float> to_float(string source) {

    }
}

export {StringUtil};