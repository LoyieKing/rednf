// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureE/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// LinkClosureE.cpp — 全量链接收敛批 E 散件 producer（LinkClosureE 批次）
//
// 覆盖符号（ORIG nm -S --defined-only 核对；报告路径见各段注释）：
//   CMaxFatigueFactorEvent::GetMaxFatigueFactor()            W 0x0812ce54
//   CBuffGroup::CBuffGroup(CUser*) / IsRight(CUser*)         W 0x08697c3e / 0x08697c64
//   SendLV70Buff(CUser*, unsigned long)                      T 0x0865defc
//   CItemMakingSkill::Send_ItemMakingSkillList(CUser*) const T 0x08545a62
//   Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int) T 0x081a329e
//
// 注：CMaxFatigueFactorEvent / CBuffGroup / CBelong / CItemMakingSkill 在
// CUserOnlyOrig.cpp 等消费 TU 内为最小声明视图；本 TU 提供权威完整视图 + 定义，
// mangled 与 ORIG 一致。CBelong 各 TU（Area.cpp 等）已有同型局部视图。

#include "CBoosterGage.h"         // charac_expand::CData 权威基类（pack(1) 0x5）
#include "PvPTypes.h"             // CMTRand（CItemMakingSkill +0x1c 字段）
#include "PacketGuard.h"
#include "GameWorld.h"            // G_GameWorld / send_to_area 两重载
#include "CUser.h"                // CUser::Send/get_area/get_unique_id/GetServerGroup
#include "CUserCharacInfo.h"      // getCurCharacVill / get_charac_level / getCurCharacNo
#include "CDataManager.h"         // G_CDataManager / find_item
#include "CItemList.h"            // Arad_GiftItem_Set（find / send_mail 声明）
#include "Arad_ItemBonus.h"       // ARAD::send_mail / ARAD_DATAS::AradMailInfo
#include "Inven_Item.h"
#include "CStackableItem.h"      // CStackableItem::m_vec168（兔盒列表 +0x168）
#include "CInventory.h"          // insertItemIntoInventory（Inven_Item 按值入包）
#include "CCharacList.h"         // CCharacList::m_characs[11]（CDataManager+0x14）
#include "CItem.h"                // CItem::make_item 虚槽（vtable slot +0x8）
#include "CMailBoxHelper.h"       // ReqDBSendNewMail
#include "LogManager.h"
#include "GameTypes.h"            // cMyTrace

class CUser;
class CStackableItem;
class CBelong;

