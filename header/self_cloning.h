
#ifndef WORM_SELF_CLONING_H
#define WORM_SELF_CLONING_H
#include "windows.h"
#include <iostream>
#include <vector>
using namespace std;
namespace clone_system {
    extern vector<char> exe_path;
    void setup();
}
#endif //WORM_SELF_CLONING_H
