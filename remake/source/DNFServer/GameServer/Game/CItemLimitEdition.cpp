// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemLimitEdition/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CItemLimitEdition / CItemLimitEditionMgr 实现。
// 唯一权威声明见 CItemLimitEdition.h。逐符号按 ORIG objdump 转写。
//
// ORIG 关键事实：
//   - CItemLimitEdition ctor 0x084e9732：rep movsl ×0x12 复制前 0x48 字节，
//     再把 info+0x18 写 this+0x48（初始 curSellNum）。对象尺寸 0x4c。
//   - makeInfoPacket 0x085130e8 按 18 字段顺序写（含剩余时间 = sellEndTime-now 或 0）。
//   - CItemLimitEditionMgr 用 __gnu_cxx::hash_map；getItemInfo 返回指针，未命中返回 0；
//     updateItem 命中 updateCurSellNum 后返回 1，未命中返回 0（Ghidra：0x084e9904）。
//   - makeItemLimitEditionInfo/RemoveInfo：循环遍历 m_items，写计数与包。

#include "CItemLimitEdition.h"

#include <string.h>

#include "CSystemTime.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"   // PacketGuard 完整定义（本 TU 直接以 packet.<member>() 调用，
                           // 不再经 InterfacePacketBuf* 基类中转 —— ORIG 直读形参形）

#include "CItemLimitEdition/CItemLimitEdition.inc"


#include "CItemLimitEdition/dtor_CItemLimitEdition.inc"


#include "CItemLimitEdition/updateCurSellNum.inc"


// [b28d-51] 判词 N（ORIG 0x085130e8，N82/N86 族）：PacketGuard 引用形参 +
// sret/临时槽方向差；语义/调用集齐，byte 层封顶。
// [round5 已修 IDENTICAL/AE] 真因不是槽方向而是**字段类型**：ORIG 各 put_* 实参装载形
//   两段 `mov mem,%eax; mov %eax,%edx`（与 put_int(int) 异型）vs 单段 `mov mem,%edx`
//   （同型）。探针 /tmp/probe_ci3.cpp（int→int 单段；unsigned→int 两段；enum→int 两段；
//   long→int 单段）+ /tmp/probe_ci4.cpp（unsigned int→unsigned long 单段、int→ulong 两段）
//   定出：8 个字段实为 unsigned int（+0x00/+0x08/+0x10/+0x14/+0x20/+0x2c/+0x30/+0x48）、
//   m_itemIdx 亦 unsigned（put_item_idx 实参单段）、m_avatarTypeSelect 为 signed char
//   （ORIG `movzbl 0xc(%eax),%eax; movsbl %al,%edx`）。头已同步（P254 档③，sizeof/偏移不变）。
//   同时把时间比较按 ORIG 书写方向改回 `m_sellEndTime <= getCurSec()`（ORIG 先载字段到 %ebx
//   跨 call 保活、`cmp %eax,%ebx; setle`）。修后 144=144 AE，getIPGNO 返回类型同步 unsigned。
// [L13-group6 P0/P183] ORIG 每个调用点 fresh 读 `0xc(%ebp)`（`mov 0xc(%ebp),%eax`
// 直压实参），我方具名 `InterfacePacketBuf* buf` 占 -0xc 槽且每处 reload（帧
// 0x24 vs ORIG 0x14）⇒ 删具名局部，以宏（P49 寻址助手用宏）逐点内联同一表达式。
#define CILE_PACKET_BUF(p) ((InterfacePacketBuf*)(void*)&(p))
#include "CItemLimitEdition/makeInfoPacket.inc"

#undef CILE_PACKET_BUF

#include "CItemLimitEdition/getIPGNO.inc"


#include "CItemLimitEdition/getRelateditem.inc"


#include "CItemLimitEdition/getRelatedItemCnt.inc"


#include "CItemLimitEdition/getAvatarTypeSelect.inc"


#include "CItemLimitEdition/getPriceCera.inc"


#include "CItemLimitEdition/getPriceGold.inc"


#include "CItemLimitEdition/getPriceWinPoint.inc"


#include "CItemLimitEdition/getTotalSellNum.inc"


#include "CItemLimitEdition/isSellComplete.inc"


#include "CItemLimitEdition/getCurSellNum.inc"


#include "CItemLimitEdition/getSellEndTime.inc"


#include "CItemLimitEdition/getFeaturedIdx.inc"


#include "CItemLimitEdition/getCondCharacJob.inc"


