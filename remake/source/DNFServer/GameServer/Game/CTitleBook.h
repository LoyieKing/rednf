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

#include <deque>
#include <map>
#include <stack>
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


// ---- 称号元素（0x20 字节） ----
struct stTitleElement
{
    // b28c-L26：ORIG C1 0x08aa95ae 发射序 = m_index(-1) → vector(+0x4) →
    // string(+0x18) → +0x1c → +0x10 → +0x14 → +0x20 → +0x24（后五个**非**声明序）
    // ⇒ m_index 走 meminit（声明序首位，先于两个类成员构造），其余五个标量
    // 归 ctor 体按 ORIG 书写序赋值（4.4 对全 meminit 形会重排标量段）。
    stTitleElement()
        : m_index(-1),
          m_fitJobs(),
          m_strText()
    {
        m_achievementIdx = -1;
        m_field10 = -1;
        m_field14 = 0;
        m_field20 = 0;
        m_field24 = -1;
    }

    int m_index;                  // +0x00（-1 = 空槽）
    std::vector<int> m_fitJobs;   // +0x04（ORIG D1 0x828d926 析构此 vector）
    int m_field10;                // +0x10（ORIG C1 初始化为 -1；脚本导入不写）
    int m_field14;                // +0x14（ORIG C1 初始化为 0）
    std::string m_strText;        // +0x18（ORIG D1 析构此 string）[推断：称号文本]
    int m_achievementIdx;         // +0x1c（-1 = 无成就关联）
    int m_field20;                // +0x20（ORIG C1 初始化为 0）
    int m_field24;                // +0x24（ORIG C1 初始化为 -1）
};  // sizeof 0x28（ORIG C1 0x08aa95ae / C1ERKS_ 0x0828f43e 逐字段实证）

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
        // §4.5/§8 判据（Bridge21B）：m_data 为非虚 void* 基指针（stTitleRewardBasis
        // 无虚表 sizeof 0xc），vtable+0xC 虚 deleting-dtor 分发无法以真实 delete 表达
        // （派生奖赏 vtable 未建模=b22 单）；运行期导入域 data 恒空（no-op）。保留直调。
        // ORIG D1 0x08aa9652：两次独立判空嵌套；vptr/vfn 全程寄存器直传，无中间槽。
        if (m_data != 0)
        {
            if (m_data != 0)
            {
                // ORIG：mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx; call *%edx —— 虚删除。
                typedef void (*DeletingDtor)(void*);
                reinterpret_cast<DeletingDtor>(
                    (*reinterpret_cast<void***>(m_data))[3])(m_data);
            }
        }
        m_data = 0;
    }
    /* D1 残差 N7 封顶（b26 NL26）：唯一差异为 vfn 寄存器互换——ORIG
     * `mov (%eax),%edx; call *%edx` vs 4.4 恒 `mov (%eax),%eax; call *%eax`
     * （临时/引用/限定名均无效，负例表 N7；game_func_report CAchievement.cpp
     * 实测仅剩此行差）。后续批次勿再耗轮次。 */

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
    {
        m_type = 0;          // ORIG C1：m_type=0 先于派生字段(全于 body)
        m_appendage = 0x4d;
        m_valueType = 2;
        m_value = 0.0f;
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
    stRewardGiveEmotion()
    {
        m_type = 1;          // ORIG C1：m_type=1 先于 m_emotionId(全于 body)
        m_emotionId = -1;
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

// 字符串 +0x04/+0x08/+0x14/+0x18/+0x20/+0x24/+0x30，int +0x00/+0x0c/+0x10/
// +0x1c/+0x28/+0x2c，rewards vector +0x34）----
struct stTitleSection
{
    stTitleSection()
        : m_category(4), m_startIdx(-1), m_endIdx(-1),
          m_imageParam(-1), m_iconId(-1), m_frameType(0)
    {
        // ORIG C1 0x08aa979e：+0x00=4、+0x0c/+0x10/+0x1c/+0x28=-1、+0x2c=0
        //（string/vector 成员自动构造）
    }
    int m_category;                          // +0x00
    std::string m_sectionName;               // +0x04（[section name]）
    std::string m_field8;                    // +0x08（ORIG 实证 string；解析不使用）
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
    // ORIG C1/C2 0x08aa9906（W）：三个 int 清零。
    stAdjust()
    {
        m_questIdx = 0;
        m_field4 = 0;
        m_achievementIdx = 0;
    }
    unsigned int m_questIdx;       // +0x00
    int m_field4;                  // +0x04
    unsigned int m_achievementIdx; // +0x08
};

struct stItemLockInfo
{
    int m_unlockSec;          // +0x00（解锁请求时刻；完成时刻 = +3 天）
    unsigned char m_state;    // +0x04（1=已锁定，2=解锁倒计时中）
    char m_pad5[3];           // +0x05

    // ORIG W ctor _ZN14stItemLockInfoC1Ev @0x844d406（清零 +0..+7 逐字节）。
    stItemLockInfo()
    {
        m_unlockSec = 0;
        m_state = 0;
        m_pad5[0] = 0;
        m_pad5[1] = 0;
        m_pad5[2] = 0;
    }
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

// ---- 物品锁解锁计时器（ORIG W 符号全部内联于 0x854301e 全局初始化段；
//      方法地址：IncreaseItemLockTimer 0x854147c / DecreaseItemLockTimer
//      0x85414f0 / NextItemLockTimer 0x8541502 / GetTimerCount 0x854303a /
//      ctor 0x8543044（m_count=0）/ Reset 0x8543052（m_count=0））----
class CItemLockTimer
{
public:
    CItemLockTimer();                                       // ORIG 0x8543044：m_count = 0
    void Reset();                                           // ORIG 0x8543052：m_count = 0
    int GetTimerCount();                                    // ORIG 0x854303a（非 const）
    void IncreaseItemLockTimer(CUser* user);                // ORIG 0x854147c
    void DecreaseItemLockTimer();                           // ORIG 0x85414f0
    void NextItemLockTimer(CUser* user);                    // ORIG 0x8541502

    int m_count;  // +0x00：处于倒计时中的物品数
};

// CItemLock：ORIG 布局（ctor 0x854156a / ~0x8541644 / _reset 0x8541a3e）：
//   charac_expand::CData 基类（vptr +0x00 / m_bIsSave +0x04）
//   +0x08 std::map<unsigned char, stItemLockInfo>
//   +0x20 std::stack<unsigned char, std::deque<unsigned char> >（空闲 lock 号池）
//   +0x48 std::vector<stItemLockRef>（SendItemLockList 待发列表）
//   +0x54 CItemLockTimer。
// 对象由角色扩展数据区承载（跨 TU reinterpret_cast）；STL 容器随 GCC4.4 自然布局。

class CItemLock : public charac_expand::CData
{
public:
    CItemLock();                                            // ORIG C1/C2 0x854156a
    virtual bool loadData(CUser* pUser, char* pData);       // ORIG 0x8541714
    virtual void getData(char* pData) const;                // ORIG 0x8541958
    virtual bool _saveData(CUser* pUser);                   // ORIG 0x854176e
    virtual void _reset();                                  // ORIG 0x8541a3e
    virtual ~CItemLock();                                   // ORIG D1/D2 0x8541644 / D0 0x85416f6

    std::vector<stItemLockRef>& GetItemLockRefVec();
    void SendItemLockList(CUser* user);                    // ORIG 0x85427a0
    bool CheckItemLock(unsigned char b) const;             // ORIG 0x8541a96
    void MakeItemLockPacket(PacketGuard& packet, unsigned char b) const;  // ORIG 0x85426a6

    unsigned char GetItemLockInfo(unsigned char lock, int& outSec) const;  // ORIG 0x854261e
    void UpdateItem(CUser* user, Inven_Item* item);        // ORIG 0x8541e4e

    // ---- 锁 / 解锁业务（ORIG 报告 docs/class_func_reports/item_lock__CItemLock/）----
    int IsAbleItemLock(unsigned char lock) const;                       // ORIG 0x8541afe：栈空→0x16 否则 0
    int IsAbleItemUnlock(unsigned char lock, int& outSec) const;        // ORIG 0x8541b2a：命中且 state==1→0 否则 0x13
    int IsAbleItemUnlockCancel(unsigned char lock) const;               // ORIG 0x8541bb0：命中且 state==2→0 否则 0x13
    int DoItemLock(CUser* user, int space, int slot);                   // ORIG 0x8542192
    int DoItemUnlock(CUser* user, int space, int slot);                 // ORIG 0x854231a
    int DoItemUnlockCancel(CUser* user, int space, int slot);           // ORIG 0x85424c2
    int OnItemUnlockWaitTimeout(CUser* user);                           // ORIG 0x8542a6a
    void PopItemLock(CUser* user);                                      // ORIG 0x8542b80
    void PushItemLock(CUser* user, unsigned char lock, stItemLockInfo* info);  // ORIG 0x8542a8a
    void VerifyItemLock(CUser* user);                                   // ORIG 0x8542e98
    int _findItem(unsigned char lock);                                  // ORIG 0x8542e12

    void SendItemUnlock(CUser* user, int space, int slot);              // ORIG 0x85420e0
    void SendRequestItemLock(CUser* user, int space, int slot);         // ORIG 0x8541ea0
    void SendRequestItemUnlock(CUser* user, int space, int slot, int remainSec);  // ORIG 0x8541f5a
    void SendRequestItemUnlockCancel(CUser* user, int space, int slot); // ORIG 0x8542026
    void UpdateCreatereItem(CUser* user, Inven_Item* item);             // ORIG 0x8541c36
    void UpdateAvatarItem(CUser* user, Inven_Item* item);               // ORIG 0x8541d42

    std::map<unsigned char, stItemLockInfo> m_lockMap;     // ORIG +0x08
    std::stack<unsigned char, std::deque<unsigned char> > m_lockStack;  // ORIG +0x20（空闲 lock 号池）
    std::vector<stItemLockRef> m_lockList;                 // ORIG +0x48
    CItemLockTimer m_lockTimer;                            // ORIG +0x54
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
