// GmCmds_E.cpp — game_master GM 命令批次 E（11 类）。
// 对照 docs/class_func_reports/game_master__<Class>/：
//   CMobileRestrictCommand / CMonsterDropTest / NpcLimitBuyItem /
//   COneADayItemShop / CPcroomPremiumCmd / CPcroomRewardReset / CPickup /
//   CPowerWarStraightVictory / CRandomObjectTest / CRecipeSuccess / CRefund
//
// 头文件 GameMasterCmd.h 本批禁止编辑：+0x08 通用槽在头里为占位
// std::string（gcc4.4 旧 ABI sizeof=4）。本批多数类的 ORIG 真实语义是
// int（atoi/sscanf 解析结果），经 str_as_int / str_as_int2 别名访问；
// CPowerWarStraightVictory 的 +0x08 真实语义是 short（仅写 2 字节），
// 经 str_as_short 访问；CMobileRestrictCommand 的 +0x0c 为 uint（无占位名），
// 从 &m_str0+4 别名访问（sizeof=0x10 与 vptr+m_parser+8 一致）。
//
// 辅助函数带参者（头内占位声明为无参）以 G 批次同款 asm 标签自由函数承载
// ORIG 成员符号（i386 成员 ABI = cdecl + this 首参压栈，extern 兼容）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <list>
#include "GameWorld.h"
#include "LimitNpcBuyItemRequestInfo.h"
#include <vector>
#include <string>
#include <algorithm>

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "WarField.h"
#include "CBattle_Field.h"
#include "CDataManager.h"
#include "CGlobalEffectManager.h"
#include "DBUpdatePcroomDailyReward.h"  // DBUpdatePcroomDailyReward::MSG（MoreTypes.h 占位已迁入类）
#include "GlobalData.h"
#include "PvPTypes.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CHandlePremium.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "TaiwanInternal.h"
#include "MoreTypes.h"
#include "CInventory.h"
#include "InterSelectPcroomDailyReward.h"

// ---------------------------------------------------------------------------
// 外部符号（本批禁改共享头）：
//
// ORIG T 0x084aeac6 _ZN11game_master12CBoosterTest11SendChatMsgEP5CUserPc
//   —— CBoosterTest::SendChatMsg(CUser*, char*)（static 语义：实现不读 this）。
//   归属 CBoosterTest 批次；此处仅声明对接。
void game_master_CBoosterTest_SendChatMsg_Orig(void* user, char* msg)
    asm("_ZN11game_master12CBoosterTest11SendChatMsgEP5CUserPc");

// ORIG T 0x084af504 _ZN11game_master12CBoosterTest16makeFileContentsEP5CUserPcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
//   —— CBoosterTest::makeFileContents(CUser*, char*, map<int,int>&)。仅声明。
void game_master_CBoosterTest_makeFileContents_Orig(void* user, char* buf,
                                                    std::map<int, int>* stat)
    asm("_ZN11game_master12CBoosterTest16makeFileContentsEP5CUserPcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// ORIG T 0x084aebf2 _ZN11game_master12CBoosterTest11printClientEP5CUserbPKcS4_
//   —— CBoosterTest::printClient(CUser*, bool, const char*, const char*)。仅声明。
void game_master_CBoosterTest_printClient_Orig(void* user, bool last,
                                               const char* name, const char* body)
    asm("_ZN11game_master12CBoosterTest11printClientEP5CUserbPKcS4_");

// ORIG T 0x081090fa _Z14StringToNumberPKc —— 全局工具：s ? strtoul(s,0,10) : 0。
// 项目尚无定义点；weak 定义避免与他批冲突（若他处强定义则以彼为准）。
__attribute__((weak)) int StringToNumber(const char* s);
__attribute__((weak)) int StringToNumber(const char* s)
{
    if (s == 0)
    {
        return 0;
    }
    return static_cast<int>(strtoul(s, 0, 10));
}


// ORIG W 0x0822b64c _ZNK12CDataManager22getVendingMachineTableEv
//   return (char*)dm + 0xb424（&m_vendingMachine）。weak 定义防跨批冲突。
__attribute__((weak)) std::vector<stVendingMachine_t>*
CDataManager_getVendingMachineTable_Orig(const void* dm)
    asm("_ZNK12CDataManager22getVendingMachineTableEv");
__attribute__((weak)) std::vector<stVendingMachine_t>*
CDataManager_getVendingMachineTable_Orig(const void* dm)
{
    return &const_cast<CDataManager*>(
        static_cast<const CDataManager*>(dm))->m_vendingMachine;
}

// ORIG T 0x0815cf06 _ZN25DBUpdatePcroomDailyReward11makeRequestEP5CUser
//   （Dispatchers.cpp 标注「未建模」，此处按 objdump 全量转写；weak 定义）。
//   流程：SmallStreamPool.Acquire("local",0x325) → CStreamGuard(st,true) →
//   Taiwan::internal_stream(guard, pack 0xe, GetUID()) → GetInBuffer<MSG>()；
//   null 则 cMyTrace 报错；否则 MSG.acc_id(+0)/state(+4 byte) 后投递 DB_Q。
__attribute__((weak)) void DBUpdatePcroomDailyReward_makeRequest_Orig(void* user)
    asm("_ZN25DBUpdatePcroomDailyReward11makeRequestEP5CUser");
__attribute__((weak)) void DBUpdatePcroomDailyReward_makeRequest_Orig(void* user)
{
    StreamPool* pool = GlobalData::s_stream_pool;
    Stream* st = pool->Acquire("local", 0x325);
    CStreamGuard guard(st, true);
    Taiwan::internal_stream(guard,
                            static_cast<TaiwanInternalPack::T>(0xe),
                            reinterpret_cast<CUser*>(user)->GetUID());
    DBUpdatePcroomDailyReward::MSG* msg =
        guard.GetInBuffer<DBUpdatePcroomDailyReward::MSG>();
    if (msg == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x32b, 0);
        trace("[Taiwan, Pcroom] msg is null");
    }
    else
    {
        msg->m_id = reinterpret_cast<CUser*>(user)->get_acc_id();
        msg->m_remain = reinterpret_cast<CUser*>(user)->m_field8ec32;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

namespace game_master
{

namespace
{

// 注：历史版本以 std::string 占位 +0x08 槽并按 ORIG int/short 语义别名访问
//（gcc4.4 旧 ABI string sizeof=4）；该批占位字段已全部具名化
//（GameMasterCmd.h：m_duration/m_param1/m_param2/m_result/m_straight/
// m_mode/m_count/m_restrictSwitch 等），本命名空间仅保留空壳。

} // namespace

// ===========================================================================
// CMobileRestrictCommand（sizeof=0x10）
//   +0x08 string m_str0（目标限制域："mobile"/"character"）
//   +0x0c uint 限制开关（StringToNumber(args[2])!=0；无头内占位名，别名访问）
// ===========================================================================

// ORIG ctor 0x082a89b8：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084b3bdc：parser 非空时 m_str0=args[1]；+0xc=StringToNumber(args[2])!=0。
void CMobileRestrictCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        m_restrictSwitch =
            static_cast<unsigned int>(StringToNumber(m_parser->m_extra[2].c_str()) != 0);
    }
}

// ORIG execute 0x084b3c60：m_str0=="mobile" 时改 garena auth data +0x2c 字节并打日志；
// =="character" 分支仅做比较（结果丢弃，空体）。
void CMobileRestrictCommand::execute()
{
    CUser* user = GetUser();
    if (m_str0 == "mobile")
    {
        char* auth = user->getGarenaAuthData();
        *(auth + 0x2c) = static_cast<char>(m_restrictSwitch);
        const char* yesNo;
        if (m_restrictSwitch == 1)
        {
            yesNo = "yes";
        }
        else
        {
            yesNo = "no";
        }
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x183d, 0);
        trace("[GMCommand] Set mobile restricted. mobile auth:%s", yesNo);
    }
    else if (m_str0 == "character")
    {
        // ORIG 仅比较、无副作用
    }
}

