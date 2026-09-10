// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inven_Item/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Inven_Item 还原（2026-08-16）。
// 布局/函数形态以 ORIG 反汇编为准（AE 口径：调用目标归一）。

#include "Inven_Item.h"

// ORIG 全局（0x943ddc0）：空槽模板。定义于本 TU 顶部 —— ORIG 的
// _GLOBAL__I_g_emptySlot @ 0x0850d0cb 位于 Inven_Item 内联函数群首部，
// 供 CAccountCargo/CInventory 等 extern 引用。
Inven_Item g_emptySlot;

// ===================== stAmplifyOption_t（ORIG 0x80cb75c 族） =====================

#include "Inven_Item/getAbilityType.inc"
{
    if (!hasAbility())
    {
        return 0;
    }
    if (!isIdentified())
    {
        return 0x80;
    }
    return m_abilityType;
}

#include "Inven_Item/getAbilityValue.inc"
{
    if (!hasAbility() || !isIdentified())
    {
        return 0;
    }
    return m_abilityValue;
}

#include "Inven_Item/reset.inc"
{
    m_abilityType = 0;
    m_abilityValue = 0;
    m_lock = 0;
}

// ORIG W 0x0822b3d8：返回 bool = !isIdentified()（已识别→false 不改动）。
#include "Inven_Item/identify.inc"
{
    if (isIdentified())
    {
        return false;
    }
    m_abilityType &= 0x7f;
    return true;
}

#include "Inven_Item/isIdentified.inc"
{
    // ORIG 0x081507bc：not 后 shr $0x7,%al（8 位逻辑右移，无 setne）。& 0x80 形式保持
    // 字节移位；原 (uchar)>>7 被 4.4 升格 sar$7+test+setne（N87 移位升格）。
    return ((~m_abilityType) & 0x80) != 0;
}

#include "Inven_Item/hasAbility.inc"
{
    return m_abilityType != 0;
}

#include "Inven_Item/_setAbility.inc"
                                    bool identified)
{
    m_abilityType = type;
    m_abilityValue = value;
    if (!identified)
    {
        m_abilityType |= 0x80;
    }
}

#include "Inven_Item/assign.inc"
{
    m_abilityType = type;
    m_abilityValue = value;
}

#include "Inven_Item/SetLock.inc"
{
    m_lock = lock;
}

#include "Inven_Item/GetLock.inc"
{
    return m_lock;
}

#include "Inven_Item/getValues.inc"
                                  unsigned short& value) const
{
    type = m_abilityType;
    value = m_abilityValue;
}

// ===================== Inven_Item 内嵌辅助类 =====================

#include "Inven_Item/reset_1.inc"
{
    // [b28d-solo-Inven_Item] ORIG 0x080cb778（W，0x26 字节）三段位清：
    //   movzbl (%eax),%edx; and $0xfe/%edx; mov %dl,(%eax) ×3（掩码 0xfe/0xf9/0x7）
    // ⇒ 源形 = 位域清零（1+2+5 布局）。uchar 标量/指针 cast 形 4.4 恒发
    //   `movzbl (%eax),%eax; mov %eax,%edx; and…; mov this,%eax`（+4 条，探针 E1
    //   r1/r2/r3 三形同），位域形逐条全等（探针 E2/E3）。
    m_bits.m_flag = 0;
    m_bits.m_type = 0;
    m_bits.m_skillId = 0;
}

#include "Inven_Item/reset_2.inc"
{
    m_field0 = 0;
    m_field1 = 0;
    m_field2 = 0;
}

// RandomOptionSeed::reset 定义已迁 DnfItemInfo.cpp（unit-at-a-time outgoing
// 塌缩修复：本 TU 可见 callee 体使 RandomOption::reset outgoing 塌缩 0x4，
// ORIG sub $0x18；挪走后 4.4 恢复 0x18）。

#include "Inven_Item/reset_3.inc"
{
    m_field0.reset();
    m_field3.reset();
    m_field6.reset();
    m_seed9.reset();
    m_fielda.reset();
    m_seedd.reset();
}

// ORIG 0x822abd0 W：movzbl (this) → 首字节==0 返回 true。
#include "Inven_Item/empty.inc"
{
    if (m_field0.m_field0 == 0)
        return true;
    return false;
}

// ORIG W 0x080cb332：构造后调 reset()（位域清零）。
#include "Inven_Item/UpgradeSeparateInfo.inc"
{
    reset();
}

#include "Inven_Item/reset_4.inc"
{
    // ORIG 0x80cb346：位域 b0:5/b1:1/b2:2 清零 → 三次位清
    // （&0xffffffe0 → &0xffffffdf → &0x3f），整字节清零。
    struct Bits { unsigned char a:5, b:1, c:2; };
    reinterpret_cast<Bits*>(&m_field)->a = 0;
    reinterpret_cast<Bits*>(&m_field)->b = 0;
    reinterpret_cast<Bits*>(&m_field)->c = 0;
}

