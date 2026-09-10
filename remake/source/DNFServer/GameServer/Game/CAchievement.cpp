// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CAchievement/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// [P327] ORIG 的 _GLOBAL__I_ 键名为 _ZN12CAchievementC2Ev（ORIG 0x0828d853），
// 且 ORIG C1/C2 地址 0x0828b614 早于 stAchievementC1 0x0828d870 ⇒ 原码把
// CAchievement ctor 定义在本 TU 最前。GI 键名 = TU 内第一个强符号外部链接
// 实体名；把本定义前置即对齐键名（只移函数，不改构造序）。
#include "CAchievement/CAchievement.inc"
}

// stAchievement::stAchievement()/isComplete()、stAchievement_DBData::
// stAchievement_DBData() 已按 ORIG W 绑定改为 CAchievement.h 类内 inline 定义
//（P233；外联定义会发射 T 强符号）。

// ============================================================================
// CAchievement
// ============================================================================

#include "CAchievement/dtor_CAchievement.inc"
}

// ---- 触发 / 校验 / 完成 ----

#include "CAchievement/setTrigger.inc"
}

// [b28d-solo-CAchievement AE] 修法：ORIG 0x0828b81e 为 `mov 0xc(%ebp),%eax` +
// 三次 `cmp $imm,%eax`
// （寄存器链，P286/P251/P261）⇒ 源形是 switch（if 链在 4.4 恒 mem 直比，P209）。
// ORIG cmp 顺序 198b→19c4→1983 且两个 level 块 tail-merge 到同一地址 +0x6c。
// level 极性按 ORIG `cmp $0x45; setle %al; test; je <mov $1>` ⇒
// 源形 `if (level <= 0x45) return 0; return 1;`（P56/P292）。
#include "CAchievement/_verifyTrigger.inc"
}

#include "CAchievement/_setTrigger.inc"
}

#include "CAchievement/isClearedAchievement.inc"
}

#include "CAchievement/_checkComplete.inc"
}

// [b28d-solo-CAchievement AE] 修法：删 book/inven 具名局部改匿名全表达式链
//（P0/P278；ORIG 0x0828cd6x 无这两槽，GetCharacExpandData/getCurCharacInvenR
//  结果寄存器直传）⇒ 帧 0xc0、push esi+ebx、全部槽号与 ORIG 逐条对齐。
// .o 口径显示 DIFF 仅因 call 重定位标签未解析（P264）。
#include "CAchievement/_forceClear.inc"
}

// [b28d-solo-CAchievement N104/P223 封顶，2026-09-09 复测] 修法同 _forceClear：
// ORIG 0x0828ce40 无 book/inven 具名槽（GetCharacExpandData/getCurCharacInvenW
// 结果寄存器直传），改匿名全表达式链后帧 0x40/槽号/push esi+ebx 全部对齐
//（P0/P278）。残差 = 尾部岛数 ORIG 5 个 `nop; jmp +0x248` + 尾裸 nop vs 4.4
// 4 个（invenItem==0 与 addInfo!=titleIdx 共落点，our 168）或 6 个（our 172，
// 见下）；本函数只此一处形态差 + call 重定位标签伪差（P264）。语义等价。
// 复测四形（fresh check_tu_routed，orig=170）：①&& 短路（当前）168/4 岛；
// ②嵌套 if 168/4 岛；③否定 goto+尾标签串 172/6 岛；④早返 172/6 岛。
// P223 负例成立：ORIG 末条件直落 epilogue（+0x248）属 4.1 布局，4.4 恒给
// 中继岛 ⇒ 无法同时给出「5 岛 + 尾裸 nop」，取指令数差最小的 && 形。
#include "CAchievement/_forceReset.inc"
}

#include "CAchievement/_getTrigger.inc"
}

// [b28d-solo-CAchievement N95/N82 封顶] ORIG 0x0828d092 非 static 成员：
// loadData 0x0828c956 压 this + in->m_saveTime 两实参，体内 0x8(%ebp)=this
// ⇒ 解引用 +8=m_user。旧 D68「static + self 参数」版把 m_saveTime 当 this
// 解引用 = 真 UB，已修（真语义 bug）。
// ①删 ach 具名局部：ORIG 每处重读 0x8(%ebp)（P0 表达式直用）；
// ②删 list 引用缓存：ORIG 内层条件与体各自 `it->second`（每轮 2 次
//   operator->() const，P157）；③empty()→size()!=0（ORIG 0x0808e1c0
//   `_ZNKSt6vectorIiSaIiEE4sizeEv`），quest 改 const Quest* 使
//   m_triggerData[0] 走 const operator[]（ORIG 0x080ea574）；
// ④special bool 改内联复合条件（ORIG mov $1/$0 物化岛）；
// ⑤删 level 缓存：ORIG 两臂各自 get_charac_level()（P48）；
// ⑥isClearMission 实参 0x83/0x87/0x19c0 用具名局部（ORIG -0x48/-0x44/-0x1c）。
// [残差，2026-09-09 复测] 首 continue 改正向包裹后 185=185、岛数与 ORIG 逐点
// 对齐（P223：平铺三 continue 时 4.4 把 achievementIdx==-1 也岛化，多 2 条）。
// 余下残差 = ①槽位前段整体深 8（it -0x40 vs -0x38，4.4 恒预留 outgoing 对齐区
// 的版本常数 N115/N126）②实参先于 this 装载（ORIG `mov -0x2c(%ebp),%ebx;
// call getCurCharacQuestR; add $0x4,%eax` vs 4.4 后读）③N107 成员偏移取址
// add-vs-lea。P277 具名 receiver 形实测更劣（槽再深 4）⇒ 保持当前形。
#include "CAchievement/_verifyAchievement.inc"
}

