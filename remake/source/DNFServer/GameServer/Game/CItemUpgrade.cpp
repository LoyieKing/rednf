// df_game_r Game/ WongWork::CItemUpgrade 实现。
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager / CEventBase::GetRepeatEvent
#include "CEnvironment.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"            // CMTRand（+0x4e8 randInt）
#include "GameTypes.h"           // cMyTrace
#include "DisJoint.h"            // DisJoint::GetResult/getPredictResultItemTypeCount
#include "CCubeStatistic.h"      // CCubeStatistic::collectCubeStatistics
#include "STStackableScript.h"   // STEnchantSystemMapKey/MapData
#include "PacketTypes.h"         // MSG_ENCHANT 完整类型
#include "CReduceUpgradeItemPay.h"   // CReduceUpgradeItemPay（GetReduceRate 权威声明）

#include <cstring>
#include <cstdio>
#include <ctime>     // ctor 的 time(0)（CMTRand 种子）

// 类声明见 CDataManager.h（WongWork::CItemUpgrade）。
namespace
{
// [b28d-solo-CItemUpgrade] ORIG getUpgradeItemRepairCostRate 0x08549170/0x0854918d
// 为 `add $0x9c,%edx; mov 0x4(%eax,%edx,4)`：基址是表指针本身、数组成员位于
// 视图 +0x04（P311 成员数组视图）。表本体 +0x04 落在 vector 内部指针上，无法
// 直接声明成员，故以只读视图类型表达该区布局（仅 reinterpret_cast 视图，
// 不新增符号、不改任何 sizeof/偏移）。
struct UpgradeRateTableView
{
    char m_pad04[4];                  // +0x00..+0x03
    float m_rate[(0x404 - 4) / 4];    // +0x04 起 4 字节步长（同 m_bodyView.m_f32 区）
};

}  // namespace

