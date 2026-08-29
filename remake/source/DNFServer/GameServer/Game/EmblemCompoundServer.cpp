// df_game_r Game/ EmblemCompoundServer + ARAD::SCRIPT::EmblemCompound 实现。
// 逐指令转写依据 docs/class_func_reports/ARAD__SCRIPT__EmblemCompound/*.md 与
// docs/class_func_reports/EmblemCompoundServer/{EmblemCompoundServer,~EmblemCompoundServer,
// Test,Load,Destroy}.md（ORIG 地址见各函数注释）。
#include <cstdio>
#include <string>

#include "EmblemCompoundServer.h"
#include "GameTypes.h"      // cMyTrace
#include "DNFLexWrapper.h"  // loadRDARScriptFile/unloadRDARScriptFile/ScanType/
                            // ScanInt(bool*)/ScanTypeOrInt（真实符号声明）

// ---- 布局断言（ORIG 反汇编实证）----
typedef char emblem_compound_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound) == 0x84 ? 1 : -1];
typedef char emblem_compound_result_size_assert[sizeof(ARAD::SCRIPT::EmblemCompound::Result) == 0xa ? 1 : -1];
typedef char emblem_compound_server_size_assert[sizeof(EmblemCompoundServer) == 0x88 ? 1 : -1];

