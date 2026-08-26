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

// ORIG fistpll/fistpl 默认控制字（RC=00 就近舍入）。
static long long roundFpuNearest(double x)
{
    return (long long)(x + (x >= 0.0 ? 0.5 : -0.5));
}

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
    if ((unsigned int)(tick - m_lastMobDieTime) > limit)
    {
        m_dieCnt = 0;
        m_lastMobDieTime = tick;
    }
    else
    {
        ++m_dieCnt;
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
CDeathTower::CPlayData::CPlayData()
{
}

// ORIG 0x82a7572：push ebp/mov esp,ebp/pop ebp/ret —— 空函数（Inven_Item 平凡
// 析构，编译器不发清理代码，与 ORIG 完全一致）。
CDeathTower::CPlayData::~CPlayData()
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

// ORIG T 0x0845fc6c：header(0,0x27)；short(b)；short(a)；不 finalize。
void CDeathTower::CPacketHandler::makePickupItemHeader(unsigned short a, unsigned short b)
{
    put_header(0, 0x27);
    put_short((int)b);
    put_short((int)a);
}

// ORIG T 0x0845fcc4：4 队伍槽金钱信息，尾部统一 finalize(true)。
void CDeathTower::CPacketHandler::appendPickupMoney(int* moneyA, int* moneyB, CParty* party)
{
    for (int i = 0; i <= 3; ++i)
    {
        bool slotValid = false;
        if (party->get_user(i) != 0 && party->checkValidUser(i))
            slotValid = true;
        if (!slotValid)
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
    put_byte(item.m_count == 0 ? 1 : 0);                 // byte(*param_5 == 0 ? 1 : 0)
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
    put_byte(flag ? 1 : 0);                        // ebp+0x1c → param_5
    put_int((int)playTime);                        // ebp+0x24 → param_7
    const unsigned int towerIdx = CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    CDeathTowerRanking* table =
        (CDeathTowerRanking*)CDeathTowerRanking::getRankTable(tableType);
    table->makeMyBestRecordPacket(towerIdx, key, *this);   // ORIG 0x08468b8e
    table->makeTopRankerRecordPacket(towerIdx, *this);     // ORIG 0x08468c68
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
// inv.MakeItemPacket(GetInvenTypeFromItemSpace(space), slot, *this)；finalize(true)。
void CDeathTower::CPacketHandler::makeUpdateItemList(CUser* user, ENUM_ITEMSPACE space,
                                                     int slot)
{
    // GetInvenTypeFromItemSpace：ORIG 全局 _Z26GetInvenTypeFromItemSpace
    // ENUM_ITEMSPACE；权威声明见 CInventory.h（勿在 WongWork ns 内局部 extern，
    CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
    put_header(0, 0xe);
    put_byte((int)space);
    put_short(1);
    inven->MakeItemPacket((INVEN_TYPE)::GetInvenTypeFromItemSpace(space), slot, *this);
    finalize(true);
}

// ==============================================================================
// CPlayData::generateRewardExp（ORIG T 0x08462538）
//
// 权重表：DataManager 基址 +0xc+(stage+tier)*4 的 float 数组（tier 依
// CDungeon::limitOfStackableItemInTower 取 0x1464 或 0x15f4）；所得 exp 与最终
// 结算结果 >0x10c8e0(1100000) 时写 BLOOD_LOG 并截断。各加成系数仅进日志参数流。
// ==============================================================================
void CDeathTower::CPlayData::generateRewardExp(int memberIdx, int stage,
                                               unsigned int baseExp, CParty* party,
                                               CDungeonMgr& dungeonMgr)
{
    CUser* user = party->get_user(memberIdx);
    unsigned int reward = 0;

    const char limitFlag = dungeonMgr.getDungeon()->limitOfStackableItemInTower();
    {
        // DataManager 浮点权重表（tier：有限堆叠副本 0x1464，否则 0x15f4）
        const unsigned int tier = (limitFlag != '\0') ? 0x1464u : 0x15f4u;
        CDataManager* dm = G_CDataManager();
        const float weight = *(float*)((char*)dm + 0xc +
                                       ((unsigned int)stage + tier) * 4);
        // ORIG fildll(stage*base)→float→fmuls weight→fistpll（就近舍入）
        double scaled = (double)((float)((long long)stage * (long long)baseExp)) *
                        (double)weight;
        reward = (unsigned int)roundFpuNearest(scaled);
    }

    if (1100000 < reward)
    {
        const float logWeight = *(float*)((char*)G_CDataManager() + 0xc +
                                          ((unsigned int)stage + 0x1464) * 4);
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
        if (ev != 0 && ev->IsEventing(user))
            pcRoomRate = (float)(((CPCRoomBurningEvent*)ev)->GetExpFactor() / 100);
    }

    float expDoubleRate = 0.0f;
    {   // 事件 3：双倍经验（浮点 /100.0f，常量 _DAT_08c640b8 = 100.0f）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(3);
        if (ev != 0 && ev->IsEventing(0))
            expDoubleRate = (float)((CExpDoubleEvent*)ev)->GetExpFactor() / 100.0f;
    }

    int burningBonusExp = 0;
    {   // 事件 0xf：疲劳燃烧加成（千分率原始值）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if (ev != 0 && ev->IsEventing(0))
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
        channelScript->getBonusRate((unsigned char)env->get_server_group(), channelNo, dungeonIdx);

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
// +0x5e24（乘 param5 比较），计数率因子 +0x5e2c。随机分支经生成器虚槽 +0x08
// （generateRandomItem）；受控分支经虚槽 +0x0c（generateSpecificItem，ref 带
// dungeon/m_rankBonusIndex/goldBase/Factor 上下文）。受控分支额外触发幸运点
// 扣减（::CLuckPoint::UseLuckPoint）。ref 对象由其 ctor 初始化，上下文区按
// ORIG 原始偏移写入。
// ==============================================================================
void CDeathTower::CPlayData::generateRewardItem(int memberIdx, int count,
                                                const CDungeonMgr& dungeonMgr,
                                                unsigned int param4, unsigned int param5,
                                                CParty* party)
{
    m_rewards[memberIdx].m_itemCnt = count;

    CDataManager* dm = G_CDataManager();
    const int rateHigh = *(int*)((char*)dm + 0x5e20);
    const int rateLow = *(int*)((char*)dm + 0x5e24);
    const float rateFactor = *(float*)((char*)dm + 0x5e2c);

    for (int i = 0; i < count; ++i)
    {
        stGenerateRefData_t ref;
        stGenerateResult_t result;

        ::CLuckPoint luck;                  // sizeof 0x24（ORIG -0x88..-0x64 区；全局 CLuckPoint）
        if (party != 0)
            party->GetLuckPoint(&luck, false);

        const int roll = get_rand_int(1000);
        const unsigned int boundHigh = (unsigned int)(rateHigh * (int)param5);
        if ((roll > (int)boundHigh || boundHigh - (unsigned int)roll == 0))
        {
            // ---- 受控特定掉落分支（ORIG +0x2df：vtable+0x0c specific）----
            if ((unsigned int)roll <= (unsigned int)(rateLow * (int)param5) &&
                (unsigned int)(rateLow * (int)param5) - (unsigned int)roll != 0)
            {
                // ref 上下文区按 ORIG 084622bc..308 原始偏移写入
                *(char*)((char*)&ref + 0x35) = 0;
                *(char*)((char*)&ref + 0x34) = (char)param4;
                *(int*)((char*)&ref + 0x38) = 0x3f800000;      // float 1.0f 位型
                *(CDungeon**)((char*)&ref + 0x40) = dungeonMgr.getDungeon();
                *(int*)((char*)&ref + 0x3c) = 0;

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
                    CItemList* itemList = dm->getItemList();   // dm+0xc 具名访问器
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
            // ref 上下文区按 ORIG 0846205d..099 原始偏移写入
            *(char*)((char*)&ref + 0x35) = 0;
            *(char*)((char*)&ref + 0x34) = (char)param4;
            *(char*)((char*)&ref + 0x36) = 1;
            *(int*)((char*)&ref + 0x38) = 1;                   // m_rankBonusIndex
            *(unsigned short*)((char*)&ref + 0x3c) = 1;        // m_goldBase
            *(unsigned short*)((char*)&ref + 0x3e) = 1;        // m_goldFactor
            *(unsigned short*)((char*)&ref + 0x40) = 1;        // m_dungeon 低 16 位（ORIG 原样）
            *(unsigned short*)((char*)&ref + 0x42) = 1;        // m_dungeon 高 16 位（ORIG 原样）

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
                // ORIG：fildl/flds 0x5e2c/fmuls/fistpl（就近舍入）
                double boosted = (double)addInfo * (double)rateFactor;
                dst.set_add_info((int)roundFpuNearest(boosted));
            }
        }
    }
}

} // namespace WongWork
