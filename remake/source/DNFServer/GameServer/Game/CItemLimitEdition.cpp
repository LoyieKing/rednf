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

// ============================================================================
// CItemLimitEdition
// ============================================================================
#include "CItemLimitEdition/CItemLimitEdition.inc"
{
    // [b28c-L18] P83：ORIG 0x084e9732 为 `rep movsl`×0x12（结构整体赋值展开形），
    // 4.4 对 memcpy 恒 call memcpy（N78）；改同布局具名结构整体赋值（前 0x48 字节
    // 布局一致，头文件注释实证）即复现块搬移 + info+0x18 写 this+0x48（初始销量）。
    *reinterpret_cast<stItemLimitEditionItemInfo_t*>(this) = info;
    m_curSellNum = info.m_initSellNum;
}

#include "CItemLimitEdition/dtor_CItemLimitEdition.inc"
{
}

#include "CItemLimitEdition/updateCurSellNum.inc"
{
    // P95：ORIG 0x084e970e 先载 this 再载值；4.4 直成员赋值先备值。具名视图复现。
    ((CItemLimitEditionView*)this)->m_curSellNum = (int)count;
}

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
{
    CILE_PACKET_BUF(packet)->put_int(m_ipgNo);                       // +0x00
    CILE_PACKET_BUF(packet)->put_item_idx(m_itemIdx);                // +0x04（ORIG 单段 ⇒ 隐式转换）
    CILE_PACKET_BUF(packet)->put_int(m_relatedItemCnt);              // +0x08
    CILE_PACKET_BUF(packet)->put_byte((int)m_avatarTypeSelect);      // +0x0c
    CILE_PACKET_BUF(packet)->put_int(m_priceCera);                   // +0x10
    CILE_PACKET_BUF(packet)->put_int(m_priceGold);                   // +0x14
    CILE_PACKET_BUF(packet)->put_int(m_curSellNum);                  // +0x48
    CILE_PACKET_BUF(packet)->put_int(m_totalSellNum);                // +0x1c
    CILE_PACKET_BUF(packet)->put_int(m_featuredIdx);                 // +0x20
    // ORIG 0x085131bb..0x0851320b：售罄剩余时间逐分支各 fresh put_int（now>=sellEnd
    // 则 put_int(0)，否则 put_int(sellEnd - now)）；非合并后单 put_int。
    // [P66/P243 求值序] ORIG 先 `mov 0x28(%eax),%ebx`（字段，跨 getCurSec call 保活）
    // 再 `cmp %eax,%ebx; setle` —— 左操作数是 m_sellEndTime；反写（getCurSec() >= …）
    // 4.4 产 call 先、字段后载 + setge 极性 ⇒ 按 ORIG 书写方向。
    if (m_sellEndTime <= GlobalData::s_systemTime_.getCurSec())
        CILE_PACKET_BUF(packet)->put_int(0);                         // ORIG 85131e4
    else
        CILE_PACKET_BUF(packet)->put_int(m_sellEndTime - GlobalData::s_systemTime_.getCurSec()); // ORIG 851320b
    CILE_PACKET_BUF(packet)->put_int(m_field2c);                     // +0x2c
    CILE_PACKET_BUF(packet)->put_byte(m_condCharacJob);              // +0x30
    CILE_PACKET_BUF(packet)->put_short((int)m_condCharacLevelBegin); // +0x34
    CILE_PACKET_BUF(packet)->put_short((int)m_condCharacLevelEnd);   // +0x36
    CILE_PACKET_BUF(packet)->put_int(m_condAccCreateTimeBegin);      // +0x38
    CILE_PACKET_BUF(packet)->put_int(m_condAccCreateTimeEnd);        // +0x3c
    CILE_PACKET_BUF(packet)->put_int(m_condCharacCreateTimeBegin);   // +0x40
    CILE_PACKET_BUF(packet)->put_int(m_condCharacCreateTimeEnd);     // +0x44
}
#undef CILE_PACKET_BUF

// ---- getters（ORIG 逐字段偏移）----
#include "CItemLimitEdition/getIPGNO.inc"
{
    return m_ipgNo;
}

#include "CItemLimitEdition/getRelateditem.inc"
{
    return m_itemIdx;
}

#include "CItemLimitEdition/getRelatedItemCnt.inc"
{
    return m_relatedItemCnt;
}

#include "CItemLimitEdition/getAvatarTypeSelect.inc"
{
    return m_avatarTypeSelect;
}

#include "CItemLimitEdition/getPriceCera.inc"
{
    return m_priceCera;
}

#include "CItemLimitEdition/getPriceGold.inc"
{
    return m_priceGold;
}

#include "CItemLimitEdition/getPriceWinPoint.inc"
{
    return 0;   // ORIG 0x0822cb12：mov $0x0,%eax
}

#include "CItemLimitEdition/getTotalSellNum.inc"
{
    return m_totalSellNum;
}

