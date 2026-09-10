// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkMiscB_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "WongWorkMiscB_A/GetEmancipateMaterialList.inc"
{
    return m_vec198;
}

#include "WongWorkMiscB_A/GetEmancipateResultList.inc"
{
    return m_vec1a4;
}

namespace WongWork
{

// ---------------------------------------------------------------------------
// Avatar_Item — ORIG setColorEss 名称族：ORIG 真名 setColor(short,short)
// _ZN8WongWork11Avatar_Item8setColorEss W 0x082fa2fa：
//   写 packed m_color1(+0x3f)/m_color2(+0x41)。
#include "WongWorkMiscB_A/setColor.inc"
{
    m_color1 = color1;
    m_color2 = color2;
}

// _ZN8WongWork11Avatar_Item18updateEmblemSocketEjj W 0x082fa2a6：
//   以 grade 索引 m_emblemInfo.stEmblemSocket[grade]（pack(1) 下元素 6 字节，
//   起始 +0x21）；socketType(u16)==0 → return 0；否则写入 itemIdx 并 return 1。
#include "WongWorkMiscB_A/updateEmblemSocket.inc"
{
    // [b28d-solo-WongWorkMiscB_A] P248 档③ 同型化：ORIG 写段为 LHS-first
    // `lea 0x20(%eax),%edx; mov 0x10(%ebp),%eax; mov %eax,0x3(%edx)`；
    // 字段 int + 形参 unsigned 在 4.4 下被判异型 ⇒ RHS-first + `push %ebx`
    // （探针 fA/fJ）。改用 GamePODTypes.h 新增的 unsigned 同址视角
    // `emblemItemIdxU_`（探针 fU 逐条复现，含 `leal 0x20(%eax),%edx`）。
    if (m_emblemInfo.stEmblemSocket[grade].emblemSocketType_ == 0)
        return 0;
    m_emblemInfo.stEmblemSocket[grade].emblemItemIdxU_ = itemId;
    return 1;
}

// _ZN8WongWork11Avatar_ItemC1Ev/C2Ev W 0x082fa34e：ORIG 空体（nm 实证重建遗漏）。
#include "WongWorkMiscB_A/Avatar_Item.inc"

// _ZN8WongWork11Avatar_ItemD1Ev/D2Ev W 0x082fa354：ORIG 空体。
#include "WongWorkMiscB_A/dtor_Avatar_Item.inc"

// ---------------------------------------------------------------------------
// CAvatarItemMgr — ORIG _ZN8WongWork14CAvatarItemMgrC1ERKS0_ W 0x0869937c：
//   合成式逐成员拷贝（map×2 copy、counter、emblemInfo、map<int,int>×2），
//   附异常清理路径，由编译器生成等价物。
// ORIG W 0x0822d22c：三 map 隐式默认构造 + Reset（ORIG 汇编 0x822d26f call Reset）。
#include "WongWorkMiscB_A/CAvatarItemMgr.inc"
{
    Reset();
}

#include "WongWorkMiscB_A/CAvatarItemMgr_1.inc"
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
#include "WongWorkMiscB_A/handleReturn2CharacView.inc"
{
    std::vector<SUserPremiumInfo> list =
        user->GetPremiumInfo()->GetPremiumInfoList(0x270f);
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
// [b28d-solo-WongWorkMiscB_A] 两处修正：
//   ① `getItemList()` 为头内 inline，4.4 -O0 仍发 out-of-line call（ORIG 4.1 内联为
//      `mov 0xc(%eax),%eax`）⇒ 调用点改直访公有成员 `m_itemList`（p167 EXTRA
//      `CDataManager::getItemList()` 归零）。
//   ② ORIG 尾段为 P227 全表达式临时链：`call getCurCharacName; mov %eax,%ebx;`
//      跨 cMyTrace ctor 保活，ctor 后才压 op() 实参 ⇒ 具名 trace 改临时链 +
//      `__PRETTY_FUNCTION__`（P139：ORIG `_ZZ…E19__PRETTY_FUNCTION__`）。
#include "WongWorkMiscB_A/handleRemoveOneDayLethePremium.inc"
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)0x21, 0, 0, 0, true);
        user->RemoveRestrictedGoods(
            G_CDataManager()->m_itemList->GetRestrictCode(0xc5));
        handleNotifyPremiumInfo(user, 0x21, 0);
    }
    user->SetOneDayLetheFlag(CPremiumLetheManager::NOT_USE_LETHE_STATE);

    cMyTrace(__PRETTY_FUNCTION__, 0x3b4, 0)(
        "ONE_DAY_LETHE : PREMIUM_END, char(%s)", user->getCurCharacName());
}

