// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertJobLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// getItemSellPrice 定义于 Store.cpp（ORIG T 0x0898c86e）；仓库暂无共享头声明。
#include "ExpertJobLink4/getItemSellPrice.inc"
                     bool isRandomOption);

#include "CExpertJob.h"

#include <math.h>

#include "CDataManager.h"      // G_CDataManager / GetExpertJobScript(int) /
                               // GetExpertJobEtcScript
#include "CExpertJobList.h"    // STExpertJobEtcScript
#include "CItem.h"             // GetSellPrice/get_rarity/get_index/get_grade/
                               // IsRandomOption
#include "CItemAmplifier.h"    // stAmplifyOption_t::isIdentified
#include "CPrivateStoreMgr.h"  // private_store::{GetInstancePrivateStoreMgr,
                               // CPrivateStoreMgr::IsBusyPrivateStore}
#include "CUser.h"
#include "DisJoint.h"          // DisJointResult
#include "CStackableItem.h"       // m_int324(+0x324) 拆解券等级门槛
#include "DisPatcherCore.h"       // DisPatcher_DisJointItem（静态成员定义）
#include "GameRand.h"          // get_rand_int
#include "GameWorld.h"         // G_GameWorld 及各查询/移动接口
#include "GameTypes.h"         // cMyTrace
#include "GlobalData.h"        // GlobalData::s_villageObjectMgr
#include "Inven_Item.h"        // Inven_Item::m_amp
#include "PacketGuard.h"

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN10expert_job10CAlchemistC2Ev`（P327）。全限定定义可写在命名空间块
// 之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ---- CAlchemist 生命周期（ctor 报告 expert_job__CAlchemist/CAlchemist.md；
//      dtor D1/D0 0x085d11d0/0x085d1200）----
#include "ExpertJobLink4/CAlchemist.inc"
{
    // ORIG：基类构造后仅置 vptr（由编译器生成），无额外成员。
}


// ---- stItemSelectRate::stItemSelectRate（ORIG W 0x085d3bd6：5×int 清零）----
#include "ExpertJobLink4/stItemSelectRate.inc"
{
    m_itemIndex = 0;
    m_rate      = 0;
    m_float8    = 0;
    m_rateMin   = 0;
    m_rateMax   = 0;
}

// ---- DisjointMachineGrade 三参 ctor（ORIG W 0x085d3b9e）----
// 参数装载实证：+0=m_grade=第2参、+1=m_field1=第1参、+2=m_field2=第3参。
#include "ExpertJobLink4/DisjointMachineGrade.inc"
                                           unsigned char c)
{
    m_grade  = b;
    m_field1 = a;
    m_field2 = c;
}

// ---- DisjointMachineGrade 默认 ctor（ORIG W 0x0896dbcc：三字节清零）----
#include "ExpertJobLink4/DisjointMachineGrade_1.inc"
{
    m_grade  = 0;
    m_field1 = 0;
    m_field2 = 0;
}

namespace expert_job
{

// ---------------------------------------------------------------------------
// 辅助：ORIG 浮点常量（.rodata 实测）
// ORIG 所有浮点→整型转换均显式置 FPU 控制字 RC=最近偶数（fldcw 0xc<<8，
// 0x85d246f flds+fnstcw+fistpl 实证），即 x87 就近取整（round-to-nearest-even，
// x.5 边界取偶）；以 (int)value 直接转换由编译器生成同一 fistpl 序列。
// 注意不可用 +0.5f 截断：2.5 → RC=even 得 2，+0.5 截断得 3。
// ---------------------------------------------------------------------------
// [b28d-solo-ExpertJobLink4 P49]：static inline helper 在 -O0 下仍发真实 call
// 符号（p167 EXTRA `_ZN10expert_job12_GLOBAL__N_113round_nearestEf` ×6，
// ORIG 全内联展开 fldcw/fistpl）⇒ 改宏形。
#define round_nearest(v) ((int)(v))


// ---- expert_job::get_additional_item（报告 expert_job/get_additional_item.md，
//      ORIG T 0x0896d6ab）：顺序遍历表项写入 out；命中 [min,max] 区间后若剩余
//      随机值 >= 累加概率则返回，否则扣减继续。 ----
#include "ExpertJobLink4/get_additional_item.inc"
                         stItemSelectRate& out, int grade)
{
    // [b28d-solo-ExpertJobLink4] P13/P28：ORIG begin→it(-0xc)、end→e(-0x10)
    // 两具名迭代器（循环初始化区一次求值）；单语句 `it != table.end()` 让 4.4
    // 把 end 临时放深槽致 it/e 槽位互换。
    for (std::vector<stItemSelectRate>::const_iterator it = table.begin(),
             e = table.end();
         it != e; ++it)
    {
        out = *it;
        // [b28d-solo-ExpertJobLink4] P223 多岛细则：ORIG 两个失败出口各落独立
        // nop 中继岛（0x896d733 `nop;jmp` / 0x896d736 `nop`）+ body 尾 `jmp`；
        // 唯一复现形 = 两条独立失败 goto + 标签串（标签序=守卫序）。
        if (out.m_rateMin > grade)
            goto skip_min;
        if (out.m_rateMax < grade)
            goto skip_max;
            // ORIG 0x896d723 `cmp randValue,%eax(m_rate); jge <ret>`：m_rate >=
            // randValue 才返回（原 `randValue >= m_rate` 等号方向写反 = 真语义 bug）。
            if (out.m_rate >= randValue)
                return;
            randValue -= out.m_rate;
    skip_min:;
    skip_max:;
    }
}

// ---- CCharacExpertJob 分解机 getter（ORIG W 0x85d3e66 / 0x85d3e8e）----
#include "ExpertJobLink4/get_disjoint_machine_endurance.inc"
{
    return m_endurance;   // +0x08
}

#include "ExpertJobLink4/get_disjoint_machine_grade.inc"
{
    return m_enchantCount;   // +0x0c【字段名沿用既有推断名；getter 实证即分解机等级】
}

// ORIG W 0x085d3e72：alter() 后写 +0x08。
#include "ExpertJobLink4/set_disjoint_machine_endurance.inc"
{
    alter();
    m_endurance = value;
}

#include "ExpertJobLink4/dtor_CAlchemist.inc"
{
}

// ---- CAlchemist::OnTeleportCharacter（报告 OnTeleportCharacter.md，
//      ORIG T 0x085d1424 sz 0x26f）----
// [L2Free4to15.S3 复核] 唯一残差 = 符号位测试 lowering：ORIG 0x085d1453
// `mov 0x10(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je` vs 4.4
// `cmpl $0x0,0x10(%ebp); jns`（ndiff 6、gap 2）。本轮再试 `(unsigned)area>>31`
// 一形仍被 4.4 前端折成 cmpl/jns（连同旧注释四形共 5 形）⇒ N97 族封顶。
// GI 假缺口（ORIG 该符号 493 实例，工具按名错配取首个；我方 16 条纯 ioinit 正确）。
#include "ExpertJobLink4/OnTeleportCharacter.inc"
                                     int y, bool useGate)
{
    if (useGate)
    {
        area = G_GameWorld()->get_gate_area(vill);
        // [b28d-solo-ExpertJobLink4 N86b/N97]：ORIG 0x085d1453 `mov area,%eax;
        // shr $0x1f,%eax; test %al,%al; je`（4.1 符号位 lowering）；4.4 对
        // 符号位测试四形（`area<0` / `(unsigned)area>>31` / `area&0x80000000`
        // / `(unsigned)area/0x80000000u`）恒折 `cmpl $0x0; jns`（-2 条）⇒ 封顶。
        if (area < 0)
            return 0;

        if (!G_GameWorld()->get_area_center_point(vill, area, x, y))
        {
            // P139：ORIG 0x085d14a7 传 _ZZ…E19__PRETTY_FUNCTION__ 局部静态。
            // P221：全表达式临时链翻 sibling 块槽方向（ORIG trace 槽 -0x18 浅于
            // PacketGuard -0x24；具名 trace 在 4.4 下占 -0x24）。
            cMyTrace(__PRETTY_FUNCTION__, 0x8b, 5)(
                "get_area_center_point() call failed");
            return 0;
        }
    }

    // ORIG 0x085d14d7：直接 if 判断（无 needMove 中间变量）。
    if ((char)user->getCurCharacVill() != vill ||
        user->get_area(false) != area)
    {
        if (!G_GameWorld()->move_area(user, vill, area, x, y, 0, false, 0, 0, 0))
            return 0;
    }
    else
    {
        user->set_position((unsigned short)x, (unsigned short)y, '\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(x);
    packet.put_short(y);
    packet.put_byte(0);
    packet.finalize(true);

    if (vill != 0 || area != 0)
        G_GameWorld()->send_to_area(vill, area, packet);

    return 1;
}

// ---- CDisjointer::GetScript / GetEtcScript（ORIG T 0x085d1e0a / 0x085d1e3a：
//      函数级静态缓存单例；ORIG 静态变量 b 0x09472f98 / 0x09472f94）----
#include "ExpertJobLink4/GetScript.inc"
{
    static STExpertJobScript* script = 0;   // ORIG GetScript()::script
    if (script == 0)
        script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(3);
    return script;
}

#include "ExpertJobLink4/GetEtcScript.inc"
{
    static STExpertJobEtcScript* script = 0;   // ORIG GetEtcScript()::script
    if (script == 0)
        script = (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    return script;
}

// ---- CDisjointMachine 六参 ctor（objdump 0x085d1ae8 忠实转写）：
//      IObject 基类 → vptr → Rectangle 默认构造 → name 默认构造 → 逐字段赋值 →
//      碰撞盒 {x-0x50, y-0x32, 0xa0, 0x64}。 ----
#include "ExpertJobLink4/CDisjointMachine.inc"
                                   char grade, int x, int y)
{
    m_user              = user;                 // +0x14
    m_name              = name;                 // +0x18
    m_field1c           = level;                // +0x1c
    m_field20           = grade;                // +0x20（char）
    m_field24           = x;                    // +0x24（ORIG 全宽 mov，无 ushort 截断）
    m_field28           = y;                    // +0x28

    m_rect.m_pos.m_x    = m_field24 - 0x50;     // Rectangle +0x00（ORIG 成员回读）
    m_rect.m_width      = 0xa0;                 // +0x08
    m_rect.m_pos.m_y    = m_field28 - 0x32;     // +0x04（同上）
    m_rect.m_height     = 0x64;                 // +0x0c
}

// ---- CDisjointMachine::operator=（ORIG W 0x085d3ed6：基类 IObject::operator=
//      空操作 + 拷 +0x04..+0x14 与 string/+0x1c/+0x20/+0x24/+0x28，不拷 vptr）----
CDisjointMachine& CDisjointMachine::operator=(CDisjointMachine const& other)
{
    village_object::IObject::operator=(other);   // ORIG W 0x85ccadc 同款空操作
    // b28：结构整体赋值（探针 /tmp/l3/p6.cpp 实证 4.4 出 ORIG 的 dest/src 双缓存
    // 4-dword 连发形：mov 0x4(%edx),%ecx…末条 %edx 自复用，逐字段形则 16 条异形）。
    m_rect = other.m_rect;
    m_user           = other.m_user;
    m_name           = other.m_name;
    m_field1c        = other.m_field1c;
    m_field20        = other.m_field20;
    m_field24        = other.m_field24;
    m_field28        = other.m_field28;
    return *this;
}

// ---- CDisjointer::OnCreateDisjointStore（报告 OnCreateDisjointStore.md，
//      ORIG T 0x085d2d08 sz 0x391）----
// [L2Free4to15.S3 复核] p167 CLEAR（差集仅 C1/C2·D1/D2 同址别名，P93）。
// 残差 = 双对象槽方向相反（ORIG CDisjointMachine 临时 -0x3c / PacketGuard
// -0x48；我方 -0x48 / -0x1c = N93）+ EH cleanup 块序（ORIG 侧
// _Unwind_Resume 多 1）。交换声明序会改构造序/指令序 ⇒ N93 封顶。
#include "ExpertJobLink4/OnCreateDisjointStore.inc"
                                       int x, int y)
{
    if (level < 0 || GetScript()->m_limitDisjointGold < level)   // +0x28c
        return 10;

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0xbe;

    if ((char)user->getCurCharacVill() == 7)
        return 0xbe;

    // [b28d-solo-ExpertJobLink4 P0/P23]：ORIG 0x085d2d80/0x085d2dd7 两次独立
    // 调用 get_area→%ebx、getCurCharacVill→%esi（无 area/vill 具名槽）。
    if (!G_GameWorld()->is_available_point((int)user->getCurCharacVill(),
                                           user->get_area(false), x, y))
        return 0xbe;

    int restrictErr =
        G_GameWorld()->check_restrictive_commercial_transaction_zone(
            (int)user->getCurCharacVill(), user->get_area(false), x, y);
    if (restrictErr != 0)
        return restrictErr;

    // 耐久检查（两次独立取扩展数据，与 ORIG 调用序一致）。
    if (((expert_job::CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))->get_disjoint_machine_endurance() <= 0)
        return 0x16;

    // 局部构造后赋给 m_machine（ORIG 经栈上临时 + operator= + 临时析构）。
    // ORIG 0x085d2e77 `call get_disjoint_machine_grade; movsbl %al,%eax` 无
    // machineGrade 具名槽（内联进 ctor 第4实参）。
    CDisjointMachine local(
        user, name, level,
        (char)((expert_job::CCharacExpertJob*)user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0))->get_disjoint_machine_grade(),
        x, y);
    m_machine = local;

    user->set_position((unsigned short)x, (unsigned short)y, '\0');

    // [b28d-solo-ExpertJobLink4 P212/N93]：ORIG Zone 槽 -0xe + &m_machine 提前入
    // %esi（0x085d2f10）⇒ 匿名临时实参形（具名 zone 在 4.4 下占更深槽）。
    if (!GlobalData::s_villageObjectMgr->register_object(
            village_object::Zone(user->getCurCharacVill(),
                                 (unsigned char)user->get_area(false)),
            &m_machine))
        return 0x14;

    PacketGuard packet;
    packet.put_header(0, 0xce);
    m_machine.put_disjoint_machine_info(packet);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet);

    packet.clear();
    packet.put_header(1, 0xee);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);

    return 0;
}

