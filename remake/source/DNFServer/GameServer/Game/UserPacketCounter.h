#ifndef DNF_GAME_USERPACKETCOUNTER_H_
#define DNF_GAME_USERPACKETCOUNTER_H_

// ============================================================================
// message_counter::CUserPacketCounter —— 按账号统计包量并周期落盘（G4 域叶子）。
// ORIG 权威证据：
//   ctor  W 0x0847e8d2 (_ZN15message_counter18CUserPacketCounterC1EPcS1_)
//   dtor  W 0x0847e916 (_ZN15message_counter18CUserPacketCounterD1Ev)
//   IncreasePacket T 0x0856ff24 (_ZN15message_counter18CUserPacketCounter14IncreasePacketEj)
//   WriteLog       T 0x0856fff0 (_ZN15message_counter18CUserPacketCounter8WriteLogEv)
// 布局（ORIG sizeof = 0x11c）：
//   +0x000 std::map<unsigned int,int>   （acc_id → 包计数，24B）
//   +0x018 char m_logPath[0x100]        （ctor: sprintf("./log/%s/%s", a, b)）
//   +0x118 unsigned int m_totalCount
// 注：本头按验收约定以 const char* 声明 ctor（符号 C1EPKcS2_）；ORIG 二进制为
// char*（C1EPcS1_），语义一致。
// ============================================================================

#include <map>

namespace message_counter
{

class CUserPacketCounter
{
public:
    CUserPacketCounter(const char* dir, const char* name);
    ~CUserPacketCounter();

    void IncreasePacket(unsigned int accId);   // accId 命中 ++value，否则 insert(accId,1)；最后 ++total
    void WriteLog();                           // 首行 total_count(%d)，逐条 "(%s,%12d)"，随后清零

private:
    std::map<unsigned int, int> m_counter;  // +0x000
    char m_logPath[0x100];                  // +0x018
    unsigned int m_totalCount;              // +0x118
};

}  // namespace message_counter

#endif  // DNF_GAME_USERPACKETCOUNTER_H_