namespace WongWork
{
// [2026-09-10 判据㉔/P327 跨 TU 定义迁移] ORIG 地址序：C1/C2 @0x085461c8 <
//   D1/D2 @0x085462c6（同一 CItemUpgrade.cpp TU 段），且本 TU `_GLOBAL__I_`
//   键名 = `_ZN8WongWork12CItemUpgradeC2Ev`（0x0854b178，P327 取本 TU 第一个
//   强符号·外链实体=C2 形）。原定义在 CtorProducers_2C.cpp 使本 TU 键名退化为
//   `...D2Ev` ⇒ 定义迁回本文件并置于 dtor 之前。函数体逐字节照搬（0xfe 字节）。
CItemUpgrade::CItemUpgrade()
{
    // ORIG 0x085461c8 逐段转写：
    //   [表区] CItemUpgradeTable(this+0x04)（upgrade_table_t 薄包装，ctor 委托
    //          upgrade_table_t 0x89000ca）——本重建沿用类内裸偏移约定，无成员建模；
    //   [基类] IItemUpgrade(this) 0x080fc834 + vptr 写入（编译器生成；槽序
    //          ProcUpgrade→LoadUpgradeScript，ORIG vtable 0x08c9d310 实证）；
    //   [+0x4ec] = 100000（randInt 热身上界）；
    //   [热身] time(NULL) 种子 → operator new(0x9c8) → CMTRand(seed) 存 +0x4e8，
    //          循环 100 次 randInt(*+0x4ec) 预热 MT19937 状态（结果弃用）。
    // b28d-lane12: ORIG 0x085461c8 实证三修（ndiff 20→0，verdict AE）——
    //   ① 基类声明序改 CItemUpgradeTable 在前（CDataManager.h；0x085461d1 `add $0x4`
    //      +call 表区先于 0x085461e2 call IItemUpgrade 实证；primary base/布局不变）；
    //   ② 去具名 pRand 局部：0x08546242 循环每次重读 m_rand（+0x4e8，P183）；
    //   ③ range 先于 seed 声明（0x0854620c seed@-0x24 / 0x0854625f range@-0x20）
    //      + 实参内联赋值 `new CMTRand(seed = time(0))`：复现 0x0854620f
    //      `lea -0x24(%ebp),%esi` 在 operator new 前装载 &seed（四形实测唯一命中：
    //      普通形/两句式/语句表达式/逗号形均 ndiff=4）。
    m_randMax = 100000;
    unsigned long range;
    unsigned long seed;
    m_rand = new CMTRand(seed = time(0));
    for (int i = 0; i < 100; ++i)
    {
        range = static_cast<unsigned long>(m_randMax);
        m_rand->randInt(range);
    }
}

CItemUpgrade::~CItemUpgrade()
{
    // ORIG 0x85462c6：析构 +0x4e8 的 CMTRand*（构造时 new，见 0x8546247）。
    // [b28d-solo-CItemUpgrade] ORIG 0x085462f5：`movl $0x0,0x4e8(%eax)` 在 if 之外
    // 无条件执行 ⇒ 清零必须写在 if 外（写成 if 内 m_rand=0 少一条且分支布局偏移）。
    // 基类析构变体展开序（IItemUpgradeD1 this+0 → TableD1 this+4）由编译器生成，
    // 与 ORIG 同名调用一致。
    if (m_rand)
    {
        ::operator delete(m_rand);
    }
    m_rand = 0;
}

float CItemUpgrade::getUpgradeItemRepairCostRate(int idx, bool flag) const
{
    // ORIG 0x8549148：
    //   flag==true  → GlobalData::s_itemAmplifier_->getUpgradeInfoTable()（=+0x100）
    //   flag==false → this+0x04（CItemUpgradeTable 子对象，getUpgradeInfoTable 返回自身）
    //   返回 table[(idx + 0x9c) * 4 + 4] 处 float（混合表按 4 字节步长）。
    // [b28d-solo-CItemUpgrade] ORIG 0x08549148 两分支均无 tbl/rate 具名局部：
    // [判词] AE（P311 成员数组视图 UpgradeRateTableView：char[4]+float[] 以表
    //   指针为基址，复现 ORIG `add $0x9c,%edx; mov 0x4(%eax,%edx,4)`；原 NEAR）。
    //   mov 0x4(%eax,%edx,4)` vs 4.4 `add $0x9a,%edx; mov 0xc(%eax,%edx,4)`。
    //   要 disp 4/add 0x9c 需 upgrade_table_t 里 +4 处的 float 数组视图
    //   （现 union m_bodyView 起于 +0xc）；探针 /tmp/probe_repair.cpp g/h 两形
    //   4.4 恒折 add 0x9d+shl ⇒ P30/N4 结构性伪影，封顶。
    //   call getUpgradeInfoTable → mov idx,%edx; add $0x9c,%edx;
    //   mov 0x4(%eax,%edx,4),%eax; jmp 公共尾 mov %eax,-0x1c; flds -0x1c。
    // 双独立 return 形（探针 /tmp/probe_rate.cpp g2）：4.4 与 4.1 同为「两臂各
    // 算值 + jmp 合并到唯一 float 返回槽」；具名 `float rate`（g1）恒多一个
    // -0x18 拷贝槽 + flds 中转（+4 条）。表指针内联进使用点（P0 无槽往返），
    // dword 下标视图 UPG_TBL_F32（k=idx+0x9a ≡ 字节偏移 (idx+0x9c)*4+4-0xc）。
    if (flag)
    {
        return reinterpret_cast<const UpgradeRateTableView*>(
                   GlobalData::s_itemAmplifier_->getUpgradeInfoTable())
            ->m_rate[idx + 0x9c];
    }
    return reinterpret_cast<const UpgradeRateTableView*>(getUpgradeInfoTable())
        ->m_rate[idx + 0x9c];
}

int CItemUpgrade::make3rdChroniclePacket(CUser* user,
                                         const Inven_Item& item,
                                         PacketGuard* packet)
{
    if (user == 0)
        return 0;
    // ORIG 0x854a9aa：局部 STEnchantSystemMapKey keys[2]（0x10 步长，循环 C1 构造）。
    STEnchantSystemMapKey keys[2];
    // [b28d-solo-CItemUpgrade] ORIG 0x0854a9aa：count 是 **char** 局部 -0x21，
    // [判词] AE（b28d-solo：返回类型 int + 位域视图 + 位域数组视图 m_skills[2]
    //   + m_job/m_equipType enum 视图 + m_bSigned；原 DIFF 233/231 残余 = 4.1 void 早退路径的
    //   `mov $0x0,%ebx` 物化 ×2 + uchar 位与加宽（P168a③）+ 槽号平移 ⇒ N 封顶。
    // 累加形为 `addb $0x1,-0x21`（两处）；fields 指针局部不存在（逐次
    // `mov &item,%eax; movzbl 0x1f/0x20(%eax),%eax`）。
    // enchantId 声明在 count 之前（ORIG 0x0854a9e7 `movl $0x0,-0x28` 早于
    // 0x0854a9ee 的 count `movb $0x0,-0x21`）。
    int enchantId = 0;
    char count = 0;
    // ORIG 0x0854a9f5/0x0854aa07 位测试为 `movzbl; and $0x1; test %al`（无
    // uchar→int 加宽）⇒ 位域视图（探针 c8 实证）。
    if (item.m_random.m_skill0.m_bits.m_flag)
    {
        ++count;
        if (item.m_random.m_skill1.m_bits.m_flag)
            ++count;
    }
    else
    {
        // ORIG 0x0854aa18 `movsbl -0x21(%ebp),%edx` ⇒ 实参是 count 变量（P97）。
        packet->put_byte(count);
        return 0;
    }
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return 0;
    packet->put_byte(count);
    // [b28d-solo-CItemUpgrade] ORIG 0x0854aa74 循环体无 field/kind 具名局部：
    //   `mov i,%eax; mov &item,%edx; movzbl 0x1f(%edx,%eax,1),%eax; shr %al;
    //    and $0x3,%eax` 每分支各自重读（P48 无 CSE），enchantId 是循环外声明的
    //   int 槽 -0x28（ORIG 0x0854a9e7 `movl $0x0,-0x28`），if-else 链末支赋 -1。
    for (int i = 0; i < count; ++i)
    {
        if (item.m_random.m_skills[i].m_bits.m_type == 0)
            enchantId = 0x4e6;
        else if (item.m_random.m_skills[i].m_bits.m_type == 1)
            enchantId = 0x4e7;
        else if (item.m_random.m_skills[i].m_bits.m_type == 2)
            enchantId = 0x4e8;
        else if (item.m_random.m_skills[i].m_bits.m_type == 3)
            enchantId = 0x4e9;
        else
            enchantId = -1;
        keys[i].m_a = user->get_charac_job();
        keys[i].m_b = (unsigned char)user->getCurCharFirstGrowType();
        keys[i].m_c = itemScript->GetItemType();
        keys[i].m_fieldC =
            (unsigned char)item.m_random.m_skills[i].m_bits.m_skillId;
        packet->put_int(enchantId);
        // ORIG 0x0854aba0 `mov (%eax),%eax; mov %eax,%edx` 两段 ⇒ enum 视角读取
        // （P254；int 字段 4.4 恒折单段）。
        packet->put_byte(keys[i].m_job);
        // ORIG 0x0854abc2 `movzbl 0x4(%eax),%eax; movsbl %al,%edx`（零扩展读 +
        // 符号扩展到 int）⇒ m_b 按 signed char 语义传出（视图，字段保持 uchar）。
        packet->put_byte((int)keys[i].m_bSigned);
        packet->put_byte(keys[i].m_equipType);
        packet->put_byte(keys[i].m_fieldC);
    }
    return 1;
}

// ============================================================================
// enableAmplifyRandomUpgrade @ 0x854af38
//   校验随机增幅券并计算随机强化值写入 int& outCount。
//   返回错误码：0=通过, 4=找不到券/物品或非增幅券, 0xd=upgrade 越界,
//   0x13=物品不可增幅。
// [b28d-solo-CItemUpgrade 判词] DIFF（N 封顶，p167 CLEAR）。残余：
//   `_GetUpgradeInfo(...) != 1` ORIG `xor $0x1,%eax; test %al,%al; je`（3 条）
//   vs 4.4 `cmp $0x1,%eax; setne %al; test %al,%al; je`（4 条）——P292/P168a
//   定论：int callee 的调用点写法恒产 setcc，改返回 bool 则 _GetUpgradeInfo
//   尾部（ORIG 0x08546816 `call get_upgrade_info; add; pop; ret` 直返）多归一化
//   ⇒ N 封顶；另帧/槽号差（4.4 对 characLevel 等具名局部槽化）为 N82 族。
// ============================================================================
int CItemUpgrade::enableAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                             Inven_Item& ticket,
                                             int& outCount)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x0854af42 无 itemIdx/dm 具名局部（P0）：
    //   mov item,%eax; mov 0x2(%eax),%eax; mov %eax,%ebx（跨 call 保活）;
    //   call G_CDataManager; mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item。
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return 4;

    const upgrade_table_t* table =
        GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    upgrade_info_t info;              // ORIG +0x04f 直调 upgrade_info_t ctor（禁另加 memset）
    // characLevel 无具名局部：ORIG 在压完 0x10/0xc/0x8/0x4 后直接
    // `call get_charac_level; mov %eax,0x14(%esp)`（P0 内联实参）。
    if (_GetUpgradeInfo(&item, itemScript, *table, &info,
                        user->get_charac_level()) != 1)
        return 0xd;
    // ORIG +0x0a4 死存 GetUpgrade(item)：4.1 保留 call + movzbl + 落槽 -0x1c
    // （P171 死写入照抄）；写 (void)item.GetUpgrade() 会被 4.4 DCE 掉 store。
    int deadUpgrade = item.GetUpgrade();
    (void)deadUpgrade;
    // ORIG +0x0af..+0x0c3 把券信息回填进 info 后三字段（顺序 = 高偏移→低偏移）：
    //   info+0x34=0 / info+0x30=1 / info+0x2c=ticket.m_addInfo。
    info.m_discountBase = 0;
    info.m_meterialCount = 1;
    info.m_recipeItem = ticket.m_addInfo;
    if (itemScript->IsImpossibleAmplify())
        return 0x13;

    // P198① 预清两句式（ORIG +0x0e6 movl $0x0,-0x18 在压参之前）。
    int ret = 0;
    ret = _CheckCondition(user, item, itemScript, ticket, *table, info);
    // [b28d-solo-CItemUpgrade] P225 形：ORIG 0x0854b060 `cmpl $0x0,-0x18;
    // setne %al; test %al,%al; je` 四段物化 ⇒ 源码是 while+break 形
    // （if 形 4.4 恒直跳 je，少 setne/test 两条）。
    while (ret != 0)
    {
        return ret;
    }

    // ORIG +0x143 第二次 G_CDataManager()（不复用函数头），ticketIdx 无具名局部。
    CItem* ticketScript = G_CDataManager()->find_item(ticket.m_addInfo);
    if (ticketScript == 0)
        return 4;
    if (!ticketScript->is_stackable())
        return 4;
    CStackableItem* stackable = static_cast<CStackableItem*>(ticketScript);
    if (!stackable->is_random_amplify_reinforce_ticket())
        return 4;

    unsigned long range = 100;
    int roll = m_rand->randInt(range);
    outCount = stackable->get_amplify_reinforce_count(roll);
    if (outCount < 0 || outCount > 0x1f)
        return 4;
    return 0;
}

