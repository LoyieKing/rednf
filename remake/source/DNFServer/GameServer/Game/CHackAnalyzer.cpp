// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CHackAnalyzer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CHackAnalyzer 实现（G6 missing 批次，权威头
// CHackAnalyzer.h；Secu_AccountHacking 全部方法 + CHackAnalyzer 缺失方法）。
// 依据 docs/class_func_reports/WongWork__CHackAnalyzer/*.md 与
// Secu_AccountHacking/*.md 逐函数还原（##汇编 + ##反编译C 两段均已读）；
// 报告缺失的 4 个小符号（getServerHackCnt / resetServerHackCnt /
// setServerHackAccumulatedCnt / checkCollectedHack）以 ORIG nm + objdump
// 区间取证后转写并明确标注「报告缺失」。附带落 DB_SelectCharacMid /
// DB_ReqSaveHackUserPunish::makeRequest（各自报告在手，此前无 owner）。
#include "CHackAnalyzer.h"

#include <algorithm>
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameRand.h"
#include "CParty.h"
#include "CDungeon.h"
#include "CBattle_Field.h"
#include "CSystemTime.h"
#include "PacketBuf.h"
#include "DB_SelectCharacMid.h"
#include "DB_ReqSaveHackUserPunish.h"
#include "W6_HackPunishDeps.h"

// ---- 静态概率表（ORIG .rodata 0x8b3e094 / 0x8b3e09c，objdump -s 实测）----
static const short _S_HACK_CHECK_RATE_BY_PARTYCOUNT[4] = {100, 50, 33, 25};
static const short _S_HACK_CHECK_RATE_BY_DIFFICULT[5] = {25, 20, 15, 10, 0};

// 浮点比率系数（ORIG flds 0x8b3e2a0 / 0x8c00fd0，objdump 实测均 100.0f）
static float _S_HACK_RATIO_COEF_100F = 100.0f;  // 非 const：ORIG flds 内存读形（const 会被折叠成 fldl）

namespace WongWork
{

// 计数区寻址（ORIG 实测：A 表槽 = this + (t-0x65)*4 + 0x28，即
// m_cntRegion(=this+0x28) + (t-0x65)*4；B 表槽 = this + (t-0x65+0xf44)*4 + 8，
// 即 m_cntRegion + (t-0x65+0xf44)*4 + 8 - 0x28。getServerHackCnt 0x08695086 /
// addServerHackCnt 0x080f8c7e 反汇编核实）。
#define HACK_CNT_A(T) (m_serverHackCntA[(int)(T) - 0x65])
#define HACK_CNT_B(T) (m_serverHackCntB[(int)(T) - 0x65])
// B 表槽访问（ORIG addServerHackCnt 0x080f8c7e 的 lea idx-0x65+0xf44 复合寻址形；
// A 表 getter/setter 已随判据㉓ 内联化进 CHackAnalyzer.h）。
#define HACK_SLOT_B(T) (m_serverHackCntB[(int)(T) - 0x65])

// ORIG WongWork::CAutoPunishRuleHackTypeMgr 布局镜像（权威定义在
// GlobalData.cpp；此处仅按偏移只读访问 m_exceptionMap @+0x58）。「推断」标注：
// 偏移来自 mgr ctor 反汇编 0x82a5b1a。
#include "CHackAnalyzer/setUserP.inc"
}

#include "CHackAnalyzer/getUserP.inc"
}

#include "CHackAnalyzer/beginCollectHackInfo.inc"
}

#include "CHackAnalyzer/CheckExceptionCnt.inc"
}

// b28d-solo-CHackAnalyzer: AE（const_iterator 转换构造 + (int)type 内联实参
// 复现 ORIG 0x080f8320 的 P96/P236 形；旧「N 封顶 sret lowering」判词作废）。
#include "CHackAnalyzer/CheckHackRateByPartyCount.inc"
}

// b28d-solo-CHackAnalyzer: AE（同 CheckHackRateByPartyCount 的 P96/P236 形；
// ORIG 0x080f842e）。
#include "CHackAnalyzer/CheckHackRateByDifficult.inc"
}

// b28d-solo-CHackAnalyzer: AE（299/299，无实质残差；ORIG 0x080f8550）。
#include "CHackAnalyzer/AddCheckRateHackTypeList.inc"
}

#include "CHackAnalyzer/ReqDBSaveHackInfo.inc"
}

