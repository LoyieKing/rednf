// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHackLog_OverDamage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CHackLog_OverDamage.h"
#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE 权威枚举
#include "CUser.h"
#include "CParty.h"     // m_dungeon / m_battleField 视图
#include "CDungeon.h"   // get_min_level/get_index
#include "CBattle_Field.h"   // get_dungeon_diff

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_CONST[7] = {
    600, 1800, 3600, 25000, 55000, 70064, 712640
};   // {258,708,e10,61a8,d6d8,1170,ae60}

// ORIG _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO（objdump 实测）：
// {2.0,1.8,1.7,1.6,1.0}——第 3/4 项原抄错为 0.075/0.025，已按 ORIG 修正。
const double _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[5] = {
    2.0, 1.8, 1.7, 1.6, 1.0
};

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[7] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b
};

const int LEVEL_SECTION_TABLE[89] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

#include "CHackLog_OverDamage/GetDungeonLimitDamageAndHackType.inc"
    CUser* user, unsigned int& limit, WongWork::ENUM_HACKTYPE& type)
{
    // [b28d-46 判词·N/N82] ORIG 0x08286790：149 vs 147；帧 0x54 vs 0x58 与
    // 局部预清槽（movb $0,-0x31 等）平移（N82）⇒ 封顶。
    // ORIG 0x0828679e-0x082868a0：4 条件单出口 bool 汇聚（3 guard 各自
    // `je <mov$0,eax>`，通过则 `mov$1,eax`，尾 `test%al; je ret`）；
    // 每使用点重 call GetParty()+mov 0xcac（P160 去缓存）。
    // [N6S3 2026-09-11] ORIG 0x082867d0 `mov $0x1,%eax` / 0x082867d7
    //   `mov $0x0,%eax` 物化的是**条件原值**（非取反），尾 0x082867dc
    //   `test %al,%al; je 0x0828697e`（假 → 直落函数尾）⇒ 源码形
    //   `if (cond) { …全体… }`。对照探针（N6S3 /tmp/probe7.cpp）：k9 该形
    //   与 ORIG 逐条同形（jle + mov$1/jmp/mov$0 + test + je）；k3
    //   `if (!cond) return;` 则多取反（jg/jne）——故旧形两处极性不符。
    //   ORIG 三处 get_charac_level 均重 call（0x082867c7 / 0x082867f6 /
    //   0x08286909）⇒ level 不缓存（P0/P183 删具名局部）。
    // [N6S3 2026-09-11 剩余 1 处·路由轴] 唯一残差 = 浮点比较形态：
    //   ORIG 0x082868ca-0x082868d4 `fucompp; fnstsw %ax; test $0x45,%ah;
    //   sete %al`（i386 族序列，4 条）vs 现路由 447 默认 `fucomip %st(1),%st;
    //   fstp %st(0); seta %al`（3 条）。本机 -march 扫描实证：i386/i486/i586
    //   ⇒ fnstsw 序列，i686/pentium4 ⇒ fucomip（探针 /tmp/probe8.cpp）；
    //   源码三种写法（`d<1.0` / `1.0>d` / `!(d>=1.0)`）均 fucomip ⇒ 不可
    //   从源码消除。取证编译（/tmp，未改脚本/TU）：本 TU 叠
    //   `-march=i486 -mtune=generic` ⇒ 四数 identical=3 ae=5 diff=1（仅工具
    //   假缺口 static_init），即本件 DIFF→AE；现路由 identical=3 ae=4 diff=2。
    //   ⇒ 需父泳道在 dnf_game_cxx.sh 加该 TU 分支（已上报 Main/N6Free231to250）。
    // [P6AuditOldN.P6S2 2026-09-11 · 假 DIFF 回收] 路由落地后复编实测：
    //   n1_v = **AE 152/152 gap=0**（旧判词 DIFF 152/152）；TU 四数
    //   identical=3 ae=5 near=0 diff=1 missing=0（diff 仅
    //   `_Z41__static_initialization_and_destruction_0ii` 假阳性实例）。
    //   本波新钥匙（P339 声明序/P341 goto 尾/真 switch/类型轴三式/守卫包裹）
    //   对本件（浮点 lowering 轴）均不适用 ⇒ 维持 AE。
    if (user != 0
        && user->GetParty() != 0
        && static_cast<CParty*>(user->GetParty())->m_battleField.m_pDungeon != 0
        && user->get_charac_level() >= 1)
    {
        int sectionIdx = 6;                   // ORIG local_30 初值
        double ratio = 1.0;                   // ORIG local_2c 初值
        // ORIG 0x082867f0-0x0828681c：call get_charac_level → ebx →
        // `sub %eax,%edx` 后直接入槽 -0x1c（level 无槽缓存）。
        int lvDelta = user->get_charac_level()
                    - static_cast<CParty*>(user->GetParty())
                          ->m_battleField.m_pDungeon->get_min_level();   // ORIG local_20
        if (lvDelta < 0)
            lvDelta = 0;
        double divisor = 1.0;                 // ORIG local_1c 初值

        // ORIG 0x0828684d-0x082868a2：switch(dungeonIndex) 二分树
        // （cmp 0x2af8 双试 je/jg + cmp 0x64/0x2af9/0x2afe），**所有 case 汇聚
        // 同一 LAB_0828692e 尾计算块**（旧形把 limit/type 算块复制了两份）。
        // 索引式直用（ORIG 0x08286845 call get_index 后即 `cmp $0x2af8,%eax`，
        // 无 `mov %eax,-0x10(%ebp)` 具名局部转存）。
        switch (static_cast<CParty*>(user->GetParty())->m_battleField.m_pDungeon->get_index())
        {
        case 100:
            ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[
                reinterpret_cast<CBattle_Field*>(
                    &static_cast<CParty*>(user->GetParty())->m_battleField)->get_dungeon_diff()];
            break;                          // → 尾（ORIG +0xeb..+0x0d jmp 19e）
        case 11000:
            break;                          // → 尾（ORIG +0x197 `jmp 19e`）
        case 0x2af9:
            break;                          // → 尾（ORIG +0x19a `jmp 19e`）
        case 0x2afe:
            break;                          // → 尾（ORIG +0x19d `jmp 19e`）
        default:
            divisor = static_cast<double>(lvDelta / 20);
            if (divisor < 1.0)
                divisor = 1.0;
            int lvKey = user->get_charac_level() - 1;
            // [N6S3 2026-09-11] ORIG 0x082868ee-0x082868f6 无分支 clamp：
            //   `mov -0xc,%eax; sar $0x1f,%eax; not %eax; and -0xc,%eax`（不
            //   store 回槽，直接用于索引）⇒ 源码形 `lvKey & ~(lvKey >> 31)`
            //   （= max(lvKey,0)）。分支版生成 cmpl/jns/movl + 槽回写，不符。
            sectionIdx = LEVEL_SECTION_TABLE[lvKey & ~(lvKey >> 31)];
            ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[
                reinterpret_cast<CBattle_Field*>(
                    &static_cast<CParty*>(user->GetParty())->m_battleField)->get_dungeon_diff()];
            break;
        }

        limit = static_cast<unsigned int>(
            static_cast<long long>(
                static_cast<double>(_S_KILL_MONSTER_OVER_DAMAGE_CONST[sectionIdx]) *
                ratio * divisor));
        type = static_cast<WongWork::ENUM_HACKTYPE>(
            _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[sectionIdx]);
    }
}

#include "CHackLog_OverDamage/GetDeathTowerLimitDamage.inc"
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[6];
}

#include "CHackLog_OverDamage/GetDeathTowerOverDamageHackType.inc"
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
}

#include "CHackLog_OverDamage/GetDungeonLimitMaxDamage.inc"
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[6];
}

#include "CHackLog_OverDamage/GetHackTypeOverLimitMaxDamage.inc"
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
}

#include "CHackLog_OverDamage/GetBossTowerLimitDamage.inc"
{
    return _S_KILL_MONSTER_OVER_DAMAGE_CONST[5];
}

#include "CHackLog_OverDamage/GetBossTowerOverDamageHackType.inc"
{
    return static_cast<WongWork::ENUM_HACKTYPE>(0x85);
}