#include "Inven_Item/GetUpgradeSeparate.inc"
{
    return m_field & 0x1f;
}

#include "Inven_Item/SetUpgradeSeparate.inc"
{
    // ORIG 0x84e906a：位域 m_lo 装填（value 截断 and + 装填截断 and 双截断形）。
    m_bits.m_lo = value;
}

// ORIG 0x8110b0a：8 条形 = 位域 m_flag:1 读取 + unsigned char 返回（probe u1 全等）。
#include "Inven_Item/IsTradeRestriction.inc"
{
    return m_bits.m_flag;
}

#include "Inven_Item/ReservedCapacity.inc"
{
    reset();
}

#include "Inven_Item/reset_5.inc"
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ===================== Inven_Item =====================
#include "Inven_Item/Inven_Item.inc"
{
    reset();
}

#include "Inven_Item/reset_6.inc"
{
    m_field0 = 0;
    m_field1 = 0;
    m_attr = 0;
    m_addInfo = 0;
    m_addInfo2 = 0;
    m_fieldb = 0;
    m_fieldd = 0;
    m_amp.reset();
    m_random.reset();
    m_upgradeSep.reset();
    m_reserved.reset();
}

#include "Inven_Item/set_add_info.inc"
{
    m_addInfo2 = value;
}

#include "Inven_Item/get_add_info.inc"
{
    return m_addInfo2;
}

#include "Inven_Item/ResetItemAttr.inc"
{
    m_attr = 0;
}

#include "Inven_Item/GetUpgrade.inc"
{
    return m_attr & 0x1f;
}

#include "Inven_Item/SetUpgrade.inc"
{
    m_attrBits.m_attrLo = upgrade;
}

#include "Inven_Item/IncUpgrade.inc"
{
    // ORIG 0x854b4be：读侧截断留在 eax、this 重载 edx（probe i4 ++ 形全等）。
    m_attrBits.m_attrLo++;
}

#include "Inven_Item/GetItemAttr.inc"
{
    return m_attrAll.m_attrAll;
}

#include "Inven_Item/SetItemAttr.inc"
{
    m_attr = attr;
}

#include "Inven_Item/GetReSealCount.inc"
{
    // ORIG 0x822b456：movzbl 0x6 → shr $0x5,%al（位域 hi:3 读）
    return m_attrBits.m_attrHi;
}

#include "Inven_Item/SetReSealCount.inc"
{
    m_attrBits.m_attrHi = count;
}

// ORIG _Z23DecreaseTradeLimitCountR10Inven_Item T 0x0812013c：
// ORIG 块序 jne 进主流程/false 块直落 = 早退形（P16）。
// unit-at-a-time 规避（实证 /tmp/ii_half7.cpp）：本函数须置于
// GetTradeLimitCount/SetTradeLimitCount 外联定义之前，否则 4.4 删
// framePad 槽塌缩帧 0x28→0x18、count 槽 -0x9→-0x1。
#include "Inven_Item/DecreaseTradeLimitCount.inc"
{
    // ORIG 帧 0x28、count 槽 -0x9（count 前有 8 字节 dead 槽）；
    // 4.4 删 dead local，用被引用 double 撑回（探针 /tmp/dtlp/a.cpp 逐条同形）。
    double framePad;
    (void)&framePad;
    unsigned char count = item.GetTradeLimitCount();
    if (count == 0)
        return false;
    item.SetTradeLimitCount(count - 1);
    return true;
}

#include "Inven_Item/GetTradeLimitCount.inc"
{
    return (m_attr & 0xe0) >> 5;
}

#include "Inven_Item/SetTradeLimitCount.inc"
{
    m_attrBits.m_attrHi = count;
}

#include "Inven_Item/isAvatarItemType.inc"
{
    return m_field1 == 8;
}

#include "Inven_Item/IsCreatureItemType.inc"
{
    return m_field1 == 5 || m_field1 == 7 || m_field1 == 6;
}

#include "Inven_Item/isEquipableItemType.inc"
{
    return m_field1 == 1 || m_field1 == 5 || m_field1 == 6 ||
           m_field1 == 8;
}

#include "Inven_Item/isEmpty.inc"
{
    if (m_addInfo == 0)
        return true;
    return false;
}

#include "Inven_Item/setCopy.inc"
{
    m_field0 = other.m_field0;
    m_field1 = other.m_field1;
    m_addInfo = other.m_addInfo;
    m_addInfo2 = other.m_addInfo2;
    m_fieldb = other.m_fieldb;
    m_attr = other.m_attr;
    m_fieldd = other.m_fieldd;
    // b28d-60：P247/P29——ORIG 0x814a688 起为「一次载入 this/other + 连续
    // disp 拷贝」形；原 (char*)&m + k 指针算术使 4.4 逐字段重算地址（add 链，
    // 60→92 insn）。改具名视图/整体赋值：m_random/m_reserved 整体结构赋值
    // （4.4 对 20B POD 发 5 连发 mov，与 ORIG 同）。
    // b28d-solo-Inven_Item：+0x11/+0x33 两处 ORIG 为 LHS-first
    // （`mov this,%eax; mov other,%edx; mov d(%edx),…; mov …,d(%eax)`），标量/
    // union 标量视角形 4.4 恒 RHS-first（N76）。改结构赋值形即复现：
    // +0x11 → stAmplifyHeadView 四字节结构（P247，CCreatureMgr setCopy 同法）；
    // +0x33 → UpgradeSeparateInfo 整体赋值（1 字节 POD 结构赋值，探针 D2 全等）。
    m_amp.m_amp0Head = other.m_amp.m_amp0Head;
    m_random = other.m_random;
    m_upgradeSep = other.m_upgradeSep;
    m_reserved = other.m_reserved;
}