// b28d-solo-CHackAnalyzer: AE（const_iterator + 独立 begin/end 局部 + `it->`
// 实参（ORIG 4 次 operator->）+ 死拷贝 info + 尾部显式 return；旧 N89/P197
// 判词作废）——ORIG 0x080f8080。
#include "CHackAnalyzer/reportHackInfo.inc"
}

#include "CHackAnalyzer/resetHackInfo.inc"
}

// b28d-solo-CHackAnalyzer: AE（stHackInfo_t 平凡析构 + m_value unsigned +
// `m_flag04 | flag` 左操作数序；旧 N82/P254 判词作废）——ORIG 0x080f7fd2。
#include "CHackAnalyzer/_addHackInfo.inc"
}

#include "CHackAnalyzer/analyzeHackReport.inc"
}

// b28d-solo-CHackAnalyzer: N 封顶（N82+ 槽分区，本轮五形复测）——ORIG 0x080f7cca。
//   槽图（ORIG）: ratio@-0xc, elapsed@-0x10, X(8B 死区)@-0x18..-0x11,
//   numMs@-0x20..-0x19, CW@-0x22, CW2@-0x24；帧 0x38。
//   4.4 x87 工作槽恒**紧贴最深槽对象**，故两态不可两全：
//   ①无死局部（V0）：numMs@-0x18/CW@-0x1a/-0x1c（帧 0x38 ✓，numMs 浅 8）；
//   ②`long long deadPad`/`char[8]`/2×int/`long long[2]` 置于 numMs 后：
//     numMs@-0x20 ✓ 但死局部恒落 numMs **深侧**（-0x28）⇒ CW 被推至 -0x2a/-0x2c、
//     帧 0x48（4 形实测同结论；V3 置于 numMs 前反使 numMs 回 -0x18）。
//   4.4 未使用标量「按最深可用处分配」，源码不可控其浅侧落位 ⇒ N 封顶（帧/助记符全等）。
//   [round5 复核] fresh compare_tu 口径 55/55 NEAR（.o 报告 DIFF 系 N124 假报）；
//   槽图 ORIG numMs@-0x20 / CW@-0x22,CW2@-0x24 vs 我方 numMs@-0x18 / CW@-0x1a,-0x1c
//   —— 4.4 把 x87 工作槽紧贴「最深槽对象」的分配策略不可源级翻转 ⇒ 维持 N82。
#include "CHackAnalyzer/_analyzeSpeedHack.inc"
}

// 判词 N（N82 族：4.1 死局部槽位策略）：ORIG numMs@-0x20/CW@-0x22/CW2@-0x24 vs
//   本工具链 numMs@-0x18/CW@-0x1a/CW2@-0x1c —— ORIG numMs 与 elapsed(-0x10) 间
//   存在 8 字节死槽(-0x18..-0x11)。本轮实测：①未使用 `long long`/`char[8]` 局部
//   恒被置于最深槽（pad@-0x28）并抬帧 0x38→0x48（反劣）；②16 字节 POD 局部
//   （packed/非 packed）同样落 -0x30 或多出结构拷贝。4.4 无「按声明序保留死槽」
//   能力 ⇒ 帧 0x38 两侧等齐、助记符序列全等、仅 3 处槽号差，N 封顶不追。

#include "CHackAnalyzer/_analyzeDifficultyHack.inc"
}

#include "CHackAnalyzer/_analyzeMemoryHack.inc"
}

// b28d-solo-CHackAnalyzer: 槽序列视图（P311 同族）——ORIG 0x080f7e18。
#include "CHackAnalyzer/_analyzeEquipmentUpgradeHack.inc"
}

// b28d-solo-CHackAnalyzer: AE（窗口检查改正向包裹 `if (<= 0xa) {...}`，
// ORIG 0x080f7eb6 双出口岛形逐条对齐；旧 P223 边界判词作废）。
#include "CHackAnalyzer/_analyzeMonsterDieHack.inc"
}

#include "CHackAnalyzer/analyzeMonsterDieHack.inc"
}

#include "CHackAnalyzer/_reportIncurrectReport.inc"
}

#include "CHackAnalyzer/ValidateFixMobDiePos.inc"
}

#include "CHackAnalyzer/onUpdateHackCount.inc"
}