// ============================================================================
// ProcAmplifyRandomUpgrade @ 0x854ac8a
//   处理随机增幅结果：写升级值、统计、日志、广播。
//   返回错误码：0=成功, 4=失败。
// [b28d-solo-CItemUpgrade 判词] AE（P227 全表达式临时链复现 %esi/%ebx 跨 ctor
//   保活；真 bug 3 处：find_item/DB itemId 误用 ticket→item、IsRandomOption 两臂互换）。
//   原 DIFF 186/192：
//   本轮修：trace ctor 移到 characNo/NumberToString 之后（ORIG 0x0854ac93 求值序）、
//   itemIdx/dm 内联、oldUpgrade 改 int 槽。残余 = ORIG 4.1 把 characNo/idStr/
//   characName/characNo2/m_addInfo 保活于 %esi/%ebx/%edi（push edi/esi/ebx），
//   4.4 恒槽化（register 提示实测零效 192=192）⇒ N82/N18a 封顶。
// ============================================================================
int CItemUpgrade::ProcAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                           Inven_Item& ticket, int update,
                                           const char* msg)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x0854ac93 求值序：characNo（→%esi）→
    // NumberToString(get_acc_id(),0)（→%ebx）→ cMyTrace ctor（-0x34）→ trace()，
    // 两个值均无槽往返（P0）⇒ P227 全表达式临时链（ctor 前求值 varargs 并跨
    // ctor 以 callee-saved 保活；具名局部形会被 4.4 槽化）。
    cMyTrace(__PRETTY_FUNCTION__, 0x82c, 0)(
        "AmplifyRandomUpgrade m_id:%s char_no(%d) update:%d",
        NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1), update);

    // ORIG 0x0854ad0a 无 itemIdx/dm 具名局部（P0，内联实参）；源对象是
    // **item**（0x10(%ebp)，Ghidra param_2），不是 ticket（0x14）——真 bug 修正。
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return 4;

    if (!item.m_amp.hasAbility())
        return 4;

    const upgrade_table_t* table =
        GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    // ORIG 0x0854ad73：oldUpgrade 落 **int** 槽 -0x1c（movzbl 后 mov %eax）。
    int oldUpgrade = item.GetUpgrade();
    item.SetUpgrade((unsigned char)update);

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, 0);

    // ORIG 每处 use 各自重调 GetUpgrade（P48 无 CSE）：newCheck/0xc/DB/UpgradeItem/guild×2。
    if (item.GetUpgrade() >= table->m_bodyView.m_named.m_noticeUpgradeLevel)
    {
        _NoticeUpgrade(user, item, true);
    }

    if (0xc < item.GetUpgrade())
    {
        // ORIG 0x0854ae0a 第二次 GetUpgrade 结果落 -0x3c 槽（求值序 R2L 的首个
        // 求值项），其余三值（characName/characNo2/item.m_addInfo）经
        // %esi/%ebx/%edi 保活无槽往返 ⇒ 全内联实参。
        DB_ReqItemUpgradeLog::makeRequest(
            (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            item.m_addInfo, user->getCurCharacNo(), user->getCurCharacName(),
            item.GetUpgrade() & 0xff, true);
    }

    // ORIG 0x0854ae71 无 itemIndex 具名局部（P0）：get_index 结果直接在 %eax 传参，
    // 求值序 R2L（先 GetUpgrade→%ebx，后 get_index）。
    user->m_historyLog.UpgradeItem(itemScript->get_index(), oldUpgrade,
                                   item.GetUpgrade() & 0xff, true);

    // ORIG 0x0854aeb8 `je` 落 itemName 块（IsRandomOption()==false）⇒ true 用
    // **msg**、false 用 GetItemName()（原重建两臂互换 ⇒ 真 bug）。
    if (itemScript->IsRandomOption())
    {
        user->makeGuildItemUpgradMessage(msg, oldUpgrade, item.GetUpgrade() & 0xff, true);
    }
    else
    {
        // ORIG 0x0854af03 无 itemName 具名局部（P0）：GetItemName 结果直接传参。
        user->makeGuildItemUpgradMessage(itemScript->GetItemName(), oldUpgrade,
                                         item.GetUpgrade() & 0xff, true);
    }
    return 0;
}

// ============================================================================
// _GetUpgradeInfo @ 0x854678a
//   按 upgrade_table_t 取 upgrade_info 到 out。
//   返回 get_upgrade_info 结果（0=越界/失败, 1=成功）。
// ============================================================================
bool CItemUpgrade::_GetUpgradeInfo(const Inven_Item* item,
                                   const CItem* itemScript,
                                   const upgrade_table_t& table,
                                   upgrade_info_t* out, int level)
{
    if (item->GetUpgrade() >= 0x1f)
        return 0;
    // ORIG 求值序（实参 R2L）：getGrowthGrade 命名局部先求值（-0x1c），
    // return 内联实参 R2L：GetItemType → GetUpgrade（第二次调用，P48 无 CSE）
    // → get_rarity；临时值跨调用存 callee-saved（P52）。
    int growthGrade = ((CEquipItem*)itemScript)->getGrowthGrade(level);
    return table.get_upgrade_info(
        out,
        static_cast<unsigned char>(itemScript->get_rarity()),
        static_cast<unsigned char>(growthGrade),
        item->GetUpgrade(),
        (ENUM_EQUIPMENTTYPE)itemScript->GetItemType());
}

