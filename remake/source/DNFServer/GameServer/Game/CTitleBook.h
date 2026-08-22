#ifndef GAME_CTITLEBOOK_H_
#define GAME_CTITLEBOOK_H_

// df_game_r 称号簿 CTitleBook（G3-3 批次，2026-08-17）。
// 布局依据 ORIG ctor 0x864103c / _saveData 0x86414a8 反汇编推导：
//   CData 基类（vptr +0x00 / m_bIsSave +0x04）
//   CUser* m_user            +0x08
//   stTitleBook m_book       +0x0c（0x6b3a：4 类称号槽数组）
//   Inven_Item* m_category[4] +0x6b48..0x6b54（指向 m_book 内 4 类槽起点）
//   sizeof = 0x6b58。
// 静态入口（getTitle/putTitle/putTitleOnly/checkError）供 CAchievement 等调用。

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CBoosterGage.h"   // charac_expand::CData（pack(1) 基类）
#include "Inven_Item.h"     // Inven_Item（0x3d 字节）
#include "CItemAmplifier.h" // stAmplifyOption_t
#include "GameEnums.h"      // ENUM_TITLE_BOOK_INFO_TYPE 等

#ifndef DNF_ENUM_ITEMSPACE_DEFINED
#define DNF_ENUM_ITEMSPACE_DEFINED
enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_0 = 0
};
#endif

class CUser;
class PacketGuard;
class CInventory;
class CExpandEquipslot;

// ---- 称号簿分类 / 信息类型 ----
enum ENUM_TITLE_BOOK_CATEGORY
{
    ENUM_TITLE_BOOK_CATEGORY_0 = 0,
    ENUM_TITLE_BOOK_CATEGORY_1 = 1,
    ENUM_TITLE_BOOK_CATEGORY_2 = 2,
    ENUM_TITLE_BOOK_CATEGORY_3 = 3
};

// ENUM_TITLE_BOOK_INFO_TYPE 定义于 GameEnums.h。

// 各类称号槽上限（ORIG .rodata 0x8ae5a98：{0x46, 0xc8, 0x78, 0x3c}）
extern const int TITLE_BOOK_MAX[4];

// ---- 称号元素（0x20 字节） ----
struct stTitleElement
{
    int m_index;                  // +0x00（-1 = 空槽）
    std::vector<int> m_fitJobs;   // +0x04
    int m_achievementIdx;         // +0x1c（-1 = 无成就关联）
};

// ---- 称号邮件奖励（0x20 字节） ----
struct stTitleRewardBasis
{
    int m_type;             // +0x00（3 = 邮件奖励）
    int m_field4;           // +0x04
    int m_field8;           // +0x08
    std::string m_sender;   // +0x0c
    std::string m_subject;  // +0x10
    int m_itemIdx;          // +0x18
    int m_addInfo;          // +0x1c
};

// ---- 称号章节（0x40 字节） ----
struct stTitleSection
{
    int m_category;                          // +0x00
    int m_field4;                            // +0x04
    int m_field8;                            // +0x08
    int m_startIdx;                          // +0x0c
    int m_endIdx;                            // +0x10
    std::string m_name;                      // +0x14
    char m_pad18[0x1c];                      // +0x18..+0x33
    std::vector<stTitleRewardBasis*> m_rewards;  // +0x34
};

// ---- 称号簿数据（0x6b3a 字节，4 类槽数组） ----
struct stTitleBook
{
    stTitleBook();
    void clear();

    Inven_Item m_category0[70];   // +0x00
    Inven_Item m_category1[200];  // +0x10ae
    Inven_Item m_category2[120];  // +0x4056
    Inven_Item m_category3[60];   // +0x5cee
};

// ---- 成就调整表项（12 字节） ----
struct stAdjust
{
    unsigned int m_questIdx;       // +0x00
    int m_field4;                  // +0x04
    unsigned int m_achievementIdx; // +0x08
};