#include "CItemLimitEdition/getCondCharacLevelBegin.inc"


#include "CItemLimitEdition/getCondCharacLevelEnd.inc"


#include "CItemLimitEdition/getCondCharacCreateTimeBegin.inc"


#include "CItemLimitEdition/getCondCharacCreateTimeEnd.inc"


#include "CItemLimitEdition/getCondAccCreateTimeBegin.inc"


#include "CItemLimitEdition/getCondAccCreateTimeEnd.inc"


#include "CItemLimitEdition/CItemLimitEditionMgr.inc"


#include "CItemLimitEdition/dtor_CItemLimitEditionMgr.inc"


#include "CItemLimitEdition/registItem.inc"


#include "CItemLimitEdition/removeItem.inc"


#include "CItemLimitEdition/updateItem.inc"


#include "CItemLimitEdition/clear.inc"
{
    // ORIG 0x08374d4e：const_iterator it/itEnd（非 const begin()/end() 返回 iterator 后
    // 各经 _Hashtable_const_iterator::C1 转换构造，0x8374d77/0x8374d9e），end 循环外缓存；
    // 循环体 `ptEv; test %ebx,%ebx; je` + D1 + _ZdlPv（编译器判空守卫，P141，无显式 if）。
    typedef __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<CItemLimitEdition*> > ItemMap;
    ItemMap::const_iterator it = m_items.begin();
    ItemMap::const_iterator itEnd = m_items.end();
    for (; it != itEnd; ++it) {
        delete it->second;
    }
    m_items.clear();
}

#include "CItemLimitEdition/getItemInfo.inc"


#include "CItemLimitEdition/empty.inc"
{
    return m_items.empty();
}

#include "CItemLimitEdition/makeItemLimitEditionInfo.inc"
{
    // ORIG 0x085132ca：get_index 留计数位，循环遍历 m_items 调 makeInfoPacket，
    // 末尾 put_byte(index, count)。ORIG 无 buf 局部（每次 mov 0xc(%ebp),%eax 重读形参）、
    // 无 it->second 判空（0x8513324 段 ptEv 后直接取 second）、end() 循环外缓存一次。
    int countIdx = packet.get_index();
    packet.put_byte(0);
    int count = 0;
    typedef __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<CItemLimitEdition*> > ItemMap;
    ItemMap::const_iterator it = m_items.begin();
    ItemMap::const_iterator itEnd = m_items.end();
    for (; it != itEnd; ++it) {
        it->second->makeInfoPacket(packet);
        ++count;
    }
    // [L6S5 判词·N75] 全函数唯一残差 = put_byte(countIdx,count) 前两条 mov 的**装载序**：
    // ORIG 0x08513386 `mov -0xc(%ebp),%edx`(count) 先、`mov 0xc(%ebp),%eax`(packet) 后；
    // 我方反之（4.4 先取 receiver）。试过视图宏形
    // `((InterfacePacketBuf*)(void*)&packet)->put_byte(cid,count)` 实测 ndiff 2→2 **无效**
    // （已回滚）⇒ 属 P75「O0 实参装载时刻受编译器版本调度影响」结构性差异，语义/调用集
    // 全等（53=53）。
    packet.put_byte(countIdx, count);
}

#include "CItemLimitEdition/makeItemLimitEditionRemoveInfo.inc"
{
    // ORIG 0x08513382：get_index 留计数位，循环遍历 m_items 写 getIPGNO，
    // 末尾 put_byte(index, count)。ORIG 无 buf 局部、无 it->second 判空、end() 缓存。
    int countIdx = packet.get_index();
    packet.put_byte(0);
    int count = 0;
    typedef __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<CItemLimitEdition*> > ItemMap;
    ItemMap::const_iterator it = m_items.begin();
    ItemMap::const_iterator itEnd = m_items.end();
    for (; it != itEnd; ++it) {
        packet.put_int(it->second->getIPGNO());
        ++count;
    }
    // [L6S5 判词·N75] 全函数唯一残差 = put_byte(countIdx,count) 前两条 mov 装载序：
    // ORIG 0x08513428 `mov -0xc(%ebp),%edx`(count) 先 → 0x0851342b `mov 0xc(%ebp),%eax`
    // (packet) 后；我方反之（4.4 先取 receiver）。同族件（makeItemLimitEditionInfo）
    // 已实测视图宏形无效 ⇒ P75 跨版本实参装载时刻差，语义/调用集全等（56=56）。
    packet.put_byte(countIdx, count);
}
