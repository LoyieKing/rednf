#ifndef GAME_CNEXONMSGTABLE_H_
#define GAME_CNEXONMSGTABLE_H_

// df_game_r 还原 —— CNexonMsgTable / g_nexonMsgTable（LinkE2 批属主）。
// ORIG：C1 T 0x0858001c（28 条 msgs 插入 std::map<int,std::string>）、
//       GetMessageString T 0x0858010c（find → c_str，miss → ""）。
// 全局实例 B @0x0945b8e4，sizeof=0x18 == 本工具链 sizeof(std::map<int,std::string>)
// （与 CItemGenerator.h/CClientCrashDownLogStatistic.h 实测口径一致）。

#include <map>
#include <string>

class CNexonMsgTable
{
public:
    CNexonMsgTable();
    ~CNexonMsgTable();               // ORIG D1/D2 T 0x085800f8
    const char* GetMessageString(int idx);   // ORIG 返回 char const*（miss 时返回 ""）

private:
    std::map<int, std::string> m_messages;   // ORIG +0x00，sizeof 0x18
};

extern CNexonMsgTable g_nexonMsgTable;   // B @0x0945b8e4

#endif  // GAME_CNEXONMSGTABLE_H_