// ---------------------------------------------------------------------------
// global::tools::InitializeCharacStat::initCharacStat(_Additioal_info&,
//     unsigned int, int, int, int, int)
// ORIG T _ZN6global5tools20InitializeCharacStat14initCharacStatER15_Additioal_infojiiii
// @0x08145e64 sz0x43f。
// 报告 docs/class_func_reports/global__tools__InitializeCharacStat/initCharacStat.md。
// info 全零守卫 → clear → job/level 校验（0x1f 行 trace）→ memcpy 职业基表
// （CDataManager::m_characList + job*0x7dc，0x52 字节）→ 三个成长态
// get_growtype_stat 校验（0x2d 行 trace）→ info = 基表 → level-1 次分段 add
// （<=0xe 态1 / <=0x31 态2 / 其余态3）→ 0x41 行结果 trace。
// ---------------------------------------------------------------------------
namespace global
{
namespace tools
{

class InitializeCharacStat
{
public:
    static void initCharacStat(_Additioal_info& info, unsigned int characNo,
                               int job, int level, int grow1, int grow2);
};

// [b28d-solo-LinkClosureE 旧判词，已被 D08 推翻] 原 DIFF 299/314：残差 = 7 个
//   epilogue 中继岛（每守卫 1 组 `jmp+nop`，共 14 条），当时 `return`/独立 `goto` 两形
//   均不消 ⇒ 曾判 N121 封顶。**[D08 2026-09-10] 该 14 条已按 C05 范式③ 全消**
//   （our_insns 314→300，见下方判词）⇒ N121 判词作废。其余逐条全等：七守卫
//   `movzwl/mov + test + jne` 直达尾、结构体拷贝（对齐剥离 + rep movsl + 尾 2/1 字节，
//   替代 memcpy）、三 trace 槽 -0x58/-0x48/-0x38、12 个 vararg 全表达式。
// [D08 2026-09-10] 消岛后 gap 精确分解（仅余 +1）：基表源地址计算 ——
//   ORIG 0x08145f5c 段 `mov 0x14(%eax),%ecx; mov job,%eax;
//   imul $0x7dc,%eax,%edx; lea (%ecx,%edx,1),%edx` vs 我方 `mov 0x14(%eax),%eax;
//   mov %eax,%edx; imul $0x7dc,%eax,%eax; add %eax,%edx`（4.4 多一条 eax→edx 中转）。
//   探针 /tmp/addr_probe.cpp g0–g6（char* 算术 / 类型化下标 / uint 算术+回 cast /
//   交换操作数 / u 后缀 七形）读数全同 g0；唯一省掉中转的 g2（具名基址指针）引入
//   `mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax` 槽往返 ⇒ 净 −1 条反劣。判 N18a。
//   [C05 范式③ 命中·D08 实测] 七守卫由「7 条独立 `if(…) goto L_done;`」改为**单条
//   `||` 短路 `if (c0||c1||…||c6) goto L_done;`**：4.4 对短路 or 逐条
//   `movzwl/test/jne` **直达同一尾标签**，7 组 `jmp+nop` 中继岛全消 ——
//   our_insns 314→**300**（ORIG 299）、gap +15→**+1**。反例留痕：`do{…}while(0)`+9 处
//   `break` 形实测仍 314（break 出口同样物化为岛）；7 条独立 goto 形亦 314。
//   余 +1 条 = 上述基表源地址 `imul/lea` 装载形（N18a，探针 g0–g6 七形不可造）。
//   **适用边界（Main 汇总 A06/A28 反例）**：本法仅在「ORIG 有**相邻多守卫·同目标**、
//   且我方多岛」时成立 —— `_onChangeGrowType` 按同法合并两条 goto **反劣 +4**（199/207），
//   `CWorldDrop::generateRandomItem` / `CPrivateStore::IsAreaNearEntranceDungeon` 类岛根因
//   是 4.4 epilogue-threading / rel8 跳板（拉长后造跳板 +1 jmp）⇒ 源码不可控，命中即按
//   A28 封顶。判据：先数 ORIG 各 `je/jne` 的**落点是否为同一址**；同址才试合并。
//   **试前闸门（Main/C05 量化）**：仅当「我方岛数 − ORIG 岛数 ≥ 2」才试 —— 本件 7−0=7
//   远过闸门（正例）；差 1 岛的多为 4.4 rel8 跳板/epilogue-threading。同 TU 反例：
//   `add_monster_list_bossroom_condition` 三守卫同目标合并**零收益**（176/ORIG 173）；
//   正例参照 `CBattle_Field::choose_random_bossmap_from_dummymaps` 尾部岛 5→3 ⇒ DIFF→AE 236=236。
//   **三手协议（Main/C05③ 定稿）**：① 守卫为**纯条件短路（无 call）** ⇒ 本手 `||` 合并
//   （本件即此，命中）；② 守卫**含 call/setcc 操作数** ⇒ 改**正向嵌套 `if`**
//   （F3c 实证：`AvatarRechargeServer::CalcDurabilityDetail` ours 96→90=ORIG、DIFF→AE；
//   同件 `||` 合并净零）；③ 两手不成 ⇒ A28 封顶。另：「同目标 jcc 多」≠「我方多岛」
//   —— 计数须落在 `jmp+nop` 岛而非 bool 物化形（F3c `importActionDefine` 2:2 判例）。
#include "LinkClosureE/initCharacStat.inc"
                                          unsigned int characNo, int job,
                                          int level, int grow1, int grow2)
{
    // ORIG 0x08145e70..0x08145ed3：七条独立守卫各自 jne 直跳函数尾
    // [D08 2026-09-10] C05 范式③试验：七守卫合并为单条 `||` 短路 goto
    // （4.4 对短路 or 逐条 test/jne 直达同一尾标签，企消 7 组 jmp+nop 岛）。
    if (info.m_field08 != 0 || info.m_field0c != 0 || info.m_field0a != 0
        || info.m_field0e != 0 || info.m_field00 != 0 || info.m_field04 != 0
        || info.m_field42 != 0)
        goto L_done;

    info.clear();
    if (job < 0 || job > 10 || level > 0x46 || level < 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1f, 5);
        trace("[ARAD,STAT,RESET] Invalid character job. charac %u, job %d, level %d",
              characNo, job, level);
        goto L_done;
    }

