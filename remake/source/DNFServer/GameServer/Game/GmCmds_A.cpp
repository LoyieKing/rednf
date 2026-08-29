// GmCmds_A.cpp — game_master 命令体系批次 A（df_game_r ORIG T 符号族还原）。
// 负责类（11）：
//   CAutoMarketConditionsControlCmd ctor 0x082a8060 / execute 0x084a8ffc（空）
//   CAssaultAbuseUserCmd         ctor 0x082a8212 / execute 0x084a9daa（空）
//   CBloodMonsterListCmd         ctor 0x082a824a / execute 0x084a9e34
//   CAddItemPollutionCmd         ctor 0x082a82e4 / parse 0x084aa9c8 / execute 0x084aaa00
//   CAddItemByMail               ctor 0x082a853e / parse 0x084ac2aa / execute 0x084ac2e2
//   CAdjustCharacStat            ctor 0x082a855a / execute 0x084ac58c
//   CBoosterTest                 ctor 0x082a863a / parse 0x084aeb5e / execute 0x084aed04
//                                + 辅助 14 函数（getRarityStr/SendChatMsg/printServer/
//                                  printClient/getBoosterResult/getCompoundEmblemResult/
//                                  getJarResult/getRandomboxResult/get_gold_lottery_result/
//                                  is_gold_lottery_item/_selectJewelTypeIndex/
//                                  makeFileContents/make_file_gold_contents/clear）
//   CBindCubeTest                ctor 0x082a8672 / parse 0x084acbe4 / execute 0x084acc1e
//   BlueMarbleSet                ctor 0x082a88ae / parse 0x084b22ca / getParam 0x084b235c /
//                                execute 0x084b25a4
//   CBingoGmTest                 ctor 0x082a893a / parse 0x084b35c4 / execute 0x084b35fc /
//                                resetData 0x084b363e / reqValue 0x084b368a
// 对照 docs/class_func_reports/game_master__<Class>/{README,parse,execute,...}.md；
// 报告缺失的 CBoosterTest::clear 由 objdump 0x084b40b0 直转。
//
// 头文件占位字段失配说明（GameMasterCmd.h 本批禁改）：占位 m_str0(string@+0x08)
// 与下列真实标量布局同尺寸（gcc4.4 32 位 std::string 仅 4 字节），本 TU 经偏移访问：
//   CAddItemByMail/CAddItemPollutionCmd/CBingoGmTest  +0x08 int
//   CBoosterTest                                      +0x08 int +0x0c int
//   CBindCubeTest                                     +0x08 byte（signed 使用）
//   BlueMarbleSet                                     +0x08 byte +0x09 byte +0x0a byte
//                                                     +0x0c int（超出占位的部分同样偏移访问）
//   CBillingRechargeCommand                           m_str0(+0x08) 真为 string +
//                                                     +0x0c int（金额）
//
// 头文件辅助函数占位签名失配说明：GameMasterCmd.h 对各辅助统一给出无参 `void f();`
// 占位声明，但 ORIG 真实签名带参（见报告）。本批不能改头，故带参辅助一律以
// 自由函数 + asm 标签对齐 ORIG mangled 实现（32 位 O0 下非虚 thiscall 与自由函数
// ABI 一致：this 即首栈参；静态成员本就不含 this）。头中无参占位声明保持
// 未定义、未被调用，不产生代码与符号。
//   注：CBingoGmTest 的 +0x08 在头中为占位 string，ORIG 真实为 int 子命令码
//   （ctor 不初始化；本 TU 因头声明 string 成员而随编译器生成其构造，差异仅限
//   构造期清零，parse 后行为一致）。reqValue/resetData 按头中无参声明实现。

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "Inven_Item.h"
#include "CItemAmplifier.h"
#include "CMailBoxHelper.h"
#include "CHandlePremium.h"
#include "RDARScriptStringManager.h"
#include "CSystemTime.h"
#include "CBattle_Field.h"
#include "CInventory.h"
#include "CItemList.h"
#include "CStackableItem.h"
#include "CompoundEmblemDeps.h"
#include "BlueMarble.h"

// CGameManager::GetParty(int) —— T 0x08294f32（CGameManager.h 会拉入 QuickParty
// 冲突链，此处以 asm 直调替代整头包含）；G_CGameManager() —— W 0x080cc18e。
class CParty;
CParty* gmA_G_CGameManager_GetParty(void* self, int partyIndex)
    asm("_ZN12CGameManager8GetPartyEi");
class CGameManager;
CGameManager* gmA_G_CGameManager()
    asm("_Z14G_CGameManagerv");

#include "DNFFunctionLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// ORIG 数据符号（ELF .rodata 指针表，仓库尚无声明）。
extern char* CHARACTER_JOB_NAMES[];
extern char* BIND_NAMES[];
extern char* RECIPE_NAMES[];
extern char* EQUIP_NAMES[];

// ---------------------------------------------------------------------------
// BingoEvent / CBingo（ORIG 全局命名空间类；source 尚无定义 TU，仅按符号原型
// 声明所需成员。mangled 由最小类定义自然导出）。
class BingoEvent
{
public:
    void* getBingoSystem();                        // ORIG T 0x080c9fea：返回 boost::shared_ptr<CBingo> 实例地址
    void sendOneMatchLineReward(CUser& user);      // ORIG T 0x080ca3d4
};
class CBingo
{
public:
    int calNewValue(unsigned int& board, int idx); // ORIG T 0x080cad4c
};

