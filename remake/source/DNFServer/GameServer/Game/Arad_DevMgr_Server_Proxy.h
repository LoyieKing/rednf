// df_game_r 还原 —— Arad_DevMgr_Server_Proxy（ORIG localjapan/Arad_DevMgr_Server_Proxy.cpp）。
// 布局证据：docs/class_func_reports/Arad_DevMgr_Server_Proxy/
//   +0x00 int（C1 存 param_2；init 置 30000 作为连接端口）
//   +0x04 std::string m_ip（默认 "127.0.0.1"）
//   +0x08 CNetwork<6144,100000>（C1 构造；init/disconnect/recv/send 直访）
#ifndef DNF_GAME_ARAD_DEVMGR_SERVER_PROXY_H_
#define DNF_GAME_ARAD_DEVMGR_SERVER_PROXY_H_

#include <string>

#include "CNetwork.h"

class Arad_DevMgr_Server_Proxy
{
public:
    Arad_DevMgr_Server_Proxy(std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int);
    void disconnect();
    int init();
    int open();
    int recv();
    int send(char*, int);

    // ---- 对象布局（ORIG 直访成员，全部 public）----
    int m_port;                       // +0x00
    std::string m_ip;                 // +0x04
    CNetwork<6144, 100000> m_network; // +0x08
};


#endif // DNF_GAME_ARAD_DEVMGR_SERVER_PROXY_H_