// ---- CDisjointer::get_disjoint_result_（报告 get_disjoint_result_.md，
//      ORIG T 0x085d234a sz 0x6cc）。 ----
// [L2Free4to15.S3 复核] p167 CLEAR（仅 C1/C2 别名）。残差 = x87 往返
// （ORIG 4×`fstps X; flds X` 具名 float 局部，我方 0；P252 试形旧记录）
// + 我方 5 处多余槽往返（-0x28/-0x20/-0x18/-0x14/-0x1c）
// + add-vs-disp 2 处（N107）⇒ N85②/N107 封顶。
#include "ExpertJobLink4/get_disjoint_result_.inc"
                                      int level, DisJointResult& result,
                                      bool selfService, CUser* user)
{
    int itemIndex  = item->get_index();     // local_54（ORIG 计算后未再使用）
    int sellPrice  = item->GetSellPrice();  // local_50（×1.1 后作基价）
    int rarity     = item->get_rarity();    // local_4c
    int grade      = item->get_grade();     // local_48
    (void)itemIndex;

    bool unidentified = false;
    // P198①/P194：ORIG 0x85d2395 `movb $0,-0x41` 预清 + 条件 mov$1/mov$0 物化。
    unidentified =
        (invenItem != 0 && !invenItem->m_amp.isIdentified());

    // map<DisjointMachineGrade,stDisjointResult> 键：(a=rarity, b=level,
    // c=unidentified) → {m_grade=level, m_field1=rarity, m_field2=unidentified}。
    // ORIG find/end 各独立 `call GetScript`（无 script 具名槽）。
    std::map<DisjointMachineGrade, stDisjointResult>::iterator it =
        GetScript()->m_disjointResultMap.find(
            DisjointMachineGrade((unsigned char)rarity, (unsigned char)level,
                                 unidentified));
    if (it == GetScript()->m_disjointResultMap.end())
        return unidentified ? 0xd3 : 0x11;

    stDisjointResult& res = it->second;

    sellPrice = round_nearest(1.1f * sellPrice);          // ROUND(_DAT_08cc080c × price)
    // [b28d-solo-ExpertJobLink4 P252]：ORIG `mov 0x4(%eax),%eax; mov %eax,-0x3c`
    // 具名 float 局部（后续 `fmuls -0x3c` 槽往返），非直读对象字段。
    float field4 = res.m_field4;
    result.m_f[5] = res.m_field0;                         // +0x14 cube 索引

    int money = getItemSellPrice(sellPrice, 0, 0, item->IsRandomOption());
    money = (money * 1000) / GetScript()->m_baseConst;    // +0x1d4 [base const]
    result.m_f[0] = round_nearest(money * field4 / 1000.0f);   // _DAT_08cc0810
    if (result.m_f[0] < 1)
        result.m_f[0] = 1;
    result.m_f[6] = 0;                                    // +0x18 附加物 idx
    result.m_f[1] = 0;                                    // +0x04 附加物数量

    // 大奖基础率：res+0x20 ×100（导入侧第 6 列，当前导入 TU 未持久化——见
    // STExpertJobScript.h 注释）。
    int bigwinRate = res.m_field20 * 100;

    if (selfService)
    {
        unsigned char key = (unsigned char)rarity + 1;
        unsigned int rate = 0;
        std::map<unsigned char, unsigned char>::iterator etcIt =
            GetEtcScript()->m_disjointerBigWinRate.find(key);
        if (etcIt != GetEtcScript()->m_disjointerBigWinRate.end())
            rate = etcIt->second;
        bigwinRate += (int)(rate * 100);

        // ORIG 无 active 具名槽（条件直测 `mov $0,%eax; test %al,%al; je`）。
        if (user != 0 && user->get_state() >= 3 && user->getCurCharacR() != 0)
        {
            const CInventory* inv = user->getCurCharacInvenR();
            int jobLevel =
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            float bonus = 100.0f * inv->GetExpertJobSelfDisjointBigWinRate(
                                     (ENUM_EXPERT_JOB_TYPE)3, jobLevel);
            bigwinRate = round_nearest(bigwinRate + bonus);
        }
    }

    if (get_rand_int(10000) < bigwinRate)
    {
        stItemSelectRate select;
        select.m_itemIndex = -1;
        get_additional_item(res.m_bigwinItems, get_rand_int(10000), select, grade);
        if (select.m_itemIndex != -1)
        {
            result.m_f[6] = select.m_itemIndex;
            result.m_f[1] = round_nearest((float)grade / select.m_float8);
            if (result.m_f[1] < 1)
                result.m_f[1] = 1;
        }
    }
    else
    {
        stItemSelectRate select;
        select.m_itemIndex = -1;
        get_additional_item(res.m_additionalItems, get_rand_int(10000), select,
                            grade);
        if (select.m_itemIndex != -1)
        {
            result.m_f[6] = select.m_itemIndex;
            result.m_f[1] = round_nearest((float)grade / select.m_float8);
            if (result.m_f[1] < 1)
                result.m_f[1] = 1;
        }
    }

    if (selfService)
    {
        // script+0x30 [disjoint self service]：{概率, 奖励A, 奖励B}
        // （stSelfDisjoinMaterialInfo m_field0/m_field4/m_field8）。
        int* svc = &GetScript()->m_selfDisjoin.m_field0;
        // [C07 · Main 范式②(F2) 落地] ORIG 0x85d2f? 段 `mov -0x1c(%ebp),%eax;
        //   mov (%eax),%eax; mov %eax,-0x18(%ebp); cmpl $0x1,-0x4c(%ebp); jle;
        //   call get_rand_int(100); cmp -0x18(%ebp),%eax; jge`（具名 int 局部 +
        //   内存直比）。内联 `svc[0] > get_rand_int(100)` 使 4.4 把 svc[0] 升格
        //   %ebx 跨 call 并折成 register-cmp（多 1 条 mov 且判据寄存器化）。
        int selfRate = svc[0];
        if (rarity >= 2 && selfRate > get_rand_int(100))
        {
            result.m_f[2] = svc[2];   // +0x08
            result.m_f[7] = svc[1];   // +0x1c
        }

        if (user != 0 && user->get_state() >= 3 && user->getCurCharacR() != 0)
        {
            const CInventory* inv = user->getCurCharacInvenR();
            int jobLevel =
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            // [C07 · P252 试形已回退] 加具名 float 中间量（`float rawRate = f(...);
            //   float variation = rawRate / 100.0f;`）实测 gap 14→12 但 ndiff
            //   106→344（槽整体平移 4、difflib 对齐坍缩）⇒ 净负，恢复直写形。
            float variation =
                inv->GetExpertJobSelfDisjointResultVariation(
                    (ENUM_EXPERT_JOB_TYPE)3, jobLevel) / 100.0f;   // _DAT_08cc0814

            // [C07 · P214③] ORIG 在循环前发 5 条 `movl $0x0,-0xa8..-0x98(%ebp)`
            //   （连续寻址）⇒ 原码是**显式零初始化数组**（我方裸 `int tmp[5];` 无
            //   清零，缺 5 条）。
            int tmp[5] = { 0, 0, 0, 0, 0 };
            for (int i = 0; i <= 4; ++i)
            {
                tmp[i] = round_nearest(result.m_f[i] * variation);
                result.m_f[i] += tmp[i];
            }
        }
    }

    return 0;
}