// ---------------------------------------------------------------------------
// tagCompoundInfo 生命周期 —— clear W 0x0833424a / C1 0x08334344 / D1 0x08334388。
#include "WongWorkMiscB_A/tagCompoundInfo.inc"
    : m_materialA(), m_materialB(), m_avatar(), m_results()
{
}

#include "WongWorkMiscB_A/clear.inc"
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
    m_type        = 0;
    m_resultSlot  = 0xffff;
    m_results.clear();
}

// D1/D2：仅析构 vector 子对象（成员隐式析构等价）。
#include "WongWorkMiscB_A/dtor_tagCompoundInfo.inc"

// ---------------------------------------------------------------------------
// _SendError —— T 0x08333a76：SendCmdErrorPacket(user, 0x66, err)。
#include "WongWorkMiscB_A/_SendError.inc"
{
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, (unsigned char)err);
}

// ---------------------------------------------------------------------------
// _SendResult —— T 0x08333a9c：组 0x66 结果包。
// [b28d-solo-WongWorkMiscB_A] 226=226 全等（p167 44/44 CLEAR）。修法：
//   ①结果循环改迭代器形（P214④/P28：begin/end 头部预求值 + while 尾测 +
//     三次后置 `*it++` ⇒ ORIG ppEi×3 + de×3）；
//   ②去 inven/avatarMgr/jewelData/remainDate 具名局部（P52/P183：ORIG 每处重调
//     getCurCharacInvenR，GetAvatarItemMgrR 内联在链中，实参升格 callee-saved）；
//   ③`resultSlot.m_addInfo` 去 `(int)(short)` 冗余 cast（unsigned int 直传）；
//   ④返回类型 void→int（P159：ORIG 尾 `mov $0x1,%ebx; …; mov %ebx,%eax`）；
//   ⑤else 分支 put_binary 实参 (char*)→(const char*)（p167 MISSING/EXTRA 各 1 归零）。
#include "WongWorkMiscB_A/_SendResult.inc"
{
    PacketGuard packet;
    packet.put_header(1, 0x66);
    packet.put_byte(1);
    packet.put_byte(info.m_results.size() / 3);
    // [b28d-solo-WongWorkMiscB_A] P214④/P28：ORIG 为迭代器形——begin/end 头部
    // 预求值（-0x2c/-0x30 双槽）+ while 尾测 + 三次后置 `it++`（ppEi 带独立 sret
    // 临时槽 -0x1c/-0x18/-0x14）；索引形 `i += 3` 会换成 size()+下标链。
    std::vector<int>::const_iterator it = info.m_results.begin();
    std::vector<int>::const_iterator itEnd = info.m_results.end();
    while (it != itEnd)
    {
        packet.put_byte(*it++);
        packet.put_short(*it++);
        packet.put_int(*it++);
    }

    // 结果槽物品与头像持久数据回填（ORIG +0x174..+0x2fe）。
    // [b28d-solo-WongWorkMiscB_A] P52/P183：ORIG 无 inven/avatarMgr/jewelData/
    // remainDate 具名局部——`getCurCharacInvenR` 每处重调（4 次）、`GetAvatarItemMgrR`
    // 内联在链中（实参先物化 callee-saved）；`resultSlot.m_addInfo` 为 unsigned int
    // 直传（ORIG `mov -0x6f(%ebp),%eax` 单段，无 cwtl）。
    Inven_Item resultSlot =
        user->getCurCharacInvenR()->GetInvenSlot(2, (int)(short)info.m_resultSlot);
    // ORIG 0x8333c46..0x8333c5b：GetInvenSlot 后先 put_short((short)m_resultSlot)
    // （movzwl 0xd8(%ebp); cwtl → put_short），再 put_int(m_addInfo）。
    packet.put_short((int)(short)info.m_resultSlot);
    packet.put_int(resultSlot.m_addInfo);

    int curSec = GlobalData::s_systemTime_.getCurSec();
    packet.put_int(user->getCurCharacInvenR()->GetAvatarItemMgrR()
        ->GetRemainDate(resultSlot.m_addInfo2, curSec));
    // ORIG 0x8333cb9：movzwl -0x66(%ebp)（resultSlot 按值副本 +0xb）→ put_short。
    packet.put_short(resultSlot.m_fieldb);
    packet.put_int(0x1e);
    packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()
        ->getJewelSocketData(resultSlot.m_addInfo2), 0x1e);

    const unsigned char* expansion = (const unsigned char*)
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
        ->GetExpansionInfo(resultSlot.m_addInfo2);
    packet.put_int(4);
    if (expansion != 0)
    {
        packet.put_binary((const char*)expansion, 4);
    }
    else
    {
        ::stAvatarExpansionInfo_t emptyExpansion;
        emptyExpansion.init();
        // p167：ORIG 该调用点为 put_binary(char const*, int)（与 if 分支同重载），
        // `(char*)` 会选中 char* 版（EXTRA/MISSING 各 1）。
        packet.put_binary((const char*)&emptyExpansion, 4);
    }

    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---------------------------------------------------------------------------