// CParty 子对象视图（b28d-solo-CHackAnalyzer）：ORIG addServerHackCnt 0x080f8db5
// 对 party+0xcac 直读指针（`mov 0xcac(%eax),%eax`）、对 party+0xb24 取内嵌子对象
// 地址（`add $0xb24,%eax; call get_dungeon_diff`）。按 §8.3 未建模子对象具名化，
// 避免 char* 加法把 disp 折成 add 指令（P82/P301）。
namespace {
struct CPartyHackView
{
    char m_pad0[0xcac];
    CDungeon* m_pDungeon;                    // +0xcac（CDungeon*；0xb24 内嵌
                                             // CBattle_Field 仍用真实类型 cast 取址）
};
}

// b28d-solo-CHackAnalyzer: AE（CParty 视图 + 嵌套 if + 三目 lvDiff + 去
// (int)count cast + 去 dungeon/memberCount 局部；旧 N82/P183 判词作废）
// ——ORIG 0x080f8c7e。
#include "CHackAnalyzer/addServerHackCnt.inc"
}

#include "CHackAnalyzer/ReqDBSaveHackUserPunish.inc"
}

}  // namespace WongWork

// ============================================================================
// Secu_AccountHacking（全局作用域；声明见 CHackAnalyzer.h）
// ============================================================================

#include "CHackAnalyzer/init.inc"
}

#include "CHackAnalyzer/resetInfo.inc"
}

#include "CHackAnalyzer/setLoginTime.inc"
}

#include "CHackAnalyzer/getMatchingHacktype.inc"
}

#include "CHackAnalyzer/isAvatar.inc"
}

// b28d-solo-CHackAnalyzer: N 封顶（P75 跨版本装载序，本轮七形复测）——ORIG 0x08278b48。
//   唯一残差 = 循环体前两条 load：ORIG 0x08278c69 `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%ecx`
//   （i 先、msg 形参后）；我方 f1 恒 msg 先（`mov 0x10(%ebp),%ecx; mov -0xc(%ebp),%edx`），
//   其余 8 条逐条全等（含 `lea (%ecx,%eax,1); add $0x30; mov 0xb(%eax)` 常量拆分）。
//   本轮实测七形（探针 /tmp/hc_probe2.cpp）：f1 现状/f2 (unsigned)i/f3 解引用/f6 i*1/f11 双 cast
//   恒 msg 先；f4 具名视图指针、f5 具名元素指针可得 i 先但多发函数头 store（ORIG 无）；
//   f8/f14 裸指针加法得 i 先但改用 add-mem 且少 lea/或改常量拆法 ⇒ 形不符。
//   ⇒ 4.4 对「形参基址 + 下标」恒 base-first 调度，属跨版本装载序，N 封顶（指令数全等）。
//   [round5 复核] fresh 108/108（compare_tu NEAR）；唯一残差仍为该两条 load 序
//   （ORIG 0x08278c69 i 先 / 我方 msg 先），八形穷举后维持 N75/N18a 封顶。
#include "CHackAnalyzer/sendMail.inc"
#pragma pack(push, 1)
    struct CheckMailReceiverSendView
    {
        int m_checkType;              // +0x00（set<> 写入的 TYPE）
        char m_msgHead[0x32 - 4];     // +0x04..+0x31（本函数不读的消息前缀）
        unsigned int m_money;         // +0x32
        char m_pad36[0x3d - 0x36];    // +0x36..+0x3c
        unsigned int m_avatarItemIdx; // +0x3d..+0x40
    };
    struct CheckMailReceiverListEntry
    {
        unsigned int m_itemIdx;       // 条目 +0x00（ORIG 步长 0xb）
        char m_rest[0xb - 4];         // +0x04..+0x0a（本函数不读）
    };
    struct CheckMailReceiverListView
    {
        int m_checkType;              // +0x00
        char m_msgHead[0x33 - 4];     // +0x04..+0x32（本函数不读的消息前缀）
        unsigned int m_money;         // +0x33
        unsigned char m_entryCount;   // +0x37
        char m_pad38[0x3b - 0x38];    // +0x38..+0x3a
        CheckMailReceiverListEntry m_entries[1];   // +0x3b
    };