// ============================================================================
// _CheckCondition @ 0x8546342
//   校验强化/增幅条件，返回错误码（0=通过）。
// [2026-09-10 判词] DIFF（gap=2、p167 CALLSITE CLEAR 32/32）。残余 = 2 处
//   `m_i32[rarity+0x104]`：ORIG 0x08546603/0x08546635 `add $0x104,%edx;
//   mov 0xc(%eax,%edx,4)` vs 4.4 `lea 0x104(%eax),%edx`（P30 结构性伪影；
//   已实测 P311 二维指针视角反劣 ourN 323→329 ⇒ 回滚）⇒ N4 封顶。
//   本轮修 P0 内联/P222 正向包裹/UPG_TBL_I32 视图/predict 两句式/upType if-else。
//   本轮已修：删 repeatEvent 判空（ORIG 0x0854639f 直接解引用）、内联 reduce
//   （无具名局部）。残余 = 4.4 对具名局部的槽化 vs ORIG 的 %ebx/%esi 寄存器
//   保活（N82/N95 帧差 0xa4 vs 0x84）+ 常量折叠形差 ⇒ N 封顶。
// ============================================================================
int CItemUpgrade::_CheckCondition(const CUser* user, const Inven_Item& item,
                                  const CItem* itemScript,
                                  const Inven_Item& ticket,
                                  const upgrade_table_t& table,
                                  const upgrade_info_t& info)
{
    if (!item.m_amp.isIdentified())
        return 0xae;
    if (ticket.m_addInfo != info.m_recipeItem)
        return 0x16;
    if (ticket.m_addInfo2 < info.m_meterialCount)
        return 0x16;

    // ORIG 0x085463d4：GetRepeatEvent(0x32) 调两次（IsEventing 判活 + GetReduceRate）。
    int discountGroup = 0;
    // [b28d-solo-CItemUpgrade] ORIG 0x0854639f **无** repeatEvent 具名局部、也
    // **无判空**（P171/P140）：`call GetRepeatEvent; mov (%eax),%edx;
    // add $0x34,%edx; mov (%edx),%edx; ...; call *%edx` 直接走 vtable+0x34
    // （slot 13 = IsEventing(CUser*)）。第二次调用内联进 GetReduceRate 的 receiver
    // （ORIG `mov %eax,(%esp); call GetReduceRate`，无 reduce 局部槽）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x32)->IsEventing(0) &&
        !item.m_amp.hasAbility())
    {
        int rate = ((CReduceUpgradeItemPay*)
            GlobalData::s_event_manager->GetRepeatEvent(0x32))->GetReduceRate();
        if (0 < rate)
        {
            discountGroup = info.m_discountBase -
                            (info.m_discountBase * rate) / 100;
            if (discountGroup < 0)
                discountGroup = 0;
        }
    }
    else
    {
        discountGroup = info.m_discountBase;
    }

    // ORIG 0x08546468 无 inven 具名局部（P0）：getCurCharacInvenR() 结果直接
    // 作 GetInvenSlot 的 receiver（`mov %eax,0x4(%esp)`）。
    Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
    int slotItemIdx = slotItem.m_addInfo;
    // ORIG 0x0854649b 无 dm 具名局部：G_CDataManager() 结果直接作 find_item 的
    // receiver。
    CEquipItem* materialScript =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(slotItemIdx));
    if (materialScript != 0)
    {
        // ORIG：折扣 = ROUND(getUpgradeCostDiscount(material) * discountGroup /
        //                     _DAT_08c9d304 + discountGroup)。_DAT_08c9d304 为该
        // 强化成本基准浮点常量（== -100.0f）。ORIG 0x085464ca 返回值直接 fmuls
        // （无 fstps 槽）⇒ 无具名 float 局部（P0）；两次 `fildl -0x20` 即两次
        // (float)discountGroup（P243 书写序 out = out + in）。
        // ORIG 0x085464b8..0x085464c1 两次 `fildl -0x20; fstps` 到两个不同槽
        // （-0x74/-0x70）⇒ 两个具名 float 局部（同值）；末项 fadds -0x74 在内存
        // （P243 书写序）。
        float baseA = (float)discountGroup;
        float baseB = (float)discountGroup;
        discountGroup =
            (int)(materialScript->getUpgradeCostDiscount() * baseB / -100.0f +
                  baseA);
    }

    // 货币检查（ORIG 429 行重新 getCurCharacInvenR，不复用 inven）。
    if (user->getCurCharacInvenR()->get_money() < discountGroup)
        return 10;

    if (table.m_infos.size() <= item.GetUpgrade())   // ORIG 436 行 GetUpgrade
        return 0xd;
    if (item.m_field1 != 1)
        return 0x13;

    // [b28d-solo-CItemUpgrade] ORIG 顺序（0x0854655d 先范围、0x085465a5 后 ==0xb）：
    // 先 `type <= 9 || type > 0x15`，再 `type == 0xb`（原重建写反）；且 P48 无 CSE，
    // 每次比较各自独立调用 GetItemType（无 itemType 具名局部）。
    if (itemScript->GetItemType() <= 9 || itemScript->GetItemType() > 0x15)
        return 0x13;
    if (itemScript->GetItemType() == 0xb)
        return 0x13;

    // ORIG 0x085465cb 无 endurance 具名局部（P0）：`movzwl 0xb(%eax),%eax;
    // movzwl %ax,%ebx` 直用字段（P77）。
    if (item.m_fieldb != ((const CEquipItem*)itemScript)->get_endurance())
        return 7;

    // ORIG 0x8546600：读 table+(rarity+0x104)*4+0xc（intAtConst 减 0xc 内偏）；
    // 两步寻址形 `add $0x104,%edx; mov 0xc(%eax,%edx,4)` 需数组下标视图
    // （UPG_TBL_I32，P30/P309；指针算术版恒折 lea/shl）。
    // [2026-09-10 N4 实测] 尝试 P311 二维指针视角
    //   `((int(*)[0x104])(&m_i32[0]))[1][rarity]` ⇒ 4.4 改产
    //   `mov 0x41c(%eax,%edx,4)` +6 条（ourN 323→329，反劣）⇒ 回滚为
    //   一维 `m_i32[rarity+0x104]` 的 `lea 0x104(%eax),%edx` 形（P30 结构性伪影，
    //   与 ORIG 的 `add $0x104,%edx` 差 1 条/处）。
    if (UPG_TBL_I32(&table, itemScript->get_rarity() + 0x104) != -1)
    {
        // ORIG 465 行内层比较再调一次 get_rarity（不复用外层索引）；
        // ORIG 形 `cmp %eax,%ebx; setge` ⇒ 源写 upgrade >= penTable。
        if ((int)item.GetUpgrade() >=
            UPG_TBL_I32(&table, itemScript->get_rarity() + 0x104))
            return 0x5f;
    }

    // ORIG 0x0854665d 无 rarity/penalty 具名局部：get_rarity 结果在 %ebx 直传，
    // GetPenaltyType 结果直接 `cmp $0x3; sete; test; je <check_level>` 物化
    // （P0 + P222 正向包裹：penalty==3 才进 predict 块）。
    if (table.GetPenaltyType(item.GetUpgrade(),
                             (ENUM_RARITY)itemScript->get_rarity(),
                             &info) == 3)
    {
        // 若 penalty 类型==3，预测结果物品数检查（InvenEmptycount+QuickEmptyCount）。
        // ORIG 0x8546695：predict = getPredictResultItemTypeCount(rarity)+1，hasAbility 再 +1；
        // predict==0 跳过检查；<predict 返回 0x15。
        // ORIG 0x0854670c `cmp -0xc(%ebp),%eax; jae`（mem 直比 + 无符号序）⇒
        // predict 是 unsigned 槽（P209）；ORIG 0x085466b6/0x085466b9 为
        // `mov %eax,槽; addl $0x1,槽` 两句式（单表达式形恒折 add+store）。
        unsigned int predict =
            (unsigned int)G_CDataManager()->m_disJoint
                ->getPredictResultItemTypeCount(itemScript->get_rarity());
        ++predict;
        if (item.m_amp.hasAbility())
            predict += 1;
        if (predict != 0 &&
            user->getCurCharacInvenR()->InvenEmptycount(Inven_Item::ITEM_TYPE_3) +
                    user->getCurCharacInvenR()->QuickEmptyCount() <
                predict)
            return 0x15;
    }
check_level:
    // ORIG 0x08546728 `movl $0x0,槽` 预置 + if/else 直接 store（三元形产 eax 中转）。
    int upType = 0;
    if (item.m_amp.hasAbility())
        upType = 2;
    else
        upType = 1;
    if (((CEquipItem*)itemScript)->isUpgradableLevel(
            static_cast<ItemUpgradeType::T>(upType),
            user->get_charac_level()) == false)
        return 0x13;
    return 0;
}