// getItemSellPrice 定义于 Store.cpp（ORIG T 0x0898c86e）；仓库暂无共享头声明。
#include "ExpertJobLink4/getItemSellPrice_1.inc"
                     bool isRandomOption);

// ---- CExpertJobMgr::AcquireExpertJob（ORIG T 0x0849e1dc：纯转发工厂）----
#include "ExpertJobLink4/AcquireExpertJob.inc"
{
    return m_factory->AcquireExpertJob(type);
}

// ---- CExpertJobFactory::AcquireExpertJob（ORIG W 0x0849f5d2）：
//      type==1 → Enchanter 池（this+0x00）；type==3 → Disjointer 池（this+0x30）；
//      取到后先虚调用 Reset()（vtable+0xc 槽）再非虚 SetType(type)。 ----
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobLink4/AcquireExpertJob_1.inc"
{
    CExpertJob* acquired = 0;
    switch (type)
    {
    case (ENUM_EXPERT_JOB_TYPE)1:
        acquired = m_enchanterPool.Acquire();
        break;
    case (ENUM_EXPERT_JOB_TYPE)3:
        acquired = m_disjointerPool.Acquire();
        break;
    }

    if (acquired != 0)
    {
        acquired->Reset();
        acquired->SetType(type);
    }
    return acquired;
}

