// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkDeathTower_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// （见下方注释）须置于全部 include 前：首个带入 CItemGenerator.h 的链
//   （CDeathTower.h→CParty.h→…）展开时即需这两个名字。
class CMonster;
namespace WongWork { struct stGenerateRefClearReward_t; }

// ============================================================================
// df_game_r 还原 —— WongWork::CDeathTower 嵌套类（G4 批次，WongWorkDeathTower_A）
//
// 本 TU 承载：
//   · CPacketHandler 全部包构造族（ORIG T 0x0845fc6c..0x084605a1）及
//     C1/D1（ORIG W 0x08469a8c / 0x08469aa0，转发 PacketGuard）；
//   · CDungeonMgr C1/C2、D1/D2（ORIG W 0x082a74b0 / 0x082a74b6：空函数）；
//   · CPlayData D1/D2（ORIG W 0x82a7572：空函数）、generateRewardExp
//     （ORIG T 0x08462538）、generateRewardItem（ORIG T 0x08461fc0）；
//   · 生命周期闭环补件：CPlayData C1（W 0x82a7538）、stRewardItem_t C1
//     （W 0x82a7502，逐 Inven_Item() 构造）—— 显式定义以匹配 ORIG 弱符号。
// 类声明见 CDeathTower.h（唯一完整定义点）；CGameManager.h 仅 #include 该头。
// ============================================================================

#include "CDeathTower.h"

#include "CDungeon.h"
#include "CMap.h"

// CItemGenerator.h 自身顺序缺口的前置（其 _generateMonseterSpecificItem 参数用
// CMonster、_generateRandom/SpecificItem 私有签名在自身前置声明行之前引用
// stGenerateRefClearReward_t；完整定义在 CClearReward.cpp，见该头注释）。
class CMonster;
namespace WongWork { struct stGenerateRefClearReward_t; }
#include "CDataManager.h"          // G_CDataManager / GetChannelScript
#include "GlobalData.h"            // GlobalData::s_event_manager
#include "CGuildServerProxy.h"     // class CEventManager / class CEventBase
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "CServerEvent.h"
#include "CEnvironment.h"          // G_CEnvironment / get_server_group / get_channel_no
#include "CItemGeneratorMgr.h"
#include "CInventory.h"
#include "CDeathTowerRanking.h"
#include "LogManager.h"

namespace {

static const char kFile_DeathTower[] = "DeathTower.cpp";
// [N3 2026-09-11 P139] ORIG 该 TU 发射局部静态
//   `_ZZN8WongWork11CDeathTower9CPlayData17generateRewardExpEiijP6CPartyRNS0_11CDungeonMgrEE19__PRETTY_FUNCTION__`
//   @0x08c64020（size 0x7f，objdump -s 实读内容 =
//   "void WongWork::CDeathTower::CPlayData::generateRewardExp(int, int, unsigned int,
//    CParty*, WongWork::CDeathTower::CDungeonMgr&)"）；旧形手写 static const char[]
//   串内容相同但**不发射该符号**（计入 missing_in_game）⇒ 改用 __PRETTY_FUNCTION__
//   （串内容由声明拼写决定，与本处签名逐字节一致）。

} // namespace

