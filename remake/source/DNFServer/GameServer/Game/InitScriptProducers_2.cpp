// ============================================================================
// InitScriptProducers_2.cpp —— CDataManager::init（ORIG 0x08358332..0x0835f1d5，
// 报告 docs/class_func_reports/CDataManager/init.md）依赖的脚本装载自由函数
// 生产 TU（第 2 批，InitScripts_2）。
//
// 每个函数均转写自 docs/class_func_reports/_lt_global_gt_/ 对应报告（objdump
// 汇编 + Ghidra C），统一模式：loadRDARScriptFile("",path) → ScanType 分派 →
// ScanInt/ScanStr/ScanFloat 字段回填。调用点（init.md 行号 / 目标成员）：
//   ImportReturnUserRewardScript 0x089180ea  init.md:1044  this+0xb548
//   ImportBingoScript            0x089187b9  init.md:1071  this+0xb568
//   ImportSecurityReward         0x089113ec  init.md:1123  this+0x50d8
//   ImportItemMakingSkill        0x0890f564  init.md:1146  this+0xb458
//   importGrowthPower            0x08911ee3  init.md:1796  this+0x8710
//   ImportWarAreaCounter         0x089026f3  init.md:1956  this+0x87b4
//   importEquipmentPartSetInfo   0x089073d9  init.md:4749  ("Etc/EquipmentPartSet.etc",3)
//   importPairPvPStatScript      0x08977d9c  init.md:4936  this+0x4df8
//
// 附带本批函数的被调依赖（全工程此前无定义，已广播认领）：
//   LoadRewardDataByGrowType 0x08917f44 / deleteTagBracketString 0x08917db3 /
//   getSameStringIndex 0x08990ed5 / characterEquipmentTypeTags D 0x093f4fc0 /
//   makeFairPVPEquipmentPackageKey 0x088945ad / g_equipmentPartSetMap_ B 0x094f7130 /
//   STEquipmentPartSetData C1 0x891b002·D1 0x08907348。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // 脚本类型权威定义（须先于 deps 头）
#include "sync_script.h"              // STEquipmentPartSetPiece 完整类型
#include "STEquipmentScript.h"        // STEquipmentScript/m_paramMap678/ImportEquipmentScript
#include "EquipmentParameterInfo.h"   // EquipmentParameterInfo copy/operator=
#include "DataManagerInitDepends.h"   // 本批声明唯一汇总点
#include "DNFLexWrapper.h"            // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"               // LogManager::logFormat

// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME —— ORIG D 0x093f4ee0 / 0x093f4ee8
// （各 [2] int，{0,0} 静态初始化）。ORIG 中与本 TU 的 g_equipmentPartSetMap_ /
// characterEquipmentTypeTags 同源文件且为文件内首个具名对象，决定
// `_GLOBAL__I_PVP_NORMAL_BATTLE_TIME` 静态初始化键名（c6root gcc4.4 探针验证）；
// 因此定义必须置于本文件首个具名对象位置，PvP_Room.cpp 仅 extern 引用。
// ============================================================================
int PVP_NORMAL_BATTLE_TIME[2] = {0, 0};
int PVP_DM_BATTLE_TIME[2] = {0, 0};

// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
const char* characterEquipmentTypeTags[26] = {
    "[hat avatar]",    "[hair avatar]",   "[face avatar]",    "[coat avatar]",
    "[pants avatar]",  "[shoes avatar]",  "[breast avatar]",  "[waist avatar]",
    "[skin avatar]",   "[aurora avatar]", "[weapon]",         "[title name]",
    "[coat]",          "[shoulder]",      "[pants]",          "[shoes]",
    "[waist]",         "[amulet]",        "[wrist]",          "[ring]",
    "[support]",       "[magic stone]",   "[creature]",
    "[artifact red]",  "[artifact blue]", "[artifact green]",
};

// ORIG B 0x094f7130（BSS，sizeof(std::map)=0x18）；init 尾段遍历回灌 DB。
std::map<int, STEquipmentPartSetData> g_equipmentPartSetMap_;

// ============================================================================
// STEquipmentPartSetData —— ORIG C1 0x0891b002 / D1 0x08907348
// ============================================================================
// b28c-L20: AE — ORIG C1 0x0891b002 orig=32 our=32 gap=0 ndiff=0。顺序 = 成员构造（+0x04 string、+0x0c vector）
// 先于标量赋值（+0x00=-1、+0x08=0）⇒ 标量必须写在函数体，不能进初始化列表
// （进列表会被 4.4 交错到成员构造之间，ndiff 19）。
STEquipmentPartSetData::STEquipmentPartSetData()
    : m_name4(), m_pieces()
{
    m_field0 = -1;
    m_script = 0;
}