#include "Inven_Item/getPackedCode1.inc"
{
    // ORIG 0x850d102：全程 or %eax,mem 形需 code 为 unsigned int（probe p2 全等）。
    unsigned int code = 0;
    if (m_field0 != 0)
    {
        code |= 1;
    }
    code |= (m_field1 & 0x7) << 1;
    code |= m_addInfo << 4;
    code |= m_addInfo2 & 0xfff00000;
    return code;
}

#include "Inven_Item/getKey.inc"
{
    return m_addInfo;
}

#include "Inven_Item/GetInventoryType.inc"
{
    int result;
    register unsigned char t = (unsigned char)m_field1;
    if (t < 0xb)
    {
        // [b28d-solo-Inven_Item] N 封顶（语义等价，仅跨版本 codegen 差）：
        // ①开头 ORIG `movzbl 0x1(%eax),%eax; movzbl %al,%eax; cmp $0xa,%eax`
        //   （4.1 保留 (unsigned char) 转换的二次零扩展）；4.4 恒折 8 位比较
        //   `cmp $0xa,%al`（探针 f_ucast/f_uicast/f_plain 三形同）。
        // ②三次掩码测试 ORIG `mov %ebx,%eax; mov %eax,%edx; and $imm,%edx`
        //   （4.1 把 u 留在 %eax、用 %edx 作 and 临时），4.4 恒
        //   `mov %ebx,%eax; and $imm,%eax`（每次自 %ebx 重载，N18a/N7 族）。
        // 已试 12 形（register 体内/体外、int/unsigned、嵌套 if、!=0、三元、
        // 双 register 变量、具名槽）+ 4.4.6 后端复测，全部同形 ⇒ 不可造。
        // P231 纯 register 仍必要（免 u 栈槽，否则帧/槽全错位）。
        // [N2-151to170] register uchar t 单次读取（比较与 shift 共用）+ 三段独立 if
        // 各自 goto 共享尾出口（default 块 fall-through）⇒ 41→38 条 = ORIG 38（gap 0）。
        // 残余为跨版本寄存器/块序：①t 装载 4.4 落 %ebx / ORIG 落 %eax；②掩码 and 的
        // 临时 4.1 走 %edx 双指令、4.4 自 %ebx 重载折叠单指令（P248 族）；③叶块 4.1
        // 尾部集中（jne 跳叶块）/ 4.4 内联（je 跳过）⇒ 指令数已齐平，DIFF 封顶。
        register unsigned int u = 1u << t;
        if (u & 0x61f)
        {
            result = 1;
            goto done;
        }
        if (u & 0x100)
        {
            result = 2;
            goto done;
        }
        if (u & 0xe0)
        {
            result = 3;
            goto done;
        }
    }
    result = 1;
done:
    return result;
}

#include "Inven_Item/GetItemSpace.inc"
{
    int result;
    register unsigned char t = (unsigned char)m_field1;
    if (t < 0xb)
    {
        // [b28d-solo-Inven_Item] 同 GetInventoryType：N 封顶（跨版本冗余 zext
        // `movzbl %al,%eax` 被 4.4 折叠 + and 临时寄存器选择 4.1 %edx / 4.4 %eax）；
        // P231 register 免 u 槽，控制流与返回值语义一致。
        // [N2-151to170] 同 GetInventoryType：register uchar t 单次读取 + 三段独立 if
        // 各自 goto 共享尾出口 ⇒ 41→38 条 = ORIG 38（gap 0）；残余同上（跨版本
        // t/u 寄存器分配、and 临时折叠、叶块内联 vs 尾部集中）⇒ DIFF 封顶。
        register unsigned int u = 1u << t;
        if (u & 0x61f)
        {
            result = 0;
            goto done;
        }
        if (u & 0x100)
        {
            result = 1;
            goto done;
        }
        if (u & 0xe0)
        {
            result = 7;
            goto done;
        }
    }
    result = 0;
done:
    return result;
}

#include "Inven_Item/SetUpgradeSeparateAndTradeRestriction.inc"
{
    // ORIG 0x085574bc (W)：*(unsigned char*)this = value —— 整字节覆盖，
    // 同时写入 upgrade 槽位（bit0-4）与 trade-restriction 位（bit5）。
    m_field = value;
}
