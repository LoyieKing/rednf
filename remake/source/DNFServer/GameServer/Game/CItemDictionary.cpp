// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemDictionary/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CItemDictionary.h"
#include <cstring>

#include "CUser.h"   // 经 SigTypes.h 提供权威 SIG_ITEM_DICTIONARY_FLAG（0x138c）
#include "CDataManager.h"  // G_CDataManager / find_item / m_mapA928/m_mapA940
#include "CItem.h"         // CItem::is_stackable
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "StreamPool.h"

// ORIG 少量 TU 各一份 _ZL18kItemDictionaryBit（r 0x1）：字节位图每字节位数。
static const unsigned char kItemDictionaryBit = 8;

namespace {
// ORIG 无对应数据符号（nm 实证）；用 enum 常量避免发射仅-NEW 的
// (anonymous namespace)::kBinaryLength 只读符号（P154）。
enum { kBinaryLength = 5000 };
}

// ORIG 0x0811d5d4：仅 CDataC1（置基类 vptr）+ 编译器置派生 vptr，不调用 _reset。
#include "CItemDictionary/CItemDictionary.inc"
#include "CItemDictionary/dtor_CItemDictionary.inc"
// ORIG 0x0811d774：memset(this+5,0,0x1388)；不触碰 m_gainItemList。
#include "CItemDictionary/_reset.inc"
// ORIG 0x0811d63e：DB 保存（SIG_ITEM_DICTIONARY_FLAG = int characNo +
// 5000 字节位图，共 0x138c），队列 DB_Q(2)。
#include "CItemDictionary/_saveData.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemDictionary.cpp", 0x18), true);
    **guard << 0x2b2;
    **guard << (int)user->GetUID();
    SIG_ITEM_DICTIONARY_FLAG* sig =
        guard->GetInBuffer<SIG_ITEM_DICTIONARY_FLAG>();
    std::memset(sig, 0, 0x138c);
    sig->m_characNo = user->get_charac_no(-1);
    std::memcpy(sig->m_data, m_data, kBinaryLength);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}
// ORIG 0x0811d7e0：memcpy(out+4, this+5, 0x1388)，无空检查。
// ORIG out 参数先物化到局部（-0xc）再回读 +4 ⇒ 源码经局部指针。
#include "CItemDictionary/getData.inc"
{
    char* dst = out;
    std::memcpy(dst + 4, m_data, kBinaryLength);
}
#include "CItemDictionary/getGainItemCount.inc"
{
    int count = 0;
    for (int i = 0; i < static_cast<int>(kBinaryLength); ++i)   // ORIG setle（有符号）
    {
        unsigned char byte = m_data[i];   // ORIG byte 物化局部（-0x9）
        for (int bit = 0; bit <= 7; ++bit)   // ORIG cmpl $0x7;setle（有符号 <=7）
        {
            if ((byte >> bit) & 1)
            {
                ++count;
            }
        }
    }
    return count;
}
// ORIG 0x0811dba6（ret $0x4 克隆；逐条转写）：遍历位图（+0x5 起），置位处
// itemIdx=i*8+bit 经 G_CDataManager()->m_mapA940（+0xa940）翻译后
// push_back(map[itemIdx])；map 无此键则跳过。
// ORIG 为按值返回（ret $0x4 NRV 隐藏参数 @ ebp+8、this @ ebp+0xc；引用返回
// 少 NRV 参数、直接返回 &成员，符号同 mangled 但语义不同），故用局部对象。
#include "CItemDictionary/getGainItemList.inc"
// ORIG 0x0811da32：index=flag>>3、bit=flag&7 越界（index∉[0,0x1387] 或 bit∉[0,7]）
// → logFormat 后返回 1；否则返回 (m_data[index]>>bit)&1。item 参数仅入日志。
#include "CItemDictionary/getItemCheckFlag.inc"
{
    int index = (int)(flag >> 3);
    int bit = (int)(flag & 7);
    if (index < 0 || index > 0x1387 || bit < 0 || bit > 7)
    {
        LogManager::logFormat(
            1, "ItemDictionary.cpp",
            __PRETTY_FUNCTION__,
            0x7d,
            "getItemCheckFlag() characNo=%u, item_flag=%u, index=%d, flag=%d",
            item, flag, index, bit);
        return 1;
    }
    unsigned char v = m_data[index];   // ORIG 局部（栈 -0x9）
    if (!(v >> bit & 1))   // ORIG xor $0x1 取反形
    {
        return 0;
    }
    return 1;
}

