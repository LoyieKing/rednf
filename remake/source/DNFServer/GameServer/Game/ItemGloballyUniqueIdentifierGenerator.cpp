// df_game_r 还原 —— CItemGloballyUniqueIdentifierGenerator / itemGloballyUniqueIdentifier_t
// ORIG 证据（df_game_r nm -S / objdump -d，报告缺失按 §3.2 objdump 直转）：
//   reset()  W 0x085139e8：char 循环 i=0..9 把 this 前 10 字节清零（setle 判界）。
//   generate T 0x0889246c：SetServerId(field)；SetDatetime(time(0))；
//            SetSequenceNo((ushort)getSeq())。
//   getSeq   W 0x088947e2：++m_seq；若 (unsigned)m_seq > 0xea5f 则归零；返回。
#include "ItemGloballyUniqueIdentifierGenerator.h"
#include <time.h>   // time（generate SetDatetime）

void itemGloballyUniqueIdentifier_t::reset()
{
    // ORIG 0x85139e8：循环变量为 char（movsbl 读/addb 自增/cmpb $9 判界）。
    for (char i = 0; i <= 9; ++i)
        reinterpret_cast<char*>(this)[i] = 0;
}

unsigned short CItemGloballyUniqueIdentifierGenerator::getSeq()
{
    ++m_seq;
    if (m_seq > 0xea5f)
        m_seq = 0;
    return m_seq;
}

itemGloballyUniqueIdentifier_t* CItemGloballyUniqueIdentifierGenerator::generate(
    itemGloballyUniqueIdentifier_t* id, int field)
{
    id->SetServerId(field);
    id->SetDatetime(static_cast<int>(time(0)));
    id->SetSequenceNo(getSeq());
    return id;   // ORIG 0x88947e2 附近末尾 mov 0xc(%ebp),%eax
}

CItemGloballyUniqueIdentifierGenerator g_ItemGloballyUniqueIdentifierGenerator;