// ProcCompound —— T 0x083338c2。
// Make → CheckCondition(错误抛 ENUM_ERROR) → Core(同) → Result + 统计；
// 全程捕获 ENUM_ERROR 转 _SendError。
// [b28d-solo-WongWorkMiscB_A] ORIG 0x083338c2 逐点对应：①error 预清 0 后赋值
// （P198①：`movl $0x0,-0x14` 在 _CheckCondition 之前）；②错误判定用
// `while (error != 0) { throw; }` 复现 4.1 `setne %al; test %al,%al; je`
// 物化（P225；4.4 对槽比较恒直跳）；③table 具名局部（ORIG 有 -0x10 槽往返，
// P0 槽-局部律）。
#include "WongWorkMiscB_A/ProcCompound.inc"
{
    try
    {
        tagCompoundInfo info = _MakeCompoundInfo(user, msg);
        int error = 0;
        error = _CheckCondition(user, info);
        while (error != 0)
        {
            throw (ENUM_ERROR)error;
        }
        tagCompoundAvatarTable* table = &m_tables[info.m_field00Job];
        error = _ProcCompoundCore(user, info, table);
        while (error != 0)
        {
            throw (ENUM_ERROR)error;
        }
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
#include "WongWorkMiscB_A/GetGoldBonus.inc"
{
    if (m_field8bc != 0)
        return level * level * 100;
    return 0;
}

// ---------------------------------------------------------------------------
// CUserPremium::GetAddQuestItemDropRate —— ORIG …23GetAddQuestItemDropRateEi
// W 0x08694a28：level∉[0,0x69] → 0；否则 g_SPremiumInfo[level].m_field48
// （ORIG 基址 0x09483a88 = g_SPremiumInfo + 0x48，元素步长 0x120）。
#include "WongWorkMiscB_A/GetAddQuestItemDropRate.inc"
{
    if (level < 0 || level > 0x69)
        return 0;
    return g_SPremiumInfo[level].m_field48;
}

// ---------------------------------------------------------------------------
// CItemUpgrade::find3rdChronicleEnchantData —— ORIG T 0x0854a916。
// itemScript=CDataManager::find_item(typeIdx)；空→0；map@item+0x2f4 查 key，
// 未命中→0；命中返回 &it->second（Ghidra 报告 node+0x10 语义即 second 地址）。
// [b28d-solo-WongWorkMiscB_A] ①去 `EnchantMap&` 具名引用（P0/P183：ORIG 无引用槽，
// find/end 各自重算 `mov itemScript,%eax; lea 0x2f4(%eax),%ecx`）；②+0x2f4 改用
// 权威成员 `CStackableItem::m_enchantMap`（§4.3 去裸偏移）；③条件改正极性
// `if (it != end) return &it->second; return 0;`（P220/P47：ORIG `je` 落失败尾块）。
#include "WongWorkMiscB_A/find3rdChronicleEnchantData.inc"
    STEnchantSystemMapKey key, int type)
{
    CItem* itemScript = G_CDataManager()->find_item(type);
    if (itemScript == 0)
        return 0;
    typedef std::map<STEnchantSystemMapKey, STEnchantSystemMapData> EnchantMap;
    EnchantMap::iterator it =
        ((CStackableItem*)itemScript)->m_enchantMap.find(key);
    if (it != ((CStackableItem*)itemScript)->m_enchantMap.end())
        return &it->second;
    return 0;
}

// ---------------------------------------------------------------------------
// CItemUpgrade::_getAdditionalDisjointItem(bool 版) —— ORIG T 0x08546a96。
// [b28d-solo-WongWorkMiscB_A] ORIG 0x08546a96 逐点对应：
//   ①at() 结果为**值拷贝**局部 `STResultItemCnt elem`（trivial 拷贝展开 4+2 两段
//     `mov (%eax),%edx; mov %edx,-0x2e; movzwl 0x4(%eax),%eax; mov %ax,-0x2a`），
//     非引用（引用多 1 指针槽 + 每次回读，P0）；
//   ②slotIdx/count 类型由 make_pair mangled `IRmRjE` 定：`unsigned long`/`unsigned int`，
//     count 预清 0 + 分支赋值（P198①）；
//   ③PRETTY 改 `__PRETTY_FUNCTION__`（ORIG `movl $sym,0x4(%esp)` 单条，P139）。
#include "WongWorkMiscB_A/_getAdditionalDisjointItem.inc"
    unsigned int up, const CItem* itemScript,
    IntPairVector& items,          // [P3 2026-09-11 P139] ORIG prett 实串用 typedef
                                   // 名（`... const CItem*, IntPairVector&, ...`，
                                   // 0x82 字节）⇒ 拼写改 typedef（mangled/ABI 不变，
                                   // 仅 __PRETTY_FUNCTION__ 串由 0xbc→0x82 与 ORIG 同）。
    const upgrade_table_t& table, bool flag)
{
    (void)itemScript;
    try
    {
        STResultItemCnt elem = table.m_destruction.m_vec.at(up);   // at 越界抛异常 → catch 吞掉并 trace
        unsigned long slotIdx;
        unsigned int count;
        if ((int)(short)elem.m_a != up)
            goto L_island1;
        slotIdx = table.m_destruction.m_field58;
        count = 0;
        if (flag)
            count = (unsigned int)(short)elem.m_c;
        else
            count = (unsigned int)(short)elem.m_b;
        if (count == 0)
            goto L_island2;
        items.push_back(std::make_pair(slotIdx, count));
    L_island1:;
    L_island2:;
    }
    catch (...)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x121, 5)("3RD_CHRONICLE : OUT OF RANGE!!");
    }
}

