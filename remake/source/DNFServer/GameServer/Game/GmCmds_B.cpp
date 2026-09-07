// GmCmds_B.cpp — game_master 命令体系还原·批次 B（11 类）。
//
// 对照 docs/class_func_reports/game_master__{CChangeCharacName,CChangeExpertJobCmd,
//   CChangeExpertJobExpCmd,CChangeNPCFavorCmd,CChangeNPCMaxDayCmd,CCharacStatCmd,
//   CChoiceItem,CCirculationStatisticCmd,CClearAvatarInvenCmd,CClearInventoryCmd,
//   CClearRewardTest}/{parse,execute,printClient}.md。
//
// 占位字段说明（本批次禁止编辑 GameMasterCmd.h）：
//   头文件对这些类统一给出占位成员 std::string m_str0（+0x08）。ORIG 布局中
//   CChangeExpertJobCmd/CChangeExpertJobExpCmd/CChangeNPCFavorCmd/
//   CChangeNPCMaxDayCmd/CChoiceItem 的 +0x08（及 NPC 双参数类的 +0x0c）实为
//   int 槽（ctor 不构造、parse 按 atoi 写入），故此处按存储位置 reinterpret
//   访问；对象大小与 ORIG 一致（32 位 COW std::string 为 4 字节）。
//   CChangeCharacName 的 +0x08 是真 std::string（ORIG ctor 调 _ZNSsC1Ev）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CDataManager.h"
#include "CItem.h"
#include "SkillSlot.h"
#include "STExpertJobScript.h"
#include "CExpertJob.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "LogManager.h"
#include "CBattle_Field.h"
#include "CDungeon.h"
#include "CParty.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------
// ORIG T 0x08426590 _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
// （静态成员；仓库尚无该类声明，按 objdump 符号补 extern 声明）
// ---------------------------------------------------------------------------
class DB_ReqChangeCharacName
{
public:
    static void makeRequest(int uid, int characNo, char const* name, int flag);
};

namespace game_master
{

// ===========================================================================
// CChangeCharacName — ORIG W ctor 0x082a8418：基类 vptr → 自身 vptr(0x8c81930)，
// string@+0x08 默认构造（_ZNSsC1Ev）。
// ===========================================================================

// ORIG parse 0x084ab524：trim(parser->args)；m_str0 = parser->args（新角色名）。
void CChangeCharacName::parse()
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}

// ORIG execute 0x084ab558。
void CChangeCharacName::execute()
{
    CUser* user = GetUser();
    DB_ReqChangeCharacName::makeRequest(user->GetUID(), user->getCurCharacNo(),
                                        m_str0.c_str(), 1);
}

// ===========================================================================
// CChangeExpertJobCmd — ORIG W ctor 0x082a816a：仅基类 vptr + 自身 vptr
// (0x8c81b70)；+0x08 为 int 槽【专家职业类型】，不构造。
// ===========================================================================

// ORIG parse 0x084a98c8：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
void CChangeExpertJobCmd::parse()
{
    trim(m_parser->m_args);
    m_jobType = atoi(m_parser->m_args.c_str());   // P0：无具名局部（ORIG 无槽）
}

// ORIG execute 0x084a9900。
void CChangeExpertJobCmd::execute()
{
    int jobType = m_jobType;
    GetUser()->SetCurCharacExpertJobType(jobType);

    STExpertJobScript* script =
        static_cast<STExpertJobScript*>(G_CDataManager()->GetExpertJobScript(jobType));
    if (script != 0)
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < script->m_growSkillVec.size(); ++i)
        {
            if (GetUser()->getCurCharacR() == 0)
            {
                continue;
            }
            // pair = [skillIdx, level]（STExpertJobScript+0xc [skill]）
            int skillIdx = script->m_growSkillVec[i].first;
            int skillLevel = script->m_growSkillVec[i].second;
            int curJob = GetUser()->get_charac_job();
            SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                GetUser()->getCurCharacSkillW());         // 占位声明返回 char*，ORIG 实为 SkillSlot*
            skillW->growtype_skill(curJob, skillIdx, skillLevel,
                                   (ENUM_SKILL_TREE_KIND)0);
            skillW->growtype_skill(curJob, skillIdx, skillLevel,
                                   (ENUM_SKILL_TREE_KIND)1);
        }
        expert_job::CCharacExpertJob* expand =
            reinterpret_cast<expert_job::CCharacExpertJob*>(
                GetUser()->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0));
        expand->InitScript(script);
        GetUser()->SetCurCharacExpertJobExp(0);
    }

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    GetUser()->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, GetUser());
}

