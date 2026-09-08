// ============================================================================
// expert_extraction 域 —— ORIG 19 符号实现（报告
// docs/class_func_reports/expert_extraction/ 与 expert_extraction__C* 各子目录）。
// 类声明见 CExtraction.h / CAlchemistExtraction.h / CEnchanterExtraction.h /
// CDollControllerExtraction.h / CExtractionMgr.h（ctor/dtor 定义 GlobalData.cpp）。
// vtable 槽序（vptr 指向 vtable+8，ctor vptr 赋值 + extraction 族虚调用实测）：
//   +0x08 extraction / +0x0c check_error / +0x10 inven_process /
//   +0x14 _check_need_slot / +0x18 get_extraction_count / +0x1c _check_error
// ============================================================================

#include "CExtraction.h"
#include "CAlchemistExtraction.h"
#include "CEnchanterExtraction.h"
#include "CDollControllerExtraction.h"
#include "CExtractionMgr.h"

#include "CUser.h"          // CUserCharacInfo / CUser::CheckItemLock / InventoryMemory
#include "CInventory.h"     // CInventory / CSecu_ProtectionField / GlobalData::s_pSecuProtectionField
#include "CDataManager.h"   // G_CDataManager / find_item / GetExpertJobScript
#include "CItem.h"
#include "CEquipItem.h"     // CEquipItem::m_itemType（+0x234）
#include "CStackableItem.h" // m_int334/+0x334、m_int338/+0x338、m_int33c/+0x33c
#include "STExpertJobScript.h"
#include "STItemScript.h"   // ENUM_EXPERT_JOB_TYPE
#include "GameWorld.h"      // G_GameWorld / IsPVPChannel
#include "GameTypes.h"      // cMyTrace / LogManager::logFormat
#include "PvPTypes.h"       // CMTRand::randInt

#include <utility>

namespace expert_extraction
{

// ============================================================================
// GetExtracterType —— ORIG T 0x084a2904（报告 GetExtracterType.md）
//   专业职业类型 → 提取器索引：2→0(Alchemist)/4→2(DollController)/1→1(Enchanter)/其它→3
// ============================================================================
int GetExtracterType(ENUM_EXPERT_JOB_TYPE job)
{
    switch (job)
    {
    case (ENUM_EXPERT_JOB_TYPE)2:
        return 0;
    case (ENUM_EXPERT_JOB_TYPE)4:
        return 2;
    case (ENUM_EXPERT_JOB_TYPE)1:
        return 1;
    default:
        return 3;
    }
}

// ============================================================================
// CExtraction::_check_need_slot —— ORIG T 0x084a29f4
//   两个空槽位（ITEM_TYPE 10 与 0）任一存在即可。
// ============================================================================
bool CExtraction::_check_need_slot(CUser* user)
{
    const CInventory* inven = user->getCurCharacInvenR();
    int s1 = inven->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
    int s2 = inven->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
    return !(s1 < 0 && s2 < 0);
}

// ============================================================================
// CExtraction::check_error —— ORIG T 0x084a2a5e（报告 check_error.md）
//   PVP 频道拒 0x60；职业不符 0x13；材料1 必须存在且可堆叠（0x11）；
//   材料2（被提取装备）：非头像/非生物/类型合法（0x11）、稀有度==2（0x8c）、
//   已鉴定（0xae）、可分解（0x17）、非绑定（0x11）、未锁（0xd5）、
//   安全保护 CheckItemRarity（0x28）返回其错误码。
// ============================================================================
// [b28d-54 N] ORIG 0x084a2a5e：`cmpl $imm,mem` vs ORIG 载寄存器后比较（P209）
//   + 实参装载序（P23/P75）；调用序列全等（180 vs 181）。
int CExtraction::check_error(CUser* user, char a, short b, short c)
{
    if (G_GameWorld()->IsPVPChannel())
    {
        return 0x60;
    }
    if (user->GetCurCharacExpertJobType() != (int)a)
    {
        return 0x13;
    }
    // [b23S3C] P48 去缓存 + P0 律：ORIG 0x084a2a74/0x84a2b47 两次独立 call
    // getCurCharacInvenR、返回值免槽直落实参（`mov %eax,0x4(%esp)`），原码无
    // invenR 具名局部（该局部使 c1/c2 槽整体下移 4，与 ORIG -0x18/-0x14 错位）。
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* c1 = G_CDataManager()->find_item(item1.m_addInfo);
    if (c1 == 0)
    {
        return 0x11;
    }
    if (!c1->is_stackable())
    {
        return 0x11;
    }
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)c);
    CItem* c2 = G_CDataManager()->find_item(item2.m_addInfo);
    if (c2 == 0)
    {
        return 0x11;
    }
    if (c2->IsAvatarItem())
    {
        return 0x11;
    }
    if (c2->IsCreatureItem())
    {
        return 0x11;
    }
    if (c2->GetItemType() == 0xb)
    {
        return 0x11;
    }
    if (c2->get_rarity() != 2)
    {
        return 0x8c;
    }
    if (!item2.m_amp.isIdentified())
    {
        return 0xae;
    }
    if (c2->IsImpossibleDisjoint())
    {
        return 0x17;
    }
    // [b23S3C] P0 律：ORIG 0x084a2c6b `mov %eax,-0x10(%ebp)` + 回读比较 ⇒ 具名
    // attachType 局部（缺它整帧少 4 字节：c1/c2 落 -0x14/-0x10、两个 Inven_Item
    // sret 结构落 -0x51/-0x8e，与 ORIG -0x55/-0x92 全帧错位）。
    int attachType = c2->GetAttachType();
    if (attachType == 2)
    {
        return 0x11;
    }
    if (user->CheckItemLock(1, (int)c))
    {
        return 0xd5;
    }
    int r = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28, c2, &item2);
    if (r != 0)
    {
        return r;
    }
    return 0;
}