namespace game_master
{

namespace
{

// ---- 占位字段槽访问（见文件头布局说明）----
inline int& cmd_int0(void* cmd)
{
    return *reinterpret_cast<int*>(static_cast<char*>(cmd) + 0x08);
}
inline int& cmd_int1(void* cmd)
{
    return *reinterpret_cast<int*>(static_cast<char*>(cmd) + 0x0c);
}
inline unsigned char& cmd_byte0(void* cmd)
{
    return *reinterpret_cast<unsigned char*>(static_cast<char*>(cmd) + 0x08);
}

} // namespace

// ---------------------------------------------------------------------------
// ORIG 真实符号原型（带参辅助；asm 标签对齐 ORIG mangled。本 TU 提供定义，
// 头文件中的无参占位声明保持未定义、未被调用）。

// CBoosterTest::getRarityStr(ENUM_RARITY) —— T 0x084aea80（static 语义：不触碰 this）
char const* gmA_booster_getRarityStr(ENUM_RARITY rarity)
    asm("_ZN11game_master12CBoosterTest12getRarityStrE11ENUM_RARITY");

// CBoosterTest::SendChatMsg(CUser*, char*) —— T 0x084aeac6（static 语义）
void gmA_booster_SendChatMsg(CUser* user, char* msg)
    asm("_ZN11game_master12CBoosterTest11SendChatMsgEP5CUserPc");


// CBoosterTest::is_gold_lottery_item(int) —— T 0x084afccc（thiscall；this 未使用仍按 ABI 传递）
bool gmA_booster_is_gold_lottery_item(CBoosterTest* self, int idx)
    asm("_ZN11game_master12CBoosterTest20is_gold_lottery_itemEi");

// CBoosterTest::getJarResult(CItem const*, std::map<int,int>&) —— T 0x084afba8
void gmA_booster_getJarResult(CBoosterTest* self, CItem const* item,
                              std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest12getJarResultEPK5CItemRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// CBoosterTest::get_gold_lottery_result(CItem const*, std::map<int,int>&) —— T 0x084afd1c
void gmA_booster_get_gold_lottery_result(CBoosterTest* self, CItem const* item,
                                         std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest23get_gold_lottery_resultEPK5CItemRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// CBoosterTest::getBoosterResult(CItem const*, std::map<int,int>&) —— T 0x084af9d6
void gmA_booster_getBoosterResult(CBoosterTest* self, CItem const* item,
                                  std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest16getBoosterResultEPK5CItemRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// CBoosterTest::getRandomboxResult(CItem const*, std::map<int,int>&) —— T 0x084afe3a
void gmA_booster_getRandomboxResult(CBoosterTest* self, CItem const* item,
                                    std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest18getRandomboxResultEPK5CItemRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// CBoosterTest::getCompoundEmblemResult(CItem const*, std::map<int,int>&) —— T 0x084af0d6
bool gmA_booster_getCompoundEmblemResult(CBoosterTest* self, CItem const* item,
                                         std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest23getCompoundEmblemResultEPK5CItemRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// CBoosterTest::_selectJewelTypeIndex(unsigned int, unsigned short const*) const —— T 0x084af3b2
int gmA_booster__selectJewelTypeIndex(CBoosterTest const* self, unsigned int count,
                                      unsigned short const* sockets)
    asm("_ZNK11game_master12CBoosterTest21_selectJewelTypeIndexEjPKt");

// CBoosterTest::makeFileContents(CUser*, char*, std::map<int,int>&) —— T 0x084af504（static 语义）
void gmA_booster_makeFileContents(CUser* user, char* fileBuf, std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest16makeFileContentsEP5CUserPcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// CBoosterTest::make_file_gold_contents(CUser*, char*, std::map<int,int>&) —— T 0x084af7fc（static 语义）
void gmA_booster_make_file_gold_contents(CUser* user, char* fileBuf, std::map<int, int>& result)
    asm("_ZN11game_master12CBoosterTest23make_file_gold_contentsEP5CUserPcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// BlueMarbleSet::getParam(std::string) —— T 0x084b235c（thiscall，string 按值传参）
void gmA_blueMarbleSet_getParam(BlueMarbleSet* self, std::string param)
    asm("_ZN11game_master13BlueMarbleSet8getParamESs");

// CEventManager::GetRepeatEvent(int) —— T 0x08115998（GlobalData.h 仅前向声明）
void* gmA_eventMgr_GetRepeatEvent(void* mgr, int type)
    asm("_ZN13CEventManager14GetRepeatEventEi");

// WongWork::CCompoundAvatar::ProcTest(ENUM_CHARACTERJOB, ENUM_EQUIPMENTTYPE, int, int,
//     std::map<int,int>&) —— T 0x08333fda（static；CCompoundAvatar.h 缺声明）
void gmA_compoundAvatar_ProcTest(void* avatar, int job, int equip, int recipe, int bind,
                                 std::map<int, int>& result)
    asm("_ZN8WongWork15CCompoundAvatar8ProcTestE17ENUM_CHARACTERJOB18ENUM_EQUIPMENTTYPEiiRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE");

// ===========================================================================
// CAutoMarketConditionsControlCmd — ORIG W ctor 0x082a8060：基类 vptr → 自身
// vptr(0x8c81c58)，无字段（sizeof=0x8）。
// ===========================================================================
CAutoMarketConditionsControlCmd::CAutoMarketConditionsControlCmd()
{
}

// ORIG execute 0x084a8ffc：ret（纯空实现）。
void CAutoMarketConditionsControlCmd::execute()
{
}

// ===========================================================================
// CAssaultAbuseUserCmd — ORIG W ctor 0x082a8212：基类 vptr → 自身 vptr(0x8c81ae0)。
// ===========================================================================
CAssaultAbuseUserCmd::CAssaultAbuseUserCmd()
{
}

// ORIG execute 0x084a9daa：ret（纯空实现）。
void CAssaultAbuseUserCmd::execute()
{
}

// ===========================================================================
// CBloodMonsterListCmd — ORIG W ctor 0x082a824a：基类 vptr → 自身 vptr(0x8c81ab0)。
// ===========================================================================
CBloodMonsterListCmd::CBloodMonsterListCmd()
{
}

// ORIG execute 0x084a9e34：仅地下城状态(state==5)且有队伍时，把当前地图怪物
// uid 表拼成文本经 0x0c 号包回发给 GM。
void CBloodMonsterListCmd::execute()
{
    CUser* user = GetUser();
    if (user->get_state() != 5)
    {
        return;
    }

    short partyIndex = user->GetPartyIndex();
    CParty* party = gmA_G_CGameManager_GetParty(gmA_G_CGameManager(),
                                                static_cast<int>(partyIndex));
    if (party == 0)
    {
        return;
    }

    std::string message("monster list : ");
    char buf[0xff];
    memset(buf, 0, sizeof(buf));

    // CBattle_Field 为 CParty+0xb24 嵌入子对象（CParty.h 占位，全项目同口径视图）。
    // ORIG 0x84a9e??：GetMapInfo() 返回 MapInfo*，取其 +0x0c m_monsterMap。
    CBattle_Field* battleField =
        reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
    std::map<int, map_monster>* monsters = &battleField->GetMapInfo()->m_monsterMap;

    for (std::map<int, map_monster>::iterator it = monsters->begin();
         it != monsters->end(); ++it)
    {
        sprintf(buf, "(%d) ", it->first);
        message += buf;
    }

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(message.size()));
    packet.put_str(const_cast<char*>(message.c_str()), static_cast<int>(message.size()));
    packet.finalize(true);
    GetUser()->Send(packet);
}

// ===========================================================================
// CAddItemPollutionCmd — ORIG W ctor 0x082a82e4：基类 vptr → 自身 vptr(0x8c81a20)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================
CAddItemPollutionCmd::CAddItemPollutionCmd()
    : m_str0()
{
}

// ORIG parse 0x084aa9c8：trim(args)；+0x08 = atoi(args.c_str())。
void CAddItemPollutionCmd::parse()
{
    trim(m_parser->m_args);
    cmd_int0(this) = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084aaa00：itemId==-1 先开通 PC 房特权；随后发放物品并在
// 无强化属性时随机投一次强化投资，最后日志 + 物品更新包。
void CAddItemPollutionCmd::execute()
{
    if (cmd_int0(this) == -1)
    {
        int start = GlobalData::s_systemTime_.getCurSec();
        int end = GlobalData::s_systemTime_.getCurSec();
        WongWork::CHandlePremium::handleSetUserPCRoom(GetUser(),
                                                      static_cast<long>(start),
                                                      static_cast<long>(end) + 0x15180);
    }

    int limit = 100;
    CStackableItem* stackable =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(cmd_int0(this)));
    if (stackable != 0 && stackable->is_stackable())
    {
        if (stackable->getStackableLimit() < limit)
        {
            limit = stackable->getStackableLimit();
        }
    }

    ENUM_ITEMSPACE space;
    CUser* user = GetUser();
    int slot = user->AddItem(cmd_int0(this), limit, static_cast<eItemAddReason>(6),
                             space, 0);
    if (slot >= 0)
    {
        int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
        CInventory* inven = user->getCurCharacInvenW();
        Inven_Item* ref = inven->GetInvenRef(invenType, slot);
        if (ref != 0)
        {
            if (ref->m_amp.getAbilityType() == 0)
            {
                int type = get_rand_int(4) + 1;
                if (0 < type && type < 5)
                {
                    stInvestAmplifyOption_t invest;
                    invest.m_amount = type;
                    GlobalData::s_itemAmplifier_->investAmplifyOption(stackable, *ref,
                                                                      invest);
                    unsigned char abilityType = 0;
                    unsigned short abilityValue = 0;
                    ref->m_amp.getValues(abilityType, abilityValue);
                    ref->m_amp.assign(abilityType | 0x80, abilityValue);
                }
            }
            if (slot >= 0)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__, 0x5b0, 0);
                trace("%s:'%s' GM Mode Add Item(%d).",
                      NumberToString(user->get_acc_id(), 0),
                      user->getCurCharacName(), slot);
            }
            user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1), space, slot);
        }
    }
}

// ===========================================================================
// CAddItemByMail — ORIG W ctor 0x082a853e：基类 vptr → 自身 vptr(0x8c81840)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================
CAddItemByMail::CAddItemByMail()
    : m_str0()
{
}

// ORIG parse 0x084ac2aa：trim(args)；+0x08 = atoi(args.c_str())。
void CAddItemByMail::parse()
{
    trim(m_parser->m_args);
    cmd_int0(this) = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084ac2e2：合法物品以系统邮件发放（0 号特例 count=1，1 号特例
// 金币条目手工填充，其余走 CItem::make_item 虚拷贝并附加 add_info=1）。
void CAddItemByMail::execute()
{
    CUser* user = GetUser();
    int itemId = cmd_int0(this);
    if (G_CDataManager()->find_item(itemId) == 0)
    {
        return;
    }

    char title[0x100];
    memset(title, 0, sizeof(title));
    char sender[0x15];
    memset(sender, 0, sizeof(sender));
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_144", 0),
            0xff);
    strncpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_143", 0),
            0x14);

    int addInfo = 1;
    int count = 0;
    Inven_Item item;
    if (itemId == 0)
    {
        count = addInfo;
    }
    else if (itemId == 1)
    {
        item.m_addInfo = 1;          // +0x02
        item.m_field1 = 2;           // +0x01
        item.m_addInfo2 = addInfo;   // +0x07
        item.m_fieldb = 1;           // +0x0b
        item.ResetItemAttr();
    }
    else
    {
        CItem* proto = G_CDataManager()->find_item(itemId);
        if (proto == 0)
        {
            return;
        }
        item.m_addInfo = itemId;     // +0x02
        proto->make_item(item);      // ORIG vtbl[2]
        item.set_add_info(addInfo);
    }

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, static_cast<unsigned int>(count),
        static_cast<unsigned int>(user->getCurCharacNo()), title,
        static_cast<int>(strlen(title)), 0u, user->GetServerGroup(), false, false);
}

// ===========================================================================
// CAdjustCharacStat — ORIG W ctor 0x082a855a：基类 vptr → 自身 vptr(0x8c81828)。
// ===========================================================================
CAdjustCharacStat::CAdjustCharacStat()
{
}

// ORIG execute 0x084ac58c：重算角色属性后回发 basic_info 包。
void CAdjustCharacStat::execute()
{
    CUser* user = GetUser();
    user->adjust_charac_stat();   // 真实定义 UserPartyLink4.cpp（T 0x08664766）

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 1);
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// CBoosterTest — ORIG W ctor 0x082a863a：基类 vptr → 自身 vptr(0x8c81750)。
// 布局：+0x08 int 物品索引（clear 置 -1）、+0x0c int 统计次数。
// ===========================================================================
CBoosterTest::CBoosterTest()
{
}

// ORIG parse 0x084aeb5e：clear() 后 sscanf(args,"%d%d",&+0x08,&+0x0c)。
void CBoosterTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d", &cmd_int0(this), &cmd_int1(this));
}

