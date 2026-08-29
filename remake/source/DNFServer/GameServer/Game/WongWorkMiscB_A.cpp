// ============================================================================
// WongWorkMiscB_A.cpp — G7 杂项第二组（Avatar/Premium/Compound）
//
// 逐指令忠实转写以下 ORIG df_game_r 真实符号（定义唯一归属本 TU；
// 对应权威头为纯追加声明）：
//   WongWork::Avatar_Item::setColor(short,short)                 W 0x082fa2fa
//   WongWork::Avatar_Item::updateEmblemSocket(uint,uint)          W 0x082fa2a6
//   WongWork::Avatar_Item::~Avatar_Item() D1/D2                   W 0x082fa354
//   WongWork::CAvatarItemMgr::CAvatarItemMgr(const&)              W 0x0869937c
//   WongWork::CHandlePremium::handleReturn2CharacView             T 0x086af96e
//   WongWork::CHandlePremium::handleRemoveOneDayLethePremium      T 0x086b0594
//   WongWork::CCompoundAvatar::tagCompoundInfo::{C1,C2}           W 0x08334344
//   WongWork::CCompoundAvatar::tagCompoundInfo::clear             W 0x0833424a
//   WongWork::CCompoundAvatar::tagCompoundInfo::{D1,D2}           W 0x08334388
//   WongWork::CCompoundAvatar::_SendError                         T 0x08333a76
//   WongWork::CCompoundAvatar::_SendResult                        T 0x08333a9c
//   WongWork::CCompoundAvatar::ProcCompound                       T 0x083338c2
//
// 报告依据：docs/class_func_reports/WongWork__{CHandlePremium,CCompoundAvatar}/
// 及 objdump 反汇编（地址见各行注释）。
// ============================================================================

#include "AvatarItem.h"
#include "CAvatarItemMgr.h"
#include "CHandlePremium.h"
#include "CCompoundAvatar.h"

#include <vector>

#include "CUser.h"                  // CUser / SUserPremiumInfo / SendCmdErrorPacket
#include "CUserCharacInfo.h"        // SetOneDayLetheFlag / getCurCharacName / getCurCharacInvenR
#include "CDataManager.h"           // G_CDataManager()->getItemList()
#include "CItemList.h"             // CItemList::GetRestrictCode
#include "CPremiumLetheManager.h"  // CPremiumLetheManager::NOT_USE_LETHE_STATE
#include "GameTypes.h"             // cMyTrace
#include "GlobalData.h"            // GlobalData::s_systemTime_ (CSystemTime)
#include "CInventory.h"            // GetInvenSlot / GetAvatarItemMgrR
#include "SPremiumInfo.h"           // g_SPremiumInfo（GetAddQuestItemDropRate 表源）
#include "STStackableScript.h"      // STEnchantSystemMapKey/Data 权威定义
#include "CItemAmplifier.h"         // upgrade_table_t 权威布局（+0x48c/+0x490 视图）
#include "CStackableItem.h"         // ProcUpgradeChronicle 材料券脚本（GetEmancipate 视图）
#include "CEquipItem.h"             // m_vec198/m_vec1a4（emancipate 材料/结果表）
// ---------------------------------------------------------------------------
// CEquipItem::GetEmancipateMaterialList/GetEmancipateResultList —— ORIG 弱符号
// W 0x0854b528 / 0x0854b536：push ebp; mov esp,ebp; mov 0x8(%ebp),%eax;
// add $0x198/$0x1a4,%eax; pop ebp; ret —— 直返 &m_vec198 / &m_vec1a4（P1 视图）。
// 本 TU 独占定义（与 CEquipItem.h 声明配套），调用方 ProcUpgradeChronicle 使用。
std::vector<std::pair<int, int> >& CEquipItem::GetEmancipateMaterialList()
{
    return m_vec198;
}

std::vector<std::pair<int, int> >& CEquipItem::GetEmancipateResultList()
{
    return m_vec1a4;
}