// ============================================================================
// CExtraction::inven_process —— ORIG T 0x084a2cec（报告 inven_process.md）
//   副本背包试删材料 → find_item 构造新物品（make_item + set_add_info + 物品索引）
//   → 副本试插（0x35,0,1）；成功后真实背包删材料再插入，outSlots 输出
//   {插入槽, 物品索引, add_info}。
// ============================================================================
// [b28d-54 N] ORIG 0x084a2cec：帧/槽平移（N82）+ 模板实例槽（N86）；调用序列全等。
bool CExtraction::inven_process(CUser* user, short param,
                                std::vector<std::pair<int, int> >& needItems,
                                std::vector<int>& outSlots)
{
    CInventory* invenW = user->getCurCharacInvenW();
    InventoryMemory mem;
    CInventory copy;
    copy.SetInventoryMemory(&mem);
    copy.setCopy(*(const CInventory*)user->getCurCharacInvenRefR());
    if (copy.delete_item((INVEN_TYPE)1, (int)param, 1,
                         (eItemDelReason)0x2d, 0) == 0)
    {
        return false;
    }
    int first = needItems[0].first;
    int second = needItems[0].second;
    CItem* item = G_CDataManager()->find_item(first);
    Inven_Item newItem;
    item->make_item(newItem);
    newItem.set_add_info(second);
    newItem.m_addInfo = first;
    int slot1 = copy.insertItemIntoInventory(newItem, (eItemAddReason)0x35, 0, 1);
    if (slot1 == -1)
    {
        return false;
    }
    invenW->delete_item((INVEN_TYPE)1, (int)param, 1,
                        (eItemDelReason)0x2d, 1);
    int slot2 = invenW->insertItemIntoInventory(newItem, (eItemAddReason)0x35, 1, 1);
    outSlots.clear();
    outSlots.push_back(slot2);
    outSlots.push_back(first);
    outSlots.push_back(second);
    return true;
}

// ============================================================================
// CExtractionMgr::get_extracter —— ORIG T 0x084a3166（报告 get_extracter.md）
// ============================================================================
CExtraction* CExtractionMgr::get_extracter(unsigned int index)
{
    if (index > 2)
    {
        return 0;
    }
    return m_slots[index];
}

// ============================================================================
// CAlchemistExtraction::get_alchemist_extraction_type —— ORIG T 0x084a355c
//   m_itemType(+0x234) 跳表：10→2；0xc..0x10→0；0x11..0x15→1；其它→3
// ============================================================================
int CAlchemistExtraction::get_alchemist_extraction_type(CEquipItem const* equip)
{
    switch (equip->m_itemType)
    {
    case 10:
        return 2;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
        return 0;
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
        return 1;
    default:
        return 3;
    }
}

// ============================================================================
// CAlchemistExtraction::get_extraction_count —— ORIG T 0x084a3596
//   grade / 5，0 → 1。
// ============================================================================
int CAlchemistExtraction::get_extraction_count(Inven_Item item)
{
    CItem* c = G_CDataManager()->find_item(item.m_addInfo);
    int g = c->get_grade() / 5;
    if (g == 0)
    {
        g = 1;
    }
    return g;
}