// ORIG clear 0x084b40b0（报告缺失，objdump 直转）：+0x08=-1、+0x0c=0。
void CBoosterTest::clear()
{
    cmd_int0(this) = -1;
    cmd_int1(this) = 0;
}

// ORIG execute 0x084aed04：LOTTERY 类物品抽样统计并把结果文件内容分包回显。
void CBoosterTest::execute()
{
    CUser* user = GetUser();

    char fileName[0xff];
    memset(fileName, 0, sizeof(fileName));
    char fileBuf[95000];
    memset(fileBuf, 0, sizeof(fileBuf));

    static char sMsgStart[] = "LOTTERY 아이템 결과체크를 시작합니다";
    gmA_booster_SendChatMsg(user, sMsgStart);

    if (cmd_int0(this) == -1)
    {
        static char sNeedIdx[] = "아이템 인덱스를 입력하세요";
        gmA_booster_SendChatMsg(user, sNeedIdx);
        return;
    }

    CItem* item = G_CDataManager()->find_item(cmd_int0(this));
    if (item == 0)
    {
        static char sNoItem[] = "존재하지 않는 아이템입니다";
        gmA_booster_SendChatMsg(user, sNoItem);
        return;
    }

    if (cmd_int1(this) > 1000000 || cmd_int1(this) < 0)
    {
        static char sRange[] = "0 ~ 1000000번 통계까지만 지원됩니다";
        gmA_booster_SendChatMsg(user, sRange);
        return;
    }
    if (cmd_int1(this) == 0)
    {
        cmd_int1(this) = 10000;
    }

    char countMsg[21];
    sprintf(countMsg, "%d번 통계입니다", cmd_int1(this));
    gmA_booster_SendChatMsg(user, countMsg);

    bool goldLottery = gmA_booster_is_gold_lottery_item(this, item->get_index());

    std::map<int, int> result;
    switch (item->GetItemType())                       // ORIG vtbl[3]
    {
    case 6:
    case 7:
    case 0xd:
        if (!goldLottery)
        {
            gmA_booster_getJarResult(this, item, result);
        }
        else
        {
            gmA_booster_get_gold_lottery_result(this, item, result);
        }
        break;
    case 0xf:
    case 0x10:
        gmA_booster_getBoosterResult(this, item, result);
        break;
    case 0x14:
        if (!gmA_booster_getCompoundEmblemResult(this, item, result))
        {
            static char sNotEmblem[] = "엠블렘 합성 대상이 아닙니다.";
            gmA_booster_SendChatMsg(GetUser(), sNotEmblem);
        }
        break;
    case 0x1a:
    case 0x1b:
        gmA_booster_getRandomboxResult(this, item, result);
        break;
    default:
    {
        static char sNotLottery[] = "LOTTERY 아이템이 아닙니다";
        gmA_booster_SendChatMsg(GetUser(), sNotLottery);
        return;
    }
    }

    bool more = true;
    memcpy(fileBuf, "item_id\titem_name\trarity\titem_cnt\tprice\tsell_price\tusable_lv\n",
           0x3e);
    while (more)
    {
        if (!goldLottery)
        {
            gmA_booster_makeFileContents(user, fileBuf, result);
        }
        else
        {
            gmA_booster_make_file_gold_contents(user, fileBuf, result);
        }
        sprintf(fileName, "lottery_%d_%d.txt", cmd_int0(this), cmd_int1(this));
        if (result.empty())
        {
            more = false;
        }
        CBoosterTest::printClient(user, more, fileName, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    static char sDone[] = "LOTTERY 아이템 결과체크가 완료되었습니다";
    gmA_booster_SendChatMsg(user, sDone);
}

// ---- CBoosterTest 辅助函数（ORIG 逐符号转写）------------------------------

// ORIG getRarityStr 0x084aea80：稀有度 → 文件列名。
char const* gmA_booster_getRarityStr(ENUM_RARITY rarity)
{
    switch (rarity)
    {
    case RARITY_COMMON:    return "common";
    case RARITY_UNCOMMON:  return "uncommon";
    case RARITY_RARE:      return "rare";
    case RARITY_UNIQUE:    return "unique";
    case RARITY_EPIC:      return "epic";
    case RARITY_CHRONICLE: return "chronicle";
    default:               return "unknown";
    }
}

// ORIG SendChatMsg 0x084aeac6：make_packet_chat_msg(type=2,len=12) + Send。
void gmA_booster_SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    GameWorld* gw = G_GameWorld();
    gw->make_packet_chat_msg(packet, user, msg, static_cast<int>(strlen(msg)),
                             2, static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
}

// ORIG printServer 0x084aebae：fopen(path,"wt") + fputs + fclose。
void CBoosterTest::printServer(char const* path, char const* contents)
{
    FILE* fp = fopen(path, "wt");
    if (fp == 0)
    {
        goto done;
    }
    fputs(contents, fp);
    fclose(fp);
done:
    return;
}

// ORIG printClient 0x084aebf2：0x13e 包（more 标志 + 双字符串）。
void CBoosterTest::printClient(CUser* user, bool more, char const* name, char const* contents)
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(more);
    packet.put_int(static_cast<int>(strlen(name)));
    packet.put_str(const_cast<char*>(name), static_cast<int>(strlen(name)));
    packet.put_int(static_cast<int>(strlen(contents)));
    packet.put_str(const_cast<char*>(contents), static_cast<int>(strlen(contents)));
    packet.finalize(true);
    user->Send(packet);
}

// ORIG is_gold_lottery_item 0x084afccc：lotto 表项 id(+2)==0 且数量(+7)>0。
bool gmA_booster_is_gold_lottery_item(CBoosterTest* /*self*/, int idx)
{
    Inven_Item lotto;
    G_CDataManager()->getItemList()->get_lotto_item(idx, reinterpret_cast<char*>(&lotto));
    return lotto.m_addInfo == 0 &&
           lotto.m_addInfo2 > 0;
}

// ORIG getJarResult 0x084afba8：统计 lotto 表产出物品次数。
void gmA_booster_getJarResult(CBoosterTest* self, CItem const* item,
                              std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < cmd_int1(self); ++i)
    {
        int idx = const_cast<CItem*>(item)->get_index();
        G_CDataManager()->getItemList()->get_lotto_item(idx, reinterpret_cast<char*>(&lotto));
        int itemId = lotto.m_addInfo;
        std::map<int, int>::iterator pos = result.find(itemId);
        if (pos == result.end())
        {
            result.insert(std::make_pair(itemId, 1));
        }
        else
        {
            pos->second = pos->second + 1;
        }
    }
}