namespace WongWork
{

// ---------------------------------------------------------------------------
// Avatar_Item — ORIG setColorEss 名称族：ORIG 真名 setColor(short,short)
// _ZN8WongWork11Avatar_Item8setColorEss W 0x082fa2fa：
//   写 packed m_color1(+0x3f)/m_color2(+0x41)。
void Avatar_Item::setColor(short color1, short color2)
{
    m_color1 = (unsigned short)color1;
    m_color2 = (unsigned short)color2;
}

// _ZN8WongWork11Avatar_Item18updateEmblemSocketEjj W 0x082fa2a6：
//   以 grade 索引 m_emblemInfo.stEmblemSocket[grade]（pack(1) 下元素 6 字节，
//   起始 +0x21）；socketType(u16)==0 → return 0；否则写入 itemIdx 并 return 1。
int Avatar_Item::updateEmblemSocket(unsigned int grade, unsigned int itemId)
{
    if (grade >= sizeof(m_emblemInfo.stEmblemSocket) /
                     sizeof(m_emblemInfo.stEmblemSocket[0]))
        return 0;
    if (m_emblemInfo.stEmblemSocket[grade].emblemSocketType_ == 0)
        return 0;
    m_emblemInfo.stEmblemSocket[grade].emblemItemIdx_ = (int)itemId;
    return 1;
}

// _ZN8WongWork11Avatar_ItemD1Ev/D2Ev W 0x082fa354：ORIG 空体。
Avatar_Item::~Avatar_Item() {}

// ---------------------------------------------------------------------------
// CAvatarItemMgr — ORIG _ZN8WongWork14CAvatarItemMgrC1ERKS0_ W 0x0869937c：
//   合成式逐成员拷贝（map×2 copy、counter、emblemInfo、map<int,int>×2），
//   附异常清理路径，由编译器生成等价物。
// ORIG W 0x0822d22c：三 map 隐式默认构造（空用户体）。
CAvatarItemMgr::CAvatarItemMgr() {}

CAvatarItemMgr::CAvatarItemMgr(const CAvatarItemMgr& other)
    : m_registeredItems(other.m_registeredItems),
      m_waitAssignItems(other.m_waitAssignItems),
      m_tempKeyCounter(other.m_tempKeyCounter),
      m_emblemInfo(other.m_emblemInfo),
      m_emblemEnduranceMap(other.m_emblemEnduranceMap),
      m_enduranceUpdateMap(other.m_enduranceUpdateMap)
{
}

// ---------------------------------------------------------------------------
// CHandlePremium::handleReturn2CharacView —— T 0x086af96e。
// 取 premium 列表(type 上限 0x270f)，逐项：type!=0 且 state==3(已过期) 时以
// handleSetUserPremium(type,0,0,0,true) 重设（链路收尾重算）。
void CHandlePremium::handleReturn2CharacView(CUser* user)
{
    std::vector<SUserPremiumInfo> list;
    user->GetPremiumInfo()->GetPremiumInfoList(list, 0x270f);
    for (std::size_t i = 0; i < list.size(); ++i)
    {
        if (list[i].m_type != 0 && list[i].m_state == 3)
            handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)list[i].m_type,
                                 0, 0, 0, true);
    }
}

// ---------------------------------------------------------------------------
// CHandlePremium::handleRemoveOneDayLethePremium —— T 0x086b0594。
// isAffectedPremium(0x21) 时撤销该 premium、取限制物品码并移除受限物品、通知；
// 无条件置非使用 Lethe 状态并 trace 角色名。
void CHandlePremium::handleRemoveOneDayLethePremium(CUser* user)
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)0x21, 0, 0, 0, true);
        CItemList* itemList = G_CDataManager()->getItemList();
        int restrictCode = itemList->GetRestrictCode(0xc5);
        user->RemoveRestrictedGoods(restrictCode);
        handleNotifyPremiumInfo(user, 0x21, 0);
    }
    user->SetOneDayLetheFlag(CPremiumLetheManager::NOT_USE_LETHE_STATE);

    static const char* __PRETTY_FUNCTION_NAME__ =
        "static void WongWork::CHandlePremium::handleRemoveOneDayLethePremium(CUser*)";
    cMyTrace trace(__PRETTY_FUNCTION_NAME__, 0x3b4, 0);
    trace("ONE_DAY_LETHE : PREMIUM_END, char(%s)", user->getCurCharacName());
}

// ---------------------------------------------------------------------------
// tagCompoundInfo 生命周期 —— clear W 0x0833424a / C1 0x08334344 / D1 0x08334388。
CCompoundAvatar::tagCompoundInfo::tagCompoundInfo()
    : m_materialA(), m_materialB(), m_avatar(), m_results()
{
}