// ============================================================================
// CAlchemistExtraction::_check_error —— ORIG T 0x084a35f2
//   材料1（槽 a）的 +0x334 等级字段 → 脚本允许等级；材料2（槽 b）的
//   usableLevel（0 时 grade）；超出 → 0xd。
// ============================================================================
// [b28d-54 N] ORIG 0x084a35f2：4.4 槽复用（mat1/mat2/limit 压缩，帧 -4，
//   N82/N95）+ 装载序（P23）。三派生类同族（Enchanter 0x084a3b92 /
//   DollController 0x084a4286 同形）。语义等价。
int CAlchemistExtraction::_check_error(CUser* user, short a, short b)
{
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)a);
    CStackableItem* mat1 = (CStackableItem*)G_CDataManager()->find_item(item1.m_addInfo);
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* mat2 = G_CDataManager()->find_item(item2.m_addInfo);
    int limit = ((STExpertJobScript*)G_CDataManager()->GetExpertJobScript(2))
                    ->GetAlchemistExtractionPossibleLevel(mat1->m_int334);
    int need = mat2->getUsableLevel();
    if (need == 0)
    {
        need = mat2->get_grade();
    }
    if (limit < need)
    {
        return 0xd;
    }
    return 0;
}

// ============================================================================
// CAlchemistExtraction::check_error —— ORIG T 0x084a3716
// b28d-54 判词：ORIG 0x084a3716 尾部 `call *%ecx; mov %eax,-0xc(%ebp);
//   mov -0xc(%ebp),%eax`（槽往返）⇒ 原码有具名局部 `e = _check_error(...); return e;`
//   （P0：ORIG 有槽往返 ⇒ 必须声明局部；直接 `return f()` 丢 store/load 对）。
// ============================================================================
int CAlchemistExtraction::check_error(CUser* user, char a, short b, short c)
{
    int e = CExtraction::check_error(user, a, b, c);
    if (e != 0)
    {
        return e;
    }
    if (!_check_need_slot(user))
    {
        return 4;
    }
    e = _check_error(user, b, c);
    return e;
}

// ============================================================================
// CAlchemistExtraction::extraction —— ORIG T 0x084a3182（报告 extraction.md）
//   按结果表随机选一条目（CMTRand），命中则 get_extraction_count 作数量，
//   needItems 输出 {itemId, count}。
// ============================================================================
// [b28d-54 N] ORIG 0x084a3182：帧/槽平移（N82）+ 模板/寄存器角色（N7/N86）。
bool CAlchemistExtraction::extraction(CUser* user, short param,
                                      std::vector<std::pair<int, int> >& needItems)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(1, (int)param);
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(2);
    STExpertJobScript::stAlchemistExtractionResultInfo* info =
        script->GetAlchemistExtractionResultInfo();
    int type = get_alchemist_extraction_type(equip);
    if (type == 3)
    {
        LogManager::logFormat(1, "Extraction.cpp", __PRETTY_FUNCTION__, 0xe0,
                              "CAlchemistExtraction::extraction ERROR : result_type(%d)", type);
        return false;
    }
    int sum = info->m_count[type];
    unsigned long range = (unsigned long)(sum - 1);
    int pick = m_rand->randInt(range) + 1;
    std::vector<STExpertJobScript::stMonsterCardBindResult>& vec = info->m_vec[type];
    if (vec.size() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xec, 5);
        trace("CAlchemistExtraction::extraction ERROR : size(%d)", (int)vec.size());
        return false;
    }
    if (vec.size() != 0 && sum == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xf1, 5);
        trace("CAlchemistExtraction::extraction ERROR : size(%d), sum(%d)",
              (int)vec.size(), sum);
        return false;
    }
    int acc = 0;
    unsigned int i = 0;
    while (i < vec.size())
    {
        if (acc < pick &&
            acc + (int)(unsigned short)vec[i].m_field4 >= pick)
        {
            int count = get_extraction_count(item);
            needItems.push_back(std::make_pair(vec[i].m_field0, (int)count));
            break;
        }
        acc += (int)(unsigned short)vec[i].m_field4;
        ++i;
    }
    if (needItems.empty())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x102, 5);
        trace("CAlchemistExtraction::extraction ERROR : result is empty");
        return false;
    }
    return true;
}