// ============================================================================
// _NoticeUpgrade @ 0x8546824
//   广播强化通知包（header 0x56）。
// ============================================================================
void CItemUpgrade::_NoticeUpgrade(const CUser* user, const Inven_Item& item,
                                  bool isAmplify)
{
    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(1);
    packet.put_byte(isAmplify);
    packet.put_short((int)user->get_unique_id());
    packet.put_int(item.m_addInfo);
    packet.put_byte((int)item.GetUpgrade());
    item.m_random.m_random.put_packet_random_option(packet);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

// ============================================================================
// isDoEnchant @ 0x0854977a（isDoEnchant.md）：randInt(100)（经 this+0x4e8
//   CMTRand）与 buf[(item+0x1f & 1) ? 1 : 0]（char 符号扩展）比较；rand<=bufVal
//   返回 true。
// [b28d-solo-CItemUpgrade 判词] AE（位域视图 m_bits.m_flag 复现
//   `movzbl; and $1; test %al`）。原 DIFF 35/36 残余 1 条：
//   位测试段 ORIG `and $0x1,%eax; test %al,%al` vs 4.4 `movzbl %al,%eax;
//   and $0x1,%eax; test %eax,%eax`（uchar 位与加宽，P168a③ 跨版本，探针
//   /tmp/probe_isd.cpp 五形 f1..f5 全败）⇒ N 封顶。
// ============================================================================
bool CItemUpgrade::isDoEnchant(const Inven_Item& item, char* buf)
{
    unsigned long range = 100;
    int roll = m_rand->randInt(range);
    int bufVal = 0;
    // P191：ORIG 0x085497b3 `jne +0x4b`（bit 置位→尾部 buf[1] 块），fall-through
    // 为 buf[0]。`==0` 负向判定翻转分支极性（免 xor 物化），buf[0] 直落、buf[1]
    // 走 jne 尾岛。残余仅 `movzbl %al`+`test %eax`（4.4 uchar→int 展宽）见 N/P196
    // 注，语义正确。
    // [b27 复证封顶] bit 测试段 ORIG and 后 test %al 8 位直测 vs 4.4 恒
    // movzbl %al,%eax 加宽（P168a③ 同族，uchar 字段位与不可造）。
    if (item.m_random.m_skill0.m_bits.m_flag == 0)
        bufVal = (signed char)buf[0];
    else
        bufVal = (signed char)buf[1];
    if (roll <= bufVal)
        return true;
    return false;
}

// ============================================================================
// ProcUpgrade(MSG_ENCHANT*) @ 0x0854a1ba（ProcUpgrade_0854a1ba.md）
//   三代强化（ENCHANT 消息）主流程。返回错误码；0 成功。
// [2026-09-10 判词] DIFF（gap 4→0；origN=ourN=437；jcc 逐位一致；imm 多重集一致；
//   p167 CLEAR 41/41）。
//   本轮真 bug 修：①**范围检查方向反**——ORIG 0x0854a39c `jl +0x204`/0x0854a3b0
//   `jg +0x204` 均跳「继续」，fall-through 才是 `mov $0x13`，即正极性
//   `if (m_0d >= start && m_0d <= end) return 0x13`（[3,8]=ITEM_TYPE_0 非装备栏
//   槽 ⇒ 报错）；旧写 `m_0d < start || m_0d > end` 方向全反（判据⑧/jcc 逐位）；
//   ②**GetSellItemPrice 第三实参来源错**——ORIG 0x0854a5f0 读 slotA+0x07
//   (m_addInfo2)，旧写 `(short)msg->m_19`（判据㉕）；③删 ORIG 无有的 `& 0xffff`
//   截断（判据⑮）。
//   本轮形修：P198① 预清三处（cond/penalty/doEnchant 两句式）、P225 while 形 ×2、
//   P0 去 equipType/ret 具名槽、P174 返回类型 int→ENUM_ERROR（__PRETTY_FUNCTION__
//   串 0x53 与 ORIG 全等）、P248① ProcUpgrade_3rdChronicle int→bool、P272 块序。
//   残余 = 帧 0x1ac/0x19c 与槽号平移（N82/N95）+ 位域读 2 处多 `movzbl %al,%eax`
//   （P168a③）⇒ N 封顶。
// ============================================================================
// [P174 串定返回类型] ORIG __PRETTY_FUNCTION__ 串 = "ENUM_ERROR WongWork::
//   CItemUpgrade::ProcUpgrade(CUser*, MSG_ENCHANT*, PacketGuard*)" ⇒ 返回
//   ENUM_ERROR；函数体 return 立即数与 ORIG 一致（枚举底层 int）。
ENUM_ERROR CItemUpgrade::ProcUpgrade(CUser* user, MSG_ENCHANT* msg, PacketGuard* packet)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x0854a1ba 开头**无** user/msg/packet 判空
    // （P171 照抄：加防御反而多 3 组 cmp/je 并顶浅全帧槽）。
    CInventory* invenW = user->getCurCharacInvenW();
    // ORIG 亦无 invenW 判空（mov %eax,-0x30 后直接作 GetInvenSlot 的 receiver）。

    Inven_Item slotA = invenW->GetInvenSlot(1, msg->m_0d);
    if (slotA.isEmpty())
        return (ENUM_ERROR)4;
    if (slotA.m_addInfo != msg->m_0f)
        return (ENUM_ERROR)4;

    // ORIG ProcUpgrade_0854a1ba 554 行：Inven_Item::Inven_Item(&material) 先
    // 默认构造，再 GetInvenSlot 返回临时 + 拷贝赋值（4.1 无 copy-elision）。两段式必写。
    Inven_Item material;
    material = invenW->GetInvenSlot(1, msg->m_13);
    if (material.isEmpty())
        return (ENUM_ERROR)0x16;

    int start = 0, end = 0;
    invenW->getStartEndPos((Inven_Item::ITEM_TYPE)0, start, end);
    // [jcc 逐位核对 2026-09-10] ORIG 0x0854a39c `cmp %eax,%edx; jl +0x204`（m_0d < start
    //   → 继续）、0x0854a3b0 `cmp %eax,%edx; jg +0x204`（m_0d > end → 继续），
    //   fall-through 才是 `mov $0x13`。即原码为**正极性 &&**：目标槽位落在
    //   ITEM_TYPE_0 范围 [3,8]（CSlotBoundChecker::init insert(3,8,ITEM_TYPE_0)）
    //   内 ⇒ 非装备栏槽位 ⇒ 返回 0x13。旧重建写成 `m_0d < start || m_0d > end`
    //   方向全反（只有 [3,8] 内才放行）⇒ 真语义 bug。
    if (msg->m_0d >= start && msg->m_0d <= end)
        return (ENUM_ERROR)0x13;

    CItem* itemScript = G_CDataManager()->find_item(slotA.m_addInfo);
    if (itemScript == 0)
        return (ENUM_ERROR)4;
    CItem* materialScript = G_CDataManager()->find_item(material.m_addInfo);
    if (materialScript == 0)
        return (ENUM_ERROR)4;

    // [2026-09-10 P198① 预清必写] ORIG 0x0854a416 在 call 前有
    //   `movl $0x0,-0x24(%ebp)`（cond 预清）⇒ 源必须两句式；
    //   [P225] ORIG 0x0854a45b `cmpl $0x0,-0x24; setne %al; test %al,%al; je`
    //   四段物化 ⇒ while+break 族形。
    int cond = 0;
    cond = _CheckCondition(user, slotA, itemScript, material, msg->m_19);
    while (cond != 0)
    {
        LogManager::logFormat(1, "ItemUpgrade.cpp",
            __PRETTY_FUNCTION__,
            0x756,
            "3RD_CHRONICLE_LOG : ENCHANT SYSTEM _CheckCondition() ERROR!! CHARAC NO(%u) ERROR(%u)",
            user->getCurCharacNo(), cond);
        return (ENUM_ERROR)cond;
    }

    // [2026-09-10 P198① 预清必写] ORIG 0x0854a4b0 `movb $0x0,-0x1e(%ebp)` 在
    //   isDoEnchant 调用前预清 doEnchant ⇒ 源必须两句式。
    bool doEnchant = false;
    doEnchant = isDoEnchant(slotA, ((CStackableItem*)materialScript)->m_chars30d);

    // ORIG ProcUpgrade_0854a1ba 605/611 行：delete_item 与 material2 的 GetInvenSlot
    // 各自重新调用 getCurCharacInvenW()（不复用函数头的 invenW/local_34）。
    if (!user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, msg->m_13, 1, (eItemDelReason)0xc, true))
        return (ENUM_ERROR)0x11;

    packet->put_byte(1);
    packet->put_short(msg->m_13);
    Inven_Item material2 = user->getCurCharacInvenW()->GetInvenSlot(1, msg->m_13);
    // [2026-09-10 判据⑮ 多余截断] ORIG 0x0854a595..0x0854a5a4：get_add_info 返回值
    //   经 %eax 直压 0x4(%esp) 传 put_short(int)（无 movzwl/and $0xffff）⇒ 原码
    //   无 `& 0xffff` 截断，删之照抄。
    packet->put_short(material2.get_add_info());
    if (doEnchant)
        packet->put_byte(1);   // ORIG 619 行
    else
        packet->put_byte(0);   // ORIG 616 行

    // [2026-09-10 P272 块序] ORIG 0x0854a5d7 `movzbl doEnchant; xor $0x1; test
    //   %al,%al; je 0x0854a6fc`：**!doEnchant 真则直落 _ProcPenalty 块**
    //   （0x0854a5e6），3rdChronicle 块在 0x0854a6fc（then 在后、else 直落）。
    //   正极性 `if (doEnchant) {A} else {B}` 在 4.4 反产「then 直落」⇒ 写
    //   `if (!doEnchant) {B} else {A}` 复现。
    // [P0] ORIG 3rdChronicle 调用点**无 ret 具名槽**（call 结果直接 xor 判），
    //   且 [P248 档①] callee 返回 bool ⇒ 写 `if (!ProcUpgrade_3rdChronicle(...))`。
    if (!doEnchant)
    {
        int sellPrice = 0;
        // [2026-09-10 判据㉕ 实参来源] ORIG 0x0854a5f0 `mov -0xd2(%ebp),%eax;
        //   movswl %ax,%ebx`：-0xd2 = slotA(-0xd9)+0x07 = m_addInfo2（数量域），
        //   **不是** msg->m_19（ORIG 全函数 movzbl 0x19(%eax) 仅 3 次：_CheckCondition/
        //   ProcUpgrade_3rdChronicle/Enchant3rdChronicleItem 各一次）。旧重建传
        //   (short)msg->m_19 ⇒ 售价统计用错数量来源（真 bug）。
        G_Store()->GetSellItemPrice(slotA, itemScript, (short)slotA.m_addInfo2,
                                    false, sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);
        // [2026-09-10 P198①] ORIG 0x0854a655 `movl $0x0,-0x1c(%ebp)` 预清；
        //   [P225] ORIG 0x0854a697 `cmpl $0x0,-0x1c; setne %al; test %al,%al; je`。
        int penalty = 0;
        penalty = _ProcPenalty(user, &slotA, itemScript, msg->m_0d, packet);
        while (penalty != 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x77e, 5);
            trace("3RD_CHRONICLE_LOG : ENCHANT SYSTEM _ProcPenalty() ERROR!! ERROR CODE(%d)!! CHARAC NO(%u)\n",
                  penalty, user->getCurCharacNo());
            return (ENUM_ERROR)penalty;
        }
    }
    else
    {
        // ORIG CItem 虚表 +0xc（GetItemType）返回装备类型。
        // [2026-09-10 P0] ORIG 0x0854a706..0x0854a72d：GetItemType() 结果经
        //   %eax 直接压 0x8(%esp)，**无 equipType 具名槽**（无 store/load 往返）
        //   ⇒ 表达式内联进实参。
        if (!ProcUpgrade_3rdChronicle(user,
                                      (ENUM_EQUIPMENTTYPE)itemScript->GetItemType(),
                                      msg->m_19, slotA, materialScript))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x786, 5);
            trace("3RD_CHRONICLE_LOG : ENCHANT SYSTEM ProcUpgrade_3rdChronicle() ERROR!! CHARAC NO(%u)\n",
                  user->getCurCharacNo());
            return (ENUM_ERROR)1;
        }
    }

    if (slotA.m_addInfo != 0)
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_0d, slotA);
    // ORIG 0x0854a8a0 `movzwl 0xd(%eax),%eax; movzwl %ax,%eax`（零扩展两段）
    // ⇒ 原码此处按 unsigned short 读 m_0d（PacketTypes.h 声明为 short，冻结头
    // 不改，调用点显式收窄复现）。
    user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                         (unsigned short)msg->m_0d);

    // ORIG 0x0854a8b2 `movb $0x0,-0x1d; addb $0x1,-0x1d` ⇒ count 是 **char**；
    // 且无 f 指针局部（`movzbl -0xba(%ebp),%eax` = slotA+0x1f 直取）。
    char count = 0;
    if ((slotA.m_random.m_enchFields[0] & 1) != 0)  ++count;
    if ((slotA.m_random.m_enchFields[1] & 1) != 0)  ++count;

    user->m_historyLog.Enchant3rdChronicleItem(doEnchant,
        itemScript->get_index(), material.m_addInfo, msg->m_19, count);
    return ENUM_ERROR_NONE;
}