// [2026-09-09 复测] 真 bug 已修（find_quest 实参 questIdx→achievementIdx，
// ORIG 0x0828ca18 压 -0x18=m_achievementIdx）；titleIdx 内联进 putTitleOnly
// 实参位（P0，ORIG 结果留 %eax 直压）；补 -0x1c 死局部（P171）；声明序改
// questIdx/unused/achievementIdx/slot/category 后 168=168 且 6 条初始化与
// ORIG 槽值/发射序全等（questIdx@-0x20、死@-0x1c、achievementIdx@-0x18、
// slot@-0x24、category@-0x28、i@-0x14；&category/&slot 实参槽序经 0x0828cbb6
// 段核对）。余下残差仅 N7 寄存器角色（isClearedQuest/find_quest 的 ebx↔edx
// 缓存互换、movzwl 目标寄存器）+ N107 取址形。
#include "CAchievement/adjust.inc"
}

// ---- 发送 / 存档 ----

// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %edx↔%eax 互换（121 条逐条
// 长度/助记符全等仅寄存器名差），临时/引用/限定名不可翻。
#include "CAchievement/sendList.inc"
}

#include "CAchievement/_sendSetTrigger.inc"
}

#include "CAchievement/_sendComplete.inc"
}

// [b28d-solo-CAchievement AE] 修法：①删 postal 具名局部（P0）②sectionIdx int 化
//（P218：size_t 转换中转触发 ORIG 的索引先求值 `mov -0x10(%ebp),%ebx` + add 形）。
// 帧 0x70、section/reward/字符串临时槽号全对齐；.o 口径 DIFF 为重定位伪差（P264）。
#include "CAchievement/_sendCompleteReward.inc"
}

#include "CAchievement/_sendEachAchieveReward.inc"
}

// [b28d-solo-CAchievement N7/N109 封顶] 三保活值 callee-saved 角色互换
//（ORIG GetServerGroup→edi、subject.c_str→esi、getCurCharacNo→ebx vs 4.4
//  →esi/→ebx/→edi；74 条长度/助记符全等，无虚调用，实参求值序相同）。
// P248 三档类型旋钮不适用（无 store 序/类型差）；本 TU 已路由 4.4.6 后端仍不翻。
// ORIG 0x0828c152。
#include "CAchievement/_sendPostal.inc"
}

#include "CAchievement/_gm_init.inc"
}

// [2026-09-09 复测 AE] 旧 N78 判词作废：ORIG 0x0828d450 的两处 memset(0x400)
// 只有**循环前**那次被 4.1 内联为 rep stos（0x0828d467..0x47b），循环内那次
// 仍是 call memset（0x0828d70c）。修法（P328⑨）：把 `char buf[0x400];` +
// 显式 memset 改为声明即初始化 `char buf[0x400] = {0};`，并把声明放到
// `stAchievement ach;` **之后**以保持 ORIG 的「ach C1 → buf 清零」发射序；
// 循环内 memset 不动。改后 194=194，本件由 DIFF 转 AE（槽/寄存器全对齐）。
#include "CAchievement/_gm_report.inc"
}

#include "CAchievement/_gm_clearAll.inc"
}

#include "CAchievement/_reset.inc"
}

#include "CAchievement/_saveData.inc"
}

// [b28d-solo-CAchievement AE] 真语义 bug 修复：`_verifyAchievement` 由 static
// 改非 static（CAchievement.h）后本函数调用点压 this + in->m_saveTime 两实参，
// 与 ORIG 0x0828c956 一致（旧 static 版把 m_saveTime 当 this 解引用 = UB）。
#include "CAchievement/loadData.inc"
}

#include "CAchievement/getData.inc"
}

// ---- 章节查询 ----

#include "CAchievement/_getSectionIdx.inc"
}

#include "CAchievement/_getPostalReward.inc"
}

// [b28d-solo-CAchievement AE] 修法：ORIG 0x0828bdc6 result 无栈槽往返（ebx=循环完成标志、
// esi=返回值，尾部 `test %ebx; je; mov $1,%esi; mov %esi,%eax`）⇒ 原码是
// **两 return 形**（`return false` 在析构范围内 → 4.1 保存 esi/ebx 后跳
// cleanup），不是 result 局部 + break（后者 4.4 占槽 -0x10 + setne）。
#include "CAchievement/_getTabComplete.inc"
}