// ===========================================================================
// CChangeExpertJobExpCmd — ORIG W ctor 0x082a8186：仅 vptr 设置；
// +0x08 为 int 槽【专家职业经验增量】。
// ===========================================================================

// ORIG parse 0x084a9b98：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
void CChangeExpertJobExpCmd::parse()
{
    trim(m_parser->m_args);
    m_exp = atoi(m_parser->m_args.c_str());       // P0：无具名局部（ORIG 无槽）
}

// ORIG execute 0x084a9bd0。
void CChangeExpertJobExpCmd::execute()
{
    expert_job::CExpertJob::IncreaseExpertJobExp(GetUser(), m_exp);
}

// ===========================================================================
// CChangeNPCFavorCmd — ORIG W ctor 0x082a831c：仅 vptr；
// +0x08/+0x0c 为双 int 槽【npcId / favor】。
// ===========================================================================

// ORIG parse 0x084aadd8：trim(args)；pos=args.find(" ")；
// *(int*)(this+8)=atoi(substr(0,pos))；*(int*)(this+0xc)=atoi(substr(pos+1))。
void CChangeNPCFavorCmd::parse()
{
    trim(m_parser->m_args);
    int pos = m_parser->m_args.find(" ", 0);
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1, std::string::npos);
    int v0 = atoi(first.c_str());
    int v1 = atoi(second.c_str());
    m_npcId = v0;
    m_favor = v1;
}

// ORIG execute 0x084aaef2：空实现（ret）。
void CChangeNPCFavorCmd::execute()
{
}

// ===========================================================================
// CChangeNPCMaxDayCmd — ORIG W ctor 0x082a8300：仅 vptr；
// +0x08/+0x0c 为双 int 槽【npcId / maxDay】。
// ===========================================================================

// ORIG parse 0x084aacb8：与 CChangeNPCFavorCmd::parse 同构。
void CChangeNPCMaxDayCmd::parse()
{
    trim(m_parser->m_args);
    int pos = m_parser->m_args.find(" ", 0);
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1, std::string::npos);
    int v0 = atoi(first.c_str());
    int v1 = atoi(second.c_str());
    m_npcId = v0;
    m_maxDay = v1;
}

// ORIG execute 0x084aadd2：空实现（ret）。
void CChangeNPCMaxDayCmd::execute()
{
}

// ===========================================================================
// CCharacStatCmd — ORIG W ctor 0x082a8098：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084a905e：读 AdditionalInfo 移速/攻速/施速，组串回发本人。
void CCharacStatCmd::execute()
{
    char buf[255];                                        // ORIG -0x127(%ebp)，255 字节清零
    memset(buf, 0, sizeof(buf));

    unsigned char const* add = static_cast<unsigned char const*>(
        GetUser()->getCurCharacAddInfoR());
    sprintf(buf, "charac_stat move_speed=%d,attack_speed=%d,cast_speed=%d",
            *(int const*)(add + 0x42),
            (unsigned int)*(unsigned short const*)(add + 0x46),
            (unsigned int)*(unsigned short const*)(add + 0x48));

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(strlen(buf));
    packet.put_str(buf, strlen(buf));
    packet.finalize(true);
    GetUser()->Send(packet);
}

// ===========================================================================
// CChoiceItem — ORIG W ctor 0x082a87c2：仅 vptr；+0x08 为 int 槽【物品索引】。
// ===========================================================================