    {
        // ORIG 0x08145f4c..0x08145fb9：结构体拷贝（对齐剥离 + rep movsl + 尾字节）
        _Additioal_info base = *reinterpret_cast<const _Additioal_info*>(
            reinterpret_cast<const char*>(G_CDataManager()->m_characList) +
            job * 0x7dc);

        const void* stat1 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(0, 0);
        const void* stat2 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(grow1, 0);
        const void* stat3 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(grow1, grow2);
        if (stat1 == 0 || stat2 == 0 || stat3 == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x2d, 5);
            trace("[ARAD,STAT,RESET] Invalid character grow type. charac %u, first %d, second %d",
                  characNo, grow1, grow2);
            goto L_done;
        }

        info = base;
        for (int i = 1; i < level; ++i)
        {
            if (i <= 0xe)
                info.add(*reinterpret_cast<const _Additioal_info*>(stat1));
            else if (i <= 0x31)
                info.add(*reinterpret_cast<const _Additioal_info*>(stat2));
            else
                info.add(*reinterpret_cast<const _Additioal_info*>(stat3));
        }

        // ORIG 0x0814616e..0x081461e8：12 个 vararg 全为表达式（右起求值，前 9 个 spill、
        // 末 3 个落 %edi/%esi/%ebx）⇒ 不设具名局部。
        cMyTrace(__PRETTY_FUNCTION__, 0x41, 0)(
            "[ARAD][RESET STAT] Init Character : %d, stat : hp %d, mp %d, pyatt %d, pydef %d, mgatt %d, mgdef %d, speed %d, element %d, element %d, element %d, element %d)",
            characNo, (int)info.m_field00, info.m_field04, (int)info.m_field08,
            (int)info.m_field0a, (int)info.m_field0c, (int)info.m_field0e,
            info.m_field42, (int)(short)info.m_field10[0],
            (int)(short)info.m_field10[1], (int)(short)info.m_field10[2],
            (int)(short)info.m_field10[3]);
    }
L_done:;
}

}  // namespace tools
}  // namespace global

// ---------------------------------------------------------------------------
// CMaxFatigueFactorEvent::GetMaxFatigueFactor()
// ORIG W _ZN22CMaxFatigueFactorEvent19GetMaxFatigueFactorEv @0x0812ce54 sz0xb：
//   push/mov; mov 0x8(%ebp),%eax; mov 0xc(%eax),%eax; pop; ret —— 返回 +0xc int。
// 报告缺失（objdump 转写）。消费方 CUserOnlyOrig.cpp（A-2 域，禁碰）。
// ---------------------------------------------------------------------------
class CMaxFatigueFactorEvent
{
public:
    int GetMaxFatigueFactor();   // ORIG W 0x0812ce54

    char m_pad0c[0xc];           // +0x00..+0x0b（布局未还原区）
    int m_fatigueFactor;         // +0x0c（GetMaxFatigueFactor 返回值）
};

#include "LinkClosureE/GetMaxFatigueFactor.inc"
{
    return m_fatigueFactor;
}

// ---------------------------------------------------------------------------
// CBuffGroup（CBelong 派生，ORIG vtable @0x08cf3998 slot0=IsRight）
// CBuffGroup::CBuffGroup(CUser*) W @0x08697c3e：call CBelongC1 → 置 vptr
//   → this+0x04 = user。IsRight W @0x08697c64（报告缺失，objdump 转写）：
//   user 等级 >0x45 → 0；|dx|>0x12c 或 |dy|>0x12c → 0；否则 1。
// ---------------------------------------------------------------------------
class CBelong
{
public:
    virtual char IsRight(CUser* user) = 0;   // 抽象虚槽（ORIG 基类槽 __cxa_pure_virtual）
};

class CBuffGroup : public CBelong
{
public:
    CBuffGroup(CUser* user);                 // ORIG W 0x08697c3e
    char IsRight(CUser* user);               // ORIG W 0x08697c64（虚槽 slot0）

    CUser* m_user;                           // +0x04
};

#include "LinkClosureE/CBuffGroup.inc"
{
    m_user = user;
}

