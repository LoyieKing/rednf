// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHackAnalyzer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CHackAnalyzer 实现（G6 missing 批次，权威头
// CHackAnalyzer.h；Secu_AccountHacking 全部方法 + CHackAnalyzer 缺失方法）。
// 依据 docs/class_func_reports/WongWork__CHackAnalyzer/*.md 与
// Secu_AccountHacking/*.md 逐函数还原（##汇编 + ##反编译C 两段均已读）；
// 报告缺失的 4 个小符号（getServerHackCnt / resetServerHackCnt /
// setServerHackAccumulatedCnt / checkCollectedHack）以 ORIG nm + objdump
// 区间取证后转写并明确标注「报告缺失」。附带落 DB_SelectCharacMid /
// DB_ReqSaveHackUserPunish::makeRequest（各自报告在手，此前无 owner）。
// [GI 判据 · Main 裁决 2026-09-11，含更正] `_Z41__static_initialization_and_destruction_0ii`
// 的逐 TU DIFF 一律为工具假缺口：ORIG 该符号有 493 个实例，工具按名错配取首个
// （0x0807ee76，属 PacketTypes TU）⇒ 读数不可比，跳过、勿改。本 TU 静态数据
// （_S_HACK_CHECK_RATE_BY_*/_S_HACK_RATIO_COEF_100F）均为常量初始化，
// 按 GI 键 `_GLOBAL__I_...` 体间接目标复读本 TU 自身实例 = 16 条纯 ioinit ⇒ 无例外项。
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

// ---- 静态概率表（ORIG .rodata 0x8b3e094 / 0x8b3e09c，raw 逐项复核）----
static const short _S_HACK_CHECK_RATE_BY_PARTYCOUNT[4] = {100, 50, 33, 25};
// [R12fixG2 修正 · ④#4] ORIG @0x08b3e09c raw = `14 00 0f 00 0a 00 00 00 00 00` ⇒ {20,15,10,0,0}；
// 旧值 {25,20,15,10,0} 系整体挪一位并多出首项 25（与 raw 不符）。消费点
// CheckHackRateByDifficult.inc:19，difficult 界检查 [0,4]（ORIG 0x080f84f4/f8/fe 的 js/jle）与表长 5 一致。
static const short _S_HACK_CHECK_RATE_BY_DIFFICULT[5] = {20, 15, 10, 0, 0};

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

#include "CHackAnalyzer/setUserP.inc"


#include "CHackAnalyzer/getUserP.inc"


#include "CHackAnalyzer/beginCollectHackInfo.inc"


#include "CHackAnalyzer/CheckExceptionCnt.inc"


#include "CHackAnalyzer/CheckHackRateByPartyCount.inc"


#include "CHackAnalyzer/CheckHackRateByDifficult.inc"


#include "CHackAnalyzer/AddCheckRateHackTypeList.inc"


#include "CHackAnalyzer/ReqDBSaveHackInfo.inc"


#include "CHackAnalyzer/reportHackInfo.inc"


#include "CHackAnalyzer/resetHackInfo.inc"


#include "CHackAnalyzer/_addHackInfo.inc"


#include "CHackAnalyzer/analyzeHackReport.inc"
{
    // ORIG 0x080f7a22（analyzeHackReport.md）：头 2 字节标志位驱动各检测分支；
    // 失败读包路径一律 return 0，成功路径 return 1；flags&0x100 时对 param_1
    // 直连其分析器加 0x32c 计数。
    // ORIG 局部 IO 字段统一位于 -0x4c 基址的 packed 缓冲：+0xd flags(u16)、
    // +0xf speedMs(u32)、+0x13 byteVal(u8)、+0x40 memVal(u32)；地址链
    // lea -0x4c;add off 形（帧 0x78）。
    struct __attribute__((packed))
    {
        unsigned char pad0[0xd];
        unsigned short flags;      // +0xd
        unsigned int speedMs;      // +0xf
        char byteVal;              // +0x13
        unsigned char pad1[0x40 - 0x14];
        unsigned int memVal;       // +0x40
    } buf;
    if (!stGetHackTypeOperator<unsigned short, 2>::getHackType(
            pkt, *(unsigned short*)(char*)&buf.pad0[0xd]))
        return 0;

    if (m_baseHackType == 0)
        return 1;

    if ((buf.flags & 0x1) != 0)
    {
        if (!pkt->get_int(*(unsigned int*)(char*)&buf.pad0[0xf]))
            return 0;
        _analyzeSpeedHack(buf.speedMs);
    }
    if ((buf.flags & 0x2) != 0)
    {
        _analyzeDifficultyHack('\0');
        ((CUser*)m_baseHackType)->DisConnSig((DISCONN_SIG)0x18, false, false);
    }
    if ((buf.flags & 0x8) != 0)
    {
        if (!pkt->get_int(*(unsigned int*)(char*)&buf.pad1[0x2c]))
            return 0;
        _analyzeMemoryHack(buf.memVal);
    }
    if ((buf.flags & 0x4) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE,
                     (ENUM_HACKTYPE)0x3c5, 0);
        ReqDBSaveHackUserPunish(((CUser*)m_baseHackType)->get_acc_id(), 0xa8);
        ((CUser*)m_baseHackType)->DisConnSig((DISCONN_SIG)0x18, false, false);
    }
    if ((buf.flags & 0x80) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MONSTER_DIE, (ENUM_HACKTYPE)0x3bd, 0);
    }
    if ((buf.flags & 0x10) != 0)
    {
        if (!pkt->get_byte(*(char*)&buf.byteVal))
            return 0;
        _addHackInfo(REPORT_4_HACK_FLAG_PAYLOAD_BYTE, (ENUM_HACKTYPE)0x3be,
                     (int)*(char*)&buf.byteVal);
    }
    if ((buf.flags & 0x20) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MISC, (ENUM_HACKTYPE)0x3c6, 0);
    }
    if ((buf.flags & 0x100) != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x32c,
                                                  1, 0, 0);
    }
    return 1;
}