// b28c-L20: AE — ORIG D1 0x08907348 orig=52 our=52 gap=0 ndiff=0：`m_script = 0` 在 if 之外无条件执行（P141：
// 保留无条件 delete，编译器自带判空守卫，ORIG 双 test/je 形）。
STEquipmentPartSetData::~STEquipmentPartSetData()
{
    if (m_script != 0)
        delete m_script;
    m_script = 0;
}

// ============================================================================
// makeFairPVPEquipmentPackageKey —— ORIG T 0x088945ad
// (short)a 落低 16 位、(short)b 落高 16 位后按 int 返回。
// ============================================================================
int makeFairPVPEquipmentPackageKey(int job, int idx)
{
    // ORIG 0x088945ad：单 int 槽 -0x4，`mov %ax,-0x4`（job 低16）→
    // `mov %ax,-0x2`（idx 高16）→ `mov -0x4,%eax` 整体回读。
    // 两次 16-bit store 到同一 32-bit 槽 ⇒ P180 union 视图逐半字写（移位/or 形
    // 被 4.4 生 movzwl+shl+or，int 局部赋 ushort 会走整 dword store 均不可）。
    union { int u; unsigned short v[2]; } key;
    key.v[0] = (unsigned short)job;
    key.v[1] = (unsigned short)idx;
    return key.u;
}

// ============================================================================
// getSameStringIndex —— ORIG T 0x08990ed5
// strcmp 线性查表，命中返回下标，未命中返回 -1。
// ============================================================================
int getSameStringIndex(const char* s, const char** table, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (std::strcmp(s, table[i]) == 0)
            return i;
    }
    return -1;
}

// deleteTagBracketString（ORIG T 0x08917db3）定义归 InitScriptProducers_1.cpp
// （InitScripts_1 认领），本 TU 经 DataManagerInitDepends.h 声明引用。

// ============================================================================
// LoadRewardDataByGrowType —— ORIG T 0x08917f44
// 扫描到 endTag 结束；每条：deleteTagBracketString→getCharacterJobToEnumValue
// （-1 → 0x3693），grow=ScanInt 必需；循环 level=ScanInt（失败→回到外层标签循
// 环）、data 两 int（失败→0x36a3/0x36a7）插入 out.m_map0。返回 0=正常结束。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 117）：
//   ①key/data 提到**函数头**构造一次（ORIG 0x08917f5b/0x08917f66，非每轮重构造）；
//   ②P164 纯 goto 循环复位头部条件块（0x08917f71），m_grow 失败与 ScanType 失败
//   共享尾部 `mov $0,%ebx` 块。
int LoadRewardDataByGrowType(stLevelUpRewardItemScript& out, std::string endTag)
{
    std::string token;
    bool ok = false;
    // ORIG 0x08917f5b/0x08917f66：key(-0x36)/data(-0x40) 在**函数头**各构造一次
    // （循环外复用），非每轮重构造。
    LevelUpRewardItemTableKey key;
    LevelUpRewardItemData data;

    // P164：4.4 把 while 旋转成尾部条件头；纯 goto 循环复位到 ORIG 的头部条件块
    // （0x08917f71），ScanType 失败与 m_grow 失败共享尾部 `mov $0,%ebx` 块。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == endTag)
            return 0;

        deleteTagBracketString(token);
        const int job = getCharacterJobToEnumValue(token);
        if (job == -1)
            return 0x3693;

        key.m_job = (unsigned char)job;                  // +0x02
        key.m_grow = (unsigned char)ScanInt(&ok);        // +0x03
        if (!ok)
            goto scan_done;

        while (true)
        {
            key.m_lv = (short)ScanInt(&ok);              // +0x00（b28c-L20: signed short）
            if (!ok)
                break;                                   // 回到外层标签扫描

            data.m_field0 = ScanInt(&ok);
            if (!ok)
                return 0x36a3;
            data.m_field4 = ScanInt(&ok);
            if (!ok)
                return 0x36a7;

            out.m_map0.insert(
                std::make_pair(key, data));
        }
    }
    goto scan_top;
scan_done:
    return 0;
}