// ORIG 0x0811db60：最后一个非零字节下标+1（无非零字节返回 0）。
#include "CItemDictionary/getLengthBinary.inc"
{
    int length = 0;
    for (int i = 0; i < static_cast<int>(kBinaryLength); ++i)
    {
        if (m_data[i] != 0)
            length = i + 1;
    }
    return length;
}
// ORIG 0x0811d79a：先经虚表槽7 调 _reset()，再 memcpy(data+4 → this+5, 0x1388)，返回 true。
#include "CItemDictionary/loadData.inc"
{
    // N7 封顶：虚调用函数指针/this 载入寄存器 ORIG(edx/eax) 与 4.4(eax/edx) 互换，
    // 助记符序列全等，跨版本寄存器分配差。
    _reset();
    char* p = data;   // ORIG data 物化到局部（-0xc）再回读 +4
    std::memcpy(m_data, p + 4, kBinaryLength);
    return true;
}
// ORIG 0x0811dce6：向用户发送单项词典标记（接口包 0x181：byte=1 + int item）。
#include "CItemDictionary/sendItemDictionaryInfo.inc"
{
    if (!user) return;
    PacketGuard guard;
    guard.put_header(0, 0x181);
    guard.put_byte(1);
    guard.put_int(static_cast<int>(item));
    guard.finalize(true);
    user->Send(guard);
}
#include "CItemDictionary/sendItemDictionaryInfoTotal.inc"
{
    if (user == 0)
        return;
    PacketGuard guard;
    guard.put_header(0, 0x18a);
    const int length = getLengthBinary();
    if (length == 0)
        guard.put_int(0);                     // ORIG 0x0811dde6：空词典显式写 0
    else
    {
        guard.put_int(length);
        // ORIG 0x0811de24 → _ZN18InterfacePacketBuf10put_binaryEPci（非 const 重载，
        // W 0x0811df08）；传 const 会选到 PKci（0x0815096a）错目标。
        guard.put_binary(reinterpret_cast<char*>(m_data), length);
    }
    guard.finalize(true);
    user->Send(guard);
}
// ORIG 0x0811d80e：user==0 返回；item 存在（find_item）且 is_stackable → 返回
// （仅"存在且非堆叠"继续）；m_mapA928.find(item) 命中则
// setItemCheckFlag(user, item, map[item])。
#include "CItemDictionary/setGainItemFlag.inc"
{
    if (user == 0)
        return;
    CItem* ci = G_CDataManager()->find_item(static_cast<int>(item));
    // 语义纠错（b28 lane9）：ORIG 0x0811d833 处 `cmpl $0,-0x10; je <mov $1>`——
    // find_item 返回空指针时物化 bVar1=true 直接早退（与 is_stackable 命中同出口）；
    // 旧写法 `ci != 0 && is_stackable()` 短路使 ci==0 落入主体、错调 setItemCheckFlag。
    // 恢复 Ghidra C 原式 (ci == 0 || is_stackable) 为真即返回。
    if (ci == 0 || ci->is_stackable())
    {
        return;
    }
    std::map<int, int>::iterator it =
        G_CDataManager()->m_mapA928.find(static_cast<int>(item));
    if (it == G_CDataManager()->m_mapA928.end())   // ORIG call eq;jne 早退形
    {
        return;
    }
    const unsigned int flag = static_cast<unsigned int>(it->second);
    setItemCheckFlag(user, item, flag);
}
// ORIG 0x0811d9d7：user==0 直接返回；index=flag>>3、bit=flag&7 越界（
// index∉[0,0x1387] 或 bit∉[0,7]）→ logFormat 后返回；getItemCheckFlag(-1,flag)
// 已置位则跳过；否则 m_data[index] += (1<<bit)（加法非 or），
// sendItemDictionaryInfo + CData::alter()。
#include "CItemDictionary/setItemCheckFlag.inc"
                                       unsigned int flag)
{
    if (user == 0)
        return;
    int index = (int)(flag >> 3);
    int bit = (int)(flag & 7);
    if (index < 0 || index > 0x1387 || bit < 0 || bit > 7)
    {
        LogManager::logFormat(
            1, "ItemDictionary.cpp",
            __PRETTY_FUNCTION__,
            0x62,
            "setItemCheckFlag() characNo=%u, item_flag=%u, index=%d, flag=%d",
            user->get_charac_no(-1), flag, index, bit);
        return;
    }
    unsigned char v = m_data[index];   // ORIG 死读（保留以匹配指令序列）
    if (!getItemCheckFlag(user->get_charac_no(-1), flag))
    {
        m_data[index] =
            static_cast<unsigned char>(m_data[index] + (1 << bit));
        sendItemDictionaryInfo(user, item);
        alter();
        return;
    }
    else
    {
    }
    return;
}