// ===========================================================================
// CMonsterDropTest（sizeof=0x10）
//   +0x08 int mode（0=single / 1=passed-map 保存 / 2=passed 统计 / 3=gold 统计）
//   +0x0c int count（统计次数，0 视作 10000；上限 mode2=10000 其余=100000）
// ===========================================================================

// ORIG ctor 0x082a861e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG clear 0x084b4096：两 int 槽清零。
void CMonsterDropTest::clear()
{
    m_mode = 0;
    m_count = 0;
}

// ORIG parse 0x084ace8a：clear；sscanf(args,"%d%d",&mode,&count)；随后按发起者
// party 校验：无 party 提示；mode==1 清空 passed 列表后压入当前图并开启
// 地图保存；mode∈{2,3} 关闭地图保存。
void CMonsterDropTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d",
           &m_mode, &m_count);

    CUser* user = GetUser();
    if (user != 0)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party == 0)
        {
            game_master_CBoosterTest_SendChatMsg_Orig(user,
                                                      const_cast<char*>("Party가 없습니다"));
        }
        else
        {
            unsigned int mode = static_cast<unsigned int>(m_mode);
            if (mode == 1)
            {
                std::vector<MapInfo>* passed =
                    static_cast<std::vector<MapInfo>*>(party->getPassedMapInfoList());
                passed->clear();
                CBattle_Field* bf = &party->m_battleField;
                MapInfo* cur = bf->GetCurrentMapInfo();
                passed->push_back(*cur);
                party->setDungeonMapSaving(true);
            }
            else if (mode != 0 && mode < 4)
            {
                party->setDungeonMapSaving(false);
            }
        }
    }
}

// ORIG checkCommandExecuteError(CParty*) 0x084ad82a（带参，asm 承载）：
// 返回错误文案指针或 0。
// ---- ORIG 符号前向声明（定义见后；asm 标签承载成员 mangled 名） ----
void CMonsterDropTest_makeGeneralItemDropList_Orig(void* self,
                                                   std::list<map_item>* items,
                                                   std::map<int, int>* out)
    asm("_ZN11game_master16CMonsterDropTest23makeGeneralItemDropListERSt4listI8map_itemSaIS2_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");
void CMonsterDropTest_makeGoldItemDropList_Orig(void* self,
                                                std::list<map_item>* items,
                                                std::map<int, int>* out)
    asm("_ZN11game_master16CMonsterDropTest20makeGoldItemDropListERSt4listI8map_itemSaIS2_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");
void CMonsterDropTest_makeUpgradeDropList_Orig(void* self,
                                               std::list<map_item>* items,
                                               std::map<int, std::map<int, int> >* out)
    asm("_ZN11game_master16CMonsterDropTest19makeUpgradeDropListERSt4listI8map_itemSaIS2_EERSt3mapIiS6_IiiSt4lessIiESaISt4pairIKiiEEES8_SaIS9_ISA_SD_EEE");
void CMonsterDropTest_processKillMonster_Orig(void* self, CParty* party,
                                              MapInfo* mapInfo, map_monster* mob,
                                              std::vector<bool>* flags, int killCnt,
                                              std::list<map_item>* items)
    asm("_ZN11game_master16CMonsterDropTest18processKillMonsterEP6CPartyR7MapInfoR11map_monsterRSt6vectorIbSaIbEEiRSt4listI8map_itemSaISC_EE");
