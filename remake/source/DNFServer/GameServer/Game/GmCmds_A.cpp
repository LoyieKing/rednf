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
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CInventory.h"
#include "CItemList.h"
#include "CStackableItem.h"
#include "CompoundEmblemDeps.h"
#include "BlueMarble.h"
#include "CCompoundAvatar.h"      // WongWork::CCompoundAvatar::ProcTest（桥清零）
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent（桥清零，权威头）
#include "CGameManager.h"         // CGameManager::GetParty / G_CGameManager（桥清零）

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
// [b23S3C] P201 桥转真：BingoEvent/CBingo 改 include 权威头
// （BingoEvent.h：getBingoSystem/sendOneMatchLineReward；CBingoTest.h：
//  CBingo::calNewValue(unsigned int&, int)），reqValue 调用点
//  boost::shared_ptr<CBingo>::operator-> 真调用（ORIG +0x8f call 080ce766）。
// ---------------------------------------------------------------------------
#include "BingoEvent.h"
#include "CBingoTest.h"
#include <boost/shared_ptr.hpp>

namespace game_master
{

// ---- GM 命令对象参数区一律具名字段访问（§4.3 裸偏移清零，2026-09-06）：
// GameMasterCmd.h 已按 ORIG POD 落真实字段（CAddItemPollutionCmd/CAddItemByMail
// ::m_itemIdx、CBoosterTest::m_mode(+0x08)/m_count(+0x0c)、CBindCubeTest::m_flag、
// BlueMarbleSet::m_flag0/1/2(+0x08/09/0a)+m_param(+0x0c)、CBingoGmTest::m_subCmd），
// 原 cmd_int0/cmd_int1/cmd_byte0 三宏（内部为 char*+0x08/0x0c 裸偏移）已删除。


// ===========================================================================
// CAutoMarketConditionsControlCmd — ORIG W ctor 0x082a8060：基类 vptr → 自身
// vptr(0x8c81c58)，无字段（sizeof=0x8）。
// ===========================================================================

// ORIG execute 0x084a8ffc：ret（纯空实现）。
void CAutoMarketConditionsControlCmd::execute()
{
}

// ===========================================================================
// CAssaultAbuseUserCmd — ORIG W ctor 0x082a8212：基类 vptr → 自身 vptr(0x8c81ae0)。
// ===========================================================================

// ORIG execute 0x084a9daa：ret（纯空实现）。
void CAssaultAbuseUserCmd::execute()
{
}

// ===========================================================================
// CBloodMonsterListCmd — ORIG W ctor 0x082a824a：基类 vptr → 自身 vptr(0x8c81ab0)。
// ===========================================================================

// ORIG execute 0x084a9e34：仅地下城状态(state==5)且有队伍时，把当前地图怪物
// uid 表拼成文本经 0x0c 号包回发给 GM。
// [b28d-47] verdict=DIFF(routed) [N84/N82] ORIG 0x084a9e34 orig/ours=214/185
//   修：put_str const_cast 去除（P167 重载 Pci→PKci）。残余：EXTRA memset（我方
//   char buf[0xff] 零初始化走 call memset，ORIG 为内联 rep stos，N78/N84 族）+
//   MISSING GetUser ×1（ORIG 三处独立取 user，槽/调用点差）+ 槽平移。
void CBloodMonsterListCmd::execute()
{
    CUser* user = GetUser();
    if (user->get_state() != 5)
    {
        return;
    }

    short partyIndex = user->GetPartyIndex();
    CParty* party = G_CGameManager()->GetParty(static_cast<int>(partyIndex));
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
        &party->m_battleField;
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
    // [b28d-47] P167 重载选错修正：ORIG 调 `put_str(char const*, int)`（PKci），
    // const_cast 使其选 `put_str(char*, int)`（Pci）。
    packet.put_str(message.c_str(), static_cast<int>(message.size()));
    packet.finalize(true);
    GetUser()->Send(packet);
}

// ===========================================================================
// CAddItemPollutionCmd — ORIG W ctor 0x082a82e4：基类 vptr → 自身 vptr(0x8c81a20)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================

// ORIG parse 0x084aa9c8：trim(args)；+0x08 = atoi(args.c_str())。
void CAddItemPollutionCmd::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084aaa00：itemId==-1 先开通 PC 房特权；随后发放物品并在
// 无强化属性时随机投一次强化投资，最后日志 + 物品更新包。
// [b28d-47] verdict=DIFF(routed) [N82/P112] ORIG 0x084aaa00 orig/ours=187/180
//   残差：全帧槽号平移 + 寄存器角色差（跨版本分配）。
void CAddItemPollutionCmd::execute()
{
    if (m_itemIdx == -1)
    {
        // ORIG：getCurSec→lea +0x15180 直传，start/end 无栈槽（右值内联）
        WongWork::CHandlePremium::handleSetUserPCRoom(
            GetUser(),
            static_cast<long>(GlobalData::s_systemTime_.getCurSec()),
            static_cast<long>(GlobalData::s_systemTime_.getCurSec()) + 0x15180);
    }

    int limit = 100;
    CStackableItem* stackable =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(m_itemIdx));
    if (stackable != 0 && stackable->is_stackable())
    {
        if (stackable->getStackableLimit() < limit)
        {
            limit = stackable->getStackableLimit();
        }
    }