#include "CHackAnalyzer/_analyzeSpeedHack.inc"


// 判词 N（N82 族：4.1 死局部槽位策略）：ORIG numMs@-0x20/CW@-0x22/CW2@-0x24 vs
//   本工具链 numMs@-0x18/CW@-0x1a/CW2@-0x1c —— ORIG numMs 与 elapsed(-0x10) 间
//   存在 8 字节死槽(-0x18..-0x11)。本轮实测：①未使用 `long long`/`char[8]` 局部
//   恒被置于最深槽（pad@-0x28）并抬帧 0x38→0x48（反劣）；②16 字节 POD 局部
//   （packed/非 packed）同样落 -0x30 或多出结构拷贝。4.4 无「按声明序保留死槽」
//   能力 ⇒ 帧 0x38 两侧等齐、助记符序列全等、仅 3 处槽号差，N 封顶不追。

#include "CHackAnalyzer/_analyzeDifficultyHack.inc"


#include "CHackAnalyzer/_analyzeMemoryHack.inc"


#include "CHackAnalyzer/_analyzeEquipmentUpgradeHack.inc"
{
    // ORIG 0x080f7e18（_analyzeEquipmentUpgradeHack.md）：当前角色背包装备栏
    // 前 0x16 格 Inven_Item::GetUpgrade 低字节须逐一等于客户端上报表值。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE))
        return;

    // [hc 收] ORIG 0x080f7e18 寻址链五条 = imul $0x3d;add $0x10;add inv(-0x10 槽);
    // add $0xc;mov %eax,(%esp)（两常量分列、i 侧先算）。旧裸偏移链 4.4 恒折
    // inv-first + 0x1c 单常量（判 N1+N18a 已作废）。视觉具名化 = 槽序列视图：
    // CInventory 起始 +0x10 为 0x3d 步长槽数组、槽内 +0xc 为 Inven_Item 区；
    // 探针 /tmp/hc_probe.cpp fF/fG 与 ORIG 逐条同形（元素内 char[0x31] 数组退化）。
    // 语义承载 = 槽序列（未建模子对象占位，§5.3），GetUpgrade 具名访问槽内项。
    struct InvenSlotView
    {
        char m_head[0xc];        // +0x00..+0x0b（本函数不读）
        char m_item[0x31];       // +0x0c：Inven_Item 区（0x3d - 0xc）
    };
    struct InvenSlotsView
    {
        char m_head[0x10];       // +0x00..+0x0f
        InvenSlotView m_slots[0x16];
    };
    static_assert(sizeof(InvenSlotView) == 0x3d, "InvenSlotView stride drift");
    static_assert(offsetof(InvenSlotView, m_item) == 0xc, "slot item offset drift");
    static_assert(offsetof(InvenSlotsView, m_slots) == 0x10, "slots base drift");

    const InvenSlotsView* inv = reinterpret_cast<const InvenSlotsView*>(
        ((CUser*)m_baseHackType)->getCurCharacInvenR());
    for (int i = 0; i < 0x16; ++i)
    {
        if (((const Inven_Item*)inv->m_slots[i].m_item)->GetUpgrade() != table[i])
        {
            _addHackInfo(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE,
                         (ENUM_HACKTYPE)0x3c3, 0);
            return;
        }
    }
}

#include "CHackAnalyzer/_analyzeMonsterDieHack.inc"


#include "CHackAnalyzer/analyzeMonsterDieHack.inc"


