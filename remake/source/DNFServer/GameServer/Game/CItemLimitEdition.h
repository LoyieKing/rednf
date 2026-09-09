// df_game_r 还原 —— CItemLimitEdition / CItemLimitEditionMgr / stItemLimitEditionItemInfo_t
// （G4 Cera 商城限购版，唯一权威声明点）。
// 布局依据 ORIG nm -C（getters 0x0822ca98..0x0822cb64、ctor 0x084e9732、
// registItem 0x084e975e、getItemInfo 0x0817a0a8、updateCurSellNum 0x084e970e、
// makeInfoPacket 0x085130e8、makeItemLimitEditionInfo 0x085132ca、
// makeItemLimitEditionRemoveInfo 0x08513382）+ objdump -d 逐字段偏移。
//
// 关键 ORIG 事实：
//   - stItemLimitEditionItemInfo_t 前 0x48 字节与 CItemLimitEdition 头 0x48 字节一致；
//     ctor 0x084e9732 以 rep movsl ×0x12 复制 0x48 字节，再把 info+0x18 写入对象 +0x48
//     （初始 curSellNum）。CItemLimitEdition 对象尺寸 = 0x4c（registItem `new 0x4c`）。
//   - CItemLimitEditionMgr 用 __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*>
//     （非 std::map）；成员 +0x00。ctor 仅构造容器；dtor 先 clear() 再容器 dtor。
//   - makeInfoPacket 按 ORIG 顺序写 header 字段：
//     put_int(+0x00) / put_item_idx(+0x04) / put_int(+0x08) / put_byte(+0x0c) /
//     put_int(+0x10) / put_int(+0x14) / put_int(+0x48 curSellNum) / put_int(+0x1c) /
//     put_int(+0x20) / [剩余时间 = sellEndTime-now 或 0] / put_int(+0x2c) /
//     put_byte(+0x30) / put_short(+0x34) / put_short(+0x36) / put_int(+0x38) /
//     put_int(+0x3c) / put_int(+0x40) / put_int(+0x44)。

#ifndef DNF_CITEMLIMITEDITION_H
#define DNF_CITEMLIMITEDITION_H

#include <ext/hash_map>
#include <functional>
#include <stddef.h>

class PacketGuard;

// 条目数据结构：前 0x48 字节语义与 CItemLimitEdition 头一致（ORIG ctor 复制 0x48）。
struct stItemLimitEditionItemInfo_t
{
    int              m_ipgNo;                 // +0x00（getIPGNO）
    int              m_itemIdx;               // +0x04（getRelateditem / put_item_idx）
    int              m_relatedItemCnt;        // +0x08（getRelatedItemCnt）
    unsigned char    m_avatarTypeSelect;      // +0x0c（getAvatarTypeSelect / put_byte）
    char             m_pad0d[3];              // +0x0d
    int              m_priceCera;             // +0x10（getPriceCera）
    int              m_priceGold;             // +0x14（getPriceGold）
    int              m_initSellNum;           // +0x18（ctor 写入对象 +0x48 的初始销量）
    int              m_totalSellNum;          // +0x1c（getTotalSellNum / isSellComplete）
    int              m_featuredIdx;           // +0x20（getFeaturedIdx）
    int              m_field24;               // +0x24（0x48 复制段内未具名 getter）
    int              m_sellEndTime;           // +0x28（getSellEndTime，剩余时间计算源）
    int              m_field2c;               // +0x2c（makeInfoPacket put_int）
    int              m_condCharacJob;         // +0x30（getCondCharacJob / put_byte）
    unsigned short   m_condCharacLevelBegin;  // +0x34（getCondCharacLevelBegin / put_short）
    unsigned short   m_condCharacLevelEnd;    // +0x36（getCondCharacLevelEnd / put_short）
    int              m_condAccCreateTimeBegin;  // +0x38（getCondAccCreateTimeBegin）
    int              m_condAccCreateTimeEnd;    // +0x3c（getCondAccCreateTimeEnd）
    int              m_condCharacCreateTimeBegin;  // +0x40（getCondCharacCreateTimeBegin）
    int              m_condCharacCreateTimeEnd;    // +0x44（getCondCharacCreateTimeEnd）
};

class CItemLimitEdition
{
public:
    CItemLimitEdition(const stItemLimitEditionItemInfo_t& info);   // ORIG 0x084e9732
    ~CItemLimitEdition();                                          // ORIG 0x08374d48