// ORIG get_gold_lottery_result 0x084afd1c：统计金币产出（key=+7 数量，value=1）。
void gmA_booster_get_gold_lottery_result(CBoosterTest* self, CItem const* item,
                                         std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < cmd_int1(self); ++i)
    {
        int idx = const_cast<CItem*>(item)->get_index();
        G_CDataManager()->getItemList()->get_lotto_item(idx, reinterpret_cast<char*>(&lotto));
        int goldCount = lotto.m_addInfo2;
        std::map<int, int>::iterator pos = result.find(goldCount);
        if (pos == result.end())
        {
            result.insert(std::make_pair(goldCount, 1));
        }
        else
        {
            pos->second = pos->second + 1;
        }
    }
}

// ORIG getBoosterResult 0x084af9d6：统计 booster 产出（stStackableBoosterItemInfo_t）。
void gmA_booster_getBoosterResult(CBoosterTest* self, CItem const* item,
                                  std::map<int, int>& result)
{
    std::vector<stStackableBoosterItemInfo_t> items;
    for (int i = 0; i < cmd_int1(self); ++i)
    {
        static_cast<CStackableItem const*>(item)->getBoosterItem(items);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
             it != items.end(); ++it)
        {
            int itemId = it->m_itemIdx;
            std::map<int, int>::iterator pos = result.find(itemId);
            if (pos == result.end())
            {
                result.insert(std::make_pair(itemId, 1));
            }
            else
            {
                pos->second = pos->second + 1;
            }
        }
        items.clear();
    }
}