// ORIG parse 0x084b0a34：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
void CChoiceItem::parse()
{
    trim(m_parser->m_args);
    m_itemIdx = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084b0a6e。
void CChoiceItem::execute()
{
    if (m_itemIdx == 0)
    {
        G_CDataManager()->setChoiceItem(false, 0xffffffffu);
        return;
    }
    CItem* item = G_CDataManager()->find_item(m_itemIdx);
    // ORIG：bad = (item==0 || is_stackable) bool 汇聚（mov$1 落 +0x61 fall，mov$0
    // 为 je 岛），if(!bad) 才 setChoiceItem(true, idx)。
    register unsigned char bad asm("eax") = (item == 0) || item->is_stackable();
    if (bad)
        goto done;
    G_CDataManager()->setChoiceItem(true, (unsigned int)m_itemIdx);
done:
    ;
}

// ===========================================================================
// CCirculationStatisticCmd — ORIG W ctor 0x082a8044：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084a8ff6：空实现（ret）。
void CCirculationStatisticCmd::execute()
{
}

// ===========================================================================
// CClearInventoryCmd — ORIG W ctor 0x082a8338：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084aaef8：清空背包槽 [3,0x137] 后 send_itemspace(INVENTORY)。
void CClearInventoryCmd::execute()
{
    CUser* user = GetUser();
    for (int slot = 3; slot <= 0x137; ++slot)
    {
        // ORIG：getCurCharacInvenW 结果经 eax 直通 +0x650 读，无具名 inven 槽。
        user->getCurCharacInvenW()->m_pEquipSlot[slot].reset();
    }
    if (user->send_itemspace(0) != 1)                     // ENUM_ITEMSPACE_INVENTORY=0
    {
        // P44：NumberToString 即时求值直压 0x14(%esp)，无 acc 具名槽。
        LogManager::logFormat(
            1, "GameMaster.cpp",
            __PRETTY_FUNCTION__, 0x63b,
            "CClearInventoryCmd::execute, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
    }
}

// ===========================================================================
// CClearAvatarInvenCmd — ORIG W ctor 0x082a8354：仅 vptr（无成员）。
// ===========================================================================

// ORIG execute 0x084aafb0：逐 avatar 槽 [0,0x68] 删除非空物品并 SendAvatarEvent，
// 末尾 send_itemspace(AVATAR)。
void CClearAvatarInvenCmd::execute()
{
    CUser* user = GetUser();
    // [b23S3C] P48 去缓存：ORIG getCurCharacInvenR ×2（+0x28/+0x58）、
    // getCurCharacInvenW ×2（+0x78/+0x10f）各用点独立求值，无 invenR/invenW 槽；
    // addInfo2 预清槽 -0x10（+0x1c movl $0x0）⇒ 循环外 `int addInfo2 = 0;`；
    // NumberToString 结果直落 0x14(%esp) 实参槽（无 acc 局部，P0 寄存器直传）。
    int addInfo2 = 0;
    for (int slot = 0; slot <= 0x68; ++slot)
    {
        if (user->getCurCharacInvenR()->m_pAvatarSlot[slot].m_addInfo != 0)
        {
            addInfo2 = user->getCurCharacInvenR()
                           ->m_pAvatarSlot[slot].m_addInfo2;
            if (!user->getCurCharacInvenW()
                     ->delete_item(INVEN_TYPE_AVATAR, slot, 1,
                                   eItemDelReason_3, true))
            {
                LogManager::logFormat(
                    1, "GameMaster.cpp",
                    __PRETTY_FUNCTION__, 0x64d,
                    "CClearAvatarInvenCmd::execute, pUser(%s)->getCurCharacInvenW()->delete_item return FAILED",
                    NumberToString(user->get_acc_id(), 0));
                return;
            }
            user->getCurCharacInvenW()->SendAvatarEvent(0x36, addInfo2, 1,
                                                        slot + 10, 0);
        }
    }
    // ORIG xor$1 形需 send_itemspace 返 bool（Main 已代落 CUser.h:2843）。
    if (user->send_itemspace(1) != 1)   // ENUM_ITEMSPACE_AVATAR=1
    {
        LogManager::logFormat(
            1, "GameMaster.cpp",
            __PRETTY_FUNCTION__, 0x656,
            "CClearAvatarInvenCmd::execute, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
            NumberToString(user->get_acc_id(), 0));
    }
}

// ===========================================================================
// CClearRewardTest — ORIG W ctor 0x082a8902：仅 vptr（无成员）。
// ===========================================================================

// ORIG parse 0x084b2a02：空实现（ret）。
void CClearRewardTest::parse()
{
}


// ORIG execute 0x084b2a08。
void CClearRewardTest::execute()
{
    CUser* user = GetUser();
    if (user == 0 || user->getCurCharacR() == 0 || user->get_state() != 5)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0 || party->m_battleField.m_pDungeon == 0)
    {
        return;
    }
    CDungeon* dungeon = party->m_battleField.m_pDungeon;

    // CBattle_Field 子对象位于 CParty+0xb24（占位期经 m_battleField 视图访问）。
    CBattle_Field* battleField =
        &party->m_battleField;
    int diff = battleField->get_dungeon_diff();

    int standardLevel = 0;
    if (dungeon->get_dimension_possible() < 1)
    {
        standardLevel = dungeon->get_standard_level();
    }
    else
    {
        standardLevel = party->getStandardDimensionLevel();
    }

    WongWork::stGenerateRefData_t ref;                    // ORIG 0814a706 ctor
    ref.m_field34 = (char)standardLevel;                  // +0x34（基准值）
    ref.m_field35 = (char)diff;                           // +0x35（难度差【推断】）
    ref.m_rankBonusIndex = 0x3f800000;                    // +0x38（float 位型常量）
    ref.m_goldBase = 0;                                   // +0x3c 4 字节写 0
    ref.m_goldFactor = 0;
    ref.m_dungeon = dungeon;                              // +0x40

    WongWork::CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
    WongWork::CItemGenerator* generator =
        mgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2);

    // ---- 第一轮：默认掉率 → cltr_normal.txt ----
    {
        WongWork::stGenerateResult_t result;
        for (int i = 0; i < 100000; ++i)
        {
            generator->generateRandomItem(ref, result);
        }

        std::map<int, int> statMap;
        for (std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
             it != result.m_dropItems.end(); ++it)
        {
            int itemIdx = it->m_addInfo;
            std::map<int, int>::iterator mit = statMap.find(itemIdx);
            if (mit != statMap.end())
            {
                ++mit->second;
            }
            else
            {
                statMap.insert(std::make_pair(itemIdx, 1));
            }
        }

        std::string out;
        char line[1024];
        for (std::map<int, int>::iterator it = statMap.begin();
             it != statMap.end(); ++it)
        {
            memset(line, 0, sizeof(line));
            if (it == statMap.begin())
            {
                memcpy(line,
                       "    Item Index\t\t                     Item Name\t\tRarity\t\tCount\n",
                       0x3f);
                out += line;
                memset(line, 0, 0x400);
            }
            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                sprintf(line, "%14d\t\t%30s\t\t%6d\t\t%5d\n", item->get_index(),
                        item->GetItemName(), item->get_rarity(), it->second);
                out += line;
            }
        }
        printClient(GetUser(), "cltr_normal.txt", out.c_str());
    }

    // ---- 第二轮：ref+0x3c 置 1（金币基数）→ 清空结果容器后重采样 ----
    ref.m_goldBase = 1;                                   // ORIG movl $0x1,-0x120(%ebp)
    ref.m_goldFactor = 0;
    {
        WongWork::stGenerateResult_t result;
        for (int i = 0; i < 100000; ++i)
        {
            generator->generateRandomItem(ref, result);
        }

        std::map<int, int> statMap;
        for (std::vector<Inven_Item>::iterator it = result.m_dropItems.begin();
             it != result.m_dropItems.end(); ++it)
        {
            int itemIdx = it->m_addInfo;
            std::map<int, int>::iterator mit = statMap.find(itemIdx);
            if (mit != statMap.end())
            {
                ++mit->second;
            }
            else
            {
                statMap.insert(std::make_pair(itemIdx, 1));
            }
        }

        std::string out;
        char line[1024];
        for (std::map<int, int>::iterator it = statMap.begin();
             it != statMap.end(); ++it)
        {
            memset(line, 0, sizeof(line));
            if (it == statMap.begin())
            {
                memcpy(line,
                       "    Item Index\t\t                     Item Name\t\tRarity\t\tCount\n",
                       0x3f);
                out += line;
                memset(line, 0, 0x400);
            }
            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                sprintf(line, "%14d\t\t%30s\t\t%6d\t\t%5d\n", item->get_index(),
                        item->GetItemName(), item->get_rarity(), it->second);
                out += line;
            }
        }
        printClient(GetUser(), "cltr_gold.txt", out.c_str());
    }
}

// ORIG printClient 0x084b3398
// _ZN11game_master16CClearRewardTest11printClientEP5CUserPKcS4_：0x13e 包封装两段
// 字符串（文件名 + 内容）发本人。
void CClearRewardTest::printClient(CUser* user, char const* fileName, char const* content)
{
    PacketGuard packet;
    packet.put_header(0, 0x13e);
    packet.put_byte(1);
    packet.put_int(strlen(fileName));
    packet.put_str(fileName, strlen(fileName));
    packet.put_int(strlen(content));
    packet.put_str(content, strlen(content));
    packet.finalize(true);
    user->Send(packet);
}

} // namespace game_master