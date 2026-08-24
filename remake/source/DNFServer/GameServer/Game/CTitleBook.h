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
    std::vector<int> m_fitJobs;   // +0x04（ORIG D1 0x828d926 析构此 vector）
    char m_pad10[8];              // +0x10
    std::string m_strText;        // +0x18（ORIG D1 析构此 string）[推断：称号文本]
    int m_achievementIdx;         // +0x1c（-1 = 无成就关联）
};  // sizeof 0x20

// ---- 奖励基类（0xc 字节；ORIG C1 W 0x08aa9632 / D1 W 0x08aa9652）----
// C1：type=4、byte4=0、data=0。D1：data 非空时经其 vtable 槽 +0xC
// （deleting dtor 槽）销毁并清空——导入域产生的奖励 data 恒为空。
// 三个派生类字段均自 +0x0c 起：stRewardChangeStatus(0x18) /
// stRewardGiveEmotion(0x10) / stRewardPostal(0x20)，尺寸经 importRewards
// 的 operator new 实参逐一实证。
struct stTitleRewardBasis
{
    stTitleRewardBasis()
    {
        m_type = 4;
        m_flag4 = 0;
        m_data = 0;
    }
    ~stTitleRewardBasis()
    {
        if (m_data != 0)
        {
            // ORIG：mov (%eax),%eax; add $0xc,%eax; call *(%eax) —— 虚删除。
            typedef void (*DeletingDtor)(void*);
            void** vptr = *reinterpret_cast<void***>(m_data);
            reinterpret_cast<DeletingDtor>(vptr[3])(m_data);
        }
        m_data = 0;
    }

    int m_type;             // +0x00（change=0 / emotion=1 / postal=3 / 基类=4）
    unsigned char m_flag4;  // +0x04（C1 置 0）
    void* m_data;           // +0x08（多态子对象指针；D1 经 vtable+0xC 删除）
};  // sizeof == 0xc

static_assert(sizeof(stTitleRewardBasis) == 0xc,
              "stTitleRewardBasis size drift");

// ---- 变更属性奖励（ORIG C1 W 0x08aa9690：type=0、appendage=0x4d、
// valueType=2、value=0f）----
struct stRewardChangeStatus : public stTitleRewardBasis
{
    stRewardChangeStatus()
        : m_appendage(0x4d), m_valueType(2), m_value(0.0f)
    {
        m_type = 0;
    }

    int m_appendage;     // +0x0c（importRewards 覆写为 loadAppendageType）
    int m_valueType;     // +0x10（loadValueType："+"=0、"%"=1、其他=2）
    float m_value;       // +0x14（ScanFloat，fstps）
};  // sizeof == 0x18

static_assert(sizeof(stRewardChangeStatus) == 0x18,
              "stRewardChangeStatus size drift");

// ---- 给予表情奖励（ORIG C1 W 0x08aa96cc：type=1、emotionId=-1）----
struct stRewardGiveEmotion : public stTitleRewardBasis
{
    stRewardGiveEmotion() : m_emotionId(-1)
    {
        m_type = 1;
    }

    int m_emotionId;     // +0x0c（importRewards ScanInt 覆写）
};  // sizeof == 0x10

static_assert(sizeof(stRewardGiveEmotion) == 0x10,
              "stRewardGiveEmotion size drift");

// ---- 发送邮件奖励（ORIG C1 W 0x08aa96f2：三 string 构造、两 int 清零、
// type=3；与 CAchievement::_getPostalReward 的 type==3 判定一致）----
struct stRewardPostal : public stTitleRewardBasis
{
    stRewardPostal() : m_itemIdx(0), m_addInfo(0)
    {
        m_type = 3;
    }

    std::string m_sender;    // +0x0c
    std::string m_subject;   // +0x10
    std::string m_message;   // +0x14
    int m_itemIdx;           // +0x18
    int m_addInfo;           // +0x1c
};  // sizeof == 0x20

static_assert(sizeof(stRewardPostal) == 0x20,
              "stRewardPostal size drift");

// ---- 称号章节（0x40 字节；布局经 importTitleSection 报告逐偏移实证：
// 字符串 +0x04/+0x14/+0x18/+0x20/+0x24/+0x30，int +0x00/+0x08/+0x0c/+0x10/
// +0x1c/+0x28/+0x2c，rewards vector +0x34）----
struct stTitleSection
{
    int m_category;                          // +0x00
    std::string m_sectionName;               // +0x04（[section name]）
    int m_field8;                            // +0x08
    int m_startIdx;                          // +0x0c（importTitleSection：tab*10）
    int m_endIdx;                            // +0x10（startIdx+10）
    std::string m_name;                      // +0x14（[name]）
    std::string m_imagePath;                 // +0x18（[image path]）
    int m_imageParam;                        // +0x1c（image path 后随 int）
    std::string m_effectPath;                // +0x20（[effect path]）
    std::string m_icon;                      // +0x24（[section icon]）
    int m_iconId;                            // +0x28
    int m_frameType;                         // +0x2c（[frame type]）
    std::string m_lockMessage;               // +0x30（[lock message]）
    std::vector<stTitleRewardBasis*> m_rewards;  // +0x34
};

static_assert(sizeof(stTitleSection) == 0x40,
              "stTitleSection size drift");

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

// CItemLock：ORIG 布局（GetItemLockInfo 0x854261e / SendItemLockList 0x85427a0 /
// MakeItemLockPacket 0x85426a6 反汇编）：+0x08 map<uchar,stItemLockInfo>，
// +0x48 vector<stItemLockRef>。对象由角色扩展数据区承载（跨 TU reinterpret_cast）。
struct stItemLockInfo
{
    char m_pad0[4];           // +0x00
    int m_unlockSec;          // +0x04（解锁完成时刻）
    unsigned char m_state;    // +0x08（2=解锁倒计时中）
    char m_pad9[3];
};

class CItemLock
{
public:
    std::vector<stItemLockRef>& GetItemLockRefVec();
    void SendItemLockList(CUser* user);                    // ORIG 0x85427a0
    bool CheckItemLock(unsigned char b) const;             // ORIG 0x8541a96
    void MakeItemLockPacket(PacketGuard& packet, unsigned char b) const;  // ORIG 0x85426a6

    unsigned char GetItemLockInfo(unsigned char lock, int& outSec) const;  // ORIG 0x854261e
    void UpdateItem(CUser* user, Inven_Item* item);        // ORIG 0x8541e4e

    char m_pad00[8];                                       // +0x00
    std::map<unsigned char, stItemLockInfo> m_lockMap;     // +0x08
    std::vector<stItemLockRef> m_lockList;                 // +0x48
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