namespace ARAD
{
namespace SCRIPT
{

// ============================================================================
// EmblemCompound —— ORIG ctor 0x088b1408 / dtor 0x088b14c2。
// 成员按声明序自构造/自析构即与 ORIG 一致：
//   ctor：map(+0x00) → map(+0x18) → 7×vector(+0x30 起，步长 0xc)
//   dtor：逆序析构 7 个 vector → map(+0x18) → map(+0x00)
// ============================================================================
EmblemCompound::EmblemCompound()
{
}

EmblemCompound::~EmblemCompound()
{
}

// ============================================================================
// loadScript —— ORIG T 0x088b154a。
// "Etc/EmblemRenewal/" + "emblem_compound_jpn.etc"；三节：
//   [emblem compound cost]   grade value         → m_cost（重复 grade → printf+-2）
//   [emblem compound result] grade 4×short flag  → m_result（new Result(0xa)；
//                              重复 grade → delete+printf+-2）
//   [emblem list]            listIdx itemIndex   → m_list[listIdx].push_back
// 循环形态与 ORIG 相同：节内由 ScanTypeOrInt 取数值项（isType==false&&isInt==true
// 才处理）；遇 TYPE/EOF 退出后回到外层 ScanType(token,true)。查重均为
// end()→find()→operator==、命中则插入否则报错的分支形态（与 ORIG 一致）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
int EmblemCompound::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "emblem_compound_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    while (ScanType(token, true))
    {
        if (token == "[emblem compound cost]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                int value = ScanInt((bool*)0);   // ORIG +0xb6：value 先扫再查重
                std::map<int, int>::iterator endIt = m_cost.end();
                if (m_cost.find(grade) == endIt)
                    m_cost.insert(std::map<int, int>::value_type(grade, value));
                else
                {
                    printf("[emblem compound cost] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem compound result]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG：operator new(0xa) 裸分配 POD，字段直接写入（无构造调用）。
                Result* result = static_cast<Result*>(::operator new(sizeof(Result)));
                result->m_listIdx = static_cast<short>(ScanInt((bool*)0));
                result->m_itemIndex = static_cast<short>(ScanInt((bool*)0));
                result->m_itemCount = static_cast<short>(ScanInt((bool*)0));
                result->m_field6 = static_cast<short>(ScanInt((bool*)0));
                result->m_selectable = (ScanInt((bool*)0) == 1);
                std::map<int, Result*>::iterator endIt = m_result.end();
                if (m_result.find(grade) == endIt)
                    m_result.insert(std::map<int, Result*>::value_type(grade, result));
                else
                {
                    ::operator delete(result);
                    printf("[emblem compound result] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem list]")
        {
            for (;;)
            {
                int listIdx = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                unsigned long itemIndex = static_cast<unsigned long>(ScanInt((bool*)0));
                // ORIG 直接 this + listIdx*12 + 0x30 索引（无边界检查，忠实保留）。
                m_list[listIdx].push_back(itemIndex);
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// clearScript —— ORIG T 0x088b1926：清 cost map → begin/end 各取一次后遍历
// delete Result* → 清 result map。不清理 m_list（ORIG 如此）。
// ============================================================================
void EmblemCompound::clearScript()
{
    m_cost.clear();
    std::map<int, Result*>::iterator it = m_result.begin();
    std::map<int, Result*>::iterator endIt = m_result.end();
    for (; it != endIt; ++it)
        ::operator delete(it->second);
    m_result.clear();
}

// getCost —— ORIG T 0x088b19b0：find→end→operator!=，命中返回 value，未命中 -1。
int EmblemCompound::getCost(int grade)
{
    std::map<int, int>::iterator it = m_cost.find(grade);
    if (it != m_cost.end())
        return it->second;
    return -1;
}

// getResult —— ORIG T 0x88b1a14：同上形态，命中返回 Result*，未命中 0。
EmblemCompound::Result* EmblemCompound::getResult(int grade)
{
    std::map<int, Result*>::iterator it = m_result.find(grade);
    if (it != m_result.end())
        return it->second;
    return 0;
}

std::vector<unsigned long>* EmblemCompound::getList(int grade)
{
    // ORIG：先默认构造迭代器，再赋值 find 结果（帧内多出 C1 调用与拷贝）。
    std::map<int, Result*>::iterator it;
    it = m_result.find(grade);
    if (it != m_result.end())
        return &m_list[it->second->m_listIdx];
    return 0;
}

}  // namespace SCRIPT
}  // namespace ARAD

// ============================================================================
// 注：ORIG C1 中间的 ARAD::Singleton<EmblemCompoundServer>::Singleton()（W
// 0x08193636，空基类构造）未建模——与 CerashopAddRestrict::Manager 同口径，
// Singleton<T> 采用 Arad_DataManager.h 的 Meyers 模板。
// ============================================================================
EmblemCompoundServer::EmblemCompoundServer()
{
}

EmblemCompoundServer::~EmblemCompoundServer()
{
}

// Load —— ORIG T 0x081915ba：cMyTrace(pretty,0x1c7,9,true,false) +
// operator()("\t- Loading Emblem Compound script - ") + loadScript()==0。
bool EmblemCompoundServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c7, 9, true, false);
    trace("\t- Loading Emblem Compound script - ");
    return loadScript() == 0;
}

// Destroy —— ORIG T 0x0819162c：return 1。基类 ScriptInterface 槽1 为 void
// （仓库权威约定），可观察行为一致。
void EmblemCompoundServer::Destroy()
{
}

// Test —— ORIG T 0x081915b4：空（非虚成员）。
void EmblemCompoundServer::Test()
{
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetEmblemInfo        ORIG T 0x08191818（GetEmblemInfo.md 全文转写）
//   GetEmblemRandomIndex ORIG T 0x081919e8（GetEmblemRandomIndex.md 全文转写）
//   IsResultSelectable   ORIG T 0x08191ad6（IsResultSelectable.md 全文转写）
//   CheckCondition       ORIG T 0x08191636（CheckCondition.md 全文转写）
//   Process              ORIG T 0x08191afa（Process.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "MoreTypes.h"          // arad::SigAradEmblemCompound 完整定义
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item / ENUM_CMDPACKET
#include "LogManager.h"
#include "PacketGuard.h"
#include "CMailBoxHelper.h"     // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"
#include "CompoundEmblemDeps.h" // getCompoundEmblemStatistic
#include "AvatarConvertServer.h" // StatisticsEmblemCreate

bool EmblemCompoundServer::GetEmblemInfo(CUser* user,
                                         arad::SigAradEmblemCompound* sig,
                                         int idx)
{
    unsigned short slot = *reinterpret_cast<const unsigned short*>(
        reinterpret_cast<const char*>(sig) + idx * 8);
    const Inven_Item* ref = user->getCurCharacInvenR()->GetInvenRef(1, (int)slot);
    if ((ref == 0 || ref->isEmpty()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x16);
        ARAD::PRINT_ERROR::isEmpty("EMBLEM COMPOUND", user, slot);
        return false;
    }
    if (user->CheckItemLock(1, (int)slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock("EMBLEM COMPOUND", user, slot);
        return false;
    }
    CItem* item = G_CDataManager()->find_item((int)ref->m_addInfo);
    if (item == 0 || item->GetItemType() != 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x11);
        ARAD::PRINT_ERROR::find_item("EMBLEM COMPOUND", user,
                                     (int)ref->m_addInfo);
        return false;
    }
    // 写回条目 grade(+2)/itemIdx(+4)
    unsigned short* entryGrade =
        reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(sig) +
                                          idx * 8 + 2);
    int* entryItem = reinterpret_cast<int*>(reinterpret_cast<char*>(sig) +
                                            idx * 8 + 4);
    *entryGrade = (unsigned short)item->get_grade();
    *entryItem = (int)ref->m_addInfo;
    return true;
}

int EmblemCompoundServer::GetEmblemRandomIndex(int grade,
                                               unsigned short selectIdx)
{
    ARAD::SCRIPT::EmblemCompound::Result* result = m_result[grade];
    // ORIG：rand(1000) < elem+2(m_itemIndex) → 取 +0(m_listIdx)，否则 +4(m_itemCount)
    short listIdx;
    if (get_rand_int(1000) < (int)(short)result->m_itemIndex)
        listIdx = result->m_listIdx;
    else
        listIdx = result->m_itemCount;

    std::vector<unsigned long>& list = m_list[listIdx];
    if (list.empty())
        return 0;
    if ((int)result->m_selectable == 1)
        return (int)list[selectIdx];
    return (int)list[get_rand_int((int)list.size())];
}

bool EmblemCompoundServer::IsResultSelectable(int grade)
{
    ARAD::SCRIPT::EmblemCompound::Result* result = m_result[grade];
    return result->m_selectable != 0;
}

bool EmblemCompoundServer::CheckCondition(CUser* user,
                                          arad::SigAradEmblemCompound* sig)
{
    if (!GetEmblemInfo(user, sig, 0))
        return false;
    if (!GetEmblemInfo(user, sig, 1))
        return false;
    if (!(sig->m_grade1 == sig->m_grade0 && sig->m_grade0 < 7))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd8);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x1e8,
            "EMBLEM COMPOUND : [grade1:%d] [grade2:%d] [charac_no:%d]",
            (int)sig->m_grade0, (int)sig->m_grade1,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    int need = m_cost[sig->m_grade0];      // ORIG map::operator[]（未命中插入 0）
    int money = user->getCurCharacInvenR()->get_money();
    if (money < need)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 10);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x1f0,
            "EMBLEM COMPOUND : [money:%d] [need:%d] [charac_no:%d]",
            money, m_cost[sig->m_grade0],
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    return true;
}

bool EmblemCompoundServer::Process(CUser* user,
                                   arad::SigAradEmblemCompound* sig)
{
    int resultIdx = GetEmblemRandomIndex(sig->m_grade0, sig->m_selectIdx);
    if (resultIdx == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x13);
        return false;
    }
    CItem* resultItem = G_CDataManager()->find_item(resultIdx);
    if (resultItem == 0 || resultItem->GetItemType() != 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x16);
        ARAD::PRINT_ERROR::find_item("EMBLEM COMPOUND", user, resultIdx);
        return false;
    }