// ============================================================================
// ImportReturnUserRewardScript —— ORIG T 0x089180ea（init.md:1044，
// "Event/ReturnUserReward.evt" → this+0xb548）。返回 0=成功，非 0=错误行。
// [pacakege item] 为 ORIG 原文拼写。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 93）：①单层
//   for(;;) + `goto scan_fail` 尾标签（P164/P223）复位 ORIG 头部条件块
//   （0x08918140）与尾部失败岛（0x08918235 nop; mov $0,%ebx）；②`[reward equip
//   item]` 用正形 == 分支（ORIG 0x089181a5 `test; je`，无 xor 物化）。
int ImportReturnUserRewardScript(const char* path, stReturnUserRewardScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x36b4;

    std::string token;
    // [b28d-47] ORIG 0x0891813b/+0x45：`movb $0x0,-0xd`（死 bool 局部，P232）
    // 与 `movl $0x0,-0xc`（ret）预清，声明序 flag→ret。
    bool flag = false;
    (void)flag;
    int ret = 0;
    out.m_levelUp.clear();

    // ORIG 0x089180ea 单层 for(;;)：三处回边（[pacakege item]/非 [reward equip item]/
    // ret==0）都回同一 ScanType 头；ScanType 失败落**尾部岛** `nop; mov $0,%ebx`
    // （P223 岛 + P164 尾标签），故写 goto 尾标签而非就地 return。
    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_fail;
        if (token == "[pacakege item]")
        {
            ScanInt(&out.m_field0);
            ScanInt(&out.m_field4);
            continue;
        }
        if (token == "[reward equip item]")
        {
            ret = LoadRewardDataByGrowType(out.m_levelUp, "[/reward equip item]");
            // ORIG 尾 `cmpl $0,-0xc; je <循环头>`（直跳）⇒ 写 break 形；`while(ret==0)`
            // 4.4 反产 `sete %al; test %al; jne` 物化（多 2 条）。
            if (ret != 0)
                break;
        }
    }
    return ret;
scan_fail:
    return 0;
}

// ============================================================================
// ImportBingoScript —— ORIG T 0x089187b9（init.md:1071，"Event/BingoEvent.evt"
// → this+0xb568）。ORIG 所有路径恒返回 0（含加载失败），忠实保留。
// 段：[board]→m_map18、[quiz value]→m_map0、[reward]→m_map30、[use item]→m_vec48。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82/N89 封顶（原 DIFF 311）ORIG
//   0x089187b9 orig/ours=337/338，P167 CLEAR（41/41）。本波修 p167 差集：
//   ①make_pair 全部去 const 左值（ORIG 3 实例 <int&,vector<int>&>/<int&,int&>/
//   <int&,pair<int,int>&>）；②补 `values.clear()`（ORIG 0x089188ad 显式调用）；
//   ③上限变量化 `int limit = 25`（ORIG -0x20 存 0x19，setl 形）；④P164 goto
//   主循环（0x08918800）；⑤count/ok 失败改尾岛 goto scan_done。残余：槽位/块
//   布局跨版本差。
// [P330 bool→int] 与 DataManagerInitDepends.h 声明同步（ORIG caller 4B store
// 无 movzbl ⇒ int）。ORIG 所有出口 ebx=0 ⇒ 两个 return 0。
int ImportBingoScript(const char* path, stBingoScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool ok = false;

    // P164：纯 goto 循环复位头部条件块（ORIG 0x08918800）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[board]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;                          // ORIG 0x08918c96 尾部岛
            for (int i = 0; i < count; )
            {
                // 非 const 左值 ⇒ ORIG make_pair<int&,vector<int>&>（p167）。
                int key = ScanInt(&ok);
                if (!ok)
                    break;                               // 直接回主循环，不插入

                std::vector<int> values;                 // 每行新建（ORIG ctor+clear）
                values.clear();                          // ORIG 0x089188ad 显式 clear
                int limit = 25;                          // ORIG 上限存槽 -0x20（setl 形）
                for (int j = 0; j < limit; )
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        break;
                    values.push_back(v);
                    ++j;
                }
                out.m_map18.insert(std::make_pair(key, values));
                ++i;
            }
            goto scan_top;
        }

        if (token == "[quiz value]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;
            for (int i = 0; i < count; )
            {
                int k = ScanInt(&ok);
                if (!ok)
                    goto scan_done;
                int v = ScanInt(&ok);
                if (!ok)
                    goto scan_done;
                out.m_map0.insert(std::make_pair(k, v));
                ++i;
            }
            goto scan_top;
        }

        if (token == "[reward]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;
            for (int i = 0; i < count; ++i)
            {
                std::pair<int, int> reward;              // 默认构造（ORIG pair()）
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                reward.first = ScanInt(&ok);
                if (!ok)
                    break;
                reward.second = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_map30.insert(std::make_pair(a, reward));
            }
            goto scan_top;
        }

        if (token == "[use item]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_vec48.push_back(v);
            }
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 0;
}