    void updateCurSellNum(unsigned int count);                     // ORIG 0x084e970e
    void makeInfoPacket(PacketGuard& packet) const;                // ORIG 0x085130e8

    // ---- getters（ORIG nm -C，均为 const，返回类型按 objdump 指令形态）----
    int getIPGNO() const;                       // 0x084e971c
    int getRelateditem() const;                 // 0x0817a090
    int getRelatedItemCnt() const;              // 0x0822ca98
    unsigned char getAvatarTypeSelect() const;  // 0x0822cab0
    int getPriceCera() const;                   // 0x0822cafa
    int getPriceGold() const;                   // 0x0822cb06
    int getPriceWinPoint() const;               // 0x0822cb12（恒 0）
    int getTotalSellNum() const;                // 0x084e9726
    int getCurSellNum() const;                  // 0x0822cabc
    int getSellEndTime() const;                 // 0x0822caa4
    int getFeaturedIdx() const;                 // 0x0817a09c
    bool isSellComplete() const;                // 0x0822cac8
    int getCondCharacJob() const;               // 0x0822cb1c
    unsigned short getCondCharacLevelBegin() const;  // 0x0822cb28
    unsigned short getCondCharacLevelEnd() const;    // 0x0822cb34
    int getCondCharacCreateTimeBegin() const;   // 0x0822cb40
    int getCondCharacCreateTimeEnd() const;     // 0x0822cb4c
    int getCondAccCreateTimeBegin() const;      // 0x0822cb58
    int getCondAccCreateTimeEnd() const;        // 0x0822cb64

private:
    // 头 0x48 字节（与 stItemLimitEditionItemInfo_t 一致）+ 0x48 curSellNum，总 0x4c。
    int              m_ipgNo;                 // +0x00
    int              m_itemIdx;               // +0x04
    int              m_relatedItemCnt;        // +0x08
    unsigned char    m_avatarTypeSelect;      // +0x0c
    char             m_pad0d[3];              // +0x0d
    int              m_priceCera;             // +0x10
    int              m_priceGold;             // +0x14
    int              m_pad18;                 // +0x18（初始销量，ctor 单独写入 +0x48）
    int              m_totalSellNum;          // +0x1c
    int              m_featuredIdx;           // +0x20
    int              m_field24;               // +0x24
    int              m_sellEndTime;           // +0x28
    int              m_field2c;               // +0x2c
    int              m_condCharacJob;         // +0x30
    unsigned short   m_condCharacLevelBegin;  // +0x34
    unsigned short   m_condCharacLevelEnd;    // +0x36
    int              m_condAccCreateTimeBegin;   // +0x38
    int              m_condAccCreateTimeEnd;     // +0x3c
    int              m_condCharacCreateTimeBegin;  // +0x40
    int              m_condCharacCreateTimeEnd;    // +0x44
    int              m_curSellNum;            // +0x48（curSellNum）
};

// P95 具名视图（updateCurSellNum 0x084e970e 先 this 后值 lowering）：仅表达布局。
struct CItemLimitEditionView
{
    char m_head[0x48];
    int  m_curSellNum;
};

class CItemLimitEditionMgr
{
public:
    CItemLimitEditionMgr();                                       // ORIG 0x08374e0c
    ~CItemLimitEditionMgr();                                      // ORIG 0x08374e20

    void registItem(const stItemLimitEditionItemInfo_t& info);    // ORIG 0x084e975e
    void removeItem(unsigned int itemIdx);                        // ORIG 0x084e9876
    bool updateItem(unsigned int itemIdx, unsigned int count);   // [b28d-Main·P292] int→bool（ORIG 调用点 0x084dbe31/0x084dbf58 均 `call; xor $0x1,%eax; test %al,%al`）
    void clear();                                                 // ORIG 0x08374d4e
    CItemLimitEdition* getItemInfo(unsigned int itemIdx) const;   // ORIG 0x0817a0a8
    bool empty() const;                                           // ORIG 0x084e997c
    void makeItemLimitEditionInfo(PacketGuard& packet) const;     // ORIG 0x085132ca
    void makeItemLimitEditionRemoveInfo(PacketGuard& packet) const;  // ORIG 0x08513382

private:
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>,
                        std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >
        m_items;   // +0x00（ORIG hash_map<unsigned int, CItemLimitEdition*>）
};

#endif  // DNF_CITEMLIMITEDITION_H