    ENUM_ITEMSPACE space;
    CUser* user = GetUser();
    int slot = user->AddItem(m_itemIdx, limit, static_cast<eItemAddReason>(6),
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

// ORIG parse 0x084ac2aa：trim(args)；+0x08 = atoi(args.c_str())。
void CAddItemByMail::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084ac2e2：合法物品以系统邮件发放（0 号特例 count=1，1 号特例
// 金币条目手工填充，其余走 CItem::make_item 虚拷贝并附加 add_info=1）。
// [b28d-47] verdict=DIFF(routed) [N78/N82] ORIG 0x084ac2e2 orig/ours=187/132
//   残差：EXTRA memset ×2（我方数组零初始化 call vs ORIG 内联 rep stos，N78）
//   + 槽/寄存器分配跨版本差。
void CAddItemByMail::execute()
{
    CUser* user = GetUser();
    int itemId = m_itemIdx;
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

// ORIG parse 0x084aeb5e：clear() 后 sscanf(args,"%d%d",&+0x08,&+0x0c)。
void CBoosterTest::parse()
{
    clear();
    sscanf(m_parser->m_args.c_str(), "%d%d", &m_mode, &m_count);
}

// ORIG clear 0x084b40b0（报告缺失，objdump 直转）：+0x08=-1、+0x0c=0。
void CBoosterTest::clear()
{
    m_mode = -1;
    m_count = 0;
}

// ORIG execute 0x084aed04：LOTTERY 类物品抽样统计并把结果文件内容分包回显。
// [b28d-47] verdict=DIFF(routed) [N78/N82] ORIG 0x084aed04 orig/ours=269/241
//   残差：EXTRA memset（N78 内联清零差）+ 全帧槽号平移。
void CBoosterTest::execute()
{
    CUser* user = GetUser();

    char fileName[0xff];
    memset(fileName, 0, sizeof(fileName));
    char fileBuf[95000];
    memset(fileBuf, 0, sizeof(fileBuf));

    static char sMsgStart[] = "LOTTERY 아이템 결과체크를 시작합니다";
    CBoosterTest::SendChatMsg(user, sMsgStart);

    if (m_mode == -1)
    {
        static char sNeedIdx[] = "아이템 인덱스를 입력하세요";
        CBoosterTest::SendChatMsg(user, sNeedIdx);
        return;
    }

    CItem* item = G_CDataManager()->find_item(m_mode);
    if (item == 0)
    {
        static char sNoItem[] = "존재하지 않는 아이템입니다";
        CBoosterTest::SendChatMsg(user, sNoItem);
        return;
    }

    if (m_count > 1000000 || m_count < 0)
    {
        static char sRange[] = "0 ~ 1000000번 통계까지만 지원됩니다";
        CBoosterTest::SendChatMsg(user, sRange);
        return;
    }
    if (m_count == 0)
    {
        m_count = 10000;
    }

    char countMsg[21];
    sprintf(countMsg, "%d번 통계입니다", m_count);
    CBoosterTest::SendChatMsg(user, countMsg);

    bool goldLottery = this->is_gold_lottery_item(item->get_index());

    std::map<int, int> result;
    switch (item->GetItemType())                       // ORIG vtbl[3]
    {
    case 6:
    case 7:
    case 0xd:
        if (!goldLottery)
        {
            this->getJarResult(item, result);
        }
        else
        {
            this->get_gold_lottery_result(item, result);
        }
        break;
    case 0xf:
    case 0x10:
        this->getBoosterResult(item, result);
        break;
    case 0x14:
        if (!this->getCompoundEmblemResult(item, result))
        {
            static char sNotEmblem[] = "엠블렘 합성 대상이 아닙니다.";
            CBoosterTest::SendChatMsg(GetUser(), sNotEmblem);
        }
        break;
    case 0x1a:
    case 0x1b:
        this->getRandomboxResult(item, result);
        break;
    default:
    {
        static char sNotLottery[] = "LOTTERY 아이템이 아닙니다";
        CBoosterTest::SendChatMsg(GetUser(), sNotLottery);
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
            CBoosterTest::makeFileContents(user, fileBuf, result);
        }
        else
        {
            CBoosterTest::make_file_gold_contents(user, fileBuf, result);
        }
        sprintf(fileName, "lottery_%d_%d.txt", m_mode, m_count);
        if (result.empty())
        {
            more = false;
        }
        CBoosterTest::printClient(user, more, fileName, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    static char sDone[] = "LOTTERY 아이템 결과체크가 완료되었습니다";
    CBoosterTest::SendChatMsg(user, sDone);
}

// ---- CBoosterTest 辅助函数（ORIG 逐符号转写）------------------------------

// ORIG getRarityStr 0x084aea80：稀有度 → 文件列名。
char const* CBoosterTest::getRarityStr(ENUM_RARITY rarity)
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
void CBoosterTest::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    // ORIG：strlen 先求值物化 %ebx 跨 G_GameWorld call；gw 经 eax 直通 receiver，
    // 无具名 gw 槽（Ghidra C 顺序 strlen→G_GameWorld→make_packet_chat_msg）。
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, static_cast<int>(strlen(msg)), 2,
        static_cast<ENUM_NOTIPACKET>(0xc));
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
    packet.put_str(name, static_cast<int>(strlen(name)));
    packet.put_int(static_cast<int>(strlen(contents)));
    packet.put_str(contents, static_cast<int>(strlen(contents)));
    packet.finalize(true);
    user->Send(packet);
}

// ORIG is_gold_lottery_item 0x084afccc：lotto 表项 id(+2)==0 且数量(+7)>0。
bool CBoosterTest::is_gold_lottery_item(int idx)
{
    Inven_Item lotto;
    G_CDataManager()->m_itemList->get_lotto_item(idx, reinterpret_cast<char*>(&lotto));
    return lotto.m_addInfo == 0 &&
           lotto.m_addInfo2 > 0;
}

// ORIG getJarResult 0x084afba8：统计 lotto 表产出物品次数。
// [b28d-47] verdict=DIFF(routed) [N7/N82] ORIG 0x084afba8 orig/ours=88/87
//   修：idx 实参内联（P0 去 -0xc 槽）+ `++pos->second`（单 ptEv）。
//   残余：callee-saved 角色（ORIG %esi 缓存 lotto vs 4.4 %ebx）+ 槽差（N7/N82）。
void CBoosterTest::getJarResult(CItem const* item, std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < m_count; ++i)
    {
        // [b28d-47] P0：ORIG 0x084afbe8 `call get_index; mov %eax,%ebx`（无槽）⇒
        // 实参内联，禁具名 idx（4.4 落 -0xc 槽并使帧/槽整体漂移）。
        G_CDataManager()->getItemList()->get_lotto_item(
            const_cast<CItem*>(item)->get_index(),
            reinterpret_cast<char*>(&lotto));
        int itemId = lotto.m_addInfo;
        std::map<int, int>::iterator pos = result.find(itemId);
        if (pos == result.end())
        {
            result.insert(std::make_pair(itemId, 1));
        }
        else
        {
            ++pos->second;   // ORIG 单次 ptEv + mov/add/mov（`= pos->second + 1` 4.4 双 ptEv）
        }
    }
}

// ORIG get_gold_lottery_result 0x084afd1c：统计金币产出（key=+7 数量，value=1）。
// [b28d-47] verdict=DIFF(routed) [N7/N82] ORIG 0x084afd1c orig/ours=86/87
//   修：idx 实参内联（P0）+ `++pos->second`。残余：寄存器角色/槽差。
void CBoosterTest::get_gold_lottery_result(CItem const* item,
                                           std::map<int, int>& result)
{
    Inven_Item lotto;
    for (int i = 0; i < m_count; ++i)
    {
        G_CDataManager()->getItemList()->get_lotto_item(   // [b28d-47] P0 同 getJarResult
            const_cast<CItem*>(item)->get_index(),
            reinterpret_cast<char*>(&lotto));
        int goldCount = lotto.m_addInfo2;
        std::map<int, int>::iterator pos = result.find(goldCount);
        if (pos == result.end())
        {
            result.insert(std::make_pair(goldCount, 1));
        }
        else
        {
            ++pos->second;   // ORIG 单次 ptEv（同 getJarResult）
        }
    }
}

// ORIG getBoosterResult 0x084af9d6：统计 booster 产出（stStackableBoosterItemInfo_t）。
// [b28d-47] verdict=DIFF(routed) [N7/P96] ORIG 0x084af9d6 orig/ours=139/131
//   修：make_pair 实参右值化（P61 值对）+ `++pos->second`。
//   残余：vector 遍历迭代器实例差（EXTRA ptEv/ppEv/ne，P175 族）+ 槽平移。
void CBoosterTest::getBoosterResult(CItem const* item,
                                    std::map<int, int>& result)
{
    std::vector<stStackableBoosterItemInfo_t> items;
    for (int i = 0; i < m_count; ++i)
    {
        static_cast<CStackableItem const*>(item)->getBoosterItem(items);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
             it != items.end(); ++it)
        {
            int itemId = it->m_itemIdx;
            std::map<int, int>::iterator pos = result.find(itemId);
            if (pos == result.end())
            {
                // [b28d-47] P61：实参右值化 ⇒ 实例化 make_pair<int,int>（值对），
                // 左值 itemId 会实例化 make_pair<int&,int>（P167 EXTRA）。
                result.insert(std::make_pair(static_cast<int>(itemId), 1));
            }
            else
            {
                ++pos->second;
            }
        }
        items.clear();
    }
}

// ORIG getRandomboxResult 0x084afe3a：统计 multibox 随机盒产出（pair<Inven_Item,bool>）。
// [b28d-47] verdict=DIFF(routed) [P220/P83] ORIG 0x084afe3a orig/ours=148/123
//   修：P220 正极性 `pos != end`（消 EXTRA eq/MISSING ne）+ P61 make_pair 右值化
//   + `++pos->second`。残余：EXTRA memcpy（我方 memcpy vs ORIG 结构整体赋值
//   rep movsl，N84）+ 槽平移。
void CBoosterTest::getRandomboxResult(CItem const* item,
                                      std::map<int, int>& result)
{
    Inven_Item dropped;
    std::vector<std::pair<Inven_Item, bool> > drops;
    for (int i = 0; i < m_count; ++i)
    {
        drops.clear();
        const_cast<CStackableItem*>(static_cast<CStackableItem const*>(item))->get_multibox_lottery_item(&drops);
        for (unsigned int j = 0; j < drops.size(); ++j)
        {
            memcpy(&dropped, &drops[j].first, sizeof(Inven_Item));  // ORIG 逐字段浅拷贝
            int itemId = dropped.m_addInfo;
            std::map<int, int>::iterator pos = result.find(itemId);
            // [b28d-47] P220 正极性：ORIG 用 `ne`（pos != end 时 ++），旧 `==` 形
            // 产 eq（P167 MISSING ne / EXTRA eq）。
            if (pos != result.end())
            {
                ++pos->second;   // [b28d-47] 单次 ptEv
            }
            else
            {
                result.insert(std::make_pair(static_cast<int>(itemId), 1));  // [b28d-47] P61 右值化
            }
        }
    }
}

// ORIG getCompoundEmblemResult 0x084af0d6：合成徽章统计；目标不是 16 型物品返回 false。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af0d6 orig/ours=218/200
//   残差：槽/寄存器分配跨版本差 + STL 内联实例（N89）。
bool CBoosterTest::getCompoundEmblemResult(CItem const* item,
                                           std::map<int, int>& result)
{
    unsigned short sockets[2];
    sockets[0] = static_cast<CStackableItem const*>(item)->getJewelTargetSocket();
    sockets[1] = static_cast<CStackableItem const*>(item)->getJewelTargetSocket();

    int jewelType = this->_selectJewelTypeIndex(2, sockets);
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

    for (int i = 0; i < m_count; ++i)
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
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af3b2 orig/ours=94/87
//   残差：全帧槽号整体平移 4（ORIG -0x38.. vs 我方 -0x34..）+ push ebx 差异
//   （4.1 不按 lifetime 复用栈槽，N82/P112）。
int CBoosterTest::_selectJewelTypeIndex(unsigned int count,
                                        unsigned short const* sockets) const
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
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af504 orig/ours=192/174
//   残差：槽/寄存器分配 + 字符串物化链差。
void CBoosterTest::makeFileContents(CUser* user, char* fileBuf,
                                    std::map<int, int>& result)
{
    char line[1024];
    memset(line, 0, 0x100);   // ORIG rep stos 0x100（循环内 memset 仍 0x400）

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
                    getRarityStr(static_cast<ENUM_RARITY>(info->get_rarity())),
                    it->second, info->get_price(), info->GetSellPrice(),
                    info->getUsableLevel());
        }

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "결과 파일 이어붙힘";
            SendChatMsg(user, sAppend);
            return;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }
}