// ============================================================================
// _CheckCondition(ENCHANT 5 参) @ 0x085497ea（_CheckCondition_085497ea.md）
//   三代强化条件校验。返回错误码，0=通过。
// [b28d-solo-CItemUpgrade 判词] AE（位域视图复现 `and $1; test %al` 与
//   `shr $3,%al`/`shr %al; and $3`；need 改 unsigned 复现 `cmp mem,%eax; jae`
//   单段装载；`ret != 0` 用 P225 while 形复现 setne;test;je 四段物化）。
// ============================================================================
int CItemUpgrade::_CheckCondition(const CUser* user, const Inven_Item& item,
                                  const CItem* itemScript, const Inven_Item& ticket,
                                  unsigned char param)
{
    // ORIG 0x085497ea 逐段转写（_CheckCondition_085497ea.md）。
    if (!item.m_amp.isIdentified())
        return 0xae;
    if (item.m_field1 != 1)
        return 0x13;
    if (itemScript->GetItemType() == 0xb)
        return 0x13;
    // P48 无 CSE：两臂各独立调用 GetItemType。
    if (itemScript->GetItemType() < 10 || itemScript->GetItemType() > 0x15)
        return 0x13;
    if (item.m_fieldb !=
        ((const CEquipItem*)itemScript)->get_endurance())
        return 7;

    // 空间检查：预测分解结果物品数 + 1，增幅再 +1。
    // ORIG 0x08549938 `cmp -0x14(%ebp),%eax; jae`（mem 直比 + 无符号序）⇒ need
    // 是 unsigned 槽（int 槽时 4.4 恒先 `mov %eax,%edx; mov -0x14,%eax` 两段，
    // P209）。
    unsigned int need =
        (unsigned int)G_CDataManager()->m_disJoint->getPredictResultItemTypeCount(
            itemScript->get_rarity());
    ++need;
    if (item.m_amp.hasAbility())
        ++need;
    // [b28d-solo-CItemUpgrade] ORIG 0x08549944 条件直接内联进 if（无 bool 局部）：
    //   cmpl $0,-0x14; je skip; …lea (%ebx,%eax,1); cmp -0x14(%ebp),%eax; jae skip;
    //   mov $1; jmp; skip: mov $0; test %al,%al; je。写具名 bool 会多
    //   `mov %al,-0x15; cmpb $0,-0x15` 两段槽物化（P133/P214①）。
    // P48：InvenEmptycount/QuickEmptyCount 各自 getCurCharacInvenR。
    if (need != 0 &&
        (unsigned int)(user->getCurCharacInvenR()->InvenEmptycount(
                           Inven_Item::ITEM_TYPE_3) +
                       user->getCurCharacInvenR()->QuickEmptyCount()) <
            (unsigned)need)
        return 0x15;

    const CItem* materialScript =
        G_CDataManager()->find_item(ticket.m_addInfo);
    if (itemScript == 0)
        return 0x15;

    // 附魔槽（item+0x1f/0x20 = m_random.m_skill0/1.m_field）：双槽就绪或
    // 首槽就绪且类型/技能匹配材料脚本 +0x30c（P48：逐次重新读字段）。
    // [b28d-solo-CItemUpgrade] 位域视图：ORIG 0x08549986/0x08549994 `movzbl;
    // and $0x1; test %al`（无 uchar→int 加宽）与 `shr $3,%al`（8 位无符号移位）
    // 只能由真实位域成员复现（`m_field & 1` 多 `movzbl %al,%eax`；`m_field >> 3`
    // 产 32 位 sar，探针 c1/c2/c4/c5 实证）。
    if (item.m_random.m_skill0.m_bits.m_flag &&
        item.m_random.m_skill1.m_bits.m_flag)
        return 0x13;
    if (item.m_random.m_skill0.m_bits.m_flag &&
        item.m_random.m_skill0.m_bits.m_skillId == param &&
        item.m_random.m_skill0.m_bits.m_type ==
            (int)((const CStackableItem*)materialScript)->m_char30c)
        return 0x13;

    // 遍历装备附魔表匹配 job+growType（迭代器直构 P50，end 缓存 P13）。
    // [b28d-solo-CItemUpgrade] ORIG 0x08549a0f 无 equip 具名局部（P0）：
    //   每次 `mov itemScript,%eax; mov %eax,(%esp); call getEnchantSystemData`，
    //   写具名指针会多 `mov %eax,-0x10; mov -0x10,%eax` 两段并顶浅迭代器槽。
    std::vector<STEnchantSystemData>::const_iterator it =
        ((const CEquipItem*)itemScript)->getEnchantSystemData().begin();
    std::vector<STEnchantSystemData>::const_iterator end =
        ((const CEquipItem*)itemScript)->getEnchantSystemData().end();
    bool matched = false;
    for (; it != end; ++it)
    {
        if (user->get_charac_job() == it->m_a &&
            (unsigned char)user->getCurCharFirstGrowType() == it->m_b)
        {
            matched = true;
            break;
        }
    }
    if (!matched)
        return 1;
    return 0;
}

