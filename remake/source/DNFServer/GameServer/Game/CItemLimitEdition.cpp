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

// ============================================================================
// CItemLimitEdition
// ============================================================================
CItemLimitEdition::CItemLimitEdition(const stItemLimitEditionItemInfo_t& info)
{
    // [b28c-L18] P83：ORIG 0x084e9732 为 `rep movsl`×0x12（结构整体赋值展开形），
    // 4.4 对 memcpy 恒 call memcpy（N78）；改同布局具名结构整体赋值（前 0x48 字节
    // 布局一致，头文件注释实证）即复现块搬移 + info+0x18 写 this+0x48（初始销量）。
    *reinterpret_cast<stItemLimitEditionItemInfo_t*>(this) = info;
    m_curSellNum = info.m_initSellNum;
}

CItemLimitEdition::~CItemLimitEdition()
{
}

void CItemLimitEdition::updateCurSellNum(unsigned int count)
{
    // P95：ORIG 0x084e970e 先载 this 再载值；4.4 直成员赋值先备值。具名视图复现。
    ((CItemLimitEditionView*)this)->m_curSellNum = (int)count;
}

void CItemLimitEdition::makeInfoPacket(PacketGuard& packet) const
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_int(m_ipgNo);                       // +0x00
    buf->put_item_idx((unsigned long)m_itemIdx); // +0x04
    buf->put_int(m_relatedItemCnt);              // +0x08
    buf->put_byte((int)m_avatarTypeSelect);      // +0x0c
    buf->put_int(m_priceCera);                   // +0x10
    buf->put_int(m_priceGold);                   // +0x14
    buf->put_int(m_curSellNum);                  // +0x48
    buf->put_int(m_totalSellNum);                // +0x1c
    buf->put_int(m_featuredIdx);                 // +0x20
    // ORIG 0x085131bb..0x0851320b：售罄剩余时间逐分支各 fresh put_int（now>=sellEnd
    // 则 put_int(0)，否则 put_int(sellEnd - now)）；非合并后单 put_int。
    if (GlobalData::s_systemTime_.getCurSec() >= m_sellEndTime)
        buf->put_int(0);                         // ORIG 85131e4
    else
        buf->put_int(m_sellEndTime - GlobalData::s_systemTime_.getCurSec()); // ORIG 851320b
    buf->put_int(m_field2c);                     // +0x2c
    buf->put_byte(m_condCharacJob);              // +0x30
    buf->put_short((int)m_condCharacLevelBegin); // +0x34
    buf->put_short((int)m_condCharacLevelEnd);   // +0x36
    buf->put_int(m_condAccCreateTimeBegin);      // +0x38
    buf->put_int(m_condAccCreateTimeEnd);        // +0x3c
    buf->put_int(m_condCharacCreateTimeBegin);   // +0x40
    buf->put_int(m_condCharacCreateTimeEnd);     // +0x44
}

// ---- getters（ORIG 逐字段偏移）----
int CItemLimitEdition::getIPGNO() const
{
    return m_ipgNo;
}

int CItemLimitEdition::getRelateditem() const
{
    return m_itemIdx;
}

int CItemLimitEdition::getRelatedItemCnt() const
{
    return m_relatedItemCnt;
}

unsigned char CItemLimitEdition::getAvatarTypeSelect() const
{
    return m_avatarTypeSelect;
}

int CItemLimitEdition::getPriceCera() const
{
    return m_priceCera;
}

int CItemLimitEdition::getPriceGold() const
{
    return m_priceGold;
}

int CItemLimitEdition::getPriceWinPoint() const
{
    return 0;   // ORIG 0x0822cb12：mov $0x0,%eax
}

int CItemLimitEdition::getTotalSellNum() const
{
    return m_totalSellNum;
}

bool CItemLimitEdition::isSellComplete() const
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
    return getCurSellNum() >= m_totalSellNum;   // ORIG call getCurSellNum
}

int CItemLimitEdition::getCurSellNum() const
{
    return m_curSellNum;
}

int CItemLimitEdition::getSellEndTime() const
{
    return m_sellEndTime;
}