// ============================================================================
// CEnchanterExtraction::get_extraction_count —— ORIG T 0x084a3b34：grade / 10。
// ============================================================================
int CEnchanterExtraction::get_extraction_count(Inven_Item item)
{
    CItem* c = G_CDataManager()->find_item(item.m_addInfo);
    int g = c->get_grade() / 10;
    if (g == 0)
    {
        g = 1;
    }
    return g;
}

// ============================================================================
// CEnchanterExtraction::_check_error —— ORIG T 0x084a3b92（+0x338 等级字段）
// ============================================================================
// [b28d-54 N] ORIG 0x084a3b92：同 CAlchemistExtraction::_check_error（N82/N95 槽复用）。
int CEnchanterExtraction::_check_error(CUser* user, short a, short b)
{
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)a);
    CStackableItem* mat1 = (CStackableItem*)G_CDataManager()->find_item(item1.m_addInfo);
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* mat2 = G_CDataManager()->find_item(item2.m_addInfo);
    int limit = ((STExpertJobScript*)G_CDataManager()->GetExpertJobScript(1))
                    ->GetEnchanterExtractionPossibleLevel(mat1->m_int338);
    int need = mat2->getUsableLevel();
    if (need == 0)
    {
        need = mat2->get_grade();
    }
    if (limit < need)
    {
        return 0xd;
    }
    return 0;
}

// ============================================================================
// CEnchanterExtraction::check_error —— ORIG T 0x084a3cb6
// b28d-54 判词：同 CAlchemistExtraction::check_error（ORIG 尾部槽往返 ⇒ 具名局部）。
// ============================================================================
int CEnchanterExtraction::check_error(CUser* user, char a, short b, short c)
{
    int e = CExtraction::check_error(user, a, b, c);
    if (e != 0)
    {
        return e;
    }
    if (!_check_need_slot(user))
    {
        return 4;
    }
    e = _check_error(user, b, c);
    return e;
}

// ============================================================================
// CEnchanterExtraction::extraction —— ORIG T 0x084a37c6
//   无 result_type 分支（Enchanter 单结果表）。
// ============================================================================
// [b28d-54 N] ORIG 0x084a37c6：帧/槽平移（N82）+ 模板/寄存器角色（N7/N86）。
bool CEnchanterExtraction::extraction(CUser* user, short param,
                                      std::vector<std::pair<int, int> >& needItems)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(1, (int)param);
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(1);
    STExpertJobScript::stEnchanterExtractionResultInfo* info =
        script->GetEnchanterExtractionResultInfo();
    int sum = info->m_count;
    unsigned long range = (unsigned long)(sum - 1);
    int pick = m_rand->randInt(range) + 1;
    std::vector<STExpertJobScript::stMonsterCardBindResult>& vec = info->m_vec;
    if (vec.size() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x169, 5);
        trace("CEnchanterExtraction::extraction ERROR : size(%d)", (int)vec.size());
        return false;
    }
    if (vec.size() != 0 && sum == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x16e, 5);
        trace("CEnchanterExtraction::extraction ERROR : size(%d), sum(%d)",
              (int)vec.size(), sum);
        return false;
    }
    int acc = 0;
    unsigned int i = 0;
    while (i < vec.size())
    {
        if (acc < pick &&
            acc + (int)(unsigned short)vec[i].m_field4 >= pick)
        {
            int count = get_extraction_count(item);
            needItems.push_back(std::make_pair(vec[i].m_field0, (int)count));
            break;
        }
        acc += (int)(unsigned short)vec[i].m_field4;
        ++i;
    }
    if (needItems.empty())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x17f, 5);
        trace("CEnchanterExtraction::extraction ERROR : result is empty");
        return false;
    }
    return true;
}

// ============================================================================
// CDollControllerExtraction::get_doll_controller_extraction_type —— ORIG T 0x084a3d66
//   usableLevel（0 时 grade）区间：1..0x1e→0；0x1f..0x32→1；0x33..0x50→2；其它→3
// ============================================================================
int CDollControllerExtraction::get_doll_controller_extraction_type(CEquipItem const* equip)
{
    int lv = equip->getUsableLevel();
    if (lv == 0)
    {
        lv = equip->get_grade();
    }
    if (lv >= 1 && lv <= 0x1e)
    {
        return 0;
    }
    if (lv >= 0x1f && lv <= 0x32)
    {
        return 1;
    }
    if (lv >= 0x33 && lv <= 0x50)
    {
        return 2;
    }
    return 3;
}

