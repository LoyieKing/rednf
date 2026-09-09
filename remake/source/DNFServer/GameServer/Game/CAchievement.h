#ifndef GAME_CACHIEVEMENT_H_
#define GAME_CACHIEVEMENT_H_

// df_game_r 成就 CAchievement（G3-3 批次，2026-08-17）。
// 布局依据 ORIG ctor 0x828b614 反汇编推导：
//   CData 基类（pack(1) vptr +0x00 / m_bIsSave +0x04，作为非 pack 派生
//     基类子对象按 4 对齐补齐为 8 字节）
//   CUser* m_user                     +0x08
//   std::map<uint, stAchievement>     +0x0c（sizeof 0x18）
//   char m_field24                    +0x24
//   char m_field25                    +0x25
//   sizeof = 0x26（非 pack(1)；class 声明不套 pack）。

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CTitleBook.h"
#include "Quest.h"

class CUser;
class PacketGuard;

// ---- 成就项（12 字节） ----
struct stAchievement
{
    unsigned int m_id;   // +0x00
    unsigned short m_trigger0;  // +0x04
    unsigned short m_trigger1;  // +0x06
    unsigned short m_trigger2;  // +0x08
    unsigned short m_state;     // +0x0a

    // ORIG W 弱符号 _ZN13stAchievementC1Ev/C2Ev @0x0828d870（各 0x32）
    // ⇒ 原码头内定义（外联定义会发射 T 强符号，绑定不符）。
    // 绑定探针（nm -S /tmp/tu_CAchievement.o，改前→改后，尺寸不变）：
    //   stAchievementC1Ev/C2Ev  T 0x32 → W 0x32；isComplete T 0x35 → W 0x35；
    //   stAchievement_DBDataC1Ev/C2Ev T 0x4d → W 0x4d。
    stAchievement()
    {
        m_id = 0;
        m_trigger0 = 0;
        m_trigger1 = 0;
        m_trigger2 = 0;
        m_state = 0;
    }
    // ORIG mangled 名为 _ZN13stAchievement10isCompleteEv（非 const 成员），
    // ORIG W 0x0828d8a2（0x35）⇒ 头内定义。
    bool isComplete()
    {
        return (m_trigger0 == 0) && (m_trigger1 == 0) && (m_trigger2 == 0);
    }
};

// ---- 成就存档数据（0x1808 字节；ORIG 有 C1/C2 弱构造符号） ----
struct stAchievement_DBData
{
    stAchievement m_entries[512];  // +0x00（12*512 = 0x1800）
    int m_count;                   // +0x1800
    unsigned int m_saveTime;       // +0x1804（b28c-L10：ORIG getData 0x828c6e0 RHS-first
                                   // store ⇒ 与 getCurSec() int 异型，P211②）

    // ORIG W 弱符号 _ZN20stAchievement_DBDataC1Ev/C2Ev @0x0828d8d8（各 0x4d）
    // ⇒ 头内定义（m_entries 数组由编译器自动逐个构造，ORIG esi 倒计数 512 次）。
    // 改后 CAchievement.o 帧 sub $0x10 与 ORIG 0x0828d8d8 一致（P210 未恶化）。
    stAchievement_DBData()
    {
        m_count = 0;
        m_saveTime = 0;
    }
};

// ---- 成就存档（DB，0x180c 字节） ----
struct stAchievement_DBSaveData
{
    stAchievement_DBData m_data;  // +0x00
    int m_characNo;               // +0x1808
};

// ABI 锁定（仅 sizeof：这些结构含用户声明 ctor（非标准布局），gcc4.4 下
// offsetof 不可用；成员顺序已保证 +0x1800/+0x1804/+0x1808 偏移）。
static_assert(sizeof(stAchievement) == 0x0c, "stAchievement ABI drift");
static_assert(sizeof(stAchievement_DBData) == 0x1808, "stAchievement_DBData ABI drift");
static_assert(sizeof(stAchievement_DBSaveData) == 0x180c, "stAchievement_DBSaveData ABI drift");

// ============================================================================
// CAchievement
// ============================================================================

class CAchievement : public charac_expand::CData
{
public:
    CAchievement();
    virtual ~CAchievement();

    virtual bool loadData(CUser* pUser, char* pData);
    virtual void getData(char* pData) const;
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();

    // ---- 触发 / 校验 / 完成 ----
    void setTrigger(unsigned int id, unsigned short t0, unsigned short t1,
                    unsigned short t2);
    void _setTrigger(unsigned int id, unsigned short t0, unsigned short t1,
                     unsigned short t2);
    bool _verifyTrigger(unsigned int id, unsigned short t0, unsigned short t1,
                        unsigned short t2);
    // ORIG mangled 名 _ZN12CAchievement18_verifyAchievementEj（带一个 uint
    // 形参，未使用）。b28d-solo：ORIG loadData 0x0828c956 调用点压两实参
    //（(%esp)=this、0x4(%esp)=in->m_saveTime），体内 0x8(%ebp)=this 解引用
    // +8=m_user ⇒ 非 static 成员（static 版会把 m_saveTime 当 this 解引用，
    // 真 UB）。static 不入 mangling，符号/ABI/sizeof 不变。
    void _verifyAchievement(unsigned int self);
    void _checkComplete(unsigned int id);
    bool isClearedAchievement(unsigned int id);
    void _forceClear(unsigned int id);
    void _forceReset(unsigned int id);
    void adjust();

    // ---- 发送 / 存档 ----
    void sendList();
    void _sendSetTrigger(unsigned int id, unsigned char flag);
    void _sendComplete(unsigned int id, int titleIdx,
                       ENUM_TITLE_BOOK_CATEGORY category, int slot);
    void _sendCompleteReward(const Quest* quest,
                             ENUM_TITLE_BOOK_CATEGORY category, int slot);
    void _sendEachAchieveReward(const Quest* quest);
    void _sendPostal(int itemIdx, int addInfo, std::string sender,
                     std::string subject);
    // ORIG 按隐藏返回指针（sret）ABI 调用：stAchievement 按值返回，
    // mangled 名 _ZN12CAchievement11_getTriggerEj（仅一个 uint 参数）。
    stAchievement _getTrigger(unsigned int id);
    void _gm_init();
    void _gm_report(std::string& out);
    void _gm_clearAll();

    // ---- 章节查询 ----
    static unsigned int _getSectionIdx(ENUM_TITLE_BOOK_CATEGORY category,
                                       int slot);
    bool _getTabComplete(stTitleSection* section,
                         ENUM_TITLE_BOOK_CATEGORY category);
    stTitleRewardBasis* _getPostalReward(stTitleSection* section);

    // ---- 成员 ----
    CUser* m_user;             // +0x08
    std::map<unsigned int, stAchievement> m_achievements;  // +0x0c
    char m_field24;            // +0x24
    char m_field25;            // +0x25
};

#endif  // GAME_CACHIEVEMENT_H_