// ---- CExpertJobMgr::FreeExpertJob（ORIG T 0x0849e1f8：转发工厂）----
#include "ExpertJobLink4/FreeExpertJob.inc"
{
    m_factory->FreeExpertJob(job);
}

// ---- CExpertJobFactory::FreeExpertJob（ORIG W 0x0849f63e）：
//      job 非空时按 GetType() 分派到对应池。 ----
#include "ExpertJobLink4/FreeExpertJob_1.inc"
{
    // b28：正极性包 if（P214①族/探针 /tmp/l3/p4.cpp fs2）——负极性 return
    // 早退使 4.4 尾块前多插 jmp+nop 中继；正包形末 case fall-through leave。
    if (job != 0)
    {
        switch (job->GetType())
        {
        case 1:
            m_enchanterPool.Free((CEnchanter*)job);
            break;
        case 3:
            m_disjointerPool.Free((CDisjointer*)job);
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// CDisjointMachine getter（ORIG W 0x85d3e06 / 0x082347e0 / 0x85d3e12）
// P210：以下 CDisjointer 包装（grade/isMine）必须定义于 get_user/get_grade
// 之前，否则 4.4 继承 callee outgoing 帧塌缩（sub $0x4 vs ORIG $0x18）。
// ---------------------------------------------------------------------------
// get_disjoint_machine_grade（ORIG T 0x85d1f0a：movsbl m_machine.get_grade()）
#include "ExpertJobLink4/get_disjoint_machine_grade_1.inc"
{
    return (int)m_machine.get_grade();
}

// isMine（ORIG W 0x08234854）：m_machine.get_user() == user
#include "ExpertJobLink4/isMine.inc"
{
    return m_machine.get_user() == user;
}

// get_disjoint_cost（ORIG W 0x085d3e50）
#include "ExpertJobLink4/get_disjoint_cost.inc"
{
    return m_machine.get_cost();
}

#include "ExpertJobLink4/get_cost.inc"
#include "ExpertJobLink4/get_user.inc"
#include "ExpertJobLink4/get_grade.inc"

// ---------------------------------------------------------------------------
// CDisjointer —— LinkMisc5 收口（vtable 重排后按虚槽落地）
// ---------------------------------------------------------------------------

// send_error（报告 send_error.md，ORIG T 0x085d1e62）：
// header(0,0xcb)/byte(0)/byte(err)/finalize/Send。
#include "ExpertJobLink4/send_error.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xcb);
    packet.put_byte(0);
    packet.put_byte(err);
    packet.finalize(true);
    user->Send(packet);
}

// check_disjoint_endurance（报告 check_disjoint_endurance.md，ORIG T 0x085d1f24）
#include "ExpertJobLink4/check_disjoint_endurance.inc"
{
    if (((CCharacExpertJob*)m_machine.get_user()->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance() <= 0)
        return false;
    else
        return true;
}

// IsAvailableJob（ORIG W 0x849f596：恒返 1）
#include "ExpertJobLink4/IsAvailableJob.inc"

// Reset（报告 Reset.md，ORIG T 0x085d2cf2）
#include "ExpertJobLink4/Reset.inc"

// OnStartMission（报告 OnStartMission.md，ORIG T 0x085d2a16 sz 0x15e）。
// targetUser=分解机主人；requestUser=发起请求者。
#include "ExpertJobLink4/OnStartMission.inc"
{
    // b28：清 register asm 绑定（N96/P0 全域禁），ORIG 物化岛形为 4.1 CFG 产物，
    // 先按最短语义（&& 短路 + 直接 throw）复测。
    if (targetUser != requestUser &&
        requestUser->getCurCharacMoney() < m_machine.get_cost())
        throw static_cast<ENUM_ERROR>(0x15);

    if (!targetUser->CheckMoney(m_machine.get_cost()))   // ORIG CheckMoney 返 1 为足额
    {
        send_error(targetUser, 10);
        // ORIG vptr+0x4 虚槽 = slot1 OnCloseMission(机主, 0)
        OnCloseMission(m_machine.get_user(), ENUM_CLOSE_MISSION_TYPE_0);
        throw static_cast<ENUM_ERROR>(10);
    }

    if (!check_disjoint_endurance())
    {
        send_error(targetUser, 0xbd);
        throw static_cast<ENUM_ERROR>(0xbd);
    }
}

// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x085d2b74 sz 0x17e）：
// 广播 0xd9 下线包 → 注销村庄对象 → 清专家职业 → 归还池。
#include "ExpertJobLink4/OnCloseMission.inc"
{
    (void)type;
    PacketGuard packet;
    packet.put_header(0, 0xd9);
    packet.put_short(static_cast<int>(user->get_unique_id() & 0xffff));
    packet.finalize(true);

    // b28d-61 [P200 短整直传实参] + [b28d-solo-ExpertJobLink4 P23/P134]：
    // ORIG 0x085d2be0 序 = get_area→%ebx（无 area 具名槽）→ getCurCharacVill
    // →movsbl %al,%esi→G_GameWorld()；具名 area 局部会把值落槽并改装载序。
    G_GameWorld()->send_to_area((int)user->getCurCharacVill(),
                                user->get_area(false), packet);

    // [b28d-solo-ExpertJobLink4 P212/N93]：ORIG Zone 槽 -0xa（浅于 PacketGuard
    // -0x18）+ &m_machine 提前入 %esi；具名 Zone 局部在 4.4 下占更深槽（-0x16）。
    // 改匿名临时实参（Zone 无默认 ctor，不可头部声明后赋值）。
    if (!GlobalData::s_villageObjectMgr->unregister_object(
            village_object::Zone(user->getCurCharacVill(),
                                 (unsigned char)user->get_area(false)),
            &m_machine))
    {
        LogManager::logFormat(
            1, "ProfessionDisjointer.cpp",
            // P139：ORIG 0x085d2c7f 传 _ZZ…E19__PRETTY_FUNCTION__ 局部静态，
            // 手写同内容字符串只产匿名 .rodata（符号差集残留）。
            __PRETTY_FUNCTION__,
            0x15b, "unregister_object error");
    }

    user->SetCurCharacExpertJob(0);
    GlobalData::s_ExpertJobMgr->FreeExpertJob(this);
}

// complete_disjoint（报告 complete_disjoint.md，ORIG T 0x085d31a0 sz 0x42d）。
// 耐久/经验结算 + 请求者付费、机主收款 + 机主耐久不足时经 slot1 关闭任务。
// [L2Free4to15.S3 复核] p167 CLEAR（仅 C1/C2·D1/D2 别名）。残差 = x87 比较
// 两向（ORIG `fucompp/fnstsw/test/sete` vs 4.4 `fucomip/fstp/seta`，N85）
// + 槽差 8（-0x18/-0x1c ↔ -0x20/-0x24）⇒ N85 封顶。
#include "ExpertJobLink4/complete_disjoint.inc"
{
    // 耐久消耗 [m_enduranceReduce1(+0x26c), m_enduranceReduce2(+0x270)] 随机
    // [b28d-solo-ExpertJobLink4 P0/P183]：ORIG 0x85d31a8 起 GetScript 共 3 次
    // 内联调用（0x270/0x26c/0x26c），无 script 具名槽。
    int enduranceUse =
        get_rand_int(GetScript()->m_enduranceReduce2 -
                         GetScript()->m_enduranceReduce1 + 1) +
        GetScript()->m_enduranceReduce1;

    // ORIG 扩展数据访问全程内联（无 ownerExpand 具名槽）。
    int curEndurance =
        ((CCharacExpertJob*)((CUser*)m_machine.get_user())
             ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance();
    if (curEndurance < enduranceUse)
        enduranceUse = curEndurance;

    ((CCharacExpertJob*)((CUser*)m_machine.get_user())
         ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
        ->set_disjoint_machine_endurance(curEndurance - enduranceUse);

    // 经验收益 [m_gainExp1(+0x284), m_gainExp2(+0x288)] 随机 + 概率加成项
    int expGain =
        get_rand_int(GetScript()->m_gainExp2 - GetScript()->m_gainExp1 + 1) +
        GetScript()->m_gainExp1;

    // ORIG 0x85d328d 用 getCurCharacInvenR()（const 版）取背包。
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp additionalExp;
    int jobExp = requestUser->GetCurCharacExpertJobExp();
    int jobLevel = requestUser->GetCurExpertJobLevel(jobExp);
    int jobType = requestUser->GetCurCharacExpertJobType();
    requestUser->getCurCharacInvenR()->GetExpertJobAddtionalExp(
        (ENUM_EXPERT_JOB_TYPE)jobType, jobLevel, additionalExp);
    if ((float)get_rand_int(100) < additionalExp.m_field0)
        expGain = additionalExp.m_field4 + expGain;

    if (expGain > 0)
    {
        CExpertJob::IncreaseExpertJobExp(
            m_machine.get_user(),
            m_machine.get_user()->GetCurCharacExpertJobExp() + expGain);
    }

    if (m_machine.get_user() == requestUser)
    {
        // 自助拆解：只回填金额与剩余耐久
        packet.put_int(requestUser->getCurCharacMoney());
        packet.put_int(
            ((CCharacExpertJob*)((CUser*)m_machine.get_user())
                 ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
                ->get_disjoint_machine_endurance());
    }
    else
    {
        // 他人拆解：请求者扣费 → 回包 → 机主收款 → 机主侧 0xcb 通知
        int cost = get_disjoint_cost();
        requestUser->getCurCharacInvenW()->use_money(cost, (eMoneySubReason)0x1d,
                                                     true);
        packet.put_int(requestUser->getCurCharacMoney());
        packet.put_int(
            ((CCharacExpertJob*)((CUser*)m_machine.get_user())
                 ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
                ->get_disjoint_machine_endurance());

        ((CUser*)m_machine.get_user())
            ->getCurCharacInvenW()
            ->gain_money(cost, (eMoneyAddReason)0x16, true, 0);

        PacketGuard notify;
        notify.put_header(0, 0xcb);
        notify.put_byte(1);
        notify.put_int(((CUser*)m_machine.get_user())->getCurCharacMoney());
        notify.put_int(
            ((CCharacExpertJob*)((CUser*)m_machine.get_user())
                 ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
                ->get_disjoint_machine_endurance());
        notify.finalize(true);
        ((CUser*)m_machine.get_user())->Send(notify);
    }

    // 机主耐久耗尽 → 经虚槽 slot1 OnCloseMission(机主, 0) 关闭
    if (((CCharacExpertJob*)((CUser*)m_machine.get_user())
             ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance() < 1)
        OnCloseMission(m_machine.get_user(), ENUM_CLOSE_MISSION_TYPE_0);

    return 1;
}

// get_disjoint_result（ORIG W 0x082347ec）：以机器等级/归属补全六参版本。
#include "ExpertJobLink4/get_disjoint_result.inc"
                                     DisJointResult& result, CUser* user)
{
    bool mine = isMine(user);
    // [b28d-solo-ExpertJobLink4] P23/P134：ORIG 实参右→左求值——最后实参
    // m_machine.get_user() 先算入 %ebx，mine 读槽入 %esi，grade 表达式随后入
    // %eax（无 grade 具名槽）；具名 grade 局部会把 get_user 推迟到实参点。
    return get_disjoint_result_(item, inventItem,
                                get_disjoint_machine_grade(), result, mine,
                                m_machine.get_user());
}

// ---------------------------------------------------------------------------

// IsAvailableJob（ORIG W 0x849f30c：恒返 1）
#include "ExpertJobLink4/IsAvailableJob_1.inc"


// Reset（报告 Reset.md，ORIG T 0x0849c71e sz 0x97）：清材料×2 →
// 清 +0x28..+0x38 五个 dword → SetType(1) → 清 +0x3c/+0x44/+0x40。
#include "ExpertJobLink4/Reset_1.inc"
{
    for (int i = 0; i < 2; ++i)
        m_materials[i].Reset();
    m_user = 0;
    m_peer = 0;
    m_uid = 0;
    m_peerUid = 0;
    m_field38 = 0;
    SetType((ENUM_EXPERT_JOB_TYPE)1);
    m_field3c = 0;
    m_peerRequestType = ENUM_PEER_REQUEST_TYPE_0;   // ORIG 写序：+0x44 先于 +0x40
    m_field40 = 0;
}

// OnStartMission（报告 OnStartMission.md，ORIG T 0x0849c7b6）：
// 双方登记到附魔师槽位并各自 SetCurCharacExpertJob(this)。
#include "ExpertJobLink4/OnStartMission_1.inc"
{
    m_user = targetUser;
    m_uid = (unsigned int)(targetUser->get_unique_id() & 0xffff);
    // P48：ORIG 在写 m_user(+0x28) 后 `mov 0x28(%eax),%eax` 回读成员再调
    // SetCurCharacExpertJob（0x0849c7df/0x0849c811），不复用形参。改经成员。
    m_user->SetCurCharacExpertJob(this);
    m_peer = requestUser;
    m_peerUid = (unsigned int)(requestUser->get_unique_id() & 0xffff);
    m_peer->SetCurCharacExpertJob(this);
}

// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x0849c822 sz 0x14c）：
// 按 uid 匹配离场方；type∈{1,2} 时向另一方发空 0x9e 包；最后归还池。
#include "ExpertJobLink4/OnCloseMission_1.inc"
{
    CUser* peer = 0;
    if ((unsigned int)(user->get_unique_id() & 0xffff) == m_uid)
    {
        m_user->SetCurCharacExpertJob(0);
        m_peer->SetCurCharacExpertJob(0);
        peer = m_peer;
    }
    else
    {
        // b28d-61 [P222 正向包裹翻极性，ORIG 0x0849c87x `cmp %eax,%edx; sete
        // %al; test %al,%al; je +0x187`]：旧「否定早返」形产 setne/jne 反极性。
        if ((unsigned int)(user->get_unique_id() & 0xffff) == m_peerUid)
        {
            m_user->SetCurCharacExpertJob(0);
            m_peer->SetCurCharacExpertJob(0);
            peer = m_user;
        }
        else
            return;
    }

    if (type == ENUM_CLOSE_MISSION_TYPE_1 || type == ENUM_CLOSE_MISSION_TYPE_2)
    {
        // b28d-61 [P214① 正条件内联，ORIG 0x0849c9xx 无具名 bool 槽]
        // 旧 `bool sendPeer = A && B && C; if (sendPeer)` 多 `mov %al,-0x9;
        // cmpb $0,-0xa` 槽物化。
        if (peer != 0 && peer->getCurCharacR() != 0 && m_user != m_peer)
        {
            PacketGuard packet;
            packet.put_header(0, 0x9e);
            packet.finalize(true);
            peer->Send(packet);
        }
    }
    GlobalData::s_ExpertJobMgr->FreeExpertJob(this);
}

// ---------------------------------------------------------------------------
// GetPredictResult（报告 GetPredictResult.md，ORIG T 0x085d1f68 sz 0x3e2）。
// 函数级静态结果向量（guard 变量 b 0x09472f80 / 向量 b 0x09472f88）：
//   [0] 机主本人且 rarity>1 → script+0x34（selfService[1]，奖励 A 物品）
//   [1] map 命中项的 stDisjointResult.m_field0（cube 索引）
//   [2..] 附加/大奖物品表中未收录过的 m_itemIndex（std::find 去重）
// ---------------------------------------------------------------------------
// [L2Free4to15.S3 复核] p167 CLEAR（仅 C1/C2·D1/D2 别名）。prologue/帧全等、
// ORIG-only/OURS-only 空、gap=0；残差 = 全帧槽号整体深 8
// （ORIG -0x4b/-0x50/-0x54/-0x58 ↔ 我方 -0x53/-0x58/-0x5c/-0x60）
// ⇒ N82/N95 族封顶。
#include "ExpertJobLink4/GetPredictResult.inc"
                                                Inven_Item* inventItem)
{
    static std::vector<int> result;   // ORIG GetPredictResult(...)::result
    result.clear();

    bool unidentified = false;
    // [b28d-solo-ExpertJobLink4 P198①/P194]：ORIG 0x85d1fd1 `movb $0,-0x1d`
    // 预清 + 条件 mov$1/mov$0 物化后 `mov %al,-0x1d`（赋值表达式，非 if 置位）。
    unidentified =
        (inventItem != 0 && !inventItem->m_amp.isIdentified());

    // [b28d-solo-ExpertJobLink4 P225 推广]：ORIG 0x85d2049 `cmp $0x1; setg %al;
    // test; je` 双条件各自物化 ⇒ 拆嵌套 if（`&&` 合写在 4.4 恒折 jle 直跳）。
    if (isMine(user))
    {
        if (item->get_rarity() > 1)
        {
            // ORIG 0x85d2053 `mov 0x34(%eax),%eax; mov %eax,-0x54; lea -0x54`
            // ⇒ push_back 实参先物化到临时槽（4.4 直传成员地址）。
            int selfReward = GetScript()->m_selfDisjoin.m_field4;
            result.push_back(selfReward);
        }
    }

    DisjointMachineGrade key((unsigned char)item->get_rarity(),
                             (unsigned char)get_disjoint_machine_grade(),
                             unidentified);
    // ORIG find/end 各独立 `call GetScript`（无 script 具名槽）。
    std::map<DisjointMachineGrade, stDisjointResult>::iterator it =
        GetScript()->m_disjointResultMap.find(key);
    if (it != GetScript()->m_disjointResultMap.end())
    {
        stDisjointResult& res = it->second;
        // ORIG 0x85d2117 `mov (%eax),%eax; mov %eax,-0x44` 值临时 ⇒ 右值重载
        // push_back(int&&)（p167 差集 MISSING `push_backEOi`）。
        result.push_back((int)res.m_field0);

        // [b28d-solo-ExpertJobLink4 P96/P270/P315]：p167 差集实证 ORIG 用
        // 非 const iterator（deEv/ppEv/ne 非 const 实例）；且 ORIG 0x85d2135
        // 先 `call iterator::C1Ev`（P50 复活）再 `rate = begin()`，两个循环
        // 复用同一 rate 槽 -0x58（第二个循环无 C1Ev）。
        std::vector<stItemSelectRate>::iterator rate;
        for (rate = res.m_additionalItems.begin();
             rate != res.m_additionalItems.end(); ++rate)
        {
            // [b28d-solo-ExpertJobLink4 P167]：ORIG 0x85d20xx 用
            // `call operator*; mov %eax,%ebx`（m_itemIndex@+0 ⇒ &(*rate)）
            // 而非 `operator->` ⇒ 源写 `(*rate).m_itemIndex`。
            if (std::find(result.begin(), result.end(), (*rate).m_itemIndex) ==
                result.end())
                result.push_back((*rate).m_itemIndex);
        }
        for (rate = res.m_bigwinItems.begin();
             rate != res.m_bigwinItems.end(); ++rate)
        {
            if (std::find(result.begin(), result.end(), (*rate).m_itemIndex) ==
                result.end())
                result.push_back((*rate).m_itemIndex);
        }
    }
    return &result;
}

}  // namespace expert_job

// ===========================================================================
// 分解链全局校验函数（LinkMisc5 收口；调用方 DispDebugJoint4.cpp 以 mangled
// 直连，符号与 ORIG 一致）
// ===========================================================================

#include "Arad_DataManager.h"      // ARAD::Singleton<T>
#include "ServiceRestrictManager.h"

// GetPredictDisjointItems（报告 _lt_global_gt_/GetPredictDisjointItems.md，
// ORIG T 0x081f8f8d sret）：无专家职业走 DisJoint 表静态结果向量，
// 否则走分解机实例预测。
std::vector<int>* GetPredictDisjointItems(CUser* user, CItem* item,
                                          Inven_Item& invenItem,
                                          expert_job::CDisjointer* disjointer)
{
    if (disjointer != 0)
        return disjointer->GetPredictResult(user, item, &invenItem);
    else
    {
        return G_CDataManager()->m_disJoint->GetPredictResult(item->get_rarity());
    }
}

// ---- DisJoint::getPredictResultItemTypeCount（ORIG T 0x08473148，
//      docs/class_func_reports/DisJoint/getPredictResultItemTypeCount.md）：
//      rarity ∉ [0,5] → 1；否则读 m_additional[rarity].m_count
//      （this+0x5c8+rarity*0x10，P8：Ghidra 的 (param+0x5c)*0x10+8 即 entry
//      起点 m_count，非 +8 字段），非 0 → 2，否则 → 1。 ----
#include "ExpertJobLink4/getPredictResultItemTypeCount.inc"
{
    if (rarity < 0 || rarity > 5)
        return 1;
    if (m_additional[rarity].m_count != 0)
        return 2;
    return 1;
}

// ---- DisJoint::GetPredictResult（报告 GetPredictResult.md，ORIG T 0x08473116）：
//      rarity ∈ [0,5] → &m_resultVecs[rarity]（this+0x628+rarity*0xc）；
//      越界 → &m_resultVecs[0]（this+0x628）。rarity 按有符号比较（js/jle）。 ----
#include "ExpertJobLink4/GetPredictResult_1.inc"
{
    if (rarity < 0 || rarity > 5)
        return &m_resultVecs[0];
    return &m_resultVecs[rarity];
}


// ORIG W 0x0822b1f4（声明补入 STExpertJobScript.h；定义置本 TU 保持
// expert_job 域内聚）。m_portableDisjointMap(+0x18) find(level)，未命中 -1。
#include "ExpertJobLink4/GetPortableDisjointPossibleLevel.inc"
{
    std::map<int, unsigned short>::iterator it = m_portableDisjointMap.find(level);
    if (it == m_portableDisjointMap.end())
        return -1;
    // [b28c-L18] ORIG 尾双段 `movzwl 0x4(%eax),%eax; movzwl %ax,%eax` = int 返回
    // + (unsigned short) 提升（探针 dj.cpp f_d 全等）。
    return (unsigned short)it->second;
}

// ---- DisPatcher_DisJointItem::_checkDisjointableItem
// （报告 _checkDisjointableItem.md，ORIG T 0x081fa0f6）：
// 未鉴定且无目标用户 → 0xae；物品缺失 → 0x11；
// IsImpossibleDisjoint（ORIG vptr+0x38 = vtable 槽16）→ 0x13；GetAttachType==2 → 0x11。 ----
// [b28c-31] N7+N18b：虚调用 callee ORIG %edx vs 4.4 %eax（vptr+0x38 槽）；
// 另 GetAttachType 结果 ORIG 回读槽再 `cmp $0x2,%eax`，4.4 恒折 `cmpl $0x2,-0xc`
// 内存直比（P209 附 / N18b）。
#include "ExpertJobLink4/_checkDisjointableItem.inc"
                                                    CUser* targetUser)
{
    // ORIG 0x081fa0fd..0x081fa12d：item.m_amp(+0x11).isIdentified()==false
    // 且 targetUser==0 → 0xae（未鉴定且无目标用户不可分解）
    if (!item.m_amp.isIdentified() && targetUser == 0)
        return 0xae;

    CItem* itemObj = G_CDataManager()->find_item(item.m_addInfo);
    if (itemObj == 0)
        return 0x11;

    if (itemObj->IsImpossibleDisjoint())   // ORIG vptr+0x38（IsImpossibleDisjoint，非 CharacCargo）
        return 0x13;
    // b28d-61 [P286 单 case switch 复现寄存器比较，ORIG 0x081fa183
    // `mov -0xc(%ebp),%eax; cmp $0x2,%eax; jne`]：if 形（int/unsigned/const/cast
    // 六形探针 /tmp/b61_probe_att{,2}.cpp）4.4 恒折 `cmpl $0x2,-0xc` 内存直比；
    // 单 case switch 得寄存器链。
    int attachType = itemObj->GetAttachType();
    switch (attachType)
    {
    case 2:
        return 0x11;
    }
    return 0;
}

// ---- _check_disjoint_need_slot（报告 _check_disjoint_need_slot.md，
//      ORIG T 0x081f8fe3 sz 0x2e7）：预测产物按背包类型分桶统计空位需求，
//      缺口超出第 0 类空槽数则抛 ENUM_ERROR 4。 ----
#include "ExpertJobLink4/_check_disjoint_need_slot.inc"
                               expert_job::CDisjointer* disjointer)
{
    // [b28d-solo-ExpertJobLink4 P214③/P312]：ORIG 0x81f8fef..0x81f902c 三个
    // 11-dword `rep stos`（清零序 total -0x60 → perType -0x8c → emptySlots
    // -0xb8）⇒ 三个 `= {0}` 显式零初始化数组，非循环赋值。
    int total[11] = {0};
    int perType[11] = {0};
    int emptySlots[11] = {0};

    // [P96/P270] ORIG 三处下标调用均为 const 版 `_ZNKSt6vectorIiSaIiEEixEj`
    // （0x081f906a / 0x081f908b / 0x081f90cb）⇒ 原码持 const 指针
    // （size() 仅有 const 版，故不产生 cv 线索）。
    const std::vector<int>* predicted =
        GetPredictDisjointItems(user, item, invenItem, disjointer);

    for (unsigned int i = 0; i < predicted->size(); ++i)
    {
        // [b28d-solo-ExpertJobLink4 P157/P183]：ORIG 对 (*predicted)[i] 共三次
        // 独立下标调用（判零 / find_item / probe.m_addInfo），无 itemIndex 槽。
        if ((*predicted)[i] == 0)
            continue;

        CItem* templateItem = G_CDataManager()->find_item((*predicted)[i]);
        if (templateItem == 0)
            continue;

        Inven_Item probe;                       // ORIG 局部 ctor 清零
        probe.m_addInfo = (*predicted)[i];      // 直接写 +0x02
        probe.set_add_info(1);                  // ORIG：m_addInfo2 = 1（set_add_info 语义）
        templateItem->make_item(probe);         // ORIG 0x81f90f3..0x81f9108：vptr+8
                                                // 虚槽 make_item 填充 type 等（CItem.h vtable 槽 2）
        // ORIG 用立即数 `cmpl $0xb,-0x24`（kInvalidSlot 无具名槽，P0）。
        int needSlot = Inven_Item::ITEM_TYPE_INVALID;
        // ORIG 每次调用前重新 getCurCharacInvenR()，itemIndex 取 probe.m_addInfo
        // （+0x02），probeType 取 probe.m_field1（+0x01）逐次重读（无具名局部）。
        if (user->getCurCharacInvenR()->IsExistStackable(
                probe.m_addInfo, (Inven_Item::ITEM_TYPE)probe.m_field1))
        {
            needSlot = (int)probe.m_field1;
        }
        else if (user->getCurCharacInvenR()->IsExistStackable(
                     probe.m_addInfo, (Inven_Item::ITEM_TYPE)0))
        {
            needSlot = 0;
        }

        // ORIG 0x81f9183 `cmpl $0xb,-0x24; je <total[probeType] 远端块>`：
        // 反写条件使 invalid 分支落远端（P47/P222）。
        if (needSlot != Inven_Item::ITEM_TYPE_INVALID)
        {
            // ORIG 单次索引读（`mov needSlot,%eax; mov arr(%ebp,%eax,4),%edx`）；
            // `+= 1` 在 4.4 恒多读一次索引槽（+1 条/处）。
            ++perType[needSlot];
            ++total[needSlot];
        }
        else
        {
            ++total[probe.m_field1];
        }
    }

    for (int i = 0; i < 11; ++i)
        emptySlots[i] = user->getCurCharacInvenR()->InvenEmptycount(
            (Inven_Item::ITEM_TYPE)i);

    int deficit = 0;
    for (int i = 0; i < 11; ++i)
    {
        if (emptySlots[i] + perType[i] < total[i])
            deficit += total[i] - (emptySlots[i] + perType[i]);
    }

    if (deficit > emptySlots[0])
        throw static_cast<ENUM_ERROR>(4);
}

// ---- _check_disjointable_item_state（报告 _check_disjointable_item_state.md，
//      ORIG T 0x081f8ba1 sz 0x3ec）：取源物品快照、拆解券等级门槛与
//      DisPatcher 静态校验，任一失败抛对应 ENUM_ERROR。 ----
#include "ExpertJobLink4/_check_disjointable_item_state.inc"
                                    Inven_Item& item, CItem** pItem,
                                    CUser* targetUser, unsigned short region)
{
    // 快照：GetInvenSlot(invenType, slot) 全量拷贝（0x3d 字节 sret）
    item = user->getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        throw static_cast<ENUM_ERROR>(0x11);   // 宠物装备不可拆

    if (!item.isEquipableItemType() || item.m_addInfo == 0)
        throw static_cast<ENUM_ERROR>(0x11);

    if (region != 0xffff)
    {
        Inven_Item ticket =
            user->getCurCharacInvenR()->GetInvenSlot(1, (int)region);
        CStackableItem* ticketObj =
            (CStackableItem*)G_CDataManager()->find_item(ticket.m_addInfo);
        if (ticketObj == 0)
            throw static_cast<ENUM_ERROR>(0x15);

        int ticketLevel = ticketObj->m_int324;   // +0x324 使用等级门槛
        if (ticketLevel == -1)
            throw static_cast<ENUM_ERROR>(1);   // ORIG：门槛 -1 直接抛 1

        // ORIG +0x21c：经 expert_job::CDisjointer::GetScript()（缓存单例）取脚本，
        // 非裸 G_CDataManager()->GetExpertJobScript(3)。
        // [b28d-solo-ExpertJobLink4 P0]：ORIG 0x81f8dbd `call GetScript;
        // mov %eax,-0x18` 有具名 script 槽（再回读调用），非内联接收者。
        STExpertJobScript* script = expert_job::CDisjointer::GetScript();
        int portableLimit =
            script->GetPortableDisjointPossibleLevel(ticketLevel);
        // ORIG +0x234：portableLimit==-1 亦抛 1（原实现漏该判据）。
        if (portableLimit == -1)
            throw static_cast<ENUM_ERROR>(1);

        CItem* itemObj = G_CDataManager()->find_item(item.m_addInfo);
        if (itemObj == 0)
            throw static_cast<ENUM_ERROR>(0x11);

        int usableLevel = itemObj->getUsableLevel();
        if (usableLevel == 0)
        {
            if (portableLimit < itemObj->get_grade())
                throw static_cast<ENUM_ERROR>(0xd);
        }
        else if (usableLevel > portableLimit)
        {
            throw static_cast<ENUM_ERROR>(0xd);
        }
    }

    // [b28d-solo-ExpertJobLink4 P198①]：ORIG 0x81f8ee3 先 `movl $0x0,-0x24`
    // 预清再赋值（两句式）。
    int err = 0;
    err = DisPatcher_DisJointItem::_checkDisjointableItem(item, targetUser);
    // [b28d-solo-ExpertJobLink4 P225]：ORIG 0x81f8f03 `cmpl $0,-0x24; setne
    // %al; test %al,%al; je`（4.1 bool 四段物化）⇒ while+break 形复现。
    while (err != 0)
    {
        throw static_cast<ENUM_ERROR>(err);
    }

    *pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (*pItem == 0)
        throw static_cast<ENUM_ERROR>(0x11);
}

// ---- _check_disjointable_user_state（报告 _check_disjointable_user_state.md，
//      ORIG T 0x081f8a7c sz 0x125）：状态门 + 服务限制 + 保护字段三查。 ----
#include "ExpertJobLink4/_check_disjointable_user_state.inc"
                                    char spaceChar, short slotShort)
{
    if (user->get_state() < 3)
        return false;

    // [b28d-solo-ExpertJobLink4 P59/P0/P183]：ORIG 正极性 `test %al,%al; je
    // <主流程>`（isRestricted 真→限制路径紧跟 je 之后），且 Singleton::Get /
    // s_pSecuProtectionField 均无具名槽（每次调用点重读）；GetInvenItemPtr
    // 结果经 %ebx 跨 GetItemPtr 保活 ⇒ 全内联实参形。
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 10))
    {
        user->SendCmdErrorPacket(cmd, 0xd1);
        return false;
    }

    unsigned int err = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28,
        GlobalData::s_pSecuProtectionField->GetItemPtr(user, spaceChar,
                                                       slotShort),
        GlobalData::s_pSecuProtectionField->GetInvenItemPtr(user, spaceChar,
                                                            slotShort));
    // [b28d-solo-ExpertJobLink4 P47/P137]：ORIG `je <尾 mov $1 块>` + fail 直落
    // ⇒ 反写条件使 true 走尾块 fall-through。
    if (err != 0)
    {
        user->SendCmdErrorPacket(cmd, (unsigned char)(err & 0xff));
        return false;
    }
    return true;
}