// ORIG make_file_gold_contents 0x084af7fc：金币分布行 + 尾部 max gold 合计。
// （格式串 %I64u 按 ORIG 原样保留；glibc 下由 %llu 语义承载 64 位实参。）
// [b28d-47] verdict=DIFF(routed) [N82/P112] ORIG 0x084af7fc orig/ours=121/113
//   p167 CLEAR（18/18 调用点全等）⇒ 语义完成；残差为槽/寄存器分配跨版本差。
void CBoosterTest::make_file_gold_contents(CUser* user, char* fileBuf,
                                           std::map<int, int>& result)
{
    char line[1024];
    memset(line, 0, 0x100);   // ORIG rep stos 0x100（循环内 memset 仍 0x400）

    unsigned long long maxGold = 0;
    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        sprintf(line, "%d gold count = %d \n", it->first, it->second);
        int gold = it->first * it->second;
        maxGold += gold;

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "결과 파일 이어붙힘";
            SendChatMsg(user, sAppend);
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

// ORIG parse 0x084acbe4：trim(args)；+0x08 = (byte)atoi(args.c_str())。
void CBindCubeTest::parse()
{
    trim(m_parser->m_args);
    m_flag = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084acc1e：职业 0..10 时遍历 bind×recipe×equip 组合跑合成测试，
// 每组结果落一份 bind_<job>_<bind>_<recipe>_<equip>.txt 回显给 GM。
// [b28d-47] verdict=DIFF(routed) [N78/N84] ORIG 0x084acc1e orig/ours=166/134
//   残差：EXTRA memset（我方缓冲零初始化 call vs ORIG 内联 rep stos，N78）+
//   槽/寄存器分配差。
void CBindCubeTest::execute()
{
    if (!(static_cast<signed char>(m_flag) >= 0 &&
          static_cast<signed char>(m_flag) <= 10))
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
                static_cast<WongWork::CCompoundAvatar*>(
                    GlobalData::s_pCompoundAvatar)
                    ->ProcTest(static_cast<ENUM_CHARACTERJOB>(
                                   static_cast<int>(static_cast<signed char>(m_flag))),
                               static_cast<ENUM_EQUIPMENTTYPE>(equip),
                               recipe, bind, result);
                CBoosterTest::makeFileContents(GetUser(), fileBuf, result);
                sprintf(fileName, "bind_%s_%s_%s_%s.txt",
                        CHARACTER_JOB_NAMES[static_cast<int>(
                            static_cast<signed char>(m_flag))],
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

// ORIG parse 0x084b22ca：三标记清零、token 计数清零，trim 后整串交给 getParam。
void BlueMarbleSet::parse()
{
    m_flag1 = 0;
    m_flag2 = 0;
    m_flag0 = 0;
    m_param = 0;

    trim(m_parser->m_args);
    this->getParam(m_parser->m_args);
}

// ORIG getParam 0x084b235c：按空格切 token（≤16），tokens[0]=="move"/"enter" 置
// 对应标记，atoi(tokens[1]) 写步数字节。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084b235c orig/ours=183/178
//   残差：槽号平移 + std::string 局部构造/拷贝实例差。
void BlueMarbleSet::getParam(std::string param)
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
        m_flag1 = 1;
    }
    else if (tokens[0].compare("enter") == 0)
    {
        m_flag2 = 1;
    }
    m_flag0 = static_cast<unsigned char>(atoi(tokens[1].c_str()));
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

    if (m_flag2 != 0)
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
    if (m_flag1 == 0)
    {
        return;
    }
    if (m_flag0 == 0)
    {
        return;
    }
    // P196：ORIG 对 m_flag0(+0x08, char 存 GM 参数 0-255, parse 里 static_cast<unsigned char>)
    // 作实参直传时 `movzbl %al,%eax`（无符号扩展），我方 static_cast<int> 使 4.4 出 movsbl
    // （符号）⇒ 值≥0x80 时 movePosition 收负 pos，真 bug。显式经 unsigned char 提升复现 movzbl。
    blueMarble->movePosition(user, static_cast<int>(static_cast<unsigned char>(m_flag0)));
}

// ===========================================================================
// CBingoGmTest — ORIG W ctor 0x082a893a：基类 vptr → 自身 vptr(0x8c81510)，
// 无成员初始化（+0x08 int 未初始化，ORIG 如此）。类声明见 GameMasterCmd.h
// （占位 std::string m_str0@+0x08，真实为 int 子命令码，本 TU 偏移访问）。
// ===========================================================================

// ORIG parse 0x084b35c4：trim(args)；+0x08 = atoi(args.c_str())。
void CBingoGmTest::parse()
{
    trim(m_parser->m_args);
    m_subCmd = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084b35fc：user 有效时按子命令分流。
void CBingoGmTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    if (m_subCmd == 0)
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
    static_cast<BingoData*>(user->getBingoData())->clear();
    static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
    user->sendBingoData();
}

// ORIG reqValue 0x084b368a：repeat 事件(106)有效且连线 <3 时抽新棋盘值，
// 结算连线奖励并以 0x1c3 包回报当前棋盘字。
// [b28d-47] verdict=DIFF(routed) [N18b] ORIG 0x084b368a orig/ours=152/150
//   残差：`if (gained > 3) gained = 3;` 4.4 恒折内存比较+内存写
//   （`cmpl $3,-0x10; movl $3,-0x10`）vs ORIG 寄存器中转 4 条
//   （`mov slot,%eax; cmp; mov $3,%eax; mov %eax,slot`）⇒ N18b 封顶。
void CBingoGmTest::reqValue()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    BingoEvent* event = static_cast<BingoEvent*>(
        static_cast<void*>(GlobalData::s_event_manager->GetRepeatEvent(0x6a)));
    if (event == 0)
    {
        return;
    }

    // [b23S3C] ORIG +0x48 单次 getBingoData→movzbl+cmp$2,setg 寄存器直测（无
    // lines 具名槽）；+0x74 每次 calNewValue 第 1 实参 = 独立 getBingoData() 的
    // bd 指针直传 uint&（ORIG mov %eax,%ebx；无 board 拷贝槽 -0x30）；
    // receiver 链 shared_ptr::operator-> 真调用（+0x8f）。
    if (static_cast<signed char>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4) > 2)
    {
        return;
    }

    int oldBoard = static_cast<int>(
        static_cast<BingoData*>(user->getBingoData())->m_eventId);
    int newValue = (*reinterpret_cast<boost::shared_ptr<CBingo>*>(
                        event->getBingoSystem()))
                       ->calNewValue(
                           static_cast<BingoData*>(user->getBingoData())
                               ->m_eventId,
                           m_subCmd);

    if (*reinterpret_cast<int*>(user->getBingoData()) != oldBoard)
    {
        static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
        int gained = newValue - static_cast<int>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4);
        if (gained > 0)
        {
            // [b23S3C] ORIG +0x0f5 分支形 clamp（读槽→cmp reg→jle→mov $3→写回）。
            if (gained > 3)
                gained = 3;
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
    packet.put_byte(m_subCmd);
    packet.put_int(*reinterpret_cast<int*>(user->getBingoData()));
    packet.finalize(true);
    user->Send(packet);
}

// ===========================================================================
// CBillingRechargeCommand — ORIG W ctor 0x082a898e：基类 vptr → 自身
// vptr(0x8c814c8)，string@+0x08 默认构造（货币名）；+0x0c int 数额。
// ===========================================================================

// ORIG parse 0x084b3a60：parser 有效时 m_str0=args 向量[1]，+0x0c=StringToNumber([2])。
void CBillingRechargeCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        m_amount = StringToNumber(m_parser->m_extra[2].c_str());
        return;                                    // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}

// ORIG execute 0x084b3ad4：cera→虚表[1](type=5)、point→虚表[3](type=4) 充值下单，
// 最后统一走虚表[11] 通知。CIPGHelper 仅有前向声明，按 ORIG 虚表槽间接调用。
// [b28d-47] verdict=NEAR(routed) [N7] ORIG 0x084b3ad4 orig/ours=67/67
//   残差：间接 call 目标寄存器——ORIG 4.1 %ecx；4.4.7 恒 %eax、4.4.6 恒 %edx
//   （P230 实测 446 后端亦不达 %ecx）⇒ N7 封顶，连锁实参/数据寄存器名互换。
void CBillingRechargeCommand::execute()
{
    CUser* user = GetUser();
    // b26 NEAR 封顶:三处虚调用 vtable 项寄存器 ORIG %ecx vs 4.4 %eax(N7),
    // 连锁实参/数据寄存器名互换,余逐条相等。
    // ORIG（P157 逐点重取形）：每个调用表达式独立重读 s_pIPGHelper → 取 vptr →
    // add 槽 → deref fn，随后再读一次 helper 作首实参；禁 vtable 缓存局部
    //（缓存少发 ORIG 的逐点 fetch 行且引入额外栈槽）。
    if (m_str0 == "cera")
    {
        reinterpret_cast<void (*)(void*, CUser*, int, int, char const*, char const*,
                                  char const*, int, int, int)>(
            (*reinterpret_cast<void***>(GlobalData::s_pIPGHelper))[1])(
            GlobalData::s_pIPGHelper, user, 5, m_amount, "", "", "GM", 0, 0, 0);
    }
    else if (m_str0 == "point")
    {
        reinterpret_cast<void (*)(void*, CUser*, int, int, int, int)>(
            (*reinterpret_cast<void***>(GlobalData::s_pIPGHelper))[3])(
            GlobalData::s_pIPGHelper, user, m_amount, 4, 0, 0);
    }
    reinterpret_cast<void (*)(void*, CUser*)>(
        (*reinterpret_cast<void***>(GlobalData::s_pIPGHelper))[11])(
        GlobalData::s_pIPGHelper, user);
}

} // namespace game_master