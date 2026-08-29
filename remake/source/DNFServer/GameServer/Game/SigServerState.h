// arad::SigServerState —— ORIG W 声明头（仅声明，无实现）。
// ORIG：C1/C2 W 0x08186722、init() 0x0818676d、set(bool,int,int,AradServerStateMessage const&)。
// 佐证：Arad_ServerStateManager.cpp —— sig->m_data[i]（tagData，0x14 字节）、
// tagData.m_code @ tag+0x9、tagData.m_msg @ tag+0xd；put_binary(&sig, 0x195)。
// 全部偏移【推断：字节镜像】，最终以逐指令收敛 TU 为准。
#ifndef DNF_GAME_SIGSERVERSTATE_H_
#define DNF_GAME_SIGSERVERSTATE_H_
#include <cstddef>

namespace arad
{

class SigServerState
{
public:
    class tagData
    {
    public:
        tagData();
        unsigned char m_raw[0x14];  // 0x14 字节镜像；已知 m_code@+0x9、m_msg@+0xd【推断】
    };

    SigServerState();
    void init();
    void set(bool endFlag, int category, int code, AradServerStateMessage const& msg);

    unsigned char m_raw[0x195];  // ORIG put_binary 总长 0x195【推断】（含 vptr/计数字段）
};

}  // namespace arad

#endif  // DNF_GAME_SIGSERVERSTATE_H_