// ---------------------------------------------------------------------------
// CItemUpgrade::ProcUpgradeChronicle —— ORIG T 0x085491d6
// _ZN8WongWork12CItemUpgrade20ProcUpgradeChronicleEP5CUsersjsjcRSt6vectorIiSaIiEE
// 三代编年史升级：A 槽材料券 + B 槽目标装备；扣 emancipate 材料、按
// chanceGrade 抽 equipGrade 样本、构造结果物品删旧插新。错误码 0x16/0x11。
// [b28d-solo-WongWorkMiscB_A] 349=349 NEAR、p167 44/44 CLEAR。修法：
//   ①invenW 除前两次 GetInvenSlot 外 4 处重调 getCurCharacInvenW()（P183）；
//   ②shadow.setCopy 改 getCurCharacInvenRefR()；③grade 表去 5 路 switch →
//   `m_grade[chanceGrade][r]` 单一 operator[] 调用点；④GetEmancipate() + (short)；
//   ⑤results 非 const 引用 + `*begin()` operator* + pair 拷贝（P96/P220）；
//   ⑥嵌套 if → 提前返回形（复现 ORIG seta/jbe 无符号比较与 je 落主体）；
//   ⑦P290 resultIdx 改 unsigned int（升格 callee-saved 跨 receiver call）；
//   ⑧itemType 预清 0xb（P198①）；⑨newItem 字段具名直写（去 set_add_info call）。
//   残余 N115：ORIG shadowMem 落 -0xca0b（4.1 非 4 对齐分配）vs 4.4 恒 4 对齐
//   -0xca0c ⇒ 3 处 lea 立即数差 1；aligned(1) 探针无效，其余帧/槽/调用点全等。
#include "WongWorkMiscB_A/ProcUpgradeChronicle.inc"
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
        // [b28d-solo-WongWorkMiscB_A] ORIG 0x085492dc/0x085492ea 先构造 CInventory
        // 再构造 InventoryMemory ⇒ 声明序 shadow 在前（P212/N93）。
        CInventory shadow;
        // [b28d-solo-WongWorkMiscB_A][N115] ORIG 0x085492e1 该对象落 -0xca0b
        // （4.1 栈分配非 4 对齐）vs 4.4 恒 4 对齐 -0xca0c ⇒ 3 处 `lea` 立即数差 1。
        // 探针：`__attribute__((aligned(1)))` 无效（4.4 仍 4 对齐）；
        // 帧尺寸/全部其它槽位/调用点/指令数 349=349 全等 ⇒ N115 封顶。
        InventoryMemory shadowMem;
        shadow.SetInventoryMemory(&shadowMem);
        // [b28d-solo-WongWorkMiscB_A] p167 MISSING：ORIG 调 getCurCharacInvenRefR
        // （0x081803ca），非 getCurCharacInvenR。
        shadow.setCopy(*user->getCurCharacInvenRefR());
        if (!shadow.tryDeleteEventItems(materials))
            return 0x11;

        // [b28d-solo-WongWorkMiscB_A] p167 MISSING×4：ORIG 四处均重调
        // `user->getCurCharacInvenW()`（0x08549350/0x085495a8/0x0854960c/0x0854964a），
        // 不复用开头 invenW 局部（P0/P183 去缓存）。
        // ORIG 0x0854936d 直接传 &bFound（无 `movb $0` 预清）⇒ 不初始化。
        bool bFound;
        int delResult = user->getCurCharacInvenW()->delete_event_items(
            materials, out, bFound, (eMoneySubReason)0xc, (eItemDelReason)0xc,
            false);
        (void)delResult;   // ORIG 存 -0x30 槽但从不读（P171/P232 照抄）

        // chanceGrade ∈ [0,4] → CDataManager::m_equipGrade.m_gradeN[r]
        // （5 个 vector<unsigned int> @ +0x4d5c，步长 0xc）随机取样本值，
        // 写入结果物品 m_addInfo2（ORIG uStack_110 = local_30）。
        unsigned int sample = 0;
        int r = get_rand_int(100);
        // [b28d-solo-WongWorkMiscB_A] ORIG 0x085493c1 单一 operator[] 调用点：
        // 地址 = G_CDataManager+0x4d5c+0xc*chanceGrade ⇒ `m_grade[chanceGrade][r]`
        // 直写（switch 5 路会造 5 个调用点，p167 EXTRA×4）。
        if (r >= 0 && r < 100 && chanceGrade >= 0 && chanceGrade < 5)
            sample = G_CDataManager()->m_equipGrade.m_grade[chanceGrade][r];

        // CStackableItem::GetEmancipate W 0x854b4e8：ORIG 0x085493fa 先把
        // ticketScript 拷入具名局部（-0x24，P0 槽往返）再取 this 调用
        // （p167 MISSING×1）⇒ 必须调用方法，勿直读 m_short2ee。
        CStackableItem* pScript = ticketScript;
        // ORIG 0x08549408 `cwtl`（有符号扩展）⇒ 该返回值按 short 解释。
        int emancipate = (int)(short)pScript->GetEmancipate();
        // ORIG 0x08549450 直接以 -0x54 作 stEquipConditionForChronicle 实体
        // （无预清循环，P171：CheckEquipmentItemCondition 全量写后再回读）。
        stEquipConditionForChronicle cond;

        if (emancipate == 1)
        {
            // ORIG：err = CheckEquipmentItemCondition(...)；err != 0 → 携该码退出。
            int condErr = user->getCurCharacInvenR()->CheckEquipmentItemCondition(
                (INVEN_TYPE)1, slotB, cond);
            if (condErr != 0)
                return condErr;   // shadow 析构自动完成
        }

        // CEquipItem::GetEmancipateResultList W 0x854b536（本 TU 独占定义）＝ &m_vec1a4。
        // [b28d-solo-WongWorkMiscB_A] ORIG 用**非 const**引用（begin() 非 const 版，
        // p167 EXTRA `begin() const`×2）；pair 经 operator* 拷贝到具名局部。
        std::vector<std::pair<int, int> >& results =
            targetScript->GetEmancipateResultList();
        // ORIG 0x08549469 `cmp $1; seta; test; je` ⇒ 提前返回形（非 `if(<2){}`，
        // 后者 4.4 产 setbe 反形）。
        if (results.size() > 1)
            return 0x11;
        std::pair<int, int> pairVal = *results.begin();
        // P290：实参类型 unsigned 触发 4.4 升格 callee-saved 跨 receiver call 保活
        // （ORIG 0x085494cb `mov -0x18(%ebp),%ebx; … call getCurCharacInvenR;
        //  mov %ebx,0x4(%esp)`；int 形恒 receiver-first，探针 g1/g3 实证）。
        unsigned int resultIdx = (unsigned int)pairVal.first;
        // ORIG 0x085494b4 `cmpl $0x1,-0x14; jbe`（无符号）⇒ 计数按 unsigned。
        unsigned int resultCnt = pairVal.second;
        if (resultCnt > 1)
            return 0x11;
        // ORIG 0x085494c4 `movl $0xb,-0x10` 预清后赋值（P198①）。
        int itemType = 0xb;
        itemType = user->getCurCharacInvenR()->GetItemType(resultIdx);
        if (itemType != 1)   // 必须是装备类（ORIG `je <主体>`）
            return 0x11;

        Inven_Item newItem;
        newItem.m_addInfo = (unsigned int)resultIdx;
        targetScript->make_item(newItem);  // vtable+0x08 虚槽（make_item）

        if (emancipate == 1)
        {
            if (cond.m_upgrade <= 0x1e)
                newItem.SetUpgrade(cond.m_upgrade);
            if (cond.m_hasAmplify != 0 && cond.m_amplifyType != 0)
            {
                // stAmplifyOption_t::assign W：item.m_amp(+0x11) 选项写入
                // （ORIG 0x08549578 直接 lea+add 传址，无引用局部槽）
                ((stAmplifyOption_t*)((char*)&newItem + 0x11))
                    ->assign(cond.m_amplifyType, cond.m_amplifyValue);
            }
            // ORIG 0x08549589：newItem.m_fieldd(+0x0d) ← cond.m_fieldd(+0x08)
            newItem.m_fieldd = cond.m_fieldd;
        }
        // ORIG 0x08549592：set_add_info 内联为单条 `mov %eax,-0x10c`
        // （p167 EXTRA `set_add_info` ⇒ 直写成员）。
        newItem.m_addInfo2 = (int)sample;

        user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slotB, 1, (eItemDelReason)0xc, true);
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slotB);
        user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slotA, 1, (eItemDelReason)0xc, true);
        user->getCurCharacInvenW()->insert_item_special_slot(
            newItem, 1, slotB, (eMoneyAddReason)7, (eItemAddReason)9);
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slotB);
        return 0;
    }
}

}  // namespace WongWork