// [D08 2026-09-10] DIFF 61/62 → **AE 61/61**（TU 三数 ae 20→21、diff 5→4；p167
//   CLEAR 5/5）。修法 = 配方⑦/P0：删「ORIG 无槽」的具名 `unsigned char over` 局部，
//   整个短路条件（`abs(dx)>0x12c || abs(dy)>0x12c`）内联进 if。ORIG 0x08697d00 末尾为
//   `mov $0x1/%eax; jmp; mov $0x0/%eax; test %al,%al; je`（bool 物化**直测**、全程无
//   栈槽往返）⇒ 原码无该局部；具名局部使 4.4 发 `movb %al,-0x9(%ebp);
//   cmpb $0x0,-0x9(%ebp)` 且帧 `sub $0x24` vs ORIG `sub $0x14`（多 0x10）。
//   内联后逐条全等（帧 0x14 亦归位）。旧 N18a/N96 判词作废。
#include "LinkClosureE/IsRight.inc"
{
    if (user->get_charac_level() > 0x45)
        return 0;
    if (__builtin_abs((int)(unsigned short)user->get_posX() -
                      (int)(unsigned short)m_user->get_posX()) > 0x12c
        || __builtin_abs((int)(unsigned short)user->get_posY() -
                         (int)(unsigned short)m_user->get_posY()) > 0x12c)
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// SendLV70Buff(CUser*, unsigned long)
// ORIG T _Z12SendLV70BuffP5CUserm @0x0865defc sz0x1b2。
// 报告 docs/class_func_reports/_lt_global_gt_/SendLV70Buff.md（## 汇编+## 反编译 C）。
// 包 0x100（param→int, 0xe10）直发 + send_to_area（带 CBuffGroup）；
// 包 0x103（get_unique_id 低 16 位, param）send_to_area（无 belong）。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureE] AE 115/115（p167 CLEAR 25/25）：area/vill 去具名局部
//   （P0 表达式直传 %ebx/%esi），group/packet 槽 -0x10/-0x1c 全对齐。
#include "LinkClosureE/SendLV70Buff.inc"
{
    CBuffGroup group(user);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x100);
    packet.put_byte(1);
    packet.put_int((int)buff);
    packet.put_int(0xe10);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet, &group);
    packet.clear();
    packet.put_header(0, 0x103);
    packet.put_short((int)(unsigned short)user->get_unique_id());
    packet.put_int((int)buff);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet);
}

// ---------------------------------------------------------------------------
// CItemMakingSkill（charac_expand::CData 派生）
// ORIG vtable @0x08c9c340：slot1=loadData 0x08545202、slot2=getData 0x08545b42、
//   slot4/5=D1/D0、slot6=_saveData 0x08545276、slot7=_reset 0x085453ba ——
//   与 charac_expand::CData 虚序一致（ResetDailyMidnight/ResetDaily 走基类空实现）。
// ctor 0x08545108（objdump）：CData ctor → vptr → +0x1c = new CMTRand(time(0))；
// dtor 0x08545192：delete +0x1c 后置 0 → CData dtor。字段 +0x06..+0x1b 为
// Send_ItemMakingSkillList 逐槽读取的 11 项技能等级表（movzwl 0x6(%eax,%edx,2)）。
// 其它方法（UpSkillLevel/GainItemMakingSkillPoint 等）实现由后续批次分派。
// ---------------------------------------------------------------------------
class CMTRand;

class CItemMakingSkill : public charac_expand::CData
{
public:
    int Send_ItemMakingSkillList(CUser* user) const;   // ORIG T 0x08545a62
    int GetItemMakingSkillLevel(const CStackableItem* item, CUser* user);   // ORIG T 0x08545cf4
    int GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const;       // ORIG T 0x08545c0c
    void UpSkillLevel(ENUM_ITEM_MAKING_TYPE type, int point, CUser* user);  // ORIG T 0x0854571c
    void GainItemMakingSkillPoint(const CStackableItem* item, CUser* user); // ORIG T 0x085453e0
    void SetItemMakingSkillPoint(int skillIdx, int point, CUser* user);     // ORIG T 0x08545d24
    char m_pad5;                // +0x05（1 字节；语义未还原 [推断]）
    unsigned short m_skillLevels[11];   // +0x06..+0x1b（11 项等级表）
    CMTRand* m_rand;            // +0x1c（ctor new CMTRand(time(0))，dtor delete）
};