// ============================================================================
// ProcUpgrade_3rdChronicle @ 0x0854a006（ProcUpgrade_3rdChronicle.md）
//   三代强化落点：按 key(job/growType/equipType/param) 查 itemScript+0x2f4 附魔表；
//   命中 → item+0x1f 或 item+0x20（按首字节 &1 二选一）写入附魔类型/槽位，返回 1；
//   未命中 → 0。
// [b28d-solo-CItemUpgrade 判词] AE（P285 正向 if/else{return 0} 尾 return 1 +
//   val 作用域收窄 + 位域视图）。原 DIFF 134/133：
//   本轮修：非 const map/迭代器（P96/P270，消 p167 MISSING/EXTRA 各 4）、位域赋值
//   复现三段 RMW、内联 map2f4/stackItem/f 局部。残余 = N93附（return 1 的
//   `mov $1,%ebx` 在 vector 析构之后 vs 4.4 之前）+ %al/%dl 寄存器名 ⇒ N 封顶。
// ============================================================================
// [P248 档①/P292] 调用点 ORIG 0x0854a743 `call; xor $0x1,%eax; test %al,%al; je`
//   ⇒ 返回类型 bool（int 返回恒产 sete 三段）。
bool CItemUpgrade::ProcUpgrade_3rdChronicle(CUser* user, ENUM_EQUIPMENTTYPE equipType,
                                            unsigned char param, Inven_Item& item,
                                            const CItem* itemScript)
{
    // ORIG 0x0854a006：四字段键（job/growType/equipType/param）查
    // CStackableItem+0x2f4 附魔 map（键 STEnchantSystemMapKey，值
    // STEnchantSystemMapData 含 vector<EquipmentSkillDataUp>）；命中 →
    // item+0x1f 或 item+0x20（按首字节 &1 二选一）写入附魔类型/槽位，返回 1；
    // 未命中 → 0。值 vector 仅作命中判定（拷贝后立即析构），不读取内容。
    STEnchantSystemMapKey key;
    key.m_a = user->get_charac_job();
    key.m_b = user->getCurCharFirstGrowType();
    key.m_c = (int)equipType;
    key.m_fieldC = param;

    // [b28d-solo-CItemUpgrade] ORIG 0x0854a048 无 map2f4 引用/无 f 指针局部
    // （P0）：find/end 各自重算 `mov itemScript,%eax; lea 0x2f4(%eax),%ecx`，
    // 字段访问逐次 `mov &item,%eax; movzbl 0x1f(%eax)`。
    // [b28d-solo-CItemUpgrade] ORIG 0x0854a062 用**非 const** map/迭代器
    // （`map::findERS5_` + `_Rb_tree_iterator::ne/ptEv`）；const 版会换成
    // `findERKS5_` + `_Rb_tree_const_iterator`（p167 MISSING/EXTRA 各 4）。
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData>::iterator it =
        ((CStackableItem*)itemScript)->m_enchantMap.find(key);

    if (it != ((CStackableItem*)itemScript)->m_enchantMap.end())
    {
        // ORIG：命中后拷贝值 vector（仅作命中判定，内容不读取）再修改 item。
        // itemScript 实参为 CStackableItem*（ORIG 逐次读 +0x30c 附魔类型字节，
        // 无具名 stackItem 局部）。
        {
            std::vector<EquipmentSkillDataUp> val(it->second.m_vec0);
            // [b28d-solo-CItemUpgrade] ORIG 0x0854a0bc..0x0854a169 三段 RMW 是
            // **位域赋值**形：`or $0x1` 置 bit0 / `and $0xfffffff9; or` 置 bit1-2 /
            // `and $0x7; lea (,%edx,8)` 置 bit3-7。写裸 `field |= 1; field =
            // (field&0xf9)|...` 4.4 每条多 `mov %eax,%edx` 中转 + 重载 this
            // （144 vs 134）。位域成员即 m_bits{m_flag:1,m_type:2,m_skillId:5}。
            if (item.m_random.m_skill0.m_bits.m_flag == 0)
            {
                item.m_random.m_skill0.m_bits.m_flag = 1;
                item.m_random.m_skill0.m_bits.m_type =
                    static_cast<const CStackableItem*>(itemScript)->m_char30c;
                item.m_random.m_skill0.m_bits.m_skillId =
                    param;
            }
            else
            {
                item.m_random.m_skill1.m_bits.m_flag = 1;
                item.m_random.m_skill1.m_bits.m_type =
                    static_cast<const CStackableItem*>(itemScript)->m_char30c;
                item.m_random.m_skill1.m_bits.m_skillId =
                    param;
            }
        }
    }
    else
    {
        return 0;
    }
    // [b28d-solo-CItemUpgrade] P285 形：正向 if + `else{return 0;}` + 函数尾
    // `return 1`。ORIG 0x0854a172 `call ~vector` 之后是 `jmp a180`（跳到
    // mov $1 块，unmatched 的 mov $0 块夹在中间）——直接双 return 时 4.4 把
    // mov $1 放 fall-through（少 1 条 jmp，N93附）；正向包裹后逐条复现。
    return 1;
}