int CItemLimitEdition::getFeaturedIdx() const
{
    return m_featuredIdx;
}

int CItemLimitEdition::getCondCharacJob() const
{
    return m_condCharacJob;
}

unsigned short CItemLimitEdition::getCondCharacLevelBegin() const
{
    return m_condCharacLevelBegin;
}

unsigned short CItemLimitEdition::getCondCharacLevelEnd() const
{
    return m_condCharacLevelEnd;
}

int CItemLimitEdition::getCondCharacCreateTimeBegin() const
{
    return m_condCharacCreateTimeBegin;
}

int CItemLimitEdition::getCondCharacCreateTimeEnd() const
{
    return m_condCharacCreateTimeEnd;
}

int CItemLimitEdition::getCondAccCreateTimeBegin() const
{
    return m_condAccCreateTimeBegin;
}

int CItemLimitEdition::getCondAccCreateTimeEnd() const
{
    return m_condAccCreateTimeEnd;
}

// ============================================================================
// CItemLimitEditionMgr
// ============================================================================
CItemLimitEditionMgr::CItemLimitEditionMgr()
{
}

CItemLimitEditionMgr::~CItemLimitEditionMgr()
{
    clear();
}

void CItemLimitEditionMgr::registItem(const stItemLimitEditionItemInfo_t& info)
{
    // ORIG 0x084e975e：new CItemLimitEdition(info)，按 getIPGNO 查重，命中则删旧并 erase，
    // 再 insert（key = getIPGNO）。
    CItemLimitEdition* edition = new CItemLimitEdition(info);
    unsigned int key = (unsigned int)edition->getIPGNO();
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(key);
    if (it != m_items.end()) {
        if (it->second)
            delete it->second;
        m_items.erase(it);
    }
    m_items.insert(std::make_pair(key, edition));
}

void CItemLimitEditionMgr::removeItem(unsigned int itemIdx)
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

int CItemLimitEditionMgr::updateItem(unsigned int itemIdx, unsigned int count)
{
    // ORIG 0x084e9904：find 未命中返回 0；命中 updateCurSellNum 返回 1。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(itemIdx);
    if (it == m_items.end())
        return 0;
    it->second->updateCurSellNum(count);
    return 1;
}

void CItemLimitEditionMgr::clear()
{
    // ORIG 0x08374d4e：遍历容器 delete 每个项，再容器 clear()。
    for (__gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                             __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                             std::allocator<CItemLimitEdition*> >::iterator it
             = m_items.begin();
         it != m_items.end(); ++it) {
        if (it->second)
            delete it->second;
    }
    m_items.clear();
}

CItemLimitEdition* CItemLimitEditionMgr::getItemInfo(unsigned int itemIdx) const
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

bool CItemLimitEditionMgr::empty() const
{
    return m_items.empty();
}

void CItemLimitEditionMgr::makeItemLimitEditionInfo(PacketGuard& packet) const
{
    // ORIG 0x085132ca：get_index 留计数位，循环遍历 m_items 调 makeInfoPacket，
    // 末尾 put_byte(index, count)。
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    int countIdx = buf->get_index();
    buf->put_byte(0);
    int count = 0;
    for (__gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                             __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                             std::allocator<CItemLimitEdition*> >::const_iterator it
             = m_items.begin();
         it != m_items.end(); ++it) {
        if (it->second)
            it->second->makeInfoPacket(packet);
        ++count;
    }
    buf->put_byte(countIdx, count);
}

void CItemLimitEditionMgr::makeItemLimitEditionRemoveInfo(PacketGuard& packet) const
{
    // ORIG 0x08513382：get_index 留计数位，循环遍历 m_items 写 getIPGNO，
    // 末尾 put_byte(index, count)。
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    int countIdx = buf->get_index();
    buf->put_byte(0);
    int count = 0;
    for (__gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                             __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                             std::allocator<CItemLimitEdition*> >::const_iterator it
             = m_items.begin();
         it != m_items.end(); ++it) {
        if (it->second)
            buf->put_int(it->second->getIPGNO());
        ++count;
    }
    buf->put_byte(countIdx, count);
}