// ORIG T _ZNK16CItemMakingSkill24Send_ItemMakingSkillListEP5CUser @0x08545a62。
// 报告 docs/class_func_reports/CItemMakingSkill/Send_ItemMakingSkillList.md。
// put_header(0,0x15e) → put_byte(0xb) → i=0..0xa {put_byte(i); put_short(表[i])}
// → finalize(true) → Send → 返回 1。
#include "LinkClosureE/Send_ItemMakingSkillList.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x15e);
    packet.put_byte(0xb);
    for (int i = 0; i <= 0xa; ++i)
    {
        packet.put_byte(i);
        packet.put_short((short)m_skillLevels[i]);
    }
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---------------------------------------------------------------------------
// CItemMakingSkill 技能成长链（报告 docs/class_func_reports/CItemMakingSkill/）。
// ---------------------------------------------------------------------------

// ORIG T _ZNK16CItemMakingSkill13GetSkillLevelE21ENUM_ITEM_MAKING_TYPEP5CUser
// @0x08545c0c。报告 GetSkillLevel.md：idxByExp = 第一个 vec[i].first > 当前点数
// 的下标；idxByLevel = 第一个 user 等级 < vec[i].second 的下标；取 min。
// [b28d-solo-LinkClosureE] AE 76/76（p167 CLEAR 7/7）：删 cur 具名（P0）；
//   两循环改 `for(init; i<size;) { if (cur>=first){++i} else break; }` 复现 ORIG 的
//   setge/setl + je 物化与「cond1 底部、cond2 在 body 顶、++ 在 cond2 后」布局；
//   尾部三目 `(idxByExp<=idxByLevel)?…` 对齐。idxByLevel 方向修正为 `level>=second`
//   （ORIG setge；旧写法 setl 方向相反）。
#include "LinkClosureE/GetSkillLevel.inc"
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    int idxByExp = 0;
    int idxByLevel = 0;
    for (idxByExp = 0;
         static_cast<int>(idxByExp) < static_cast<int>(table->m_vec18.size());)
    {
        if (static_cast<int>(static_cast<short>(m_skillLevels[type]))
            >= static_cast<int>(table->m_vec18[idxByExp].first))
        {
            ++idxByExp;
        }
        else
        {
            break;
        }
    }
    for (idxByLevel = 0;
         static_cast<int>(idxByLevel) < static_cast<int>(table->m_vec18.size());)
    {
        if (user->get_charac_level()
            >= static_cast<int>(table->m_vec18[idxByLevel].second))
        {
            ++idxByLevel;
        }
        else
        {
            break;
        }
    }
    return (idxByExp <= idxByLevel) ? idxByExp : idxByLevel;
}

// ORIG T _ZN16CItemMakingSkill23GetItemMakingSkillLevelEPK14CStackableItemP5CUser
// @0x08545cf4：GetSkillLevel(GetUpSkillType(item), user)。
#include "LinkClosureE/GetItemMakingSkillLevel.inc"
{
    // P0：ORIG `call GetUpSkillType; mov %eax,-0xc` 具名局部槽；直传会丢 -0xc。
    ENUM_ITEM_MAKING_TYPE type =
        static_cast<ENUM_ITEM_MAKING_TYPE>(item->GetUpSkillType());
    return GetSkillLevel(type, user);
}