// ============================================================================
// CDollControllerExtraction::get_extraction_count —— ORIG T 0x084a41a8
//   按类型：0→grade/10；1→(grade-0x14)/10；2→(grade-0x28)/10；0 → 1。
// ============================================================================
// [b28d-54 N] ORIG 0x084a41a8：帧/槽平移（N82）+ 寄存器角色（N7）。
int CDollControllerExtraction::get_extraction_count(Inven_Item item)
{
    CItem* c = G_CDataManager()->find_item(item.m_addInfo);
    int type = get_doll_controller_extraction_type((CEquipItem*)c);
    int g = 0;
    switch (type)
    {
    case 1:
        g = (c->get_grade() - 0x14) / 10;
        break;
    case 2:
        g = (c->get_grade() - 0x28) / 10;
        break;
    case 0:
        g = c->get_grade() / 10;
        break;
    default:
        break;
    }
    if (g == 0)
    {
        g = 1;
    }
    return g;
}

// ============================================================================
// CDollControllerExtraction::_check_error —— ORIG T 0x084a4286（+0x33c 等级字段）
// ============================================================================
// [b28d-54 N] ORIG 0x084a4286：同 CAlchemistExtraction::_check_error（N82/N95 槽复用）。
int CDollControllerExtraction::_check_error(CUser* user, short a, short b)
{
    Inven_Item item1 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)a);
    CStackableItem* mat1 = (CStackableItem*)G_CDataManager()->find_item(item1.m_addInfo);
    Inven_Item item2 = user->getCurCharacInvenR()->GetInvenSlot(1, (int)b);
    CItem* mat2 = G_CDataManager()->find_item(item2.m_addInfo);
    int limit = ((STExpertJobScript*)G_CDataManager()->GetExpertJobScript(4))
                    ->GetDollControllerExtractionPossibleLevel(mat1->m_int33c);
    int need = mat2->getUsableLevel();
    if (need == 0)
    {
        need = mat2->get_grade();
    }
    if (limit < need)
    {
        return 0xd;
    }
    return 0;
}

// ============================================================================
// CDollControllerExtraction::check_error —— ORIG T 0x084a43aa
// b28d-54 判词：同 CAlchemistExtraction::check_error（ORIG 尾部槽往返 ⇒ 具名局部）。
// ============================================================================
int CDollControllerExtraction::check_error(CUser* user, char a, short b, short c)
{
    int e = CExtraction::check_error(user, a, b, c);
    if (e != 0)
    {
        return e;
    }
    if (!_check_need_slot(user))
    {
        return 4;
    }
    e = _check_error(user, b, c);
    return e;
}

// ============================================================================
// CDollControllerExtraction::extraction —— ORIG T 0x084a3dce
// ============================================================================
// [b28d-54 N] ORIG 0x084a3dce：帧/槽平移（N82）+ 模板/寄存器角色（N7/N86）。
bool CDollControllerExtraction::extraction(CUser* user, short param,
                                           std::vector<std::pair<int, int> >& needItems)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(1, (int)param);
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(4);
    STExpertJobScript::stDollControllerResultInfo* info =
        script->GetDollControllerExtractionResultInfo();
    int type = get_doll_controller_extraction_type(equip);
    if (type == 3)
    {
        LogManager::logFormat(1, "Extraction.cpp", __PRETTY_FUNCTION__, 0x1d0,
                              "CDollControllerExtraction::extraction ERROR : result_type(%d)", type);
        return false;
    }
    int sum = info->m_count[type];
    unsigned long range = (unsigned long)(sum - 1);
    int pick = m_rand->randInt(range) + 1;
    std::vector<STExpertJobScript::stDollControllerResult>& vec = info->m_vec[type];
    if (vec.size() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1dc, 5);
        trace("CDollControllerExtraction::extraction ERROR : size(%d)", (int)vec.size());
        return false;
    }
    if (vec.size() != 0 && sum == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1e1, 5);
        trace("CDollControllerExtraction::extraction ERROR : size(%d), sum(%d)",
              (int)vec.size(), sum);
        return false;
    }
    int acc = 0;
    unsigned int i = 0;
    while (i < vec.size())
    {
        if (acc < pick &&
            acc + (int)vec[i].m_field8 >= pick)
        {
            int count = get_extraction_count(item);
            needItems.push_back(std::make_pair(vec[i].m_field0, (int)count));
            break;
        }
        acc += (int)vec[i].m_field8;
        ++i;
    }
    if (needItems.empty())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1f2, 5);
        trace("CDollControllerExtraction::extraction ERROR : result is empty");
        return false;
    }
    return true;
}

}  // namespace expert_extraction