#pragma pack(pop)
    static_assert(sizeof(CheckMailReceiverSendView) == 0x41,
                  "CheckMailReceiverSendView ABI drift");
    static_assert(offsetof(CheckMailReceiverListView, m_money) == 0x33 &&
                  offsetof(CheckMailReceiverListView, m_entryCount) == 0x37 &&
                  offsetof(CheckMailReceiverListView, m_entries) == 0x3b,
                  "CheckMailReceiverListView ABI drift");
    static_assert(sizeof(CheckMailReceiverListEntry) == 0xb,
                  "CheckMailReceiverListEntry stride drift");

    // ORIG T 0x08278b48（sendMail.md）：检查类型 0（金币+单 avatar）/2
    // （金币+avatar 列表）时取出金额与 avatar 物品号逐一 sendGold/sendAvatar。
    // MSG_CHECK_MAIL_RECEIVER 各字段偏移（0x33/0x37/0x3b/0x3d/0x32）来自报告
    // 汇编；该类型为 set<T> 写入的字节数据区，具名化见上方视图。
    if (((CUser*)m_userP)->getCurCharacInvenR() == 0)
        return;

    if (*(int*)msg == 0)
    {
        // P0：ORIG getCurCharacInvenR 结果不经栈槽往返（`call; mov %eax,(%esp);
        // call get_money`）⇒ 内联实参，具名指针局部多 2 条 spill。
        sendGold(name, 0, ((CheckMailReceiverSendView*)msg)->m_money,
                 ((CUser*)m_userP)->getCurCharacInvenR()->get_money(),
                 2, (char*)0);
        if (isAvatar(((CheckMailReceiverSendView*)msg)->m_avatarItemIdx))
            sendAvatar(name, 0);
    }
    else if (*(int*)msg == 2)
    {
        sendGold(name, 0, ((CheckMailReceiverListView*)msg)->m_money,
                 ((CUser*)m_userP)->getCurCharacInvenR()->get_money(),
                 2, (char*)0);
        for (int i = 0;
             i < (int)((CheckMailReceiverListView*)msg)->m_entryCount; ++i)
        {
            // b28d-solo-CHackAnalyzer: NEAR 封顶（P75/N76 跨版本装载序）。ORIG
            // 0x08278c69 循环体 `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%ecx`（索引先、
            // msg 后）；本工具链对 m_entries[i] 恒 base-first。本轮实测新增两形
            // （交换下标 i[arr]、索引 (unsigned int)i 中转）仍 base-first，六形
            // 均不可翻；两侧指令数/助记符序列全等，仅两 load 序差，语义等价。
            if (isAvatar(((CheckMailReceiverListView*)msg)->m_entries[i].m_itemIdx))
                sendAvatar(name, 0);
        }
    }
}

// b28d-solo-CHackAnalyzer: AE（正极性 || 守卫 + name!=0 块序 + 真语义 bug
// 修正（addServerHackCnt 实参 (a,money)、GoldTracking 实参 (trackType,a,ip,
// money,fatigue)）+ 内联 fatigue + 无 cast 除法 + 尾部 return 岛形；旧 N82
// 判词作废）——ORIG 0x08278cce。
#include "CHackAnalyzer/sendGold.inc"
}

#include "CHackAnalyzer/isSameAccountCharacter.inc"
}

#include "CHackAnalyzer/setPatten.inc"
}

#include "CHackAnalyzer/isGoldPatten.inc"
}

#include "CHackAnalyzer/isAvatarPatten.inc"
}

#include "CHackAnalyzer/isDetectionGoldPatten.inc"
}

#include "CHackAnalyzer/sendAvatar.inc"
}

// ============================================================================
// DB_SelectCharacMid / DB_ReqSaveHackUserPunish（本批唯一 owner；报告在手）
// ============================================================================

#include "CHackAnalyzer/makeRequest.inc"
}

// ==== 模板实例（ORIG W 0x080f9b82：get_short 转发形态）====
template <typename T, int N>
bool stGetHackTypeOperator<T, N>::getHackType(PacketBuf* pkt, T& out)
{
    return pkt->get_short(out);
}

template struct stGetHackTypeOperator<unsigned short, 2>;

// [SweepCHackAnalyzer P327] 定义后移至文件末尾：让 CHackAnalyzer::setUserP 成为本
// TU 第一个「强符号·外部链接·本 TU 定义」实体，对齐 ORIG GI 键名
// `_GLOBAL__I__ZN8WongWork13CHackAnalyzer8setUserPEP5CUser`（ORIG 0x080f97f4）；
// 本函数 ORIG T 0x08278a48（0x4f 字节，0x2c6..0x2cb/0x2cd 线性判定）。
#include "CHackAnalyzer/checkAccountHackingHackType.inc"
}