// ============================================================================
// ImportItemMakingSkill —— ORIG T 0x0890f564（init.md:1146，
// "Etc/ItemMakingSkill.etc" → this+0xb458）。返回 0=成功，非 0=错误行。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=IDENTICAL_AE（原 DIFF 196）：P164 纯
//   goto 主循环复位 ORIG 头部条件块（0x0890f5ab），四段 continue→`goto scan_top`；
//   内层 `while(true)` 各段与 ORIG 同形。
int ImportItemMakingSkill(const char* path, stItemMakingSkill& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x279e;

    std::string token;
    bool ok = false;

    // P164：4.4 把 while 旋转成尾部条件头；纯 goto 循环复位到 ORIG 头部条件块
    // （0x0890f5ab），ScanType 失败落尾部岛（0x0890f8b2 nop; mov $0,%ebx）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[itemmaking exp limit]")
        {
            while (true)
            {
                const int lv = ScanInt(&ok);
                if (!ok)
                    break;
                const int exp = ScanInt(&ok);
                if (!ok)
                    return 0x27b2;
                out.m_vec18.push_back(std::make_pair(lv, exp));
            }
            goto scan_top;
        }

        if (token == "[itemmaking exp]")
        {
            out.m_exp = ScanInt(&ok);
            if (!ok)
                return 0x27bb;
            out.m_bonusExp = ScanInt(&ok);
            if (!ok)
                return 0x27bf;
            goto scan_top;
        }

        if (token == "[itemmaking level limit]")
        {
            out.m_levelLimit = ScanInt(&ok);
            if (!ok)
                return 0x27c5;
            goto scan_top;
        }

        if (token == "[upgrade with compound]")
        {
            while (true)
            {
                stUpgradeWithCompound data;              // ctor 清零（ORIG clear）
                const int key = ScanInt(&ok);
                if (!ok)
                    break;

                data.m_field00 = ScanInt(&ok);
                if (!ok)
                    return 0x27d3;
                data.m_field04 = ScanInt(&ok);
                if (!ok)
                    return 0x27d7;
                data.m_field08 = ScanInt(&ok);
                if (!ok)
                    return 0x27db;
                data.m_rangeExp0C = (data.m_field04 - data.m_field00) * 1000 + 999;

                data.m_field10 = ScanInt(&ok);
                if (!ok)
                    return 0x27e1;
                data.m_field14 = ScanInt(&ok);
                if (!ok)
                    return 0x27e5;
                data.m_field18 = ScanInt(&ok);
                if (!ok)
                    return 0x27e9;
                data.m_rangeExp1C = (data.m_field14 - data.m_field10) * 1000 + 999;

                out.m_map0.insert(std::make_pair(key, data));
            }
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 0;
}

// ============================================================================
// importGrowthPower —— ORIG T 0x08911ee3（init.md:1796，
// "Etc/GrowthPowerRewardBuff.etc" → this+0x8710）。返回 bool（真=成功）。
// 段：[breakaway section]→m_vec0、[exp reward]→m_vecC、
//     [package reward]{[group]/[package selection]}→m_map18。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82/N112 封顶（原 DIFF）ORIG
//   0x08911ee3 orig/ours=269/268，P167 CLEAR（47/47）。本波已修真语义 bug：
//   ①[package selection] 内层用 `bool ScanInt(int*)`（ORIG `_Z7ScanIntPi`），
//   原写成 `int ScanInt(bool*)`+ok 槽；②`token != "[/package reward]"` 生成
//   operator!=（ORIG 只有 operator==，改 do+`if(==)break`）；③[exp reward]
//   m_reward 用字节视图（ORIG `mov %al,-0x21`，int 视图产 dword 写）。
//   残余：ORIG 把局部 STExpReward 落**非 4 对齐**槽 -0x21（4.1 按对齐 1 分配），
//   4.4 恒 4 对齐槽 -0x24（局部 `__attribute__((packed))` 实测无效）⇒ [exp] 段
//   槽号 +3 平移；属 N82/N112 槽分配族，语义等价。
bool importGrowthPower(char* path, STGrowthPowerData* out)
{
    if (out == 0)
        return false;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string group;

    // P164：4.4 把 while 旋转成尾部条件头；纯 goto 循环复位到 ORIG 头部条件块
    // （0x08911f38），ScanType 失败落尾部岛（0x08912244 nop; mov $1,%ebx）。
    // 每段循环内各有一个 bool ok 局部（ORIG 槽 -0x19/-0x1d 独立，且循环头预清）。
    // [package selection] 的内层扫描是 `bool ScanInt(int*)`（ORIG 0x08912136
    // `_Z7ScanIntPi`，返回值即成功标志），不是 `int ScanInt(bool*)`。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[breakaway section]")
        {
            while (true)
            {
                bool ok = false;
                STBreakAwaySection section;
                section.m_field0 = (char)ScanInt(&ok);
                if (!ok)
                    break;
                section.m_field1 = (char)ScanInt((bool*)0);
                section.m_field2 = (char)ScanInt((bool*)0);
                out->m_vec0.push_back(section);
            }
            goto scan_top;
        }

        if (token == "[exp reward]")
        {
            while (true)
            {
                bool ok = false;
                STExpReward reward;                      // ctor 仅清前 4 字节（同 ORIG）
                // ORIG 逐字节写入 +0x00..+0x03（mov %al）：
                //   +0x00=m_reward 低字节、+0x01=m_field4、+0x02=m_field5、
                //   +0x03=m_powerUp。
                reward.m_reward8 = (char)ScanInt(&ok);
                if (!ok)
                    break;
                reward.m_field4 = (char)ScanInt((bool*)0);
                reward.m_field5 = (char)ScanInt((bool*)0);
                reward.m_powerUp = (char)ScanInt((bool*)0);
                out->m_vecC.push_back(reward);
            }
            goto scan_top;
        }

        if (token == "[package reward]")
        {
            do
            {
                if (!ScanType(token, true))
                    goto scan_top;                       // EOF → 回主循环

                if (token == "[group]")
                {
                    if (!ScanStr(&group))
                        return false;

                    const int job = getCharacterJob(group.c_str());
                    const int n = ScanInt((bool*)0);
                    const int key = makeFairPVPEquipmentPackageKey(job, n);

                    std::vector<std::vector<STStackablePackageData> > groups;
                    ScanType(token, true);
                    while (token == "[package selection]")
                    {
                        int idx;
                        std::vector<STStackablePackageData> items;
                        while (true)
                        {
                            if (!ScanInt(&idx))
                                break;
                            STStackablePackageData d;
                            d.m_field0 = idx;
                            d.m_field4 = ScanInt((bool*)0);
                            items.push_back(d);
                        }
                        groups.push_back(items);
                        ScanType(token, true);
                    }
                    out->m_map18[key] = groups;
                }
                // ORIG 0x08912232 只有 operator==（无 operator!=）：`if (token ==
                // "[/package reward]") break;` 生 `test; je <循环头>` + fall 跳出。
                if (token == "[/package reward]")
                    break;
            } while (true);
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return true;
}

// ============================================================================
// ImportWarAreaCounter —— ORIG T 0x089026f3（init.md:1956，this+0x87b4，
// 固定路径 "Etc/WarArea.etc"）。返回 1=成功 / 0=失败。
// 注：ORIG 无下标越界检查（裸指针写入靠相邻区布局兜底）——照抄裸下标（P171）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82 封顶（原 DIFF 297）ORIG
//   0x089026f3 orig/ours=326/328，P167 CLEAR（32/32）。本波修真语义 bug：
//   ①删三处 ORIG 不存在的越界防御（P140/P171 照抄裸下标）；②[counter] 段复用
//   timeCount 作下标（ORIG 0x08902ae2 把 -0x30 存入 m_num 后清零复用），非独立
//   counter 局部；③doorCount/dungeonIdx 为**块内**局部（ORIG -0x20/-0x18）；
//   ④-0x34 为 ORIG 死局部；⑤maxValue 先于 maxIndex 赋值；⑥make_pair<int&,
//   stWarPoint&>（key 去 const，p167 归零）。残余：全帧槽位 +4 平移（N95/N82）
//   + do-while 尾测 setne 物化（4.4 恒物化，ORIG 直跳 jne）。
bool ImportWarAreaCounter(WarAreaCounter* out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    if (!loadRDARScriptFile("", "Etc/WarArea.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xdeb,
                              "CRITICAL ERROR : \xc0\xef\xbf\xaa "
                              "\xc4\xab\xbf\xee\xc5\xcd "
                              "\xbd\xba\xc5\xa9\xb8\xb3\xc6\xae "
                              "\xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return 0;
    }

    std::string token;
    int deadLocal = 0;                     // -0x34：ORIG 初始化后从不读（P171 死局部）
    (void)deadLocal;
    int timeCount = 0;                     // -0x30（[counter] 段复用为下标）
    bool ok = false;                       // -0x69
    int maxValue = 0;                      // -0x2c
    int maxIndex = -1;                     // -0x28

    // P164：纯 goto 循环复位头部条件块（ORIG 0x0890278c），ScanType 失败落
    // 尾部岛（0x08902b54 nop; mov $1,%ebx）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[time table]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);              // -0x24
                if (!ok)
                    goto scan_top;
                out->m_timeHourList[timeCount] = v;      // ORIG 裸下标 this+(i+0x18)*4
                ++timeCount;
            }
        }

        if (token == "[door fee]")
        {
            int doorCount = 0;                           // -0x20（块内局部）
            while (true)
            {
                const int v = ScanInt(&ok);              // -0x1c
                if (!ok)
                    goto scan_top;
                out->m_doorFee[doorCount] = v;           // ORIG 裸下标 this+(i+0x30)*4
                ++doorCount;
            }
        }

        if (token == "[ban interval]")
        {
            out->m_banInterval = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[kick point]")
        {
            out->m_kickPoint = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[ready count]")
        {
            out->m_readyCount = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[wp penalty]")
        {
            out->m_wpPenalty = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[monster exp const]")
        {
            out->m_monsterExpConst = ScanFloat(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[wararea dungeon]")
        {
            int dungeonIdx = 2;                          // -0x18（块内局部，ORIG 从 2 起）
            while (true)
            {
                const int v = ScanInt(&ok);              // -0x14
                if (!ok)
                    goto scan_top;
                out->m_warAreaDungeon[dungeonIdx] = v;   // ORIG 裸下标
                ++dungeonIdx;
            }
        }

        if (token == "[wp per monster hp]")
        {
            while (true)
            {
                stWarPoint wp;                           // -0x7c，ctor memset 0x10
                // 非 const 左值：ORIG 实例为 make_pair<int&,stWarPoint&>
                // （p167 MISSING `IRiR10stWarPoint`；const 版落 `<IRKiR...>`）。
                int key = ScanInt(&ok);                  // -0x80
                if (!ok)
                    goto scan_top;                       // 回主循环

                for (int j = 0; j < 4; ++j)              // -0x10
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        return 0;
                    wp.m_pts[j] = v;
                }
                out->m_wpPerMonsterHp.insert(
                    std::make_pair(key, wp));            // greater<int> 键序
            }
        }

        if (token == "[counter]")
        {
            out->m_num = timeCount;                      // +0x140
            timeCount = 0;                               // 复用为 counter（ORIG -0x30）
            do
            {
                const int v = ScanInt(&ok);              // -0xc
                if (!ok)
                    break;
                if (v > maxValue)
                {
                    maxValue = v;
                    maxIndex = timeCount;
                }
                out->m_counterTable[timeCount] = v;      // ORIG 裸下标 this+i*4
                ++timeCount;
            } while (out->m_num != timeCount);
            out->m_curHourIdx = maxIndex;                // ORIG 写 +0xe8
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 1;
}

// ============================================================================
// importPairPvPStatScript —— ORIG T 0x08977d9c（init.md:4936，
// this+0x4df8 ← "Etc/FairPvPStat.etc"）。恒返回 0（ORIG 全路径 ebx=0）。
// [grow type index] 以 (job,grow) 为键在 m_map0 插入清零 CharacterStatus，
// 后续段直接写该值字段；[attack speed]/[jump speed] 无 cur 判空（ORIG 缺失，
// 引用恒非空故等价）。[jump speed] 读后丢弃（ORIG 同）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N89/N82 封顶（原 DIFF 524）ORIG
//   0x08977d9c orig/ours=569/496，P167 CLEAR（56/56，§6.2 三件套①）。本波修：
//   insert 结果用 `pair<const_iterator,bool>`（ORIG 转换构造
//   `C1ISt17_Rb_tree_iteratorIS4_EbEEOS_IT_T0_E` + const_iterator ptEv；
//   `const_cast` 取 current）。残余：STL 内联实例差（N89）+ 超大件槽差（N82）。
// [BigEquipScript pass-2 2026-09-09] orig/ours=569/551（gap 73→18）：
//   ①判空下沉：ORIG 17 个数值段各自 `cmpl $0,-0xc(%ebp); jne; mov $0,%ebx;
//     jmp`（[HP MAX] 0x08977fcb … [water resistance] 0x0897860f 逐段实证；
//     [attack speed]/[jump speed] 两段无判空，P171 照抄），原写成链前单一判空；
//   ②ScanType 失败返回当前 ok 值（ORIG 0x0897864d movzbl -0xc9(%ebp),%ebx），
//     非恒 false；③段序按 ORIG（inventory limit/MP regen/move speed/attack/
//     cast/hit/jump power/jump speed/weight/dark/light/fire/water）重排并补回
//     被误覆盖的 [weight] 段；④CDataManager.h CharacterStatus 的 m_field00/
//     m_field04/m_field44 int→unsigned（ORIG [HP MAX] 0x08977fe9 / [MP MAX]
//     0x08978044 / [move speed] 0x089782cb 三段 store 为 RHS-first 中转
//     `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,disp`，而同函数 +0x3c
//     [inventory limit] 0x08978214 / +0x50 [weight] 0x089784b8 为 LHS-first
//     ⇒ P211 判异型；同尺寸同偏移、sizeof 0x54/static_assert 不变；三消费 TU
//     CUser/CDataManagerScripts/InitScriptProducers_2 四数 A/B 实测零回退）。
//   残余 18 = ORIG 19 个 continue 跳板 `nop; jmp loop`（4.1 每个 continue 发
//   3 字节跳板；4.4 jump-threading 合并为共享 jmp，P223 两种 goto 形实测均
//   -1/-2 反劣）⇒ 封 N（跨版本 continue 代码生成差）。
bool importPairPvPStatScript(FairPvPStatScript& script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script.clear();

    std::string token;
    bool ok = false;
    CharacterKind kind;
    CharacterStatus* current = 0;

    for (;;)
    {
        // ORIG 失败路径返回当前 ok 值（0897864c movzbl -0xc9(%ebp),%ebx），
        // 非恒 false（我方旧形 DIFF 1 条 movzbl）。
        if (!ScanType(token, true))
            return ok;

        if (token == "[job index]")
        {
            kind.m_job = (char)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }

        if (token == "[grow type index]")
        {
            kind.m_grow = (char)ScanInt(&ok);
            if (!ok)
                return false;

            CharacterStatus status;
            std::memset(&status, 0, sizeof(status));     // ORIG memset 0x54
            // ORIG 用 const_iterator 版 pair（p167 MISSING 转换构造
            // `C1ISt17_Rb_tree_iteratorIS4_EbEEOS_IT_T0_E` + const_iterator ptEv）。
            const std::pair<std::map<CharacterKind, CharacterStatus>::const_iterator,
                            bool> inserted =
                script.m_map0.insert(std::make_pair(kind, status));
            if (!inserted.second)
                return false;
            current = const_cast<CharacterStatus*>(&inserted.first->second);
            continue;
        }

        if (token == "[HP MAX]")
        {
            if (current == 0)
                return false;
            current->m_field00 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[MP MAX]")
        {
            if (current == 0)
                return false;
            current->m_field04 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[physical attack]")
        {
            if (current == 0)
                return false;
            current->m_field08[0] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[physical defense]")
        {
            if (current == 0)
                return false;
            current->m_field08[1] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[magical attack]")
        {
            if (current == 0)
                return false;
            current->m_field08[2] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[magical defense]")
        {
            if (current == 0)
                return false;
            current->m_field08[3] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[inventory limit]")
        {
            if (current == 0)
                return false;
            current->m_field3c = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[MP regen speed]")
        {
            if (current == 0)
                return false;
            current->m_field42 = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[move speed]")
        {
            if (current == 0)
                return false;
            current->m_field44 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[attack speed]")                   // ORIG 此段无判空
        {
            current->m_field48 = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[cast speed]")
        {
            if (current == 0)
                return false;
            current->m_field4a = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[hit recovery]")
        {
            if (current == 0)
                return false;
            current->m_field4c = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[jump power]")
        {
            if (current == 0)
                return false;
            current->m_field4e = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[jump speed]")                     // ORIG 读后丢弃
        {
            ScanInt(&ok);
            continue;
        }
        if (token == "[weight]")
        {
            if (current == 0)
                return false;
            current->m_field50 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[dark resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[6] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[light resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[7] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[fire resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[4] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[water resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[5] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
    }
    return false;
}

// ============================================================================
// ImportSecurityReward —— ORIG T 0x089113ec（init.md:1123，this+0x50d8）。
// 三段各 4 个 int；返回 bool（真=成功）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N1 封顶（原 DIFF 102）ORIG
//   0x089113ec orig/ours=125/120，P167 CLEAR（22/22）。本波修真语义 bug：
//   ①ORIG 用 `bool ScanInt(int*)`（`_Z7ScanIntPi`）**语句调用**写字段（每段 4 次
//   展开，非 for 循环），原写成 `int ScanInt(bool*)`+ok 槽+循环；②P164 goto 循环。
//   残余：5 处地址 `add $8; add $4` 两步形（4.1 常量乘法展开），4.4 恒折单条
//   `add $0xc`（探针 /tmp/probe2d.cpp g2d/g3/g4/g5/g6 全折）⇒ N1 跨版本常量算术。
bool ImportSecurityReward(SecurityReward* out)
{
    if (!loadRDARScriptFile("", "Etc/SecurityReward.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x2c36,
                              "CRITICAL ERROR : SecurityReward faild!");
        return false;
    }

    std::string token;

    // ORIG 0x089113ec：每段 4 次**展开**的 `_Z7ScanIntPi`（bool ScanInt(int*)）
    // 语句调用（返回值丢弃，P152/P171），非 for 循环；地址形 `add $8; add $4`
    // 即 4.1 对 arr[3] 的两步常量算术。
    // P164：4.4 会把 `while(cond)` 旋转成尾部条件头；用纯 goto 循环把条件头
    // 复位到前部（ORIG 08911453），失败落尾部岛（0891157a nop; mov $1,%ebx）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[GoblinPad Reward]")
        {
            ScanInt(&out->m_goblinPad[0]);
            ScanInt(&out->m_goblinPad[1]);
            ScanInt(&out->m_goblinPad[2]);
            ScanInt(&out->m_goblinPad[3]);
        }
        else if (token == "[SecurityCard Reward]")
        {
            ScanInt(&out->m_securityCard[0]);
            ScanInt(&out->m_securityCard[1]);
            ScanInt(&out->m_securityCard[2]);
            ScanInt(&out->m_securityCard[3]);
        }
        else if (token == "[OTP Reward]")
        {
            ScanInt(&out->m_otp[0]);
            ScanInt(&out->m_otp[1]);
            ScanInt(&out->m_otp[2]);
            ScanInt(&out->m_otp[3]);
        }
    }
    goto scan_top;
scan_done:
    return true;
}

// ============================================================================
// importEquipmentPartSetInfo —— ORIG T 0x089073d9（init.md:4749，
// "Etc/EquipmentPartSet.etc", ENUM_MODULE_TYPE 3）。返回 bool（真=成功）。
// 解析 [equipment part set] → g_equipmentPartSetMap_；随后逐集合
// new STEquipmentScript 并 ImportEquipmentScript(name,false,module,key)；
// 成功后把该脚本 m_paramMap678[-1]（存在时）迁移为键=pieces.size() 的条目。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N89 封顶（原 DIFF 336）ORIG
//   0x089073d9 orig/ours=374/349，p167 仅剩 `_ZNKSt17_Rb_tree_iterator<
//   pair<const int,STEquipmentPartSetData>>::ptEv` ×7（4.4 内联头内 operator->，
//   4.1 out-of-line；N89 类不可控）。本波修：①operator== 正形（去 operator!=）；
//   ②`data.m_field0 = ScanInt((bool*)0)` 去独立 key 局部；③do-while 复用
//   ScanStr 返回值（去重复调用）；④`paramMap[-1]` 替代 `def->second`（补
//   map::operator[] 调用点）；⑤P164 goto 主循环。
bool importEquipmentPartSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x168b,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 "
                              "\xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 "
                              "\xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.",
                              path);
        return false;
    }

    std::string token;
    std::string name;
    std::string pieceName;
    std::string tag;
    bool ok = false;

    g_equipmentPartSetMap_.clear();

    // P164：主循环 goto 形复位头部条件块（ORIG 0x08907477）；ScanType 失败落
    // scan_done（直接进入下方 for 循环，非 return）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        // ORIG 0x0890749d 正向 `if (token == X)` 形（用 operator==，无 operator!=）。
        if (token == "[equipment part set]")
        {
            STEquipmentPartSetData data;
            // ORIG 直接 `data+0 = ScanInt(0)`（无独立 key 局部；键取 data.m_field0）。
            data.m_field0 = ScanInt((bool*)0);
            ScanStr(&name);
            data.m_name4 = name;

            do
            {
                STEquipmentPartSetPiece piece;           // ctor 三 int=-1
                ok = ScanStr(&pieceName);                // 复用返回值作 do-while 条件
                if (ok)
                {
                    piece.m_name = pieceName;
                    ScanStr(&tag);
                    piece.m_field4 = getSameStringIndex(tag.c_str(),
                                                        characterEquipmentTypeTags,
                                                        0x1a);
                    piece.m_field8 = ScanInt((bool*)0);
                    piece.m_fieldc = ScanInt((bool*)0);
                    data.m_pieces.push_back(piece);

                    g_equipmentPartSetMap_[data.m_field0] = data;
                }
            } while (ok);
        }
    }
    goto scan_top;
scan_done:

    // ---- 逐集合装载装备脚本并合并 [-1] 参数条目 ----
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentPartSetMap_.begin();
         it != g_equipmentPartSetMap_.end(); ++it)
    {
        STEquipmentScript* script = new STEquipmentScript;   // ORIG 0x894 字节
        it->second.m_script = script;

        if (!ImportEquipmentScript(script, it->second.m_name4.c_str(), false,
                                   moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  __FUNCTION__, 0x16c2,
                                  "ERROR : \xec\x9e\xa5\xeb\xb9\x84\xeb\xb6\x80"
                                  "\xec\x9c\x84\xec\x85\x8b\xed\x8a\xb8 \xea\xb5"
                                  "\xac\xec\x84\xb1\xed\x92\x88 %s \xed\x8c\x8c"
                                  "\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d"
                                  "\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb"
                                  "\x8b\x88\xeb\x8b\xa4.",
                                  it->second.m_name4.c_str());
            delete script;
            return false;
        }

        std::map<int, EquipmentParameterInfo>& paramMap = script->m_paramMap678;
        std::map<int, EquipmentParameterInfo>::iterator def =
            paramMap.find(-1);
        if (def != paramMap.end())
        {
            // ORIG 用 `paramMap[-1]` 取引用再拷贝构造（p167 MISSING ixERS4_），
            // 非 `def->second`（后者多一次 EquipmentParameterInfo 迭代器 ptEv）。
            const EquipmentParameterInfo merged(paramMap[-1]);
            paramMap.clear();
            paramMap[it->second.m_pieces.size()] = merged;
        }
    }
    return true;
}