    int cost = m_cost[sig->m_grade0];      // ORIG map::operator[]
    CInventory* invW = user->getCurCharacInvenW();
    if (invW->use_money(cost, eMoneySubReason(0x23), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 10);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x24e,
            "EMBLEM COMPOUND : [money:%d] [need:%d] [charac_no:%d]",
            user->getCurCharacInvenR()->get_money(), m_cost[sig->m_grade0],
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    // 删除两张纹章材料（type1 slot0/slot1，reason=8）
    if (invW->delete_item(INVEN_TYPE(1), (int)sig->m_slot0, 1,
                          eItemDelReason(8), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 1);
        ARAD::PRINT_ERROR::delete_item("EMBLEM COMPOUND", user, sig->m_slot0);
        return false;
    }
    if (invW->delete_item(INVEN_TYPE(1), (int)sig->m_slot1, 1,
                          eItemDelReason(8), true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 1);
        ARAD::PRINT_ERROR::delete_item("EMBLEM COMPOUND", user, sig->m_slot1);
        return false;
    }

    // 合成结果物品：make_item → set_add_info(1) → insert(reason=9)，失败邮件补偿
    Inven_Item newItem;
    resultItem->make_item(newItem);
    newItem.set_add_info(1);
    short insertSlot = (short)invW->insertItemIntoInventory(
        newItem, eItemAddReason(9), true, true);
    if (insertSlot < 0)
    {
        const char* title =
            g_scriptStringManager_.findString(4, "game_server_msg_122", (bool*)0);
        const char* body =
            g_scriptStringManager_.findString(4, "game_server_msg_123", (bool*)0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, newItem, 0, (unsigned int)user->getCurCharacNo(), body,
            (unsigned int)std::strlen(body), 0, user->GetServerGroup(),
            false, false);
    }
    sig->m_resultIdx = resultIdx;

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x24e);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_short(sig->m_slot0);
    packet.put_short(sig->m_slot1);
    packet.put_byte(IsResultSelectable(sig->m_grade0) ? 1 : 0);
    packet.put_int(sig->m_resultIdx);
    packet.finalize(true);
    user->Send(packet);
    if (insertSlot >= 0)
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)insertSlot);

    getCompoundEmblemStatistic()->increaseCount((int)sig->m_grade0);

    std::vector<std::pair<int, int> > created;   // StatisticsEmblemCreate 入参
    created.push_back(std::make_pair((unsigned long)newItem.m_addInfo,
                                     (int)newItem.m_addInfo2));
    AvatarConvertServer::StatisticsEmblemCreate(created);
    return true;
}