// ORIG getRandomboxResult 0x084afe3a：统计 multibox 随机盒产出（pair<Inven_Item,bool>）。
void gmA_booster_getRandomboxResult(CBoosterTest* self, CItem const* item,
                                    std::map<int, int>& result)
{
    Inven_Item dropped;
    std::vector<std::pair<Inven_Item, bool> > drops;
    for (int i = 0; i < cmd_int1(self); ++i)
    {
        drops.clear();
        const_cast<CStackableItem*>(static_cast<CStackableItem const*>(item))->get_multibox_lottery_item(&drops);
        for (unsigned int j = 0; j < drops.size(); ++j)
        {
            memcpy(&dropped, &drops[j].first, sizeof(Inven_Item));  // ORIG 逐字段浅拷贝
            int itemId = dropped.m_addInfo;
            std::map<int, int>::iterator pos = result.find(itemId);
            if (pos == result.end())
            {
                result.insert(std::make_pair(itemId, 1));
            }
            else
            {
                pos->second = pos->second + 1;
            }
        }
    }
}

// ORIG getCompoundEmblemResult 0x084af0d6：合成徽章统计；目标不是 16 型物品返回 false。
bool gmA_booster_getCompoundEmblemResult(CBoosterTest* self, CItem const* item,
                                         std::map<int, int>& result)
{
    unsigned short sockets[2];
    sockets[0] = static_cast<CStackableItem const*>(item)->getJewelTargetSocket();
    sockets[1] = static_cast<CStackableItem const*>(item)->getJewelTargetSocket();

    int jewelType = gmA_booster__selectJewelTypeIndex(self, 2, sockets);
    if (jewelType == -1)
    {
        return false;
    }

    int grade = item->get_grade();
    // 徽章映射表：CDataManager + (grade*6+jewelType)*4 + 0x82c（Ghidra C 口径）。
    int emblemId = *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(G_CDataManager()) + (grade * 6 + jewelType) * 4 + 0x82c);

    CStackableItem const* emblem =
        static_cast<CStackableItem const*>(G_CDataManager()->find_item(emblemId));
    if (emblem == 0 || emblem->GetItemType() != 0x10)
    {
        return false;
    }

    for (int i = 0; i < cmd_int1(self); ++i)
    {
        std::vector<stStackableBoosterItemInfo_t> items;
        emblem->getBoosterItem(items);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
             it != items.end(); ++it)
        {
            if (G_CDataManager()->find_item(it->m_itemIdx) == 0)
            {
                continue;
            }
            int key = it->m_itemIdx;
            std::map<int, int>::iterator pos = result.find(key);
            if (pos == result.end())
            {
                result[key] = static_cast<int>(it->m_addInfo);
            }
            else
            {
                result[key] = pos->second + static_cast<int>(it->m_addInfo);
            }
        }
    }
    return true;
}