// ORIG T _ZN16CItemMakingSkill12UpSkillLevelE21ENUM_ITEM_MAKING_TYPEiP5CUser
// @0x0854571c。报告 UpSkillLevel.md：type>0xa 或表空守卫 → 直接返回；
// 仅当 point∈(0,m_bonusExp] 且 point<=vec.back().first 时加值；随后按等级
// （0x46 级钳 vec.back().first / 否则逐档钳制）+ 1000 上限 clamp；alter() 通知。
// [b28d-solo-LinkClosureE] DIFF 235/233（p167 CLEAR 24/24）：唯一残差 = 技能点加法
//   的指令选择——ORIG `movzwl→%eax; mov %eax,%ecx; mov point,%eax;
//   lea (%ecx,%eax,1),%eax; mov %eax,%ecx` vs 4.4 `movzwl→%ecx; add %eax,%ecx`
//   （差 2 条中转）⇒ N86「同一算术多 codegen」封顶（`+=` / 显式 int 提升 / 去 cast
//   三形实测同读数）。其余全等：守卫 `!(point>0 && point<=m_bonusExp && first>=point)`、
//   trace 临时链（P227 令 vararg 在 ctor 前落 %esi/%ebx）、rbegin()->first 族。
// [D08 2026-09-10] 复测加固（探针 /tmp/usk_probe.cpp f0–f7）：`+=`、显式赋值的
//   ushort/int/uint 提升、交换操作数、双层 cast、int 回 cast 八形读数**全同** f0
//   （`movzwl→%ecx; add %eax,%ecx`）；唯一异形 f3（交换+混型）产 `push %ebx` 且 point
//   升格 %ebx，与 ORIG 无 ebx 保存相反 ⇒ 反劣。ORIG 两段中转（0x085457e7 / 0x085457ef
//   各一条 `mov %eax,%ecx`）4.4 不可造 ⇒ N18a+N1 封顶。
#include "LinkClosureE/UpSkillLevel.inc"
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    if (10 < type)
        return;
    if (table->m_vec18.rbegin() == table->m_vec18.rend())   // ORIG rbegin==rend 空判定
        return;
    // ORIG 0x085457a3..0x085457d3：point<=0 / point>m_bonusExp / first<point 一律返回
    if (!(point > 0 && point <= table->m_bonusExp
          && table->m_vec18.rbegin()->first >= point))
        return;

    m_skillLevels[type] += static_cast<unsigned short>(point);
    if (user->get_charac_level() == 0x46)
    {
        if (static_cast<short>(m_skillLevels[type])
            > table->m_vec18.rbegin()->first)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xb5, 0)(
                "CHARACTER_LEVEL_CURRENT_MAX :item_making_skill_[%d] = %d",
                static_cast<int>(type),
                static_cast<int>(static_cast<short>(m_skillLevels[type])));
            m_skillLevels[type] =
                static_cast<unsigned short>(table->m_vec18.rbegin()->first);
        }
    }
    else
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < table->m_vec18.size(); ++i)
        {
            if (user->get_charac_level() < table->m_vec18[i].second)
            {
                if (static_cast<short>(m_skillLevels[type])
                    > table->m_vec18[i].first)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xc5, 0)(
                        "NO CHARACTER_LEVEL_CURRENT_MAX : item_making_skill_[%d] = %d",
                        static_cast<int>(type),
                        static_cast<int>(static_cast<short>(m_skillLevels[type])));
                    m_skillLevels[type] =
                        static_cast<unsigned short>(table->m_vec18[i].first);
                }
            }
        }
    }
    if (static_cast<short>(m_skillLevels[type]) > 0x3e8)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xce, 0)(
            "ITEM_MAKING_SKILL_LEVEL_LIMIT : item_making_skill_[%d] = %d",
            static_cast<int>(type),
            static_cast<int>(static_cast<short>(m_skillLevels[type])));
        m_skillLevels[type] = 1000;
    }
    alter();
}

// ORIG T _ZN16CItemMakingSkill24GainItemMakingSkillPointEPK14CStackableItemP5CUser
// @0x085453e0。报告 GainItemMakingSkillPoint.md：随机 gained = rand % m_bonusExp
// + m_exp（0x28/0x24）；等级/点数上限违规时 errCode/gained 清零（0xd/0x16/0x5f）；
// UpSkillLevel 后 gained=新增量；包 0x15f 应答 + ItemMakingSkillAdd 日志。
// [b28d-solo-LinkClosureE] AE 239/239（p167 CLEAR 28/28）：等级分支/循环改
//   rbegin()->first + `cur>=first` 正极性并拆嵌套 if（P225 推广，复现 setge+je 双物化）；
//   levelLimit 比较改 `skillLevel-needLevel >= m_levelLimit`（消 %ebx 中转）；
//   日志第 4 参改回 `(short)m_skillLevels[type]`（ORIG 重读成员）；
//   删发明 helper `userHistoryLogOf`（N114），改 `user->m_historyLog.ItemMakingSkillAdd`。
#include "LinkClosureE/GainItemMakingSkillPoint.inc"
{
    const ENUM_ITEM_MAKING_TYPE type =
        static_cast<ENUM_ITEM_MAKING_TYPE>(item->GetUpSkillType());
    const int needLevel = item->GetNeedLevel();
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    const int skillLevel = GetItemMakingSkillLevel(item, user);
    int gained = static_cast<unsigned int>(m_rand->randInt())
                     % static_cast<unsigned int>(table->m_bonusExp)
                 + table->m_exp;
    int errCode = 0;
    if (skillLevel - needLevel >= table->m_levelLimit)
    {
        errCode = 0xd;
        gained = 0;
    }
    if (table->m_vec18.rbegin() == table->m_vec18.rend())   // ORIG rbegin==rend 空判定（+0xe4 跳出）
        return;
    if (user->get_charac_level() == 0x46)
    {
        if (static_cast<short>(m_skillLevels[type])
            >= table->m_vec18.rbegin()->first)
        {
            errCode = 0x16;
            gained = 0;
        }
    }
    else
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < table->m_vec18.size(); ++i)
        {
            if (user->get_charac_level() < table->m_vec18[i].second)
            {
                if (static_cast<short>(m_skillLevels[type])
                    >= static_cast<int>(table->m_vec18[i].first))
                {
                    errCode = 0x16;
                    gained = 0;
                    break;
                }
            }
        }
    }
    if (0x3e7 < static_cast<short>(m_skillLevels[type]))
    {
        errCode = 0x5f;
        gained = 0;
    }
    const short before = static_cast<short>(m_skillLevels[type]);
    UpSkillLevel(type, gained, user);
    gained = static_cast<short>(m_skillLevels[type]) - before;
    PacketGuard packet;
    packet.put_header(0, 0x15f);
    packet.put_byte(errCode);
    packet.put_byte(static_cast<int>(type));
    packet.put_short(gained);
    packet.finalize(true);
    user->Send(packet);
    user->m_historyLog.ItemMakingSkillAdd(
        type, errCode, gained, static_cast<short>(m_skillLevels[type]));
}