namespace WongWork {

// ---- CDeathTower::stMapMonsterKillChecker_t（ORIG 嵌套版，布局 0x0c）----
#include "WongWorkDeathTower_A/init.inc"
{
    // ORIG 0x0823462a：m_useSkillMaterialTime = m_lastMobDieTime = m_dieCnt = 0
    m_useSkillMaterialTime = 0;
    m_lastMobDieTime = 0;
    m_dieCnt = 0;
}

#include "WongWorkDeathTower_A/setUseSkillMaterial.inc"
{
    // ORIG 0x0823464c：写入 +0x00
    m_useSkillMaterialTime = material;
}

#include "WongWorkDeathTower_A/getUseSkillMaterialTime.inc"
{
    // ORIG 0x08469cda：读取 +0x00（非 const）
    return m_useSkillMaterialTime;
}

#include "WongWorkDeathTower_A/initMonsterInfo.inc"
{
    // ORIG 0x08469c86：仅清零 +0x08
    m_dieCnt = 0;
}

#include "WongWorkDeathTower_A/dieMonster.inc"
{
    // ORIG 0x08469c96：
    //   无符号比较 (tick - m_lastMobDieTime) <= limit → ++m_dieCnt；
    //   否则 m_dieCnt = 0; m_lastMobDieTime = tick；返回 m_dieCnt。
    if ((unsigned int)(tick - m_lastMobDieTime) <= limit)
    {
        ++m_dieCnt;
    }
    else
    {
        m_dieCnt = 0;
        m_lastMobDieTime = tick;
    }
    return m_dieCnt;
}

// ==============================================================================
// 生命周期补件（ORIG W）
// ==============================================================================

// ORIG 0x82a7502：编译器生成 do-while 依次构造 m_items[10]（步长 0x3d，
// 起点 this+4）。本类字段均为非平凡默认构造成员，空函数体即由编译器逐个调用。
#include "WongWorkDeathTower_A/stRewardItem_t.inc"
{
}

// 其余标量 ORIG 不初始化；仅 m_rewards[4] 调 stRewardItem_t C1（0x82a7538）。
// [b28d-Main·检查点14 A/B] throw()（异常规格不入 mangled；声明/定义须一致）。
#include "WongWorkDeathTower_A/CPlayData.inc"
{
}

// ORIG 0x82a7572：push ebp/mov esp,ebp/pop ebp/ret —— 空函数（Inven_Item 平凡
// 析构，编译器不发清理代码，与 ORIG 完全一致）。
// [b28d-Main·检查点14 A/B] throw()（同上）。
#include "WongWorkDeathTower_A/dtor_CPlayData.inc"
{
}

// ==============================================================================
// CDungeonMgr C1/C2 @0x082a74b0、D1/D2 @0x082a74b6（四符号同址：全空函数）
// ==============================================================================
#include "WongWorkDeathTower_A/CDungeonMgr.inc"
#include "WongWorkDeathTower_A/dtor_CDungeonMgr.inc"

// ==============================================================================
// CPacketHandler C1 @0x08469a8c（仅调 PacketGuard C1）/ D1 @0x08469aa0
// ==============================================================================
#include "WongWorkDeathTower_A/CPacketHandler.inc"
#include "WongWorkDeathTower_A/dtor_CPacketHandler.inc"

// ==============================================================================
// CPacketHandler 包构造族（put_* 为基类 InterfacePacketBuf 直调）
// ==============================================================================

// ORIG T 0x0845fcc4：4 队伍槽金钱信息，尾部统一 finalize(true)。
#include "WongWorkDeathTower_A/appendPickupMoney.inc"
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG：|| 短路判定（get_user==0 直接置 1，否则 !checkValidUser 取反）
        if (party->get_user(i) == 0 || !party->checkValidUser(i))
        {
            put_byte(0);
            put_int(0);
        }
        else
        {
            put_byte(1);
            put_int(moneyA[i]);
            put_byte(1);
            put_int(moneyB[i]);
        }
    }
    finalize(true);
}

// ORIG T 0x0845fdc0：4 字节名 + ushort + short + byte + finalize(true)。
#include "WongWorkDeathTower_A/appendPickupItem.inc"
                                                   int v3, int v4)
{
    for (int i = 0; i < 4; ++i)
        put_byte((int)(signed char)name[i]);
    put_short((int)uid);
    put_short(v3);
    put_byte(v4);
    finalize(true);
}

// ORIG T 0x0845fe4c：header(1,cmd)；byte(0)；byte(err)；finalize(true)。
#include "WongWorkDeathTower_A/makeCommonError.inc"
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    finalize(true);
}

// ORIG T 0x0845fea6：header(1,cmd)；byte(0)；byte(err)；byte(slot)；finalize(true)。
#include "WongWorkDeathTower_A/makeDropItemError.inc"
                                                    char slot)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    put_byte((int)(signed char)slot);
    finalize(true);
}

// ORIG T 0x0845ff1a：塔内掉落通知包（字段偏移对应 map_item/Inven_Item 具名成员）。
// [round3-L29 2026-09-10 修复] 尾部 bool 条件物化：真因 = `map_item::m_count` 为 bool，
//   `put_byte(item.m_count == 0)` 被 4.4 折 `xor $1; movzbl`（块序亦反）。改为
//   `put_byte((int)item.m_count != 0 ? 0 : 1);`（int cast 阻断 bool 取反折叠）⇒
//   **AE 111/111 ndiff=0**（此前 DIFF 108/111 ndiff13）。ORIG 0x0845ff1a 证据形：
//   `movzbl (%eax),%eax; test %al,%al; je <mov $1>; mov $0,%eax; jmp; mov $1,%eax`。
#include "WongWorkDeathTower_A/makeDropItemNotify.inc"
                                                     unsigned short dropIdx,
                                                     const map_item& item)
{
    put_header(0, 0x28);
    put_short((int)uid);
    put_short(x);
    put_short(y);
    put_short((int)dropIdx);
    put_int(item.m_item.m_addInfo);                      // map_item+0x12 = Inven_Item+0x02
    put_byte((int)item.m_item.GetItemAttr());            // Inven_Item::GetItemAttr
    put_int(item.m_item.get_add_info());                 // Inven_Item::get_add_info
    put_short((int)item.m_item.m_fieldb);                // map_item+0x1b = Inven_Item+0x0b
    put_int(item.m_item.m_fieldd);                       // map_item+0x1d = Inven_Item+0x0d
    put_byte((int)item.m_item.m_amp.getAbilityType());   // map_item+0x21 = Inven_Item+0x11
    put_short((int)item.m_item.m_amp.getAbilityValue());
    put_packet(item.m_item);                             // 子对象整体（map_item+0x10）
    // ORIG 0x0845ff1a 尾：`test %al,%al; je <mov $1 岛>; mov $0; jmp`（false 块
    // 直落、true 岛在后）⇒ 原码按 bool 直传（三目 1:0 两支被 4.4 折 sete；
    // `!=0?0:1` 亦折 sete）——试 bool 表达式形。
    // [round3-L29] ORIG 0x0845ff1a 为**分支式物化**（`test;je L1;mov $0;jmp;L1:mov $1`），
    //   试显式三目形复现（隐式 int 转换被 4.4 折 `xor $1`）。
    put_byte((int)item.m_count != 0 ? 0 : 1);
    finalize(true);
}