#include "CItemLimitEdition/isSellComplete.inc"
{
    // [b28d-51] 判词 N（ORIG 0x0822cac8，N7/N22 寄存器角色族）：P210 帧继承已翻正
    // （sub $0x18 保守帧）。残余：ORIG `call; mov %eax,%edx; mov 0x8(ebp),%eax;
    // mov 0x1c(eax),%eax; cmp %eax,%edx`（call 值先中转 %edx、字段双基址载入 %eax）
    // vs 4.4 `mov this,%edx; mov 0x1c(edx),%edx; cmp %edx,%eax`（免中转、字段单载入
    // %edx）= N18a+N22。探针 8 形（>=/<=/!/<、显式 int cast、具名 int 局部、
    // `> x-1`、`x-1 <`；/tmp/b28d_51_probe_sell.cpp）全部不可复现，语义等价。
    // ORIG 0x0822cac8：若 totalSellNum == -1 返回 false；否则 curSellNum >= totalSellNum。
    if (m_totalSellNum == -1)
        return false;
    // [b28d-51 + 本轮复验] 操作数交换形（m_totalSellNum <= getCurSellNum()）实测 4.4 转
    //   `push %ebx; sub $0x14; mov 0x1c(%eax),%ebx; call; cmp %eax,%ebx; setle`（跨 call 保活
    //   字段 + 帧破）⇒ 更劣，已回退。ORIG @0x0822cae0 `mov %eax,%edx`（call 结果中转）为
    //   4.1 寄存器分配固有形（N18a/N22 族），8+2 形不可复现，语义等价。
    return getCurSellNum() >= m_totalSellNum;   // ORIG call getCurSellNum
}

#include "CItemLimitEdition/getCurSellNum.inc"
{
    return m_curSellNum;
}

#include "CItemLimitEdition/getSellEndTime.inc"
{
    return m_sellEndTime;
}

#include "CItemLimitEdition/getFeaturedIdx.inc"
{
    return m_featuredIdx;
}

#include "CItemLimitEdition/getCondCharacJob.inc"
{
    return m_condCharacJob;
}

#include "CItemLimitEdition/getCondCharacLevelBegin.inc"
{
    return m_condCharacLevelBegin;
}

#include "CItemLimitEdition/getCondCharacLevelEnd.inc"
{
    return m_condCharacLevelEnd;
}

#include "CItemLimitEdition/getCondCharacCreateTimeBegin.inc"
{
    return m_condCharacCreateTimeBegin;
}

#include "CItemLimitEdition/getCondCharacCreateTimeEnd.inc"
{
    return m_condCharacCreateTimeEnd;
}

#include "CItemLimitEdition/getCondAccCreateTimeBegin.inc"
{
    return m_condAccCreateTimeBegin;
}

#include "CItemLimitEdition/getCondAccCreateTimeEnd.inc"
{
    return m_condAccCreateTimeEnd;
}

// ============================================================================
// CItemLimitEditionMgr
// ============================================================================
#include "CItemLimitEdition/CItemLimitEditionMgr.inc"
{
}

#include "CItemLimitEdition/dtor_CItemLimitEditionMgr.inc"
{
    clear();
}

#include "CItemLimitEdition/registItem.inc"
{
    // ORIG 0x084e975e：new CItemLimitEdition(info)，按 getIPGNO 查重，命中则删旧并 erase，
    // 再 insert（key = 二次调用 getIPGNO —— ORIG 0x84e9803 再次 call getIPGNO，
    // 结果落 -0xc 后作 make_pair 首参；复用首个 key 局部会少一次 call = 判据㉑ 缺调用）。
    // [P212/P309 全头部声明] ORIG 槽序 edition=-0x38（深）/key=-0x34（浅）与 4.4 块内
    // 首次使用序相反 ⇒ 两局部提至函数头按 ORIG 槽深序声明、块内仅赋值。
    unsigned int key;
    CItemLimitEdition* edition;
    edition = new CItemLimitEdition(info);
    key = edition->getIPGNO();
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(key);
    if (it != m_items.end()) {
        // ORIG 0x84e97b0..0x84e97c2：ptEv 后直接 `test %ebx,%ebx; je` 判空 + D1 + _ZdlPv
        // （编译器自带 delete 判空守卫，P141）；显式 if 会多 setne 物化。
        delete it->second;
        m_items.erase(it);
    }
    m_items.insert(std::make_pair(edition->getIPGNO(), edition));
}

#include "CItemLimitEdition/removeItem.inc"
{
    // ORIG 0x084e9876：find 命中则 delete + erase。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(itemIdx);
    if (it != m_items.end()) {
        // [b28c-L29] ORIG 0x084e9876：仅一次 ptEv+test/je（编译器自带 delete
        // 判空守卫，P141）——去掉显式 if(it->second) 双重检查。
        delete it->second;
        m_items.erase(it);
    }
}

#include "CItemLimitEdition/updateItem.inc"
{
    // ORIG 0x084e9904：find 未命中返回 0；命中 updateCurSellNum 返回 1。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(itemIdx);
    if (it == m_items.end())
        return false;
    it->second->updateCurSellNum(count);
    return true;
}

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
{
    // ORIG 0x0817a0a8：find；未命中返回 0，命中返回 second。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::const_iterator it
        = m_items.find(itemIdx);
    if (it == m_items.end())
        return 0;
    return it->second;
}

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
    packet.put_byte(countIdx, count);
}