void CCompoundAvatar::tagCompoundInfo::clear()
{
    m_field00     = 0xb;      // 表索引复位（0xb == 表元素数）
    m_field04     = 0;
    m_field08     = 0;
    m_materialA.reset();
    m_field48     = 0;
    m_materialB.reset();
    m_scriptA     = 0;
    m_scriptB     = 0;
    m_field90     = 0;
    m_avatar.reset();
    m_fieldD0     = 0;
    m_slot        = 100;      // 0x64
    m_errorFlag   = 0;
    m_fieldD6     = 0;
    m_resultSlot  = 0xffff;
    m_results.clear();
}

// D1/D2：仅析构 vector 子对象（成员隐式析构等价）。
CCompoundAvatar::tagCompoundInfo::~tagCompoundInfo() {}

// ---------------------------------------------------------------------------
// _SendError —— T 0x08333a76：SendCmdErrorPacket(user, 0x66, err)。
void CCompoundAvatar::_SendError(CUser* user, ENUM_ERROR err)
{
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, (unsigned char)err);
}

// ---------------------------------------------------------------------------
// _SendResult —— T 0x08333a9c：组 0x66 结果包。
void CCompoundAvatar::_SendResult(CUser* user, const tagCompoundInfo& info)
{
    PacketGuard packet;
    packet.put_header(1, 0x66);
    packet.put_byte(1);
    packet.put_byte(info.m_results.size() / 3);
    for (std::size_t i = 0; i < info.m_results.size(); i += 3)
    {
        packet.put_byte(info.m_results[i]);
        packet.put_short(info.m_results[i + 1]);
        packet.put_int(info.m_results[i + 2]);
    }

    // 结果槽物品与头像持久数据回填（ORIG +0x174..+0x2fe）。
    const CInventory* inven = user->getCurCharacInvenR();
    Inven_Item resultSlot =
        inven->GetInvenSlot(2, (int)(short)info.m_resultSlot);
    packet.put_int((int)(short)resultSlot.m_addInfo);

    int curSec = GlobalData::s_systemTime_.getCurSec();
    const CAvatarItemMgr* avatarMgr = inven->GetAvatarItemMgrR();
    int remainDate =
        avatarMgr->GetRemainDate(resultSlot.m_addInfo2, curSec);
    packet.put_int(remainDate);
    packet.put_int(resultSlot.m_fieldb);
    packet.put_int(0x1e);
    const char* jewelData =
        avatarMgr->getJewelSocketData(resultSlot.m_addInfo2);
    packet.put_binary(jewelData, 0x1e);

    const unsigned char* expansion =
        (const unsigned char*)avatarMgr->GetExpansionInfo(resultSlot.m_addInfo2);
    packet.put_int(4);
    if (expansion != 0)
    {
        packet.put_binary((const char*)expansion, 4);
    }
    else
    {
        ::stAvatarExpansionInfo_t emptyExpansion;
        emptyExpansion.init();
        packet.put_binary((char*)&emptyExpansion, 4);
    }

    packet.finalize(true);
    user->Send(packet);
}

// ---------------------------------------------------------------------------
// ProcCompound —— T 0x083338c2。
// Make → CheckCondition(错误抛 ENUM_ERROR) → Core(同) → Result + 统计；
// 全程捕获 ENUM_ERROR 转 _SendError。
void CCompoundAvatar::ProcCompound(CUser* user, const MSG_COMPOUND_AVATAR* msg)
{
    try
    {
        tagCompoundInfo info = _MakeCompoundInfo(user, msg);
        int error = _CheckCondition(user, info);
        if (error != 0)
            throw (ENUM_ERROR)error;
        error = _ProcCompoundCore(user, info, &m_tables[info.m_field00]);
        if (error != 0)
            throw (ENUM_ERROR)error;
        _SendResult(user, info);
        _SendDisjointAvatarStatistic(info);
    }
    catch (ENUM_ERROR e)
    {
        _SendError(user, e);
    }
}

// ---------------------------------------------------------------------------
// CUserPremium::GetGoldBonus —— ORIG _ZN8WongWork12CUserPremium12GetGoldBonusEi
// W 0x08694a64（非 const）：this+0x8bc 字节非 0 → level*level*100，否则 0。
int CUserPremium::GetGoldBonus(int level)
{
    if (((const unsigned char*)this)[0x8bc] == 0)
        return 0;
    return level * level * 100;
}