// ORIG T 0x084600a6：header(1,0x32)；byte(1)；byte(order)；short(xy)；int(key)；
// finalize(true)。
#include "WongWorkDeathTower_A/makeDropItem.inc"
{
    put_header(1, 0x32);
    put_byte(1);
    put_byte((int)(signed char)order);
    put_short(xy);
    put_int(itemKey);
    finalize(true);
}

// ORIG T 0x0846012c：header(0,0x1e)；int(0)；finalize(true)。
#include "WongWorkDeathTower_A/makeFinishLoading.inc"
{
    put_header(0, 0x1e);
    put_int(0);
    finalize(true);
}

// ORIG T 0x08460176：header(0,0x20)；short(uid)；byte(0)；finalize(true)。
#include "WongWorkDeathTower_A/makeDieState.inc"
{
    put_header(0, 0x20);
    put_short((int)uid);
    put_byte(0);
    finalize(true);
}

// ORIG T 0x084601da：排行榜展示包。个人最优/榜首记录段委托给
// CDeathTowerRanking 的两个 static 表访问 + 记录序列化函数。
#include "WongWorkDeathTower_A/makeDisplayRangking.inc"
                                                      unsigned int key,
                                                      unsigned int stage,
                                                      unsigned int clearTime,
                                                      bool flag,
                                                      unsigned int dungeonIdx,
                                                      unsigned int playTime)
{
    put_header(0, 0x90);
    put_int((int)clearTime);                       // ebp+0x18 → param_4
    put_int((int)stage);                           // ebp+0x14 → param_3
    put_byte(static_cast<unsigned int>(flag));     // ORIG movzbl -0xc 直零扩（bool slot 信任）
    put_int((int)playTime);                        // ebp+0x24 → param_7
    // ORIG 0x084601da：两记录段各自重调 makeDungeonIdx2TowerIdx +
    // getRankTable（P48 无跨句缓存；this 经 %esi、towerIdx 经 %ebx 跨 call）。
    {
        // P179：ORIG towerIdx 跨 getRankTable call 经 %ebx（push %ebx 由此）。
        // N96 清绑后按 P226 裸语句表达式升格试形。
        // getRankTable 结果 %eax 直传 this 实参槽（命名指针会强制落槽回读）。
        ((CDeathTowerRanking*)CDeathTowerRanking::getRankTable(tableType))
            ->makeMyBestRecordPacket(
                ({ CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx); }),
                key, *this);                                     // ORIG 0x08468b8e
    }
    {
        ((CDeathTowerRanking*)CDeathTowerRanking::getRankTable(tableType))
            ->makeTopRankerRecordPacket(
                ({ CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx); }),
                *this);                                          // ORIG 0x08468c68
    }
    finalize(true);
}

// ORIG T 0x084602c4：header(0,0x91)；int(v1)；后接 4 组 [byte cnt；cnt×{int
// m_addInfo(Inven_Item+0x02); int get_add_info()}]，Inven_Item 步长 0x3d 遍历；
// finalize(true)。
#include "WongWorkDeathTower_A/makeDisplayReward.inc"
                                                    int cnt1, const Inven_Item* items1,
                                                    int cnt2, const Inven_Item* items2,
                                                    int cnt3, const Inven_Item* items3,
                                                    int cnt4, const Inven_Item* items4)
{
    put_header(0, 0x91);
    put_int(v1);

    put_byte(cnt1);
    for (int i = 0; i < cnt1; ++i)
    {
        put_int(items1[i].m_addInfo);
        put_int(items1[i].get_add_info());
    }
    put_byte(cnt2);
    for (int i = 0; i < cnt2; ++i)
    {
        put_int(items2[i].m_addInfo);
        put_int(items2[i].get_add_info());
    }
    put_byte(cnt3);
    for (int i = 0; i < cnt3; ++i)
    {
        put_int(items3[i].m_addInfo);
        put_int(items3[i].get_add_info());
    }
    put_byte(cnt4);
    for (int i = 0; i < cnt4; ++i)
    {
        put_int(items4[i].m_addInfo);
        put_int(items4[i].get_add_info());
    }
    finalize(true);
}

// ORIG T 0x084604b0：header(0,0x92)；byte(flag)；finalize(true)。
#include "WongWorkDeathTower_A/makeDisplayEPLP.inc"
{
    put_header(0, 0x92);
    put_byte(flag ? 1 : 0);
    finalize(true);
}

