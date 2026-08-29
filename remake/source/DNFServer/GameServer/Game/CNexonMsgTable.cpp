// df_game_r 还原 —— CNexonMsgTable（LinkE2 批）。
// 报告：docs/class_func_reports/CNexonMsgTable/{CNexonMsgTable,GetMessageString}.md
// ctor @0x0858001c：构造 map<int,std::string>，循环 0x1c=28 次插入
//   make_pair(msgs[i].key, msgs[i].str)（ORIG 全局表 msgs D @0x093abb80，
//   stride 0x8 = {int key; const char* str}，0xe0 = 28×0x8）。
// GetMessageString @0x0858010c：find(idx)；it==end → ""（ORIG &DAT_08ca9390），
//   否则 it->second.c_str()。
#include "CNexonMsgTable.h"

#include <cassert>
#include <cstddef>

// ORIG msgs 全局表逐条转写（objdump -s .data @0x093abb80；字符串指针解引用自
// .rodata 0x08ca91xx/0x08ca92xx/0x08ca93xx 区）。键序即 ORIG 存储序。
struct NexonMsgEntry
{
    int key;
    const char* str;
};

NexonMsgEntry msgs[28] =
{
    { 0x00, "nexon_pcroom_msg_00" },
    { 0x01, "nexon_pcroom_msg_01" },
    { 0x02, "nexon_pcroom_msg_02" },
    { 0x03, "nexon_pcroom_msg_03" },
    { 0x04, "nexon_pcroom_msg_04" },
    { 0x05, "nexon_pcroom_msg_05" },
    { 0x06, "nexon_pcroom_msg_06" },
    { 0x0a, "nexon_pcroom_msg_10" },
    { 0x0b, "nexon_pcroom_msg_11" },
    { 0x0d, "nexon_pcroom_msg_13" },
    { 0x0e, "nexon_pcroom_msg_14" },
    { 0x0f, "nexon_pcroom_msg_15" },
    { 0x10, "nexon_pcroom_msg_16" },
    { 0x11, "nexon_pcroom_msg_17" },
    { 0x12, "nexon_pcroom_msg_18" },
    { 0x13, "nexon_pcroom_msg_19" },
    { 0x16, "nexon_pcroom_msg_22" },
    { 0x17, "nexon_pcroom_msg_23" },
    { 0x18, "nexon_pcroom_msg_24" },
    { 0x19, "nexon_pcroom_msg_25" },
    { 0x1a, "nexon_pcroom_msg_26" },
    { 0x1b, "nexon_pcroom_msg_27" },
    { 0x5a, "nexon_pcroom_msg_90" },
    { 0x5b, "nexon_pcroom_msg_91" },
    { 0x5c, "nexon_pcroom_msg_92" },
    { 0x5d, "nexon_pcroom_msg_93" },
    { 0x5e, "nexon_pcroom_msg_94" },
    { 0x5f, "nexon_pcroom_msg_95" },
};

static_assert(sizeof(NexonMsgEntry) == 0x8, "ORIG msgs stride");
static_assert(sizeof(CNexonMsgTable) == 0x18, "ORIG g_nexonMsgTable B size (== sizeof std::map)");

CNexonMsgTable g_nexonMsgTable;   // ORIG B @0x0945b8e4

CNexonMsgTable::CNexonMsgTable()   // ORIG T 0x0858001c
{
    for (int i = 0; i < 0x1c; ++i)
        m_messages.insert(std::make_pair(msgs[i].key, msgs[i].str));
}

const char* CNexonMsgTable::GetMessageString(int idx)   // ORIG T 0x0858010c
{
    std::map<int, std::string>::iterator it = m_messages.find(idx);
    if (it != m_messages.end())
        return it->second.c_str();
    return "";                            // ORIG "" @0x08ca9390
}

CNexonMsgTable::~CNexonMsgTable()   // ORIG D1/D2 T 0x085800f8（map 成员逆序析构）
{
}
