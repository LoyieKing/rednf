// ============================================================================
// UserPacketCounter.cpp —— message_counter::CUserPacketCounter 实现
//   + cUserHistoryLog::AbnormalUser（ORIG 0x08686df4，声明在 CUser.h）。
// 全部函数体按 ORIG df_game_r objdump 逐指令转写。
// ============================================================================

#include "UserPacketCounter.h"
#include "CMyFileLog.h"
#include "CUser.h"       // cUserHistoryLog 权威声明 / CUser::LogHistory

#include <stdio.h>
#include <utility>       // std::make_pair

// ORIG 自由函数（权威声明 CInventory.h:408；此处按项目惯例局部声明，签名一致）
char* NumberToString(unsigned int value, int radix);

namespace message_counter
{

// ORIG W 0x0847e8d2：map 默认构造（编译器生成）+ total=0 +
// sprintf(m_logPath, "./log/%s/%s", dir, name)。
CUserPacketCounter::CUserPacketCounter(char* dir, char* name)
{
    m_totalCount = 0;
    sprintf(m_logPath, "./log/%s/%s", dir, name);
}

// ORIG W 0x0847e916：仅 map 析构（其余成员平凡）。
CUserPacketCounter::~CUserPacketCounter()
{
}

// ORIG T 0x0856ff24：
//   find(accId) != end → ++it->second；否则 insert(make_pair(accId, 1))；
//   最后 ++m_totalCount（+0x118）。
void CUserPacketCounter::IncreasePacket(unsigned int accId)
{
    std::map<unsigned int, int>::iterator it = m_counter.find(accId);
    if (it != m_counter.end())
    {
        ++it->second;
    }
    else
    {
        m_counter.insert(std::make_pair(accId, 1));
    }
    ++m_totalCount;
}

// ORIG T 0x0856fff0：
//   CMyFileLog(m_logPath, 0x9d)(m_logPath, "user_packet_counter total_count(%d)", total)
//   遍历 map：idStr = NumberToString(key, 0)；
//     CMyFileLog(m_logPath, 0xa0)(m_logPath, "(%s,%12d)", idStr, value)
//   收尾：total=0、map.clear()。
// （ORIG 每条记录都新建一个 CMyFileLog，忠实保留。）
void CUserPacketCounter::WriteLog()
{
    CMyFileLog head(m_logPath, 0x9d);
    head(m_logPath, "user_packet_counter total_count(%d)", m_totalCount);

    for (std::map<unsigned int, int>::iterator it = m_counter.begin();
         it != m_counter.end(); ++it)
    {
        int value = it->second;              // ORIG 先取 second 再取 first
        unsigned int accId = it->first;
        char* idStr = NumberToString(accId, 0);
        CMyFileLog entry(m_logPath, 0xa0);
        entry(m_logPath, "(%s,%12d)", idStr, value);
    }

    m_totalCount = 0;
    m_counter.clear();
}

}  // namespace message_counter

// ----------------------------------------------------------------------------
// ORIG T 0x08686df4 (_ZN15cUserHistoryLog12AbnormalUserE27ENUM_ABNORMAL_USER_LOG_TYPEtii)：
//   eax = *(this)（+0x00 m_user，无空检查）；以
//   m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d", type, value, a3, a4) 转发。
// 格式串 ORIG @0x8cf1037 已核对。
// ----------------------------------------------------------------------------
void cUserHistoryLog::AbnormalUser(ENUM_ABNORMAL_USER_LOG_TYPE type,
                                   unsigned short value, int a3, int a4)
{
    m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d",
                       (unsigned int)type, (unsigned int)value, a3, a4);
}