// ORIG _selectJewelTypeIndex 0x084af3b2：按凹槽数据加权随机选宝石类型槽。
int gmA_booster__selectJewelTypeIndex(CBoosterTest const* /*self*/, unsigned int count,
                                      unsigned short const* sockets)
{
    int counts[5] = {0, 0, 0, 0, 0};
    int cumul[5];
    int total = 0;

    for (unsigned int i = 0; i < count; ++i)
    {
        if (sockets[i] != 0)
        {
            getAvatarEmblemTypeIdxForCompountEmblem(sockets[i], counts, total);
        }
    }
    if (total == 0)
    {
        return -1;
    }

    int running = 0;
    for (int i = 0; i <= 4; ++i)
    {
        int weight = counts[i] * (10000 / total);
        cumul[i] = weight + running;
        running += weight;
    }

    int picked = get_rand_int(running);
    for (int i = 0; i <= 4; ++i)
    {
        if (picked < cumul[i])
        {
            return i;
        }
    }
    return -1;
}

// ORIG makeFileContents 0x084af504：map 结果追加为 TSV 行（超限提示续接并中止）。
void gmA_booster_makeFileContents(CUser* user, char* fileBuf, std::map<int, int>& result)
{
    char line[1024];
    memset(line, 0, sizeof(line));

    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        int itemId = it->first;
        CItem* info = G_CDataManager()->find_item(itemId);
        if (itemId == 0)
        {
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", itemId, "gold", "unknown", it->second);
        }
        else if (itemId == 1)
        {
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", itemId, "coin", "unknown", it->second);
        }
        else if (info == 0)
        {
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\n", itemId, "unknown", "unknown",
                    it->second);
        }
        else
        {
            sprintf(line, "%d\t\"%s\"\t\"%s\"\t%u\t%d\t%d\t%d\n",
                    itemId, info->GetItemName(),
                    gmA_booster_getRarityStr(static_cast<ENUM_RARITY>(info->get_rarity())),
                    it->second, info->get_price(), info->GetSellPrice(),
                    info->getUsableLevel());
        }

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "결과 파일 이어붙힘";
            gmA_booster_SendChatMsg(user, sAppend);
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }
}