// ============================================================================
// _ProcPenalty(ENCHANT 5 参) @ 0x08549ac8（_ProcPenalty_08549ac8.md）
//   三代强化惩罚：删材料 → 分解 → 附加物品入包+插入事件物品+统计。返回错误码。
// [2026-09-10 判词] DIFF（origN=ourN=357；jcc 逐位一致；p167 CLEAR 54/54）。
//   本轮真 bug 修：删 ORIG 0x08549d94 之后**无**的 `if (insertRet != 0)` 门禁
//   （判据⑮：put_byte 后直接 begin；旧包裹使 outSlots 在 insertRet==0 时被跳过）
//   + P223 if-join 岛（0x08549e99 `je 08549f7d(nop)` + body 尾 `jmp`）。
//   残余 = 实参装载调度（N7/P75）+ 槽号 ⇒ N 封顶。
//   本轮修**真语义 bug 4 处**：①djr 读索引 [4+i]→[5+i]（ORIG 0x08549c10 权威）；
//   ②make_pair(fi,fi)→make_pair(m_f[i],m_f[i+5]) 两不同字段；③第二次
//   _getAdditionalDisjointItem 漏传 hasAbility 第 5 实参（5 参重载）；
//   ④outSlots 循环整段缺失的 make_item + collectCubeStatistics + GetSellItemPrice
//   + AddValueStatistic(9) 逐物品统计已补。残余 = 帧/槽号与寄存器化 N 类 ⇒ N 封顶。
// ============================================================================
int CItemUpgrade::_ProcPenalty(CUser* user, Inven_Item* item,
                               const CItem* itemScript, unsigned short slot,
                               PacketGuard* packet)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x08549ad4 无 amp 引用局部（P0）：
    //   `mov item,%eax; add $0x11,%eax; mov %eax,(%esp); call hasAbility` 逐次直取。
    const upgrade_table_t* table = item->m_amp.hasAbility()
        ? GlobalData::s_itemAmplifier_->getUpgradeInfoTable()
        : getUpgradeInfoTable();

    // ORIG 0x08549b0c 无 invenW 具名局部（P0）：getCurCharacInvenW() 的返回值
    // 直接作为 delete_item/insert_event_items 的 receiver（两次各自重调）。
    if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                                 (eItemDelReason)0xd, true))
        return 0x11;

    unsigned int upgrade = (unsigned int)(unsigned char)item->GetUpgrade();
    packet->put_short(slot);

    DisJointResult djr;
    // ORIG 0x08549b9e 无 dj/itemIdx 具名局部：itemIdx 跨 G_CDataManager() 保活于
    // %ebx，`mov 0x1c(%eax),%eax` 直取 m_disJoint 后立即传参。
    if (G_CDataManager()->m_disJoint->GetResult(item->m_addInfo, djr) != 0)
    {
        packet->put_byte(0);
        return 0;
    }

    item->m_addInfo = 0;
    std::vector<std::pair<int, int> > addItems;
    addItems.reserve(6);
    // [b28d-solo-CItemUpgrade] insertRet 声明前置于循环之前：ORIG 槽位序为
    // insertRet -0x20 / i -0x1c（4.4 块内按 lifetime 复用会把 i 排到 -0x20），
    // 按 P212「先声明深」排声明序复现。
    int insertRet;
    for (int i = 0; i < 5; ++i)
    {
        // 真 bug 修正（ORIG 0x08549c10 权威）：条件读 `mov -0x88(%ebp,%eax,4)`
        // （eax=i+4，djr@-0x8c ⇒ m_b[i]）。
        // [b28d-solo-CItemUpgrade] 实参顺序按 ORIG 0x08549c2d/0x08549c41 权威：
        // 先算 `&djr[i]`（m_a）→ %ecx → 0x8(%esp)（第二实参），再算
        // `&djr[i+0x14]`（m_b）→ %edx → 0x4(%esp)（第一实参）⇒ 源形是
        // `make_pair(m_b[i], m_a[i])`（原重建两字段顺序写反 ⇒ 真 bug）。
        // [b28d-solo-CItemUpgrade] ORIG 0x08549c1f `je <nop 岛>` + body 尾
        // `jmp 08549c6f` + `08549c6e: nop`（4.1 空 else 占位岛）⇒ P223 复现形
        // = 否定条件 goto 到体后空标签（正向 if 会被 4.4 jump-thread 掉 jmp+nop）。
        if (djr.m_b[i] == 0)
            goto push_done;
        addItems.push_back(std::make_pair(djr.m_b[i], djr.m_a[i]));
    push_done:
        ;
    }
    // [b28d-solo-CItemUpgrade] ORIG 0x08549ca0/0x08549ce4/0x08549d2b 三段：
    //   ①无条件 4 参版（局部 *table）；②if(hasAbility) 内 4 参版（**重算**
    //   getUpgradeInfoTable()）；③无条件 5 参版（重算 table + hasAbility 第 5 实参）。
    // 原重建只有「if→4 参 + 无条件 4 参」⇒ p167 MISSING hasAbility×1、
    // getUpgradeInfoTable×1、4 参版×1、5 参版×1。
    _getAdditionalDisjointItem(upgrade, itemScript, addItems, *table);
    if (item->m_amp.hasAbility())
        _getAdditionalDisjointItem(upgrade, itemScript, addItems,
                                   *getUpgradeInfoTable());
    _getAdditionalDisjointItem(upgrade, itemScript, addItems,
        *getUpgradeInfoTable(), item->m_amp.hasAbility());

    std::vector<int> outSlots;
    insertRet = user->getCurCharacInvenW()->insert_event_items(addItems,
        outSlots, (eMoneyAddReason)7, (eItemAddReason)0x13, false, "", "");
    packet->put_byte(insertRet);

    // [b28d-solo-CItemUpgrade] ORIG 0x08549dc5 循环：每次 `*it++` 三次取
    // slotVal/idx/cnt（无 it+=3、无 &0xffff 截断），随后**逐物品**补做
    // make_item + cube 统计 + 售价统计（原重建缺整段，属语义缺失，已补）。
    // [2026-09-10 判据⑮ 多余门禁] ORIG 0x08549d94 `call put_byte` 之后**直接**
    //   0x08549d99 `lea -0x3c; call begin`（无 insertRet 判空/判非零分支）⇒
    //   原码无 `if (insertRet != 0)` 包裹，删之照抄（outSlots 为空时循环自然不执行）。
    for (std::vector<int>::const_iterator it = outSlots.begin();
         it != outSlots.end(); )
    {
        int slotVal = *it++;
        int idx = *it++;
        int cnt = *it++;
        packet->put_short(slotVal);
        packet->put_item_idx((unsigned long)idx);
        packet->put_int(cnt);
        CItem* script = G_CDataManager()->find_item(idx);
        // [2026-09-10 P223] ORIG 0x08549e99 `je 08549f7d`（4.1 if-join 岛：
        //   false 路落 nop 占位）+ body 尾 `jmp 08549f7e`。正向 if 被 4.4
        //   jump-thread 掉 jmp+nop ⇒ 负条件 goto 到尾空标签复现（P223 定形）。
        if (script == 0)
            goto next_item;
        {
            Inven_Item invenItem;
            invenItem.m_addInfo = idx;
            invenItem.set_add_info(cnt);
            script->make_item(invenItem);
            GetInstanceCubeStatistic()->collectCubeStatistics(
                script, cnt, user, (CUBE_STATISTIC_FIELD)1);
            int sellPrice = 0;
            G_Store()->GetSellItemPrice(invenItem, script,
                (short)invenItem.m_addInfo2, false, sellPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)9, user, (unsigned int)sellPrice);
        }
    next_item:
        ;
    }
    return 0;
}
}  // namespace WongWork