#include "CHackAnalyzer/_reportIncurrectReport.inc"


#include "CHackAnalyzer/ValidateFixMobDiePos.inc"
                                         unsigned short x, unsigned short y)
{
    // ORIG T 0x080f908c（ValidateFixMobDiePos.md）：查 FixMobInfo 白名单表；
    // mobId 命中而坐标出框（或 0xd6ed 特例坐标不等于框边界 xMin/xMax）时
    // addServerHackCnt(user, 0xd2, 1, 0, 0)。
    // 静态表（ORIG nm d @0x0936f9c0 size 0x84 / FixMobInfoSize @0x0936fa44
    // =11；内容 objdump -s .data 实测，第 11 项全零不参与比较循环）。
    struct FixMobInfoRec
    {
        unsigned int m_mid;       // +0x00
        unsigned short m_xMin;    // +0x04
        unsigned short m_xMax;    // +0x06
        unsigned short m_yMin;    // +0x08
        unsigned short m_yMax;    // +0x0a
    };
    static FixMobInfoRec FixMobInfo[] = {
        {0xbba,  0x468, 0x468,  0xed,  0xed },
        {0xd6ed, 0x80,  0x337,  0x9d,  0x139},
        {0x2f0,  0xb,   0xb,    0x82,  0x15e},
        {0x2ef,  0x378, 0x378,  0x82,  0x15e},
        {0xdb29, 0x4f9, 0x4f9,  0xff,  0xff },
        {0xf010, 0xffee, 0xffee, 0x120, 0x120},
        {0xf108, 0x57,  0x57,   0x175, 0x175},
        {0xef9c, 0x384, 0x384,  0xf0,  0xf0 },
        {0x3f2,  0x377, 0x377,  0xca,  0xca },
        {0xdcbb, 0x552, 0x552,  0xfb,  0xfb },
        {0,      0,     0,      0,     0    },
    };
    static unsigned short FixMobInfoSize = 11;   // ORIG 0x936fa44 movzwl 零扩展（unsigned）

    for (int i = 0; i < FixMobInfoSize - 1; ++i)
    {
        if (mobId == FixMobInfo[i].m_mid)
        {
            if (x < FixMobInfo[i].m_xMin ||
                FixMobInfo[i].m_xMax < x ||
                y < FixMobInfo[i].m_yMin ||
                FixMobInfo[i].m_yMax < y)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (ENUM_HACKTYPE)0xd2, 1, 0, 0);
                return;
            }
            if (mobId != 0xd6ed)
                return;
            if (FixMobInfo[i].m_xMin == x)
                return;
            if (FixMobInfo[i].m_xMax == x)
                return;
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0xd2, 1, 0, 0);
            return;
        }
    }
}

#include "CHackAnalyzer/onUpdateHackCount.inc"
                                      unsigned int b, unsigned int c)
{
    user->onUpdateHackCount(type, a, b, c);
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

#include "CHackAnalyzer/addServerHackCnt.inc"


#include "CHackAnalyzer/ReqDBSaveHackUserPunish.inc"


}  // namespace WongWork

// ============================================================================
// Secu_AccountHacking（全局作用域；声明见 CHackAnalyzer.h）
// ============================================================================

#include "CHackAnalyzer/init.inc"


#include "CHackAnalyzer/resetInfo.inc"


#include "CHackAnalyzer/setLoginTime.inc"


#include "CHackAnalyzer/getMatchingHacktype.inc"


#include "CHackAnalyzer/isAvatar.inc"


#include "CHackAnalyzer/sendMail.inc"


#include "CHackAnalyzer/sendGold.inc"


#include "CHackAnalyzer/isSameAccountCharacter.inc"


#include "CHackAnalyzer/setPatten.inc"


#include "CHackAnalyzer/isGoldPatten.inc"


#include "CHackAnalyzer/isAvatarPatten.inc"


#include "CHackAnalyzer/isDetectionGoldPatten.inc"


#include "CHackAnalyzer/sendAvatar.inc"


// ============================================================================
// DB_SelectCharacMid / DB_ReqSaveHackUserPunish（本批唯一 owner；报告在手）
// ============================================================================

#include "CHackAnalyzer/makeRequest.inc"


// ==== 模板实例（ORIG W 0x080f9b82：get_short 转发形态）====
template <typename T, int N>
bool stGetHackTypeOperator<T, N>::getHackType(PacketBuf* pkt, T& out)
{
    return pkt->get_short(out);
}

template struct stGetHackTypeOperator<unsigned short, 2>;

// [SplitC 2026-09-11 batch2 · TU 迁移] checkAccountHackingHackType 已迁出至 Secu_AccountHacking.cpp（ORIG GI 键对齐）。