// ---------------------------------------------------------------------------
// Arad_GiftItem_Set::send_mail(CUser*, int, int, int, int)
// ORIG T _ZN17Arad_GiftItem_Set9send_mailEP5CUseriiii @0x081a329e sz0x177。
// 报告 docs/class_func_reports/Arad_GiftItem_Set/send_mail.md（汇编+Ghidra C）。
// find(第5,4,3参) → 未命中 logFormat+0 → AradMailInfo 组装：
//   +0x0c=第3参、+0x00=GetServerGroup(字节)、+0x1c=Data.m_str4.c_str、
//   +0x18=Data.m_str8.c_str、+0x14=Data.m_str8.length、
//   m_item{ +0x0c=Data.m_item, +0x04=1, +0x08=2, +0x0a=Inven_Item+0xb(endurance) }
// → find_item 失败 logFormat+0；否则 item.make_item(inven)（CItem vtable slot
//   +0x8）→ 读 endurance → ARAD::send_mail(info)。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureE] DIFF 95/94（p167 CLEAR 12/12）：唯一残差 = make_item 的
//   间接 call 目标寄存器（ORIG `mov (%eax),%edx … call *%edx` vs 4.4.7 恒 %eax）
//   ⇒ N7 封顶（P230：需 4.4.6 后端，路由由 Main 统一管）。真 bug 已修：
//   info.m_fieldC 实参 p3→p2（ORIG 读 0x10(%ebp)）、find_item/日志源
//   data->m_item → info.m_item.m_fieldC（ORIG 从栈槽重读）；m_item 字段写序
//   m_fieldC/m_field8/m_field4/m_fieldA/m_itemAttr 与 Inven_Item 声明位置照 ORIG。
// [D08 2026-09-10] 复核：残差恰 2 处、gap −1 —— ①0x081a33e5 间接 call 目标寄存器
//   ORIG %edx / 我方 %eax（N7，P230：4.4.6 后端可修，路由归 Main）；②0x081a3387
//   `mov -0x1c(%ebp),%eax; mov %eax,%ebx`（find_item 实参装载两段）vs 我方
//   `mov -0x1c(%ebp),%ebx`（4.4 直落 callee-saved）⇒ N18a。两处均封顶。
//   [D08 2026-09-10 补] C07 形（字段读改具名 int 局部）在本件**不适用**：写成
//   `int itemIndex = info.m_item.m_fieldC; find_item(itemIndex);` 后 4.4 折叠为
//   93 条（原 94，ORIG 95）⇒ gap −1→−2 反劣，已回滚。C07 的「升格保活」只在
//   ORIG 侧具名局部值为 4.4 未折叠的字段 load 时成立，本件字段在栈上且 4.4 直落
//   callee-saved ⇒ 不具名仍最优。
//   [Main 边界并入] 同因 A14 `CPrivateStore::BuyItem` 实测 +2 劣化 ⇒ C07 完整前提 =
//   字段读被升格 callee-saved **且该值在 call 之后仍被本函数复用**；本件该值仅作
//   find_item 的**被调方按值实参**（call 后不复用）⇒ 属禁套面。
#include "LinkClosureE/send_mail.inc"
{
    Data* data = find(giftIdx, p4, p3);
    if (data == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x15d, "[ERROR] send mail - Data None ");
        return 0;
    }

    ARAD_DATAS::AradMailInfo info;
    info.m_fieldC = p2;
    info.m_isSystemMail = (unsigned char)user->GetServerGroup();
    info.m_name = const_cast<char*>(data->m_str4.c_str());
    info.m_content = const_cast<char*>(data->m_str8.c_str());
    info.m_field14 = (int)data->m_str8.length();

    info.m_item.m_fieldC = data->m_item;
    info.m_item.m_field8 = 2;
    info.m_item.m_field4 = 1;
    info.m_item.m_fieldA = 0;
    info.m_item.m_itemAttr = 0;

    Inven_Item inven;

    CItem* item = G_CDataManager()->find_item(info.m_item.m_fieldC);
    if (item == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x175, "[ARAD_RYOSIKA] Item Null. (index:%d)",
                              info.m_item.m_fieldC);
        return 0;
    }

    item->make_item(inven);
    info.m_item.m_fieldA = inven.m_fieldb;
    return ARAD::send_mail(info);
}