// ORIG T 0x0846050e：header(0,0xe)；byte(space)；short(1)；
// inv.MakeItemPacket(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot, *this)；finalize(true)。
#include "WongWorkDeathTower_A/makeUpdateItemList.inc"
                                                     int slot)
{
    // GetInvenTypeFromItemSpace：ORIG 全局 _Z26GetInvenTypeFromItemSpace
    // ENUM_ITEMSPACE；权威声明见 CInventory.h（勿在 WongWork ns 内局部 extern，
    // ORIG 求值序：put_header→put_byte→put_short→GetInvenType→getCurCharacInvenR
    put_header(0, 0xe);
    put_byte((int)space);
    put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(
        (INVEN_TYPE)::GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot, *this);
    finalize(true);
}

// ==============================================================================
// CPlayData::generateRewardExp（ORIG T 0x08462538）
//
// 权重表：ORIG `flds 0xc(dm + (stage+tier)*4)` = m_deathTower.m_rewardExpWeight
// （tier 0x1464）/ m_illusionRewardExpWeight（tier 0x15f4）——基址
// 0x5198+0x4/0x644 与 0xc+0x1464*4/0xc+0x15f4*4 精确吻合（float 读取，ORIG
// 报告 WongWork__CDeathTower__CPlayData/generateRewardExp.md）；所得 exp 与最终
// 结算结果 >0x10c8e0(1100000) 时写 BLOOD_LOG 并截断。各加成系数仅进日志参数流。
// ==============================================================================
// [b28d-24 历史] DIFF 残148（P282 候选）: vcall 偏移 52 ORIG 3 次 vs 我方 2 次。
// [2026-09-10 round5 复核] **p167 CALLSITE CLEAN 36/36**：残余仅 C1/C2 变体差
//   （_ZN23stClearRewardBonusExp_t C1==C2 同址 0x0814a800、_ZN27stClearRewardExpParameter_t
//   C1==C2 同址 0x0814a8b8 ⇒ P93 伪差，非缺口）；vcall +0x34（IsEventing）三处已齐。
//   本轮修：删 channelNo/dungeonIdx 具名局部（P0，值经 %ebx/%esi 跨 call 保活直传）、
//   G_CEnvironment 两处调用改复用 env。残余 = 我方 308 vs ORIG 291（帧 0x178 vs 0x130：
//   具名 float 局部槽基差，N82/N95 族；mn_cmp 残差全为「+mov|+mov」槽往返对）⇒ N 封顶。
// [M1S3EmblemLink 2026-09-11] **推翻「复用 env」旧修**（p167 复测 MISSING
//   `_Z14G_CEnvironmentv` -1；Ghidra ORIG 亦两次 `G_CEnvironment()`：一次供
//   `get_channel_no`、一次供 `*(env+0x378)`）⇒ 恢复两处直调，p167 归零 36/36（仅
//   C1/C2 别名对）。另落 2 处 P0：① 删 `limitFlag` 具名槽（ORIG `call
//   limitOfStackableItemInTower; test %al,%al; je`）② 删 `logWeight` 具名槽
//   （ORIG 0x084626xx `flds 0xc(%eax,%edx,4)` 直取后 `fstpl 0x20(%esp)`）。
//   读数 308→305（gap 14，ORIG 291）。试删 `expInOut`（照 Ghidra `&local_20`）
//   ⇒ 305→306 反劣，已回滚（负结论见函数体）。
//   残余：帧 0x160 vs ORIG 0x130（我方对象/float 槽分配差，N82 族；ORIG 对象
//   bonus 在 -0x68/clearParam -0xc8、floats -0x8c..-0xc0，我方补偿性换位）⇒ 封顶。
// ORIG 0x08462538
#include "WongWorkDeathTower_A/generateRewardExp.inc"
                                               unsigned int baseExp, CParty* party,
                                               CDungeonMgr& dungeonMgr)
{
    CUser* user = party->get_user(memberIdx);
    unsigned int reward = 0;

    // [M1S3EmblemLink 2026-09-11 P0] ORIG 无 limitFlag 具名槽（`call
    //   limitOfStackableItemInTower; test %al,%al; je`，0x084625xx）⇒ 内联。
    if (dungeonMgr.getDungeon()->limitOfStackableItemInTower() != '\0')
    {
        // ORIG 两分支重复代码：32 位乘积先物化、G_CDataManager 后取（求值序照 ORIG）
        reward = (unsigned int)(long long)((float)(long long)(stage * baseExp) *
            G_CDataManager()->m_deathTower.m_rewardExpWeight[stage]);
    }
    else
    {
        // ORIG 非有限堆叠档（+0x15f4）
        reward = (unsigned int)(long long)((float)(long long)(stage * baseExp) *
            G_CDataManager()->m_deathTower.m_illusionRewardExpWeight[stage]);
    }

    if (1100000 < reward)
    {
        // [M1S3EmblemLink 2026-09-11 P0] ORIG 无 logWeight 具名槽：0x084626xx 处
        //   `flds 0xc(%eax,%edx,4)` 直接内存取（G_CDataManager 后取），其前仅
        //   `mov -0x1c(%ebp),%eax`（reward）⇒ 权重表达式内联进实参。
        LogManager::logFormat(1, kFile_DeathTower, __PRETTY_FUNCTION__, 0x2ff,
            "BLOOD_LOG : EROOR DEATHTOWER REWARD EXP(%u), LAST STAGE(%d), BASE EXP(%u), EXP WEIGHT(%f)\n",
            reward, stage, baseExp,
            (double)G_CDataManager()->m_deathTower.m_rewardExpWeight[stage]);
    }

    stClearRewardExpParameter_t clearParam;   // 栈构造（ORIG ctor 0x0814a8b8）
    stClearRewardBonusExp_t bonus;            // 栈构造（ORIG ctor 0x0814a800）

    const float avatarBonus = (float)party->getAvatarBonus();
    const float creatureBonus = (float)party->getCreatureBonus(user);

    float pcRoomRate = 0.0f;
    {   // 事件 8：PC 房间燃烧（整除 /100 —— imul 0x51eb851f 形态实证）
        // [N3 2026-09-11 P0] ORIG 0x08462701..0x08462727 无 ev 具名槽：
        //   `mov s_event_manager,%eax; movl $0x8,0x4(%esp); call GetRepeatEvent;
        //    mov (%eax),%edx; add $0x34,%edx; mov (%edx),%ecx; mov -0x18(%ebp),%edx;
        //    mov %edx,0x4(%esp); mov %eax,(%esp); call *%ecx` ⇒ 虚调直内联。
        if (GlobalData::s_event_manager->GetRepeatEvent(8)->IsEventing(user))
            pcRoomRate = (float)(((CPCRoomBurningEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);
    }

    float expDoubleRate = 0.0f;
    {   // 事件 3：双倍经验（浮点 /100.0f，常量 _DAT_08c640b8 = 100.0f）
        // [N3 2026-09-11 P0] 同上：ORIG 0x08462775..0x0846279c 无 ev 具名槽（直内联）。
        if (GlobalData::s_event_manager->GetRepeatEvent(3)->IsEventing(0))
            expDoubleRate = (float)((CExpDoubleEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / 100.0f;
    }

    int burningBonusExp = 0;
    {   // 事件 0xf：疲劳燃烧加成（千分率原始值）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if (ev->IsEventing(0))
            burningBonusExp =
                ((CBurningFatigueEvent*)ev)->getBonusExpRate(user->getCurCharacUsedFatigue());
    }

    // 频道脚本 bonus rate（+0x378 = Server_Envir 组别，参照 CBossPlay 先例走
    // get_server_group() 访问器替代裸偏移）
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    // [M1S3EmblemLink 2026-09-11 p167 真缺口修正] ORIG `_Z14G_CEnvironmentv`
    //   调用 **2 次**（p167 差集 MISSING -1；Ghidra 亦作两次 `G_CEnvironment()`
    //   分别供 `get_channel_no` 与 `*(env+0x378)`）⇒ 旧「复用 env 单调用」形
    //   为误修，恢复两处直调；ORIG 无具名 CEnvironment* 槽。
    const float channelRate =
        channelScript->getBonusRate(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            (unsigned int)G_CEnvironment()->get_channel_no(),
            (unsigned int)dungeonMgr.getDungeon()->get_index());

    float premiumAdvantageRate = 0.0f;
    if (user->IsHavePremiumAdvantage())
        premiumAdvantageRate =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f;

    const float serverEventExpRate = (float)CServerEvent::GetExpRate() / 100.0f;

    const float manageBonus = party->CalcurateManageBonus((ManageBonusType::T)0, user, reward);

    // 结算基础通关经验（exp 以引用进出）
    // [M1S3EmblemLink 2026-09-11 负结论] 试删 expInOut（改传 reward 自身，照
    //   Ghidra `&local_20`）⇒ 305→306 反劣（4.4 对「返回值与 in/out 同变量」
    //   多发一条槽往返）⇒ 回滚，保留具名 expInOut。
    unsigned int expInOut = reward;
    reward = party->getClearRewardBonusExp(user, expInOut, clearParam, bonus);

    if (1100000 < reward)
    {
        LogManager::logFormat(1, kFile_DeathTower, __PRETTY_FUNCTION__, 0x324,
            "BLOOD_LOG : EROOR DEATHTOWER CLEAR REWARD EXP(%u)\n", reward);
        LogManager::logFormat(1, kFile_DeathTower, __PRETTY_FUNCTION__, 0x32e,
            "BLOOD_LOG : DEATHTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            (double)avatarBonus, (double)creatureBonus, (double)pcRoomRate,
            (double)expDoubleRate, (double)channelRate,
            (double)premiumAdvantageRate, (double)serverEventExpRate,
            (int)burningBonusExp);
        reward = 1100000;
    }

    m_rewardExp[memberIdx] = (int)reward;   // ORIG *(uint*)(this+(idx+0x26c)*4+8)
}

// ==============================================================================
// CPlayData::generateRewardItem（ORIG T 0x08461fc0）
//
// 期望掉落条数写入 m_rewards[memberIdx].m_itemCnt；循环 count 次投掷
// get_rand_int(1000)：rnd>rate 高阶掉落分支的概率锚点为 DataManager+0x5e20 /
// +0x5e24（乘 param5 比较）= m_deathTower.m_rewardItemProb[0]/[1]（0x5198+0xc88
// 起 int 数组，ORIG `mov 0x5e20(%eax),%eax` int 宽实证）；计数率因子
// +0x5e2c = m_deathTower.m_rewardGoldWeight（ORIG `flds` float）。**受控分支**
// （roll>=prob0*param5）经生成器虚槽 +0x08 = generateRandomItem、**随机分支**经
// 虚槽 +0x0c = generateSpecificItem（P282 实证，见函数头 2026-09-10 判词；旧注释
// 把两者写反）。受控分支额外触发幸运点
// 扣减（::CLuckPoint::UseLuckPoint）。ref 对象由其 ctor 初始化，上下文区按
// 具名字段写入（宽度逐条对齐 ORIG：movb/movw/movl，见
// WongWork__CDeathTower__CPlayData/generateRewardItem.md 0846205d..099 /
// 084622bc..328）。
// ==============================================================================
// [b28d-24 历史] DIFF 残380: P167 迭代器写法差（MISSING begin/deEv×5；EXTRA front/C1/D1/G_CDataManager 等）。
// [2026-09-10 round5 定稿] 真 bug 修 + P167 修（P282 流程）：
//   ① **两分支虚调目标互换（真 bug）**：P282 五步核 _ZTVN8WongWork14CItemGeneratorE
//      @0x08c9a788 —— word[2]=0x0853a3f0 setDropRatio / word[3]=0x0853a3f6 loadScript /
//      word[4]=0x0853a400 generateRandomItem / word[5]=0x0853a406 generateSpecificItem。
//      ORIG 08462311 段 `mov (%eax),%edx; add $0x8,%edx; call *%ecx` ⇒ **受控分支
//      （ref 0x3f800000、roll>=prob0*param5）调 generateRandomItem**；ORIG 08462131 段
//      `add $0xc,%edx` ⇒ 随机分支（ref 0x10001 组）调 generateSpecificItem —— 旧重建
//      恰好相反（两分支掉落语义颠倒）。
//   ② 条件符号性：ORIG `cmp -0x20(%ebp),%eax; seta %al; test %al,%al; je` 为 **unsigned**
//      （prob0*param5 <= roll ⇒ 受控）；旧重建写 `(int)boundHigh` signed 比较 + 二段
//      `||` + boundHigh 具名槽。prob1 内层 ORIG 只求值一次 G_CDataManager（旧重建
//      `<= && - != 0` 两次 ⇒ EXTRA +1）。
//   ③ P167：`.front()`→`*v.begin()`（ORIG begin×4/deEv×5）；`probe` 由 setCopy 改
//      **拷贝构造**（ORIG 084623b5..0x43f = 15×dword+1byte 块拷贝到 -0xf5）；随机分支
//      具名迭代器复用（ORIG 只 begin 一次却 deEv 两次）；随机分支 ORIG 无 `empty()`
//      守卫、无 `RewardData_Result::clear`。
//   ⇒ p167 差集 16→12 项（begin/deEv/front/empty/setCopy/G_CDataManager 全清）、
//      missing_in_game 25→24。
//   ④ [配方 C05② 正向（A27 机理）+ P0] 删 6 处 ORIG 无的具名局部（dropEmpty 直判
//      `!empty()`、itemList/grade/rarity 内联、genMgr→gen 链内联）⇒ our **388→357**
//      （ORIG 347，gap −41→**−10**）。
//   残余 = C1/C2·D1/D2 变体（P93 同址伪差：0814ab06 C2==C1、0814ab4c D2==D1）
//      + setcc 物化（ORIG seta×2/test×2，4.4 恒直跳，P197④ 族，等价 −4 条）+ ref 末
//      两 short 经 lea+add（stGenerateRefData_t 字段建模 +4）+ 杂项 ⇒ N 封顶。
//   ⑤ [C05 边界留痕] 本件 grade/rarity 在 ORIG 为**寄存器保活**（0846247c `mov %eax,%ebx`
//      / 08462489 `mov %eax,%edi`，经 `084624a7 mov %ebx,0xc(%esp)` 直传 varargs）⇒ 与 C07
//      （ORIG 用槽则写具名 int 局部）方向**相反**：此处必须内联（已按此改）。C05③ 闸门
//      （岛差 ≥2）与 C05① 多层尾块均不适用（各早退为共享 epilogue，ORIG 同形）。
// [M1S3EmblemLink 2026-09-11 试后回滚] 删独立 `::CLuckPoint luck;`（改传
//   `&result.m_luckPoint`，照 ORIG 0x08462019 `lea -0x88(%ebp),%eax; add $0xc,%eax`）
//   ⇒ 本函数 357→343（ORIG 347）、p167 47/47 归零，**但 TU 四数劣化**
//   （identical 16→7、ae 47→29、missing 24→12：该 odr-use 是 ~CLuckPoint 与
//   vector<CUser*>/map<int,…> 共 27 个 COMDAT 符号的唯一发射源）⇒ 按闸门回滚，
//   负结论见函数体 `::CLuckPoint luck;` 处注释。
//   ORIG 0x08461fc0
#include "WongWorkDeathTower_A/generateRewardItem.inc"
                                                const CDungeonMgr& dungeonMgr,
                                                unsigned int param4, unsigned int param5,
                                                CParty* party)
{
    m_rewards[memberIdx].m_itemCnt = count;

    // ORIG：rate 值在各自使用点内联 G_CDataManager() 读取（循环内每次）
    for (int i = 0; i < count; ++i)
    {
        stGenerateRefData_t ref;
        stGenerateResult_t result;

        // [M1S3EmblemLink 2026-09-11 负结论 · p167 与 TU 四数冲突实测]
        //   ORIG 0x08462019 `lea -0x88(%ebp),%eax; add $0xc,%eax; mov %eax,
        //   0x4(%esp); … call CParty::GetLuckPoint(CLuckPoint*, bool)` 传的是
        //   result 成员 m_luckPoint(+0x0c)，且 ORIG 无独立 CLuckPoint 局部；
        //   改 `&result.m_luckPoint` 后本函数 357→343、p167 47/47 归零（真对齐），
        //   **但 TU 四数劣化**：identical 16→7 / ae 47→29 / missing 24→12 ——
        //   因 `::CLuckPoint luck;` 是本 TU 内联销毁 CLuckPoint 的唯一 odr-use，
        //   它消失后 ~CLuckPoint 及 std::vector<CUser*>、std::map<int,…> 的
        //   27 个 COMDAT 符号不再发射（而 ORIG 本 TU 含这些符号）。
        //   按「四数不得劣化」闸门回滚；保留 `::CLuckPoint luck;` 直证形。
        ::CLuckPoint luck;   // sizeof 0x24；ORIG 隐式 ~CLuckPoint COMDAT 依赖此 odr-use
        if (party != 0)
            party->GetLuckPoint(&luck, false);

        const int roll = get_rand_int(1000);
        // ORIG 0846204b..0x097：`imul 0x1c(%ebp),%eax; cmp -0x20(%ebp),%eax;
        //   seta %al; test %al,%al; je <受控体>` —— 比较为 **unsigned**
        //   （prob0*param5 <= roll ⇒ 受控），且 boundHigh **不落槽**（P0：原码把
        //   表达式内联进条件，只调一次 G_CDataManager）。旧重建用 signed
        //   `(int)boundHigh` + 具名槽 + 二段比较（jl/jne），符号性与槽皆不符。
        // [A/B 轴复查 2026-09-10，**已试后回退**] ORIG 0846204f `cmp -0x20(%ebp),%eax; seta %al`
        //   为 mem 直比；改把 roll 写在比较右侧（`expr <= roll`）实测**无效** —— 4.4 仍先
        //   载 roll 入寄存器（形态 setae/setb 不变、our 357→359 劣化 +2）⇒ 按 N18b/P197④ 封顶。
        if ((unsigned int)roll >= (unsigned int)(
                G_CDataManager()->m_deathTower.m_rewardItemProb[0] * (int)param5))
        {
            // ---- 受控分支（ORIG +0x2df：vtable+0x08 random）----
            // ORIG 084622a4..0x2b6：prob1 只求值一次（`imul; cmp; seta; je`），
            //   条件 = prob1*param5 > roll（unsigned）⇒ 写单比较（旧重建写
            //   `<= && - != 0` 两次求值，多一次 G_CDataManager）。
            // 同上（A 轴/N18b 封顶）：ORIG 084622ae `cmp -0x20(%ebp),%eax; seta %al`；
            //   交换操作数同样不产生 mem 直比，已回退。
            if ((unsigned int)roll < (unsigned int)(
                    G_CDataManager()->m_deathTower.m_rewardItemProb[1] * (int)param5))
            {
                // ref 上下文区（ORIG 084622bc..328：movb/movb/movl/movl/movl）
                ref.m_field35 = 0;
                ref.m_field34 = (char)param4;
                ref.m_rankBonusIndex = 0x3f800000;             // float 1.0f 位型（ORIG movl）
                ref.m_dungeon = dungeonMgr.getDungeon();       // ORIG movl（指针）
                *reinterpret_cast<int*>(&ref.m_goldBase) = 0;  // ORIG movl $0 覆盖 +0x3c/+0x3e

                // [P282 真 bug 修] ORIG 08462311 `mov (%eax),%edx; add $0x8,%edx;
                //   mov (%edx),%ecx; call *%ecx` ⇒ 本分支（roll>=boundHigh、ref
                //   0x3f800000）走 **vtable+0x08**；ORIG _ZTV @0x08c9a788
                //   word[4]=0x0853a400=generateRandomItem ⇒ 原码本分支调
                //   generateRandomItem（旧重建调 generateSpecificItem=+0x0c，相反）。
                // [P0] ORIG 无 genMgr/gen 具名局部：084622f2..0846230d 为直链
                //   `call G_CDataManager; call getItemGenerator; …; mov (%eax),%edx`。
                G_CDataManager()->getItemGenerator()
                    ->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                    ->generateRandomItem(ref, result);

                // [P0/配方 C05② 正向（A27 机理）] ORIG 无 dropEmpty 具名局部：
                //   08462338 `call empty; xor $0x1,%eax; test %al,%al; je` 直判。
                if (!result.m_dropItems.empty())
                {
                    Inven_Item& dst = m_rewards[memberIdx].m_items[i];
                    // [P167] ORIG `call begin(); call operator*`（迭代器槽
                    //   local_30/local_2c）⇒ 源写 `*v.begin()`，非 v.front()。
                    dst.setCopy(*result.m_dropItems.begin());

                    // ORIG 084623b5..0x485：operator* 结果 15×dword+1byte 块拷贝
                    //   到 -0xf5（Inven_Item 0x3d 字节平凡拷贝）⇒ 源是拷贝构造。
                    Inven_Item probe(*result.m_dropItems.begin());
                    // [P0] ORIG 无 itemList 具名局部（08462452 `mov 0xc(%eax),%eax`
                    //   直入 find_item，dm+0xc 直读）。
                    CItem* found =
                        G_CDataManager()->m_itemList->find_item(probe.m_addInfo);
                    if (found != 0)
                    {
                        // [P0] ORIG 无 grade/rarity 具名局部：08462477/0x484 两 call
                        //   结果经 %ebx/%edi 跨调用保活，按 R2L 序直传 UseLuckPoint 的
                        //   第 4/3 实参（084624a7 `mov %ebx,0xc(%esp)`）。
                        ::CLuckPoint::UseLuckPoint(party->get_user(memberIdx),
                                                 (unsigned char)probe.m_field1,
                                                 found->get_rarity(),
                                                 found->get_grade());
                    }
                }
            }
        }
        else
        {
            // ---- 随机掉落分支（ORIG +0x0f2：vtable+0x08 random）----
            // ref 上下文区（ORIG 0846205d..099：movb/movb/movb/movl/movw×4）
            ref.m_field35 = 0;
            ref.m_field34 = (char)param4;
            ref.m_rateSlot = 1;
            ref.m_rankBonusIndex = 1;                          // ORIG movl
            ref.m_goldBase = 1;                                // ORIG movw +0x3c
            ref.m_goldFactor = 1;                              // ORIG movw +0x3e
            *reinterpret_cast<unsigned short*>(&ref.m_dungeon) = 1;       // ORIG movw 低 16 位
            *(reinterpret_cast<unsigned short*>(&ref.m_dungeon) + 1) = 1; // ORIG movw 高 16 位

            stGenerateResult_t randomResult;                   // ORIG -0xb8 独立结果区
            // [P282 真 bug 修] ORIG 08462131 `mov (%eax),%edx; add $0xc,%edx;
            //   call *%ecx` ⇒ 本分支（roll<boundHigh、ref 0x10001 组）走
            //   **vtable+0x0c** = generateSpecificItem（_ZTV word[5]=0x0853a406）。
            // [P0] ORIG 无 genMgr/gen 具名局部（084620f0..0x12b 直链）。
            G_CDataManager()->getItemGenerator()
                ->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateSpecificItem(ref, randomResult);

            // [P171] ORIG 本分支无 empty 守卫（08462df..0x4fa 直接 begin+解引用）；
            //   empty 检查只在受控分支（ORIG 08462338）出现。
            if (party != 0)
            {
                // QuickParty 奖励登记（基础奖励类型 0xb：道具奖励槽）
                // [P167] ORIG 只调一次 begin（迭代器槽 local_38）却解引用两次
                //   ⇒ 源有具名迭代器复用；`(*it).m_...` 形（deEv，非 ptEv）。
                // [P171] ORIG 无 RewardData_Result::clear 调用。
                std::vector<Inven_Item>::iterator it =
                    randomResult.m_dropItems.begin();
                QuickParty::RewardData_Param rp(party);
                rp.m_param4 = dungeonMgr.getDungeon()->get_index();
                QuickParty::RewardData_Result rs;
                rs.m_value00 = (*it).m_addInfo2;
                party->set_basic_reward((QuickParty::BasicRewardType)0xb, rp, rs);
                (*it).m_addInfo2 = rs.m_value00;
            }
            Inven_Item& dst = m_rewards[memberIdx].m_items[i];
            dst.setCopy(*randomResult.m_dropItems.begin());
            const int addInfo = dst.get_add_info();
            // ORIG：fildl addInfo/flds m_rewardGoldWeight/fmuls/fistpl（默认控制字就近舍入）
            dst.set_add_info((int)((float)addInfo *
                                   G_CDataManager()->m_deathTower.m_rewardGoldWeight));
        }
    }
}

} // namespace WongWork