// ---- 物品锁引用（4 字节） ----
namespace item_lock
{
struct stItemLockRef
{
    unsigned char m_lock;  // +0x00
    char m_space;          // +0x01
    short m_slot;          // +0x02
};

// CItemLock：方法集与 CItemLock.cpp 定义一致（跨 TU 统一）。
class CItemLock
{
public:
    std::vector<stItemLockRef>& GetItemLockRefVec();
    void SendItemLockList(CUser* user);
    bool CheckItemLock(unsigned char b) const;
    void MakeItemLockPacket(PacketGuard& packet, unsigned char b) const;
    static int CheckItemLock(CExpandEquipslot* data);
};
}

// ============================================================================
// CTitleBook
// ============================================================================

// 注意：CTitleBook 不使用 pack(1)，采用自然对齐以保证 ORIG 布局
// （CData 基类 pack 后为 5 字节，m_user 对齐到 +0x08 / m_book +0x0c /
// m_category[4] +0x6b48，sizeof 0x6b58，与 ORIG ctor/_saveData 反汇编一致）。
class CTitleBook : public charac_expand::CData
{
public:
    CTitleBook();
    virtual ~CTitleBook();

    virtual bool loadData(CUser* pUser, char* pData);
    virtual void getData(char* pData) const;
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();

    // ---- 查询 / 列表 ----
    Inven_Item* getTitleSlot(ENUM_TITLE_BOOK_CATEGORY category, int slot);
    Inven_Item getTitleSlotR(ENUM_TITLE_BOOK_CATEGORY category, int slot);
    bool findItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                      int& slot) const;
    void getItemLockList(std::vector<item_lock::stItemLockRef>& out);
    void sendList();
    void sendListOtherUser(CUser* user, ENUM_TITLE_BOOK_INFO_TYPE type);
    bool isAbleItemLock(unsigned long itemIdx);
    bool _checkFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                       int slot);
    static bool _getFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY& category,
                            int& slot);
    void deleteTimeoutItem(const std::vector<std::pair<int, int> >& list);
    int _isMatchedItem(int itemIdx,
                       const std::vector<std::pair<int, int> >& list);

    // ---- 静态入口 ----
    static int checkError(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                          int slot);
    static int getTitle(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                        int slot, int itemIdx, ENUM_ITEMSPACE space,
                        int& outSlot);
    static int putTitle(CUser* user, ENUM_ITEMSPACE space, int slot,
                        int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                        int titleSlot);
    static int putTitleOnly(CUser* user, int itemIdx);
    static int putTitleOnly(CUser* user, int itemIdx,
                            ENUM_TITLE_BOOK_CATEGORY& category, int& slot);

    // ---- 内部辅助 ----
    void _gm_clear();
    Inven_Item* _getTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot);
    Inven_Item _getTitleR(ENUM_TITLE_BOOK_CATEGORY category, int slot);
    void _insertTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot,
                      Inven_Item* item);
    void _deleteTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot);
    bool _checkInsertTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot,
                           Inven_Item* item);
    bool _checkMovetoInven(ENUM_TITLE_BOOK_CATEGORY category, int slot);
    bool _getOwnerTitleInfo(ENUM_TITLE_BOOK_CATEGORY& category, int& slot,
                            Inven_Item* out);
    int _getCategoryItemCount(ENUM_TITLE_BOOK_CATEGORY category);
    bool _empty();
    void _sendCategoryData(CUser* user, unsigned short extra,
                           ENUM_TITLE_BOOK_CATEGORY category,
                           ENUM_TITLE_BOOK_INFO_TYPE type);
    void _putItemData(PacketGuard& packet, short slot, Inven_Item& item);

    // ---- 成员 ----
    CUser* m_user;                 // +0x08
    stTitleBook m_book;            // +0x0c
    Inven_Item* m_category[4];     // +0x6b48..0x6b54
};

#endif  // GAME_CTITLEBOOK_H_