// ============================================================================
// LinkFinal2 核销 —— CItemMakingSkill::SetItemMakingSkillPoint(int,int,CUser*)
// ORIG T @0x08545d24 (0x24b)，objdump 逐指令转写（GM 设置技能点）：
//   skillIdx>0xa 或等级表空 → 直接返回；
//   point∈(0,0x3e8] 且 point<表尾 first（=表最大经验门槛）才放行（否则返回）；
//   写 m_skillLevels[skillIdx]=(short)point；
//   等级钳制：charac_level==0x46（70）→ 钳到表尾 first；否则从头扫表，
//     user 等级 < vec[i].second 且当前值 > vec[i].first 时钳到 vec[i].first 并停；
//   clamp：值 >0x3e8 → 0x3e8；alter() 通知；Send_ItemMakingSkillList(user)。
// ============================================================================
// [b28d-solo-LinkClosureE] AE 178/178（p167 CLEAR 19/19）。真 bug 修正：放行判定
//   极性反置——旧 `gate=(first<point); if(!gate)return` 在 point<=0 / point>0x3e8 时
//   继续写入技能点（ORIG 该两路 return），且 first>=point 被误拒。现按 ORIG 写
//   `if (!(point>0 && point<=0x3e8 && rbegin()->first>=point)) return;`。
//   配套：删 vec 具名引用与 cur 具名（P0/P183）、clamp 改有符号 (short) 比较。
#include "LinkClosureE/SetItemMakingSkillPoint.inc"
                                               CUser* user)
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    if (skillIdx > 0xa)                                    // ORIG 0x8545d3f
        return;
    if (table->m_vec18.rbegin() == table->m_vec18.rend())  // ORIG 空表 → 返回
        return;

    // ORIG 0x8545d8f..0x8545dd6：point<=0 / point>0x3e8 / first>=point 一律返回
    if (!(point > 0 && point <= 0x3e8
          && table->m_vec18.rbegin()->first >= point))
        return;

    m_skillLevels[skillIdx] = static_cast<unsigned short>(point);  // ORIG mov %cx,0x6(,idx,2)

    if (user->get_charac_level() == 0x46)                  // ORIG 0x8545df7
    {
        if (static_cast<short>(m_skillLevels[skillIdx])
            > table->m_vec18.rbegin()->first)              // ORIG 0x8545e34 setg
            m_skillLevels[skillIdx] =
                static_cast<unsigned short>(table->m_vec18.rbegin()->first);
    }
    else
    {
        for (unsigned int i = 0; i < table->m_vec18.size(); ++i)  // ORIG 0x8545e84 循环
        {
            if (user->get_charac_level() < table->m_vec18[i].second)
            {
                if (static_cast<short>(m_skillLevels[skillIdx])
                    > table->m_vec18[i].first)             // ORIG 0x8545ed9 setg
                {
                    m_skillLevels[skillIdx] =
                        static_cast<unsigned short>(table->m_vec18[i].first);
                    break;                                 // ORIG 0x8545f04
                }
            }
        }
    }

    if (static_cast<short>(m_skillLevels[skillIdx]) > 0x3e8)   // ORIG 0x8545f31 clamp
        m_skillLevels[skillIdx] = 0x3e8;

    alter();                                               // ORIG 0x8545f4a
    Send_ItemMakingSkillList(user);                        // ORIG 0x8545f5c
}