// ORIG make_file_gold_contents 0x084af7fc：金币分布行 + 尾部 max gold 合计。
// （格式串 %I64u 按 ORIG 原样保留；glibc 下由 %llu 语义承载 64 位实参。）
void gmA_booster_make_file_gold_contents(CUser* user, char* fileBuf,
                                         std::map<int, int>& result)
{
    char line[1024];
    memset(line, 0, sizeof(line));

    unsigned long long maxGold = 0;
    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        sprintf(line, "%d gold count = %d \n", it->first, it->second);
        unsigned int gold = static_cast<unsigned int>(it->first) * it->second;
        maxGold += gold;

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "결과 파일 이어붙힘";
            gmA_booster_SendChatMsg(user, sAppend);
            break;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }

    sprintf(line, "max gold = %llu \n", maxGold);
    strcat(fileBuf, line);
}

// ===========================================================================
// CBindCubeTest — ORIG W ctor 0x082a8672：基类 vptr → 自身 vptr(0x8c81780)，
// string@+0x08 默认构造（占位；真实 +0x08 为 byte 职业）。
// ===========================================================================
CBindCubeTest::CBindCubeTest()
    : m_str0()
{
}

// ORIG parse 0x084acbe4：trim(args)；+0x08 = (byte)atoi(args.c_str())。
void CBindCubeTest::parse()
{
    trim(m_parser->m_args);
    cmd_byte0(this) = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084acc1e：职业 0..10 时遍历 bind×recipe×equip 组合跑合成测试，
// 每组结果落一份 bind_<job>_<bind>_<recipe>_<equip>.txt 回显给 GM。
void CBindCubeTest::execute()
{
    if (!(static_cast<signed char>(cmd_byte0(this)) >= 0 &&
          static_cast<signed char>(cmd_byte0(this)) <= 10))
    {
        return;
    }

    char fileName[0xff];
    memset(fileName, 0, sizeof(fileName));
    char fileBuf[95000];
    memset(fileBuf, 0, sizeof(fileBuf));

    std::map<int, int> result;
    for (int bind = 0; bind <= 1; ++bind)
    {
        for (int recipe = 0; recipe <= 2; ++recipe)
        {
            for (int equip = 0; equip <= 7; ++equip)
            {
                memset(fileName, 0, 0xff);
                memset(fileBuf, 0, 95000);
                gmA_compoundAvatar_ProcTest(GlobalData::s_pCompoundAvatar,
                                            static_cast<int>(static_cast<signed char>(
                                                cmd_byte0(this))),
                                            equip, recipe, bind, result);
                gmA_booster_makeFileContents(GetUser(), fileBuf, result);
                sprintf(fileName, "bind_%s_%s_%s_%s.txt",
                        CHARACTER_JOB_NAMES[static_cast<int>(
                            static_cast<signed char>(cmd_byte0(this)))],
                        BIND_NAMES[bind], RECIPE_NAMES[recipe], EQUIP_NAMES[equip]);
                CBoosterTest::printClient(GetUser(), false, fileName, fileBuf);
                result.clear();
            }
        }
    }
}

// ===========================================================================
// BlueMarbleSet — ORIG W ctor 0x082a88ae：基类 vptr → 自身 vptr(0x8c81588)。
// 布局：+0x08 byte 步数、+0x09 byte move 标记、+0x0a byte enter 重置标记、
//       +0x0c int token 计数（getParam 工作区）。
// ===========================================================================
BlueMarbleSet::BlueMarbleSet()
    : m_str0()
{
}

// ORIG parse 0x084b22ca：三标记清零、token 计数清零，trim 后整串交给 getParam。
void BlueMarbleSet::parse()
{
    cmd_byte0(this) = 0;
    *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(this) + 0x09) = 0;
    *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(this) + 0x0a) = 0;
    cmd_int1(this) = 0;

    trim(m_parser->m_args);
    std::string args(m_parser->m_args);
    gmA_blueMarbleSet_getParam(this, args);
}

// ORIG getParam 0x084b235c：按空格切 token（≤16），tokens[0]=="move"/"enter" 置
// 对应标记，atoi(tokens[1]) 写步数字节。
void gmA_blueMarbleSet_getParam(BlueMarbleSet* self, std::string param)
{
    std::string tokens[16];
    int used = 0;
    int pos = 0;
    while ((pos = static_cast<int>(param.find(' ', 0))) != -1)
    {
        tokens[used] = param.substr(0, pos);
        param = param.substr(pos + 1, std::string::npos);
        ++used;
    }
    tokens[used] = param;

    if (tokens[0].compare("move") == 0)
    {
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(self) + 0x09) = 1;
    }
    else if (tokens[0].compare("enter") == 0)
    {
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(self) + 0x0a) = 1;
    }
    cmd_byte0(self) = static_cast<unsigned char>(atoi(tokens[1].c_str()));
}