// ---------------------------------------------------------------------------
// CUserPremium::GetAddQuestItemDropRate —— ORIG …23GetAddQuestItemDropRateEi
// W 0x08694a28：level∉[0,0x69] → 0；否则 g_SPremiumInfo[level].m_field48
// （ORIG 基址 0x09483a88 = g_SPremiumInfo + 0x48，元素步长 0x120）。
int CUserPremium::GetAddQuestItemDropRate(int level)
{
    if (level < 0 || level > 0x69)
        return 0;
    return g_SPremiumInfo[level].m_field48;
}

// ---------------------------------------------------------------------------
// CItemUpgrade::find3rdChronicleEnchantData —— ORIG T 0x0854a916。
// itemScript=CDataManager::find_item(typeIdx)；空→0；map@item+0x2f4 查 key，
// 未命中→0；命中返回 &it->second（Ghidra 报告 node+0x10 语义即 second 地址）。
STEnchantSystemMapData* CItemUpgrade::find3rdChronicleEnchantData(
    STEnchantSystemMapKey key, int type)
{
    CItem* itemScript = G_CDataManager()->find_item(type);
    if (itemScript == 0)
        return 0;
    typedef std::map<STEnchantSystemMapKey, STEnchantSystemMapData> EnchantMap;
    EnchantMap& enchantMap =
        *(EnchantMap*)((char*)itemScript + 0x2f4);   // CItem 未建模成员（推断）
    EnchantMap::iterator it = enchantMap.find(key);
    if (it == enchantMap.end())
        return 0;
    return &it->second;
}

// ---------------------------------------------------------------------------
// CItemUpgrade::_getAdditionalDisjointItem(bool 版) —— ORIG T 0x08546a96。
void CItemUpgrade::_getAdditionalDisjointItem(
    unsigned int up, const CItem* itemScript,
    std::vector<std::pair<int, int> >& items,
    const upgrade_table_t& table, bool flag)
{
    (void)itemScript;
    try
    {
        const STResultItemCnt& elem = table.m_destruction.getVec().at(up);   // at 越界抛异常 → catch 吞掉并 trace
        if ((int)(short)elem.m_a != (int)up)
            return;
        int slotIdx = table.m_destruction.getField58();
        short count = flag ? (short)elem.m_c : (short)elem.m_b;     // ORIG 读合并 dword 的低/高半字
        if (count != 0)
        {
            items.push_back(std::make_pair((unsigned long)slotIdx,
                                           (unsigned int)(int)count));
        }
    }
    catch (...)
    {
        static const char* PRETTY =
            "void WongWork::CItemUpgrade::_getAdditionalDisjointItem(unsigned int, "
            "CItem const*, std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&, "
            "upgrade_table_t const&, bool)";
        cMyTrace trace(PRETTY, 0x121, 5);
        trace("3RD_CHRONICLE : OUT OF RANGE!!");
    }
}

