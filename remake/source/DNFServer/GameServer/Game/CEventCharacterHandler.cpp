// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventCharacterHandler/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CEventCharacterHandler（G2-4 批次，活动角色处理）。
// 依据 docs/class_func_reports/CEventCharacterHandler.md + ORIG 反汇编。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__Z30CEventCharacterHandlerInstancev`（ORIG 0x848ecb2）。
#include <iostream>


#include <algorithm>
#include <utility>
#include <vector>

#include "EventCharacterParameterScript.h"
#include "GlobalData.h"
#include "TimerQueue.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CHandlePremium.h"
#include "CEventCharacterHandler.h"

// ---- 外部符号声明（对应 TU 翻译后移除） ----
char* NumberToString(unsigned int value, int radix);

enum eItemAddReason
{
    eItemAddReason_0 = 0
};

enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_0 = 0
};

enum ENUM_NOTIPACKET
{
    ENUM_NOTIPACKET_0 = 0
};

enum ENUM_SKILL_TREE_KIND
{
    ENUM_SKILL_TREE_KIND_0 = 0
};

#ifndef DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
#define DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
enum ENUM_PREMIUM_TYPE { ENUM_PREMIUM_TYPE_0 = 0 };
#endif

class CItem
{
public:
    bool is_stackable() const;
};

class CStackableItem
{
public:
    int getStackableLimit() const;
};

class CSkill
{
public:
    int get_group() const;
};

class SkillSlot;

class CUserCharacInfo
{
public:
    int get_charac_job() const;
    int get_charac_level() const;
    const char* getCurCharacName() const;
    class SkillSlot* getCurCharacSkillR() const;
    signed char getCurCharFirstGrowType() const;
    char getCurCharacEventCharacterGrowtype();
    void setCurCharacEventCharacterGrowtype(char growType);
};

class SkillSlot
{
public:
    int get_skillslot_no(int skillIdx, int group,
                         ENUM_SKILL_TREE_KIND treeKind, bool flag) const;
    void* get_skillslot_buf(ENUM_SKILL_TREE_KIND treeKind) const;
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };

    unsigned int get_acc_id() const;
    unsigned int GetUID() const;
    int get_state();
    bool isAffectedPremium(ENUM_PREMIUM_TYPE type) const;   // ORIG _ZNK5CUser18isAffectedPremiumE15ENUM_PREMIUM_TYPE
    void* GetPremiumInfoW();                                // ORIG WongWork::CUserPremium*
    bool HasCurCharacRentalItem(int flag) const;            // ORIG _ZNK5CUser22HasCurCharacRentalItemEi 0x86772d2
    void Send(PacketGuard& packet);                         // ORIG _ZN5CUser4SendER11PacketGuard 0x86485ba
    class UserQuest* getCurCharacQuestW();
    int AddItem(int itemIdx, int count, eItemAddReason reason,
                ENUM_ITEMSPACE& itemSpace, int param5);
    void SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE itemSpace, int slot);
    bool master_new_skill(struct stBuySkillInfo& info, bool flag);
    void SetCharacLevel(int level);
    bool ChangeGrowType_GM(int growType, int secondGrowType);
    void AddDungeonClear(int dungeonIdx, int count);
    void SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET notiType, int param);
};

class CDataManager;
CDataManager* G_CDataManager();

class CSystemTime;
CSystemTime& s_systemTime_holder();

namespace WongWork
{
// ---- CHandlePremium 组件（ORIG 报告 docs/class_func_reports/WongWork__CHandlePremium/）----
class CUserPremium
{
public:
    // ORIG _ZN8WongWork13CUserPremium10AddPremiumEiiii（签名按 handleSetUserPremium 调用点）
    void AddPremium(int type, int start, int end, int flag);
    void RemovePremium(int type);
    void ReCalcAdvantage();
    void RecalcAdditionalInfo(CUser const* user);
};

class CQuestClear
{
public:
    void setClearedQuest(unsigned int questIdx);
};
}

struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};

class CDataManager
{
public:
    const CItem* find_item(int itemIdx) const;
    const CSkill* find_skill(int job, int skillIdx) const;

    char m_pad[0xa86c];                                  // +0x00
    EventCharacterParameterScript m_eventCharacScript;   // +0xa86c
};
// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0848ecb2。
CEventCharacterHandler* CEventCharacterHandlerInstance()
{
    static CEventCharacterHandler instance;
    return &instance;
}

#include "CEventCharacterHandler/CEventCharacterHandler.inc"

#include "CEventCharacterHandler/_checkMakeCommand.inc"
}

#include "CEventCharacterHandler/_UpdateEventCharacInfo.inc"
}

#include "CEventCharacterHandler/_SetClearedQuest.inc"
}

#include "CEventCharacterHandler/_MasterNewSkill.inc"
}

#include "CEventCharacterHandler/_AddItem.inc"
}
#include "CEventCharacterHandler/_SetLevel.inc"
}

#include "CEventCharacterHandler/_SetGrowType.inc"
}

#include "CEventCharacterHandler/_makeEventCharacter.inc"
}

