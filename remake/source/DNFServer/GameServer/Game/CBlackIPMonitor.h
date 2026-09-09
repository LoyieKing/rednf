#ifndef DNF_GAME_CBLACKIPMONITOR_H_
#define DNF_GAME_CBLACKIPMONITOR_H_

// WongWork::CBlackIPMonitor —— 黑名单 IP 惩罚监视表。
// 参照 docs/class_func_reports/（DB_BlackIPMonitor/Inter_BlackIPMonitor）与 df_game_r 反汇编。
//
// 数据结构：
//   - 类持有 std::multimap<unsigned int, stBlackIPMonitorEntry_t>（ctor 0x82a6526 构造；
//     reset 0x82a6512 调 multimap::clear）。
//   - stBlackIPMonitorEntry_t 16 字节（ctor 0x84eba2c 逐字段写）：
//       +0x00 ENUM_BLACKIPMONITOR_PUNISH_TYPE m_punishType   （DB type 列）
//       +0x04 unsigned int                     m_idCnt       （DB m_id_cnt 列）
//       +0x08 long                             m_startTime   （unix 时间戳）
//       +0x0c long                             m_endTime     （unix 时间戳）
//   key = inet_addr(ip)（host byte order → 网络序整数）。
//
// 方法（全部 W 弱符号，ORIG）：
//   CBlackIPMonitor() 0x82a6526  构造 multimap
//   reset()           0x82a6512  multimap::clear()
//   insertNewIP(uint ip, ENUM type, uint cnt, long start, long end) 0x84eba54
//                     构造 entry 并 multimap::insert
//   deleteIP(char const*, uint type) 0x84ebace  遍历 [lower_bound,upper_bound) 删除 type 匹配项
//   getPunishType(char const*) 0x84ebba6  遍历 [lower,upper) 累 OR entry.m_punishType
//   GetEndTime(char const*, ENUM type) 0x84ebc8c  返回 type 匹配项的 m_endTime，否则 0
//
// 枚举数值（推断）：DB type 列取值，用于按位 OR（getPunishType）故为位标志；具体 bit
// 语义未还原，此处以通用位值占位（1/2/4/8…），不参与跨 TU 数据解释。

#include <map>

namespace WongWork
{
enum ENUM_BLACKIPMONITOR_PUNISH_TYPE
{
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_NONE = 0,
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_1   = 1,
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_2   = 2,
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_4   = 4,
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_8   = 8,
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_16  = 16,
    ENUM_BLACKIPMONITOR_PUNISH_TYPE_32  = 32
};

struct stBlackIPMonitorEntry_t
{
    // ORIG 仅此一个 ctor：0x84eba2c（W）_ZN8WongWork23stBlackIPMonitorEntry_tC1E
    //   NS_31ENUM_BLACKIPMONITOR_PUNISH_TYPEEjll。无默认 ctor（multimap 侧走
    //   insert(make_pair(ip, entry))，lower_bound/upper_bound/erase 不需 value
    //   默认构造），故原「stBlackIPMonitorEntry_t()」声明为发明符号，删除。
    stBlackIPMonitorEntry_t(ENUM_BLACKIPMONITOR_PUNISH_TYPE punishType,
                            unsigned int idCnt, long startTime, long endTime);
                                // 0x84eba2c（W）

    ENUM_BLACKIPMONITOR_PUNISH_TYPE m_punishType;  // +0x00
    unsigned int m_idCnt;                          // +0x04
    long m_startTime;                              // +0x08
    long m_endTime;                                // +0x0c
};

class CBlackIPMonitor
{
public:
    CBlackIPMonitor();                       // 0x82a6526
    void reset();                            // 0x82a6512
    void insertNewIP(unsigned int ip, ENUM_BLACKIPMONITOR_PUNISH_TYPE type,
                     unsigned int idCnt, long startTime, long endTime);  // 0x84eba54
    char deleteIP(char const* ipStr, unsigned int type);                  // 0x84ebace（ORIG test %al 返回单字节，W6 批）
    unsigned int getPunishType(char const* ipStr);                       // 0x84ebba6
    long GetEndTime(char const* ipStr, ENUM_BLACKIPMONITOR_PUNISH_TYPE type);  // 0x84ebc8c

private:
    std::multimap<unsigned int, stBlackIPMonitorEntry_t> m_monitorMap;  // +0x00
};

}  // namespace WongWork

#endif  // DNF_GAME_CBLACKIPMONITOR_H_
