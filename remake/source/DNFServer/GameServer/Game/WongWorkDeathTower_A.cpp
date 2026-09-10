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
static const char kFunc_generateRewardExp[] =
    "void WongWork::CDeathTower::CPlayData::generateRewardExp(int, int, unsigned int, "
    "CParty*, WongWork::CDeathTower::CDungeonMgr&)";

} // namespace

namespace WongWork {

// ---- CDeathTower::stMapMonsterKillChecker_t（ORIG 嵌套版，布局 0x0c）----
void CDeathTower::stMapMonsterKillChecker_t::init()
{
    // ORIG 0x0823462a：m_useSkillMaterialTime = m_lastMobDieTime = m_dieCnt = 0
    m_useSkillMaterialTime = 0;
    m_lastMobDieTime = 0;
    m_dieCnt = 0;
}

void CDeathTower::stMapMonsterKillChecker_t::setUseSkillMaterial(unsigned int material)
{
    // ORIG 0x0823464c：写入 +0x00
    m_useSkillMaterialTime = material;
}

unsigned int CDeathTower::stMapMonsterKillChecker_t::getUseSkillMaterialTime()
{
    // ORIG 0x08469cda：读取 +0x00（非 const）
    return m_useSkillMaterialTime;
}

void CDeathTower::stMapMonsterKillChecker_t::initMonsterInfo()
{
    // ORIG 0x08469c86：仅清零 +0x08
    m_dieCnt = 0;
}

unsigned int CDeathTower::stMapMonsterKillChecker_t::dieMonster(unsigned int tick, unsigned int limit)
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
CDeathTower::CPlayData::stRewardItem_t::stRewardItem_t()
{
}

// 其余标量 ORIG 不初始化；仅 m_rewards[4] 调 stRewardItem_t C1（0x82a7538）。
// [b28d-Main·检查点14 A/B] throw()（异常规格不入 mangled；声明/定义须一致）。
CDeathTower::CPlayData::CPlayData() throw()
{
}

// ORIG 0x82a7572：push ebp/mov esp,ebp/pop ebp/ret —— 空函数（Inven_Item 平凡
// 析构，编译器不发清理代码，与 ORIG 完全一致）。
// [b28d-Main·检查点14 A/B] throw()（同上）。
CDeathTower::CPlayData::~CPlayData() throw()
{
}

// ==============================================================================
// CDungeonMgr C1/C2 @0x082a74b0、D1/D2 @0x082a74b6（四符号同址：全空函数）
// ==============================================================================
CDeathTower::CDungeonMgr::CDungeonMgr() {}
CDeathTower::CDungeonMgr::~CDungeonMgr() {}

// ==============================================================================
// CPacketHandler C1 @0x08469a8c（仅调 PacketGuard C1）/ D1 @0x08469aa0
// ==============================================================================
CDeathTower::CPacketHandler::CPacketHandler() {}
CDeathTower::CPacketHandler::~CPacketHandler() {}

// ==============================================================================
// CPacketHandler 包构造族（put_* 为基类 InterfacePacketBuf 直调）
// ==============================================================================

// ORIG T 0x0845fcc4：4 队伍槽金钱信息，尾部统一 finalize(true)。
void CDeathTower::CPacketHandler::appendPickupMoney(int* moneyA, int* moneyB, CParty* party)
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
void CDeathTower::CPacketHandler::appendPickupItem(char* name, unsigned short uid,
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
void CDeathTower::CPacketHandler::makeCommonError(ENUM_CMDPACKET cmd, ENUM_ERROR err)
{
    put_header(1, (int)cmd);
    put_byte(0);
    put_byte((int)err);
    finalize(true);
}

// ORIG T 0x0845fea6：header(1,cmd)；byte(0)；byte(err)；byte(slot)；finalize(true)。
void CDeathTower::CPacketHandler::makeDropItemError(ENUM_CMDPACKET cmd, ENUM_ERROR err,
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
void CDeathTower::CPacketHandler::makeDropItemNotify(unsigned short uid, int x, int y,
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
void CDeathTower::CPacketHandler::makeDropItem(char order, int xy, int itemKey)
{
    put_header(1, 0x32);
    put_byte(1);
    put_byte((int)(signed char)order);
    put_short(xy);
    put_int(itemKey);
    finalize(true);
}

// ORIG T 0x0846012c：header(0,0x1e)；int(0)；finalize(true)。
void CDeathTower::CPacketHandler::makeFinishLoading()
{
    put_header(0, 0x1e);
    put_int(0);
    finalize(true);
}

// ORIG T 0x08460176：header(0,0x20)；short(uid)；byte(0)；finalize(true)。
void CDeathTower::CPacketHandler::makeDieState(unsigned short uid)
{
    put_header(0, 0x20);
    put_short((int)uid);
    put_byte(0);
    finalize(true);
}

// ORIG T 0x084601da：排行榜展示包。个人最优/榜首记录段委托给
// CDeathTowerRanking 的两个 static 表访问 + 记录序列化函数。
void CDeathTower::CPacketHandler::makeDisplayRangking(unsigned int tableType,
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
void CDeathTower::CPacketHandler::makeDisplayReward(int v1,
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
void CDeathTower::CPacketHandler::makeDisplayEPLP(bool flag)
{
    put_header(0, 0x92);
    put_byte(flag ? 1 : 0);
    finalize(true);
}

// ORIG T 0x0846050e：header(0,0xe)；byte(space)；short(1)；
// inv.MakeItemPacket(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), slot, *this)；finalize(true)。
void CDeathTower::CPacketHandler::makeUpdateItemList(CUser* user, ENUM_ITEMSPACE space,
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
// [b28d-24] DIFF 残148（P282 候选）: vcall 偏移 52 ORIG 3 次 vs 我方 2 次（/tmp/b28d_vcall_cand.tsv）——须按 P282 流程（vptr=_ZTV+8、word[(off+8)/4]）复核虚调目标。ORIG 0x08462538
void CDeathTower::CPlayData::generateRewardExp(int memberIdx, int stage,
                                               unsigned int baseExp, CParty* party,
                                               CDungeonMgr& dungeonMgr)
{
    CUser* user = party->get_user(memberIdx);
    unsigned int reward = 0;

    const char limitFlag = dungeonMgr.getDungeon()->limitOfStackableItemInTower();
    if (limitFlag != '\0')
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
        const float logWeight =
            G_CDataManager()->m_deathTower.m_rewardExpWeight[stage];
        LogManager::logFormat(1, kFile_DeathTower, kFunc_generateRewardExp, 0x2ff,
            "BLOOD_LOG : EROOR DEATHTOWER REWARD EXP(%u), LAST STAGE(%d), BASE EXP(%u), EXP WEIGHT(%f)\n",
            reward, stage, baseExp, (double)logWeight);
    }

    stClearRewardExpParameter_t clearParam;   // 栈构造（ORIG ctor 0x0814a8b8）
    stClearRewardBonusExp_t bonus;            // 栈构造（ORIG ctor 0x0814a800）

    const float avatarBonus = (float)party->getAvatarBonus();
    const float creatureBonus = (float)party->getCreatureBonus(user);

    float pcRoomRate = 0.0f;
    {   // 事件 8：PC 房间燃烧（整除 /100 —— imul 0x51eb851f 形态实证）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(8);
        if (ev->IsEventing(user))
            pcRoomRate = (float)(((CPCRoomBurningEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);
    }

    float expDoubleRate = 0.0f;
    {   // 事件 3：双倍经验（浮点 /100.0f，常量 _DAT_08c640b8 = 100.0f）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(3);
        if (ev->IsEventing(0))
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
    CEnvironment* env = G_CEnvironment();
    const unsigned int channelNo = (unsigned int)env->get_channel_no();
    const unsigned int dungeonIdx = (unsigned int)dungeonMgr.getDungeon()->get_index();
    const float channelRate =
        channelScript->getBonusRate(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            channelNo, dungeonIdx);

    float premiumAdvantageRate = 0.0f;
    if (user->IsHavePremiumAdvantage())
        premiumAdvantageRate =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f;

    const float serverEventExpRate = (float)CServerEvent::GetExpRate() / 100.0f;

    const float manageBonus = party->CalcurateManageBonus((ManageBonusType::T)0, user, reward);

    // 结算基础通关经验（exp 以引用进出）
    unsigned int expInOut = reward;
    reward = party->getClearRewardBonusExp(user, expInOut, clearParam, bonus);

    if (1100000 < reward)
    {
        LogManager::logFormat(1, kFile_DeathTower, kFunc_generateRewardExp, 0x324,
            "BLOOD_LOG : EROOR DEATHTOWER CLEAR REWARD EXP(%u)\n", reward);
        LogManager::logFormat(1, kFile_DeathTower, kFunc_generateRewardExp, 0x32e,
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
// +0x5e2c = m_deathTower.m_rewardGoldWeight（ORIG `flds` float）。随机分支经生成器虚槽 +0x08
// （generateRandomItem）；受控分支经虚槽 +0x0c（generateSpecificItem，ref 带
// dungeon/m_rankBonusIndex/goldBase/Factor 上下文）。受控分支额外触发幸运点
// 扣减（::CLuckPoint::UseLuckPoint）。ref 对象由其 ctor 初始化，上下文区按
// 具名字段写入（宽度逐条对齐 ORIG：movb/movw/movl，见
// WongWork__CDeathTower__CPlayData/generateRewardItem.md 0846205d..099 /
// 084622bc..328）。
// ==============================================================================
// [b28d-24] DIFF 残380: P167 迭代器写法差（MISSING begin/deEv×5；EXTRA front/C1/D1/G_CDataManager 等）——按 P167 判据改访问形。ORIG 0x08461fc0
void CDeathTower::CPlayData::generateRewardItem(int memberIdx, int count,
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

        ::CLuckPoint luck;                  // sizeof 0x24（ORIG -0x88..-0x64 区；全局 CLuckPoint）
        if (party != 0)
            party->GetLuckPoint(&luck, false);

        const int roll = get_rand_int(1000);
        const unsigned int boundHigh = (unsigned int)(
            G_CDataManager()->m_deathTower.m_rewardItemProb[0] * (int)param5);
        if ((roll > (int)boundHigh || boundHigh - (unsigned int)roll == 0))
        {
            // ---- 受控特定掉落分支（ORIG +0x2df：vtable+0x0c specific）----
            if ((unsigned int)roll <= (unsigned int)(
                    G_CDataManager()->m_deathTower.m_rewardItemProb[1] * (int)param5) &&
                (unsigned int)(G_CDataManager()->m_deathTower.m_rewardItemProb[1] *
                               (int)param5) - (unsigned int)roll != 0)
            {
                // ref 上下文区（ORIG 084622bc..328：movb/movb/movl/movl/movl）
                ref.m_field35 = 0;
                ref.m_field34 = (char)param4;
                ref.m_rankBonusIndex = 0x3f800000;             // float 1.0f 位型（ORIG movl）
                ref.m_dungeon = dungeonMgr.getDungeon();       // ORIG movl（指针）
                *reinterpret_cast<int*>(&ref.m_goldBase) = 0;  // ORIG movl $0 覆盖 +0x3c/+0x3e

                CItemGeneratorMgr* genMgr = G_CDataManager()->getItemGenerator();
                CItemGenerator* gen = genMgr->getGenerator(
                    (WongWork::CItemGeneratorMgr::eGenerateType_t)2);
                gen->generateSpecificItem(ref, result);        // 虚槽 +0x0c

                const bool dropEmpty = result.m_dropItems.empty();
                if (!dropEmpty)
                {
                    Inven_Item& dst = m_rewards[memberIdx].m_items[i];
                    dst.setCopy(result.m_dropItems.front());

                    Inven_Item probe;                          // ORIG buf(-0xf5)：整块复核
                    probe.setCopy(result.m_dropItems.front());
                    CItemList* itemList = G_CDataManager()->m_itemList;   // [b28d-Main] dm+0xc 直读
                    CItem* found = itemList->find_item(probe.m_addInfo);
                    if (found != 0)
                    {
                        const int grade = found->get_grade();
                        const int rarity = found->get_rarity();
                        ::CLuckPoint::UseLuckPoint(party->get_user(memberIdx),
                                                 (unsigned char)probe.m_field1,
                                                 rarity, grade);
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
            CItemGeneratorMgr* genMgr = G_CDataManager()->getItemGenerator();
            CItemGenerator* gen = genMgr->getGenerator(
                (WongWork::CItemGeneratorMgr::eGenerateType_t)2);
            gen->generateRandomItem(ref, randomResult);        // 虚槽 +0x08

            if (!randomResult.m_dropItems.empty())
            {
                if (party != 0)
                {
                    // QuickParty 奖励登记（基础奖励类型 0xb：道具奖励槽）
                    QuickParty::RewardData_Param rp(party);
                    rp.m_param4 = dungeonMgr.getDungeon()->get_index();
                    QuickParty::RewardData_Result rs;
                    rs.clear();
                    rs.m_value00 = randomResult.m_dropItems.front().m_addInfo2;
                    party->set_basic_reward((QuickParty::BasicRewardType)0xb, rp, rs);
                    randomResult.m_dropItems.front().m_addInfo2 = rs.m_value00;
                }
                Inven_Item& dst = m_rewards[memberIdx].m_items[i];
                dst.setCopy(randomResult.m_dropItems.front());
                const int addInfo = dst.get_add_info();
                // ORIG：fildl addInfo/flds m_rewardGoldWeight/fmuls/fistpl（默认控制字就近舍入）
                dst.set_add_info((int)((float)addInfo *
                                       G_CDataManager()->m_deathTower.m_rewardGoldWeight));
            }
        }
    }
}

} // namespace WongWork