// ORIG execute 0x084b25a4：enter 标记优先重置入场次数；否则 move 标记 + 步数
// 有效时驱动大富翁移动。
void BlueMarbleSet::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    if (*reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(this) + 0x0a) != 0)
    {
        user->resetBlueMarbleEnterCount();
        user->sendBlueMarbleEnterCount();
        return;
    }

    BlueMarble* blueMarble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (blueMarble == 0)
    {
        return;
    }
    if (*reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(this) + 0x09) == 0)
    {
        return;
    }
    if (cmd_byte0(this) == 0)
    {
        return;
    }
    blueMarble->movePosition(user, static_cast<int>(cmd_byte0(this)));
}

// ===========================================================================
// CBingoGmTest — ORIG W ctor 0x082a893a：基类 vptr → 自身 vptr(0x8c81510)，
// 无成员初始化（+0x08 int 未初始化，ORIG 如此）。类声明见 GameMasterCmd.h
// （占位 std::string m_str0@+0x08，真实为 int 子命令码，本 TU 偏移访问）。
// ===========================================================================

CBingoGmTest::CBingoGmTest()
{
}

// ORIG parse 0x084b35c4：trim(args)；+0x08 = atoi(args.c_str())。
void CBingoGmTest::parse()
{
    trim(m_parser->m_args);
    cmd_int0(this) = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084b35fc：user 有效时按子命令分流。
void CBingoGmTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    if (cmd_int0(this) == 0)
    {
        resetData();
    }
    else
    {
        reqValue();
    }
}

// ORIG resetData 0x084b363e：清 bingo 数据、置脏并发送全量数据。
void CBingoGmTest::resetData()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    BingoData* data = static_cast<BingoData*>(user->getBingoData());
    data->clear();
    data = static_cast<BingoData*>(user->getBingoData());
    *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(data) + 6) = 1;
    user->sendBingoData();
}

// ORIG reqValue 0x084b368a：repeat 事件(106)有效且连线 <3 时抽新棋盘值，
// 结算连线奖励并以 0x1c3 包回报当前棋盘字。
void CBingoGmTest::reqValue()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    BingoEvent* event = static_cast<BingoEvent*>(
        gmA_eventMgr_GetRepeatEvent(GlobalData::s_event_manager, 0x6a));
    if (event == 0)
    {
        return;
    }

    signed char lines = static_cast<signed char>(
        static_cast<BingoData*>(user->getBingoData())->m_pad4);
    if (lines > 2)
    {
        return;
    }

    int oldBoard = *reinterpret_cast<int*>(user->getBingoData());
    void* sharedBoard = event->getBingoSystem();          // boost::shared_ptr<CBingo>*
    CBingo* bingo = *reinterpret_cast<CBingo**>(sharedBoard);
    unsigned int board = static_cast<unsigned int>(oldBoard);
    int newValue = bingo->calNewValue(board, cmd_int0(this));

    if (*reinterpret_cast<int*>(user->getBingoData()) != oldBoard)
    {
        static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
        int gained = newValue - static_cast<int>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4);
        if (gained > 0)
        {
            if (gained > 3)
            {
                gained = 3;
            }
            static_cast<BingoData*>(user->getBingoData())->m_pad4 =
                static_cast<char>(newValue);
            for (int i = 0; i < gained; ++i)
            {
                event->sendOneMatchLineReward(*user);
            }
        }
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(cmd_int0(this));
    packet.put_int(*reinterpret_cast<int*>(user->getBingoData()));
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// CBillingRechargeCommand — ORIG W ctor 0x082a898e：基类 vptr → 自身
// vptr(0x8c814c8)，string@+0x08 默认构造（货币名）；+0x0c int 数额。
// ===========================================================================
CBillingRechargeCommand::CBillingRechargeCommand()
    : m_str0()
{
}

// ORIG parse 0x084b3a60：parser 有效时 m_str0=args 向量[1]，+0x0c=StringToNumber([2])。
void CBillingRechargeCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        cmd_int1(this) = static_cast<int>(StringToNumber(m_parser->m_extra[2].c_str()));
    }
}

// ORIG execute 0x084b3ad4：cera→虚表[1](type=5)、point→虚表[3](type=4) 充值下单，
// 最后统一走虚表[11] 通知。CIPGHelper 仅有前向声明，按 ORIG 虚表槽间接调用。
void CBillingRechargeCommand::execute()
{
    CUser* user = GetUser();
    void** vtable = *reinterpret_cast<void***>(GlobalData::s_pIPGHelper);

    if (m_str0 == "cera")
    {
        reinterpret_cast<void (*)(void*, CUser*, int, int, char const*, char const*,
                                  char const*, int, int, int)>(vtable[1])(
            GlobalData::s_pIPGHelper, user, 5, cmd_int1(this), "", "", "GM", 0, 0, 0);
    }
    else if (m_str0 == "point")
    {
        reinterpret_cast<void (*)(void*, CUser*, int, int, int, int)>(vtable[3])(
            GlobalData::s_pIPGHelper, user, cmd_int1(this), 4, 0, 0);
    }

    reinterpret_cast<void (*)(void*, CUser*)>(vtable[11])(GlobalData::s_pIPGHelper, user);
}

} // namespace game_master
