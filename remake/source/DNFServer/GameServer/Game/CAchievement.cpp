// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAchievement/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 成就 CAchievement 实现（G3-3 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CAchievement.md（Ghidra C + 汇编）+ ORIG
// 反汇编交叉核对；按用户规则优先具名成员访问，不做裸偏移。
#include <cstdio>
#include <cstring>
#include <ctime>
// [P338/P327] ORIG 本 TU 有 _GLOBAL__I__ZN12CAchievementC2Ev（体为
// ios_base::Init __ioinit 构造 + __cxa_atexit，帧 sub $0x18；ORIG 0x0828d813
// __static_initialization_and_destruction_0ii 仅含 __ioinit）⇒ 原码含
// <iostream>。补该 include 以发射 ORIG 实有的 __ioinit 与 GI 符号。
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "CMailBoxHelper.h"   // WongWork::CMailBoxHelper 权威叶子头（替代本 TU 局部镜像）
#include "CAchievement.h"

#include "CDataManager.h"
#include "QuestClear.h"      // WongWork::CQuestClear（+4 偏移访问）
#include "CItem.h"
#include "CMissionList_Charac.h"
#include "CStreamGuard.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "Stream.h"
#include "StreamPool.h"
#include "PacketGuard.h"

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号
// ============================================================================



class CExpandEquipslot;
class CInventory;
class UserQuest;

// 本 TU 的 CUser / CUserCharacInfo / CInventory / UserQuest 局部视图
// （真实定义在 CUser.h / CInventory.h，含与 CTitleBook 冲突的局部类；
//  按 CTitleBook.cpp 同款模式避免混入）。
class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
    int get_charac_level() const;
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };

    unsigned int GetUID() const;
    int get_charac_no(int type);
    ENUM_SERVER_GROUP GetServerGroup() const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    CExpandEquipslot* GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const;
    UserQuest* getCurCharacQuestR() const;
    void Send(PacketGuard& packet);
    void SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);
};

class UserQuest
{
public:
    // b28c-L10：与权威 CUser.h 统一（Main 代落 UserQuest::isClearQuest bool→unsigned char，
    // P174 ORIG __PRETTY_FUNCTION__ 串；返回类型不入 mangled，符号不变）。
    unsigned char isClearQuest(int questIdx) const;
};

class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);
    Inven_Item GetInvenSlot(int invenType, int slot) const;   // ORIG _ZNK10CInventory12GetInvenSlotEii
};



// cUserHistoryLog::AchievementComplete（CUser.h 权威 cUserHistoryLog 声明，
// CAchievement.cpp 使用本地最小视图；符号 _ZN15cUserHistoryLog19AchievementCompleteEi
// 由 cUserHistoryLog.cpp 提供，ORIG 0x08685762）
class cUserHistoryLog
{
public:
    void AchievementComplete(int idx);
};


// ---- CDataManager 称号表视图（+0xa788，与 CTitleBook.cpp 相同布局） ----
struct CTitleBookInfoRaw
{
    char m_pad0[0xc];                                        // +0x00
    std::vector<stTitleSection> m_sections;                  // +0x0c（0xa794）
    std::map<int, std::vector<stTitleElement> > m_elements;  // +0x18（0xa7a0）
    char m_pad34[0x8];                                       // +0x34
    std::vector<stAdjust> m_adjustList;                      // +0x3c（0xa7c4）
};

// CDataManager 称号表视图（+0xa788 起，与 CTitleBook.cpp 相同布局）。
// 宏展开为直接偏移访问：ORIG 汇编为 lea 0xa794/0xa7a0/0xa7c4(%eax)，
// 若用 static 函数在 O0 下会生成额外 call，导致助记符序列不一致。
#define GetTitleSections(mgr) \
    (*(std::vector<stTitleSection>*)((char*)(mgr) + 0xa794))
#define GetTitleElements(mgr) \
    (*(std::map<int, std::vector<stTitleElement> >*)((char*)(mgr) + 0xa7a0))
#define GetAdjustList(mgr) \
    (*(std::vector<stAdjust>*)((char*)(mgr) + 0xa7c4))

// Quest::m_postalReward（+0x9c）的逐字段视图：ORIG 成就邮件按
// {标题, 内容, 道具 idx, addInfo} 读取（Quest.h 的 PostalReward 未具名 +0x08）。
struct PostalRewardView
{
    std::string m_title;     // +0x00
    std::string m_contents;  // +0x04
    int m_itemIdx;           // +0x08
    int m_addInfo;           // +0x0c
};

// ============================================================================
// stAchievement / stAchievement_DBData
// ============================================================================

#include "CAchievement/CAchievement.inc"


// stAchievement::stAchievement()/isComplete()、stAchievement_DBData::
// stAchievement_DBData() 已按 ORIG W 绑定改为 CAchievement.h 类内 inline 定义
//（P233；外联定义会发射 T 强符号）。

// ============================================================================
// CAchievement
// ============================================================================

#include "CAchievement/dtor_CAchievement.inc"


// ---- 触发 / 校验 / 完成 ----

#include "CAchievement/setTrigger.inc"


#include "CAchievement/_verifyTrigger.inc"
                                  unsigned short t1, unsigned short t2)
{
    switch (id)
    {
    case 0x19c4:
    {
        time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm local;
        localtime_r(&now, &local);
        if (local.tm_hour < 0)
        {
            break;
        }
        if (local.tm_hour > 0x14)
        {
            break;
        }
        return 0;
    }
    case 0x198b:
    case 0x1983:
        if (m_user->get_charac_level() <= 0x45)
        {
            return 0;
        }
        break;
    }
    return 1;
}

#include "CAchievement/_setTrigger.inc"


#include "CAchievement/isClearedAchievement.inc"


#include "CAchievement/_checkComplete.inc"


#include "CAchievement/_forceClear.inc"


#include "CAchievement/_forceReset.inc"


#include "CAchievement/_getTrigger.inc"


#include "CAchievement/_verifyAchievement.inc"


#include "CAchievement/adjust.inc"


// ---- 发送 / 存档 ----

#include "CAchievement/sendList.inc"


#include "CAchievement/_sendSetTrigger.inc"


#include "CAchievement/_sendComplete.inc"


#include "CAchievement/_sendCompleteReward.inc"


#include "CAchievement/_sendEachAchieveReward.inc"


#include "CAchievement/_sendPostal.inc"


#include "CAchievement/_gm_init.inc"


#include "CAchievement/_gm_report.inc"


#include "CAchievement/_gm_clearAll.inc"


#include "CAchievement/_reset.inc"


#include "CAchievement/_saveData.inc"


#include "CAchievement/loadData.inc"


#include "CAchievement/getData.inc"


// ---- 章节查询 ----

#include "CAchievement/_getSectionIdx.inc"


#include "CAchievement/_getPostalReward.inc"


#include "CAchievement/_getTabComplete.inc"