// ---------------------------------------------------------------------------
// CItemUpgrade::ProcUpgradeChronicle —— ORIG T 0x085491d6
// _ZN8WongWork12CItemUpgrade20ProcUpgradeChronicleEP5CUsersjsjcRSt6vectorIiSaIiEE
// 三代编年史升级：A 槽材料券 + B 槽目标装备；扣 emancipate 材料、按
// chanceGrade 抽 equipGrade 样本、构造结果物品删旧插新。错误码 0x16/0x11。
int CItemUpgrade::ProcUpgradeChronicle(CUser* user, short slotA, unsigned int,
                                       short slotB, unsigned int,
                                       char chanceGrade, std::vector<int>& out)
{
    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item ticket = invenW->GetInvenSlot(1, slotA);
    Inven_Item target = invenW->GetInvenSlot(1, slotB);

    CStackableItem* ticketScript =
        (CStackableItem*)G_CDataManager()->find_item(ticket.m_addInfo);
    CEquipItem* targetScript =
        (CEquipItem*)G_CDataManager()->find_item(target.m_addInfo);

    // CEquipItem::GetEmancipateMaterialList W 0x854b528（本 TU 独占定义）＝ &m_vec198。
    const std::vector<std::pair<int, int> >& materials =
        targetScript->GetEmancipateMaterialList();

    if (!user->getCurCharacInvenR()->CheckNeedItemList(materials))
        return 0x16;

    {
        // ORIG：局部 CInventory 阴影 + InventoryMemory；失败路径仅析构阴影后
        // 返回（栈对象自动完成）。ORIG 阴影区声明于帧深 0xca80。
        InventoryMemory shadowMem;
        CInventory shadow;
        shadow.SetInventoryMemory(&shadowMem);
        shadow.setCopy(*user->getCurCharacInvenR());
        if (!shadow.tryDeleteEventItems(materials))
            return 0x11;

        bool bFound = false;
        invenW->delete_event_items(materials, out, bFound,
                                   (eMoneySubReason)0xc, (eItemDelReason)0xc,
                                   false);

        // chanceGrade ∈ [0,4] → CDataManager::m_equipGrade.m_gradeN[r]
        // （5 个 vector<unsigned int> @ +0x4d5c，步长 0xc）随机取样本值，
        // 写入结果物品 m_addInfo2（ORIG uStack_110 = local_30）。
        unsigned int sample = 0;
        int r = get_rand_int(100);
        if (r >= 0 && r < 100 && chanceGrade >= 0 && chanceGrade < 5)
        {
            stEquipmentGradeSample& grade = G_CDataManager()->m_equipGrade;
            switch (chanceGrade)
            {
            case 0: sample = grade.m_grade0[r]; break;
            case 1: sample = grade.m_grade1[r]; break;
            case 2: sample = grade.m_grade2[r]; break;
            case 3: sample = grade.m_grade3[r]; break;
            default: sample = grade.m_grade4[r]; break;
            }
        }

        // CStackableItem::GetEmancipate W 0x854b4e8（弱内联）＝ movzwl m_short2ee。
        int emancipate = (int)ticketScript->m_short2ee;
        // stEquipConditionForChronicle 现为空壳占位；ORIG 输出布局：
        // +0 upgrade、+1/+2 两标志字节、+4 u16 强化选项 value。按该布局提供
        // 存储，由 CInventory::CheckEquipmentItemCondition 填充后回读。
        unsigned char condBuf[0x10];
        for (int i = 0; i < 0x10; ++i)
            condBuf[i] = 0;
        stEquipConditionForChronicle& cond =
            *(stEquipConditionForChronicle*)condBuf;

        if (emancipate == 1)
        {
            // ORIG：err = CheckEquipmentItemCondition(...)；err != 0 → 携该码退出。
            int condErr = user->getCurCharacInvenR()->CheckEquipmentItemCondition(
                (INVEN_TYPE)1, slotB, cond);
            if (condErr != 0)
                return condErr;   // shadow 析构自动完成
        }

        // CEquipItem::GetEmancipateResultList W 0x854b536（本 TU 独占定义）＝ &m_vec1a4。
        const std::vector<std::pair<int, int> >& results =
            targetScript->GetEmancipateResultList();
        int err = 0x11;
        do
        {
            if (results.size() >= 2)
                break;
            int resultIdx = results.begin()->first;
            int resultCnt = results.begin()->second;
            if (resultCnt >= 2)
                break;
            if (invenW->GetItemType(resultIdx) != 1)   // 必须是装备类
                break;

            Inven_Item newItem;
            *(unsigned int*)((char*)&newItem + 0x02) =
                (unsigned int)resultIdx;               // ORIG：idx 双半字写入 m_addInfo
            targetScript->make_item(newItem);          // vtable+0x08 虚槽（make_item）

            if (emancipate == 1)
            {
                if (condBuf[0] <= 0x1e)
                    newItem.SetUpgrade(condBuf[0]);
                if (condBuf[1] != 0 && condBuf[2] != 0)
                {
                    // stAmplifyOption_t::assign W：item.m_amp(+0x11) 选项写入
                    stAmplifyOption_t& amp =
                        *(stAmplifyOption_t*)((char*)&newItem + 0x11);
                    amp.assign(condBuf[2],
                               (unsigned short)(condBuf[4] |
                                                (condBuf[5] << 8)));
                }
            }
            newItem.set_add_info((int)sample);         // +0x07 ← 随机样本值

            invenW->delete_item((INVEN_TYPE)1, slotB, 1,
                                (eItemDelReason)0xc, true);
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 slotB);
            invenW->delete_item((INVEN_TYPE)1, slotA, 1,
                                (eItemDelReason)0xc, true);
            invenW->insert_item_special_slot(newItem, 1, slotB,
                                             (eMoneyAddReason)7,
                                             (eItemAddReason)9);
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 slotB);
            err = 0;
        } while (0);
        return err;   // ORIG：其余失败路径统一 0x11（shadow 析构自动完成）
    }
}

}  // namespace WongWork