void CMonsterDropTest_processDungeonDrop_Orig(void* self, CParty* party,
                                              std::map<int, int>* drop,
                                              std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest18processDungeonDropEP6CPartyRSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS3_IiSA_S5_SaIS6_IS7_SA_EEE");
void CMonsterDropTest_singleMapMonsterKill_Orig(void* self, CParty* party,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest20singleMapMonsterKillEP6CPartyRSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS3_IiSA_S5_SaIS6_IS7_SA_EEE");
void CMonsterDropTest_passedMapMonsterKill_Orig(void* self, CParty* party,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest20passedMapMonsterKillEP6CPartyRSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS3_IiSA_S5_SaIS6_IS7_SA_EEE");
void CMonsterDropTest_getDungeonDropResult_Orig(void* self,
                                                std::list<map_item>* items,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest20getDungeonDropResultERSt4listI8map_itemSaIS2_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS6_IiSD_S8_SaIS9_ISA_SD_EEE");
const char* CMonsterDropTest_checkCommandExecuteError_Orig(
    void* self, CParty* party)
    asm("_ZN11game_master16CMonsterDropTest24checkCommandExecuteErrorEP6CParty");
const char* CMonsterDropTest_checkCommandExecuteError_Orig(
    void* self, CParty* party)
{
    CMonsterDropTest* cmd = static_cast<CMonsterDropTest*>(self);
    int mode = cmd->m_mode;
    unsigned int count = static_cast<unsigned int>(cmd->m_count);
    if (party == 0)
    {
        return "Party가 없습니다";
    }
    if (mode == 1)
    {
        return "user가 지나간 맵을 저장합니다";
    }
    if (mode == 2)
    {
        if (10000 < count)
        {
            return "passed map 통계는 0 ~ 10000번까지만 지원됩니다";
        }
    }
    else
    {
        if (100000 < count)
        {
            return "0 ~ 100000번 통계까지만 지원됩니다";
        }
    }

    CBattle_Field* bf = &party->m_battleField;
    int idx = bf->get_dungeon_index();
    if (G_CDataManager()->find_dungeon(idx) == 0)
    {
        return "Dungeon에서 사용가능하지 않습니다";
    }
    if (party->m_battleField.m_bloodState == 1)
    {
        return "연습모드 입니다";
    }
    return 0;
}

// ORIG processDungeonDrop(CParty*, map&, map2&) 0x084ad8dc：按 mode 分派击杀模拟。
void CMonsterDropTest_processDungeonDrop_Orig(void* self, CParty* party,
                                              std::map<int, int>* drop,
                                              std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest18processDungeonDropEP6CPartyRSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS3_IiSA_S5_SaIS6_IS7_SA_EEE");
void CMonsterDropTest_processDungeonDrop_Orig(void* self, CParty* party,
                                              std::map<int, int>* drop,
                                              std::map<int, std::map<int, int> >* grade)
{
    CMonsterDropTest* cmd = static_cast<CMonsterDropTest*>(self);
    int mode = cmd->m_mode;
    if (mode == 0)
    {
        CMonsterDropTest_singleMapMonsterKill_Orig(self, party, drop, grade);
    }
    else if (static_cast<unsigned int>(mode) - 2u < 2u)
    {
        CMonsterDropTest_passedMapMonsterKill_Orig(self, party, drop, grade);
    }
}

// ORIG processKillMonster(CParty*, MapInfo&, map_monster&, vector<bool>&, int, list&)
// 0x084add20：全局效果 1/4 放大掉落次数（向零截断换算，min/max 夹取）；
// 统计队员 premium 0x4f 数并取加成；hell 组计入全灭标记；最终转发
// CBattle_Field::_MakeDropItems。
void CMonsterDropTest_processKillMonster_Orig(void* self, CParty* party,
                                              MapInfo* mapInfo, map_monster* mob,
                                              std::vector<bool>* flags, int killCnt,
                                              std::list<map_item>* items)
    asm("_ZN11game_master16CMonsterDropTest18processKillMonsterEP6CPartyR7MapInfoR11map_monsterRSt6vectorIbSaIbEEiRSt4listI8map_itemSaISC_EE");
void CMonsterDropTest_processKillMonster_Orig(void* self, CParty* party,
                                              MapInfo* mapInfo, map_monster* mob,
                                              std::vector<bool>* flags, int killCnt,
                                              std::list<map_item>* items)
{
    (void)self;
    CGlobalEffectManager* effMgr = GlobalData::s_globalEffectManager_;

    if (effMgr->isAffectedEffect(1))
    {
        float effVal = effMgr->getEffectValue(1);
        float baseF = G_CDataManager()->getDropRate();
        // ORIG 0x84add20：条件 effVal > (int)baseF（fildll 载入截断整数 base 与
        // effVal 浮点比较）；加成 (base - effVal) * killCnt（符号与 baseF 相反）。
        int base = static_cast<int>(baseF); // ORIG fistpll 截断口径
        if (static_cast<float>(base) < effVal)
        {
            killCnt += static_cast<int>((static_cast<float>(base) - effVal)
                                        * static_cast<float>(killCnt));
            int cap = static_cast<int>(static_cast<float>(killCnt) * effVal);
            killCnt = std::max(0, std::min(killCnt, cap));
        }
    }
    if (effMgr->isAffectedEffect(4))
    {
        float effVal = effMgr->getEffectValue(4);
        float baseF = G_CDataManager()->getDropRate();
        int base = static_cast<int>(baseF);
        if (static_cast<float>(base) < effVal)
        {
            killCnt += static_cast<int>((static_cast<float>(base) - effVal)
                                        * static_cast<float>(killCnt));
            int cap = static_cast<int>(static_cast<float>(killCnt) * effVal);
            killCnt = std::max(0, std::min(killCnt, cap));
        }
    }

    // premium 0x4f 队员计数与其独立掉落加成
    unsigned char premCnt = 0;
    int premRate = 0;
    for (int i = 0; i < 4; i++)
    {
        if (party->checkValidUser(i))
        {
            CUser* member = party->GetMember()[i].m_pUser;
            if (member->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
            {
                premCnt = premCnt + 1;
            }
        }
    }
    if (premCnt != 0 && premCnt < 5)
    {
        for (int i = 0; i < 4; i++)
        {
            if (party->checkValidUser(i))
            {
                CUser* member = party->GetMember()[i].m_pUser;
                premRate = static_cast<int>(
                    member->getAddIndependentDropRateFromPremium(
                        static_cast<ENUM_PREMIUM_TYPE>(0x4f), premCnt));
                break;
            }
        }
    }

    bool hellKilledAll = false;
    if (mob->m_hellPartyGroupIdx != 0)
    {
        CBattle_Field* bf = &party->m_battleField;
        bf->KillHellPartyGroupMonsterCnt(*mapInfo, *mob, hellKilledAll);
    }

    int memberCount = party->get_member_count();
    CBattle_Field* bf = &party->m_battleField;
    bf->_MakeDropItems(*items, *mapInfo, *mob,
                       static_cast<unsigned short>(0), memberCount, 100, killCnt,
                       hellKilledAll, *flags, 1.0f, premRate);
}

// ORIG singleMapMonsterKill 0x084ad93a：当前地图全部 m_dropCnt==100 怪 × count 次。
void CMonsterDropTest_singleMapMonsterKill_Orig(void* self, CParty* party,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest20singleMapMonsterKillEP6CPartyRSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS3_IiSA_S5_SaIS6_IS7_SA_EEE");
void CMonsterDropTest_singleMapMonsterKill_Orig(void* self, CParty* party,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
{
    std::list<map_item> items;
    int killCnt = 100;
    std::vector<bool> flags;
    flags.push_back(true);
    flags.push_back(true);

    CBattle_Field* bf = &party->m_battleField;
    MapInfo* cur = bf->GetCurrentMapInfo();

    for (unsigned int round = 0; round < static_cast<unsigned int>(
             static_cast<CMonsterDropTest*>(self)->m_count); round++)
    {
        for (std::map<int, map_monster>::iterator it = cur->m_monsterMap.begin();
             it != cur->m_monsterMap.end(); ++it)
        {
            if (it->second.m_dropCnt == 100)
            {
                CMonsterDropTest_processKillMonster_Orig(self, party, cur, &it->second,
                                                         &flags, killCnt, &items);
                CMonsterDropTest_getDungeonDropResult_Orig(self, &items, drop, grade);
                items.clear();
            }
        }
    }
}

// ORIG passedMapMonsterKill 0x084adaf8：passed 列表每张图同样处理。
void CMonsterDropTest_passedMapMonsterKill_Orig(void* self, CParty* party,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest20passedMapMonsterKillEP6CPartyRSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS3_IiSA_S5_SaIS6_IS7_SA_EEE");
void CMonsterDropTest_passedMapMonsterKill_Orig(void* self, CParty* party,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
{
    std::list<map_item> items;
    int killCnt = 100;
    std::vector<bool> flags;
    flags.push_back(true);
    flags.push_back(true);

    std::vector<MapInfo>* passed =
        static_cast<std::vector<MapInfo>*>(party->getPassedMapInfoList());

    for (unsigned int round = 0; round < static_cast<unsigned int>(
             static_cast<CMonsterDropTest*>(self)->m_count); round++)
    {
        for (std::vector<MapInfo>::iterator mit = passed->begin();
             mit != passed->end(); ++mit)
        {
            for (std::map<int, map_monster>::iterator it = mit->m_monsterMap.begin();
                 it != mit->m_monsterMap.end(); ++it)
            {
                if (it->second.m_dropCnt == 100)
                {
                    CMonsterDropTest_processKillMonster_Orig(self, party, &*mit,
                                                             &it->second, &flags,
                                                             killCnt, &items);
                    CMonsterDropTest_getDungeonDropResult_Orig(self, &items, drop, grade);
                    items.clear();
                }
            }
        }
    }
}

// ORIG getDungeonDropResult(list&, map&, map2&) 0x084ae0e2：按 mode 归账。
void CMonsterDropTest_getDungeonDropResult_Orig(void* self,
                                                std::list<map_item>* items,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
    asm("_ZN11game_master16CMonsterDropTest20getDungeonDropResultERSt4listI8map_itemSaIS2_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEERS6_IiSD_S8_SaIS9_ISA_SD_EEE");
void CMonsterDropTest_getDungeonDropResult_Orig(void* self,
                                                std::list<map_item>* items,
                                                std::map<int, int>* drop,
                                                std::map<int, std::map<int, int> >* grade)
{
    CMonsterDropTest* cmd = static_cast<CMonsterDropTest*>(self);
    int mode = cmd->m_mode;
    if (mode != 2)
    {
        if (mode == 3)
        {
            CMonsterDropTest_makeGoldItemDropList_Orig(self, items, drop);
            return;
        }
        if (mode != 0)
        {
            return;
        }
    }
    CMonsterDropTest_makeGeneralItemDropList_Orig(self, items, drop);
    CMonsterDropTest_makeUpgradeDropList_Orig(self, items, grade);
}

// ORIG makeGeneralItemDropList 0x084ae734：普通物品按 itemId 计数；
// itemId==0（金币类）把 Inven_Item.m_addInfo2 累计到键 0。
void CMonsterDropTest_makeGeneralItemDropList_Orig(void* self,
                                                   std::list<map_item>* items,
                                                   std::map<int, int>* out)
    asm("_ZN11game_master16CMonsterDropTest23makeGeneralItemDropListERSt4listI8map_itemSaIS2_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");
void CMonsterDropTest_makeGeneralItemDropList_Orig(void* self,
                                                   std::list<map_item>* items,
                                                   std::map<int, int>* out)
{
    (void)self;
    for (std::list<map_item>::iterator it = items->begin(); it != items->end(); ++it)
    {
        int key = it->m_item.m_addInfo;
        std::map<int, int>::iterator mit = out->find(key);
        if (key == 0)
        {
            if (mit == out->end())
            {
                out->insert(std::make_pair(key, it->m_item.m_addInfo2));
            }
            else
            {
                mit->second += it->m_item.m_addInfo2;
            }
        }
        else
        {
            if (mit == out->end())
            {
                out->insert(std::make_pair(key, 1));
            }
            else
            {
                mit->second++;
            }
        }
    }
}

// ORIG makeGoldItemDropList 0x084ae944：itemId==0 的金币掉落按金额分组计数。
void CMonsterDropTest_makeGoldItemDropList_Orig(void* self,
                                                std::list<map_item>* items,
                                                std::map<int, int>* out)
    asm("_ZN11game_master16CMonsterDropTest20makeGoldItemDropListERSt4listI8map_itemSaIS2_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");
void CMonsterDropTest_makeGoldItemDropList_Orig(void* self,
                                                std::list<map_item>* items,
                                                std::map<int, int>* out)
{
    (void)self;
    for (std::list<map_item>::iterator it = items->begin(); it != items->end(); ++it)
    {
        if (it->m_item.m_addInfo == 0)
        {
            std::map<int, int>::iterator mit = out->find(it->m_item.m_addInfo2);
            if (mit == out->end())
            {
                out->insert(std::make_pair(it->m_item.m_addInfo2, 1));
            }
            else
            {
                mit->second++;
            }
        }
    }
}

// ORIG makeUpgradeDropList 0x084ae14c：非堆叠掉落按 grade 分桶；
// 桶内记录 [强化等级]=次数 与 [特殊类型+1000]=次数（特殊类型由
// m_addInfo2 映射：0x5aa7→1 / 0xfa6→2 / 0x6a23→3 / 0x5df0→4 / 0x7270→5）。
void CMonsterDropTest_makeUpgradeDropList_Orig(void* self,
                                               std::list<map_item>* items,
                                               std::map<int, std::map<int, int> >* out)
    asm("_ZN11game_master16CMonsterDropTest19makeUpgradeDropListERSt4listI8map_itemSaIS2_EERSt3mapIiS6_IiiSt4lessIiESaISt4pairIKiiEEES8_SaIS9_ISA_SD_EEE");
void CMonsterDropTest_makeUpgradeDropList_Orig(void* self,
                                               std::list<map_item>* items,
                                               std::map<int, std::map<int, int> >* out)
{
    (void)self;
    for (std::list<map_item>::iterator it = items->begin(); it != items->end(); ++it)
    {
        if (it->m_item.m_addInfo == 0)
        {
            continue;
        }
        CItem* ci = G_CDataManager()->find_item(it->m_item.m_addInfo);
        if (ci->is_stackable())
        {
            continue;
        }

        int special = 0;
        int v = it->m_item.m_addInfo2;
        if (v == 0x5df0)
        {
            special = 4;
        }
        else if (v < 0x5df1)
        {
            if (v == 0xfa6)
            {
                special = 2;
            }
            else if (v == 0x5aa7)
            {
                special = 1;
            }
        }
        else
        {
            if (v == 0x6a23)
            {
                special = 3;
            }
            else if (v == 0x7270)
            {
                special = 5;
            }
        }

        int grade = ci->get_grade();
        std::map<int, std::map<int, int> >::iterator git = out->find(grade);
        if (git == out->end())
        {
            std::map<int, int> inner;
            inner.insert(std::make_pair(
                static_cast<int>(it->m_item.GetUpgrade()), 1));
            inner.insert(std::make_pair(special + 1000, 1));
            out->insert(std::make_pair(grade, inner));
        }
        else
        {
            int up = static_cast<int>(it->m_item.GetUpgrade());
            std::map<int, int>::iterator f = git->second.find(up);
            if (f == git->second.end())
            {
                git->second.insert(std::make_pair(up, 1));
            }
            else
            {
                f->second++;
            }
            int k2 = special + 1000;
            std::map<int, int>::iterator g = git->second.find(k2);
            if (g == git->second.end())
            {
                git->second.insert(std::make_pair(k2, 1));
            }
            else
            {
                g->second++;
            }
        }
    }
}

// ORIG makeFileGoldContents 0x084ad6d6：gold 统计追加到文件缓冲；容量不足时提示。
void CMonsterDropTest_makeFileGoldContents_Orig(void* user, char* fileBuf,
                                                std::map<int, int>* stat)
    asm("_ZN11game_master16CMonsterDropTest20makeFileGoldContentsEP5CUserPcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");
void CMonsterDropTest_makeFileGoldContents_Orig(void* user, char* fileBuf,
                                                std::map<int, int>* stat)
{
    char line[1024];
    memset(line, 0, 0x100);   // ORIG rep stos 0x100（循环内 memset 仍 0x400）

    for (std::map<int, int>::iterator it = stat->begin(); it != stat->end();)
    {
        sprintf(line, "\"%s\"\t%d\t%d\n", "gold", it->first, it->second);
        size_t used = strlen(fileBuf);
        size_t addLen = strlen(line);
        if (used + addLen > 89999)
        {
            game_master_CBoosterTest_SendChatMsg_Orig(user, const_cast<char*>("결과 파일 이어붙힘"));
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, sizeof(line));
        stat->erase(it++);
    }
}

// ORIG execute 0x084acf98：主流程（校验 → 分派击杀 → 分模式落盘 → 升级分表输出）。
void CMonsterDropTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    const char* err = CMonsterDropTest_checkCommandExecuteError_Orig(this, party);
    if (err != 0)
    {
        game_master_CBoosterTest_SendChatMsg_Orig(user,
                                                  const_cast<char*>(err));
        return;
    }

    game_master_CBoosterTest_SendChatMsg_Orig(user, const_cast<char*>("Dungeon Monster Drop 결과체크를 시작합니다"));

    if (m_count == 0)
    {
        m_count = 10000;
    }
    char cnt[21];
    sprintf(cnt, "%d번 통계입니다", m_count);
    game_master_CBoosterTest_SendChatMsg_Orig(user, cnt);

    // ORIG +0x11f：落盘文件名缓冲进入循环前一次性清 255 字节
    //（后续仅由 sprintf 覆盖；非法 mode 无分支写它时仍是空串）。
    char fname[256];
    memset(fname, 0, 0xff);

    char fileBuf[95000];
    memset(fileBuf, 0, sizeof(fileBuf));
    std::map<int, int> drop;
    std::map<int, std::map<int, int> > grade;

    CMonsterDropTest_processDungeonDrop_Orig(this, party, &drop, &grade);

    bool loop = true;
    // ORIG 表头：mode∈{0,2} 走同一段通用表头；mode==3 金币表头；其余无表头。
    if (m_mode != 2)
    {
        if (m_mode == 3)
        {
            memcpy(fileBuf, "item_name\tgold_quantity\t drop_cnt\n", 0x23);
            goto header_done;
        }
        if (m_mode != 0)
        {
            goto header_done;
        }
    }
    memcpy(fileBuf,
           "item_id\t item_name\t rarity\t drop_cnt\t price\t sell_price\t usable_lv\n",
           0x44);
header_done:

    while (loop)
    {
        // ORIG while 体首部每轮重读 this+0x08（mode）。
        int mode = m_mode;
        if (mode == 2)
        {
            sprintf(fname, "passed_dungeon_drop_%d.txt", m_count);
            game_master_CBoosterTest_makeFileContents_Orig(user, fileBuf, &drop);
        }
        else if (mode == 3)
        {
            sprintf(fname, "gold_dungeon_drop_%d.txt", m_count);
            CMonsterDropTest_makeFileGoldContents_Orig(user, fileBuf, &drop);
        }
        else if (mode == 0)
        {
            sprintf(fname, "single_dungeon_drop_%d.txt", m_count);
            game_master_CBoosterTest_makeFileContents_Orig(user, fileBuf, &drop);
        }

        if (drop.empty())
        {
            loop = false;
        }
        game_master_CBoosterTest_printClient_Orig(user, loop, fname, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    if (grade.size() != 0)
    {
        std::string out;
        out.clear();
        for (std::map<int, std::map<int, int> >::iterator it = grade.begin();
             it != grade.end(); ++it)
        {
            for (int key = 0; key < 0xb; key++)
            {
                char line[1024];
                memset(line, 0, sizeof(line));
                std::map<int, int>::iterator f = it->second.find(key);
                if (f != it->second.end())
                {
                    sprintf(line, "%5d\t\t%2d\t\t%8d\n", it->first, f->first, f->second);
                    out += line;
                }
            }
            for (int key = 0x3e9; key < 0x3ee; key++)
            {
                char line[1024];
                memset(line, 0, sizeof(line));
                std::map<int, int>::iterator f = it->second.find(key);
                if (f != it->second.end())
                {
                    sprintf(line, "%5d\t\t%2d\t\t%8d\n", it->first, f->first, f->second);
                    out += line;
                }
            }
        }
        game_master_CBoosterTest_printClient_Orig(user, true,
                                                  "drop_grade_list.txt", out.c_str());
    }

    game_master_CBoosterTest_SendChatMsg_Orig(user, const_cast<char*>("Dungeon Monster Drop 결과체크가 완료되었습니다"));
}

// ===========================================================================
// NpcLimitBuyItem（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a885a：基类 ctor → 自身 vptr，无数据成员。

// ORIG parse 0x084b1f22：空实现。
void NpcLimitBuyItem::parse()
{
}

// ORIG execute 0x084b1f28：向监控代理发送 LimitNpcBuyItemRequestInfo
//（type 0x27d8 / len 10）请求限购信息重载。
void NpcLimitBuyItem::execute()
{
    CUser* user = GetUser();
    if (user != 0)
    {
        LimitNpcBuyItemRequestInfo req;
        ENUM_SERVER_GROUP group = user->GetServerGroup();
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
        proxy->SendPacket((char*)&req, 10);
    }
}

// ===========================================================================
// COneADayItemShop（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a868e：基类 ctor → 自身 vptr，无数据成员。

// ORIG execute 0x084b0610：G_GameWorld()->testResetOneADayItemShop()。
void COneADayItemShop::execute()
{
    G_GameWorld()->testResetOneADayItemShop();
}

// ===========================================================================
// CPcroomPremiumCmd（sizeof=0xc）
//   +0x08 int 时长（秒；atoi(args)，占位槽别名访问）
// ===========================================================================

// ORIG ctor 0x082a822e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084a9db0：trim(args)；+0x08 = atoi(args)。
void CPcroomPremiumCmd::parse()
{
    trim(m_parser->m_args);
    m_duration = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084a9de8：以当前系统秒为起点设置 PC 房 premium 区间
// [now, now+dur]，交 WongWork::CHandlePremium::handleSetUserPCRoom 生效。
void CPcroomPremiumCmd::execute()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int dur = m_duration;
    CUser* user = GetUser();
    WongWork::CHandlePremium::handleSetUserPCRoom(user, curSec,
                                                  static_cast<long>(dur) + curSec);
}

// ===========================================================================
// CPcroomRewardReset（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a89e2：基类 ctor → 自身 vptr，无数据成员。

// ORIG parse 0x084b3d12：仅读 m_parser 判存（mov/test/ret），无副作用。
void CPcroomRewardReset::parse()
{
    if (m_parser)
    {
        // ORIG 无分支体（test 后直接 ret）
    }
}

// ORIG execute 0x084b3d20：置 CUser+0x8ec32=4（每日奖励待更新态）、
// 发 DB 更新请求并回发查询包，最后日志 acc id。
void CPcroomRewardReset::execute()
{
    CUser* user = GetUser();
    user->m_field8ec32 = 4;
    DBUpdatePcroomDailyReward_makeRequest_Orig(user);
    InterSelectPcroomDailyReward::SendPacket(user);
    char* idStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x1852, 0);
    trace("[GMCommand] Pcroom daily reward reset m_id:%s", idStr);
}

// ===========================================================================
// CPickup（sizeof=0x10）
//   +0x08 int 参数一（atoi(args[0..space))）
//   +0x0c int 参数二（atoi(args[space+1..])；无头内占位名，别名访问）
//   注：ORIG execute 为空函数（0x084ac966 直接 ret）。
// ===========================================================================

// ORIG ctor 0x082a85e6：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084ac84c：trim；find(" ") 切两段分别 atoi 入 +0x08/+0x0c。
void CPickup::parse()
{
    trim(m_parser->m_args);
    size_t pos = m_parser->m_args.find(" ", 0);
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1);
    m_param1 = atoi(first.c_str());
    m_param2 = atoi(second.c_str());
}

// ORIG execute 0x084ac966：空实现。
void CPickup::execute()
{
}

// ===========================================================================
// CPowerWarStraightVictory（sizeof=0xc）
//   +0x08 short 连胜数（atoi(args) 截断为 16 位写入）
// ===========================================================================

// ORIG ctor 0x082a83c4：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084ab202：trim(args)；*(short*)(this+8) = (short)atoi(args)。
void CPowerWarStraightVictory::parse()
{
    trim(m_parser->m_args);
    m_straight = static_cast<short>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084ab23e：有队伍设队伍连胜，否则设个人（CUserCharacInfo）连胜。
void CPowerWarStraightVictory::execute()
{
    CUser* user = GetUser();
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        user->setStraightVictories(m_straight);
    }
    else
    {
        party->setStraightVictories(m_straight);
    }
}

// ===========================================================================
// CRandomObjectTest（sizeof=0x10）
//   +0x08 int mode（1=售货机统计，其余无动作）
//   +0x0c int count（统计次数，0 视作 10000；合法域 [0,100000]）
// ===========================================================================

// ORIG ctor 0x082a8656：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG clear 0x084b40ca：两 int 槽清零。
void CRandomObjectTest::clear()
{
    m_mode = 0;
    m_count = 0;
}

// ORIG parse 0x084b0032：clear；sscanf(args,"%d%d")。
void CRandomObjectTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d",
           &m_mode, &m_count);
}
void CRandomObjectTest_processVendingMachineItemLottery_Orig(
    void* self, CMTRand& rng, const stVendingMachineInfo_t* info, Inven_Item* item)
    asm("_ZN11game_master17CRandomObjectTest32processVendingMachineItemLotteryER7CMTRandPK22stVendingMachineInfo_tR10Inven_Item");

// ORIG checkCommandExecuteError 0x084b02be：count ∉ [0,100000] 报错。
// 头内占位声明为 void checkCommandExecuteError()，与 ORIG char* 返回不符；
// 以 asm 自由函数承载 ORIG 符号。
const char* CRandomObjectTest_checkCommandExecuteError_Orig(void* self)
    asm("_ZN11game_master17CRandomObjectTest24checkCommandExecuteErrorEv");
const char* CRandomObjectTest_checkCommandExecuteError_Orig(void* self)
{
    int count = static_cast<CRandomObjectTest*>(self)->m_count;
    if (count < 0x186a1 && count >= 0)
    {
        return 0;
    }
    return "0 ~ 100000번 통계까지만 지원됩니다";
}

// ORIG getVendingMachineInfo 0x084b0444：找 vending 表首个 m_id==1 条目中
// 首个 m_groupNum==1 的 info；找不到返回 0。同上以 asm 承载（头内占位 void）。
stVendingMachineInfo_t* CRandomObjectTest_getVendingMachineInfo_Orig(void* self)
    asm("_ZN11game_master17CRandomObjectTest21getVendingMachineInfoEv");
stVendingMachineInfo_t* CRandomObjectTest_getVendingMachineInfo_Orig(void* self)
{
    std::vector<stVendingMachine_t>* table =
        CDataManager_getVendingMachineTable_Orig(G_CDataManager());

    std::vector<stVendingMachine_t>::iterator mit = table->begin();
    while (true)
    {
        if (!(mit != table->end()) || mit->m_id == 1)
        {
            break;
        }
        ++mit;
    }

    // ORIG：外层即便因 end 退出仍取 m_infoList（迭代器失效路径原样保留）。
    std::vector<stVendingMachineInfo_t>::iterator iit = mit->m_infoList.begin();
    while (true)
    {
        if (!(iit != mit->m_infoList.end()))
        {
            return 0;
        }
        if (iit->m_groupNum == 1)
        {
            break;
        }
        ++iit;
    }
    return &*iit;
}

// ORIG getVendingMachineItem(map&) 0x084b02e6：以 time 种子 Mersenne 随机
// 模拟 count 次抽取，统计 itemId 出现次数。
void CRandomObjectTest_getVendingMachineItem_Orig(void* self,
                                                  std::map<int, int>* out)
    asm("_ZN11game_master17CRandomObjectTest21getVendingMachineItemERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");
void CRandomObjectTest_getVendingMachineItem_Orig(void* self,
                                                  std::map<int, int>* out)
{
    stVendingMachineInfo_t* info =
        CRandomObjectTest_getVendingMachineInfo_Orig(self);
    if (info != 0)
    {
        Inven_Item item;
        unsigned long seed = static_cast<unsigned long>(time(NULL));
        CMTRand rng(seed);
        for (int i = 0; i < static_cast<CRandomObjectTest*>(self)->m_count; i++)
        {
            item.m_addInfo = -1;
            CRandomObjectTest_processVendingMachineItemLottery_Orig(self, rng, info,
                                                                    &item);
            int key = item.m_addInfo;
            std::map<int, int>::iterator it = out->find(key);
            if (it == out->end())
            {
                out->insert(std::make_pair(key, 1));
            }
            else
            {
                it->second++;
            }
        }
    }
}

// ORIG processVendingMachineItemLottery(CMTRand&, info const*, Inven_Item&)
// 0x084b0550：在 [0, m_giveCount-1] 掷点，扫 m_vecGive 取首个累计权重
// 大于掷点的输出，把 itemId/m_count 写入物品 +0x02/+0x07。
void CRandomObjectTest_processVendingMachineItemLottery_Orig(
    void* self, CMTRand& rng, const stVendingMachineInfo_t* info, Inven_Item* item)
    asm("_ZN11game_master17CRandomObjectTest32processVendingMachineItemLotteryER7CMTRandPK22stVendingMachineInfo_tR10Inven_Item");
void CRandomObjectTest_processVendingMachineItemLottery_Orig(
    void* self, CMTRand& rng, const stVendingMachineInfo_t* info, Inven_Item* item)
{
    (void)self;
    unsigned long range = static_cast<unsigned long>(info->m_giveCount - 1);
    int roll = rng.randInt(range);

    for (std::vector<stVendingMachineOutput_t>::const_iterator it =
             info->m_vecGive.begin();
         it != info->m_vecGive.end(); ++it)
    {
        if (static_cast<unsigned int>(it->m_cumWeight) > static_cast<unsigned int>(roll))
        {
            item->m_addInfo = it->m_itemId;
            item->m_addInfo2 = static_cast<int>(it->m_count);
            return;
        }
    }
}

// ORIG execute 0x084b0082：校验后按 mode==1 跑售货机统计并分片落盘。
void CRandomObjectTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    const char* err = CRandomObjectTest_checkCommandExecuteError_Orig(this);
    if (err != 0)
    {
        game_master_CBoosterTest_SendChatMsg_Orig(user,
                                                  const_cast<char*>(err));
        return;
    }

    if (m_count == 0)
    {
        m_count = 10000;
    }
    char cnt[21];
    sprintf(cnt, "%d번 통계입니다", m_count);
    game_master_CBoosterTest_SendChatMsg_Orig(user, cnt);

    std::map<int, int> stat;
    if (m_mode == 1)
    {
        game_master_CBoosterTest_SendChatMsg_Orig(user, const_cast<char*>("vending machine 결과체크를 시작합니다"));
        CRandomObjectTest_getVendingMachineItem_Orig(this, &stat);

        char fileBuf[95000];
        memset(fileBuf, 0, sizeof(fileBuf));
        char fname[256];
        fname[0] = '\0';   // ORIG 循环前置零（memset@plt 口径）

        bool loop = true;
        while (loop)
        {
            game_master_CBoosterTest_makeFileContents_Orig(user, fileBuf, &stat);
            sprintf(fname, "etc_random_test_%d.txt", m_count);
            if (stat.empty())
            {
                loop = false;
            }
            game_master_CBoosterTest_printClient_Orig(user, loop, fname, fileBuf);
            memset(fileBuf, 0, sizeof(fileBuf));
        }

        game_master_CBoosterTest_SendChatMsg_Orig(user, const_cast<char*>("결과체크가 완료되었습니다"));
    }
}

// ===========================================================================
// CRecipeSuccess（sizeof=0xc）
//   +0x08 int 强制成功率档位（0=无 / 1=success / 2=fail）
// ===========================================================================

// ORIG ctor 0x082a8972：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084b399a：parser 非空时按 args[1] 文本映射 1/2/0。
void CRecipeSuccess::parse()
{
    if (m_parser != 0)
    {
        std::string& arg = m_parser->m_extra[1];
        if (arg == "success")
        {
            m_result = 1;
        }
        else if (arg == "fail")
        {
            m_result = 2;
        }
        else
        {
            m_result = 0;
        }
    }
}

// ORIG execute 0x084b3a34：GetUser()->recipeForceProbability(slot)。
void CRecipeSuccess::execute()
{
    CUser* user = GetUser();
    user->recipeForceProbability(m_result);
}

// ===========================================================================
// CRefund（sizeof=0x10）
//   +0x08 int 参数一 / +0x0c int 参数二（ORIG execute 为空函数，参数仅解析）
// ===========================================================================

// ORIG ctor 0x082a8602：基类 ctor → 自身 vptr；string@+0x08 默认构造。

// ORIG parse 0x084ac96c：trim；find(" ") 切两段分别 atoi 入 +0x08/+0x0c。
void CRefund::parse()
{
    trim(m_parser->m_args);
    size_t pos = m_parser->m_args.find(" ", 0);
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1);
    m_param1 = atoi(first.c_str());
    m_param2 = atoi(second.c_str());
}

// ORIG execute 0x084aca86：空实现。
void CRefund::execute()
{
}

} // namespace game_master