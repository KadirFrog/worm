#ifndef WORM_SERVER_MANAGER_H
#define WORM_SERVER_MANAGER_H

#include <string>
#include <curl/curl.h>
using namespace std;
namespace server_manager {
    string get_raw_file(const std::string& url);
}

#endif //WORM_SERVER_MANAGER_H