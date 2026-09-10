// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScriptDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ScriptDepsB.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"           // stTitleBookInfo（m_map18 权威布局）
#include "CEnvironment.h"           // less_TCHAR / SpinLock
#include "CItemUpgrade_Separate.h"  // GenuineDamageUpgradeTable 及元素 POD
#include "CTitleBook.h"             // stTitleSection / stTitleRewardBasis 族
#include "DNFLexWrapper.h"          // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"

// ============================================================================
// Etc/EtcParameter.etc 常量定义（ORIG BSS：g_physicalDamageRateConst_ 等，
// nm 实测 B 0x093f5d10..0x093f5d83；本 TU 为唯一定义点）
// ============================================================================
float g_physicalDamageRateConst_;
float g_physicalDamageAddConst_;
float g_physicalReduceRateConst_;
float g_physicalReduceAddConst_;
float g_physicalRandomBaseConst_;
float g_physicalRandomRateConst_;
float g_magicalDamageRateConst_;
float g_magicalDamageAddConst_;
float g_magicalReduceRateConst_;
float g_magicalReduceAddConst_;
float g_magicalRandomBaseConst_;
float g_magicalRandomRateConst_;
float g_commonElementBaseConst_;
float g_commonElementRateConst_;
float g_commonElementRateConstInPVP_;
float g_commonElementRateConstInLadderPVP_;
float g_hpIncreaseRateConst_;
float g_hpRegenRateConst_;
float g_mpIncreaseRateConst_;
float g_mpRegenRateConst_;
float g_activeStatusDamageRateConst_;
float g_activeStatusDefenseRateConst_;
float g_physicalDefenseAddConst_;
float g_magicalDefenseAddConst_;
float g_hpRegenRateAddRateConst_;
float g_mpRegenRateAddRateConst_;

// ============================================================================
// loadValueType —— ORIG 0x08892ac4（_Z13loadValueTypeRKSs）
// "+"=0、"%"=1、其他=2。
// ============================================================================
#include "ScriptDepsB/loadValueType.inc"
{
    if (token == "+")
        return 0;
    if (token == "%")
        return 1;
    return 2;
}

// ============================================================================
// loadAppendageType —— ORIG 0x0889253b（_Z17loadAppendageTypeRKSs）
// 纯字符串→枚举映射链；比较顺序与 ORIG 级联完全一致，未匹配返回 -1。
// ============================================================================
#include "ScriptDepsB/loadAppendageType.inc"
{
    if (token == "[physical attack]")                  return 0x00;
    if (token == "[magical attack]")                   return 0x01;
    if (token == "[physical defense]")                 return 0x03;
    if (token == "[magical defense]")                  return 0x02;
    if (token == "[equipment physical attack]")        return 0x04;
    if (token == "[equipment physical defense]")       return 0x05;
    if (token == "[jump power]")                       return 0x08;
    if (token == "[attack speed]")                     return 0x0a;
    if (token == "[move speed]")                       return 0x0b;
    if (token == "[cast speed]")                       return 0x0c;
    if (token == "[hp max]")                           return 0x0d;
    if (token == "[hp regen rate]")                    return 0x0e;
    if (token == "[magical critical hit rate]")        return 0x10;
    if (token == "[physical critical hit rate]")       return 0x0f;
    if (token == "[physical back attack critical rate]") return 0x11;
    if (token == "[magical back attack critical rate]")  return 0x12;
    if (token == "[fire element]")                     return 0x14;
    if (token == "[water element]")                    return 0x15;
    if (token == "[dark element]")                     return 0x16;
    if (token == "[light element]")                    return 0x17;
    if (token == "[stuck]")                            return 0x1b;
    if (token == "[mp regen rate]")                    return 0x1d;
    if (token == "[hit recovery]")                     return 0x1e;
    if (token == "[equipment magical attack]")         return 0x1f;
    if (token == "[equipment magical defense]")        return 0x20;
    if (token == "[good hit]")                         return 0x21;
    if (token == "[all active status resistance]")     return 0x1a;
    if (token == "[all element]")                      return 0x18;
    if (token == "[fire element attack]")              return 0x2a;
    if (token == "[water element attack]")             return 0x2b;
    if (token == "[dark element attack]")              return 0x2c;
    if (token == "[light element attack]")             return 0x2d;
    if (token == "[all element attack]")               return 0x2e;
    if (token == "[mp max]")                           return 0x30;
    if (token == "[separate attack]")                  return 0x35;
    if (token == "[antievil]")                         return 0x37;
    if (token == "[physical absolute damage]")         return 0x38;
    if (token == "[village moving speed]")             return 0x3f;
    if (token == "[magical absolute damage]")          return 0x3c;
    if (token == "[physical absolute defense]")        return 0x3d;
    if (token == "[magical absolute defense]")         return 0x3e;
    if (token == "[physical attack bonus]")            return 0x06;
    if (token == "[magical attack bonus]")             return 0x07;
    return -1;
}

// ============================================================================
// importEtcParameterScriptForServer —— ORIG 0x080966fa
// 报告 _lt_global_gt_/importEtcParameterScriptForServer.md：
//   loadRDARScriptFile("", "Etc/EtcParameter.etc") 失败→false；
//   do { ScanType(token,false); } while (token != "[damage fomula constant]");
//   随后按序 ScanFloat 写入 26 个 g_*Const_ 全局 → true。
//   （ORIG 另构造一个未使用局部 string local_20，无语义，略。）
// ============================================================================
// [b28d-solo-ScriptDepsB AE: 补 ORIG 两枚 string + int/float/bool 死局部（P232/P171
//  照抄）：ctor 序 -0x18→-0x1c、清零序 -0x14(int 单段)/-0x10(float 两段 eax 中转)/-0x9(bool)，
//  目标串以立即数压参。ORIG 0x080966fa]
#include "ScriptDepsB/importEtcParameterScriptForServer.inc"
{
    if (!loadRDARScriptFile("", "Etc/EtcParameter.etc"))
        return false;

    std::string token;
    std::string unused;
    int deadInt = 0;
    float deadFloat = 0.0f;
    bool deadBool = false;

    for (;;)
    {
        ScanType(token, false);
        if (token == "[damage fomula constant]")
            break;
    }

    g_physicalDamageRateConst_            = ScanFloat((bool*)0);
    g_physicalDamageAddConst_             = ScanFloat((bool*)0);
    g_physicalReduceRateConst_            = ScanFloat((bool*)0);
    g_physicalReduceAddConst_             = ScanFloat((bool*)0);
    g_physicalRandomBaseConst_            = ScanFloat((bool*)0);
    g_physicalRandomRateConst_            = ScanFloat((bool*)0);
    g_magicalDamageRateConst_             = ScanFloat((bool*)0);
    g_magicalDamageAddConst_              = ScanFloat((bool*)0);
    g_magicalReduceRateConst_             = ScanFloat((bool*)0);
    g_magicalReduceAddConst_              = ScanFloat((bool*)0);
    g_magicalRandomBaseConst_             = ScanFloat((bool*)0);
    g_magicalRandomRateConst_             = ScanFloat((bool*)0);
    g_commonElementBaseConst_             = ScanFloat((bool*)0);
    g_commonElementRateConst_             = ScanFloat((bool*)0);
    g_commonElementRateConstInPVP_        = ScanFloat((bool*)0);
    g_commonElementRateConstInLadderPVP_  = ScanFloat((bool*)0);
    g_hpIncreaseRateConst_                = ScanFloat((bool*)0);
    g_hpRegenRateConst_                   = ScanFloat((bool*)0);
    g_mpIncreaseRateConst_                = ScanFloat((bool*)0);
    g_mpRegenRateConst_                   = ScanFloat((bool*)0);
    g_activeStatusDamageRateConst_        = ScanFloat((bool*)0);
    g_activeStatusDefenseRateConst_       = ScanFloat((bool*)0);
    g_physicalDefenseAddConst_            = ScanFloat((bool*)0);
    g_magicalDefenseAddConst_             = ScanFloat((bool*)0);
    g_hpRegenRateAddRateConst_            = ScanFloat((bool*)0);
    g_mpRegenRateAddRateConst_            = ScanFloat((bool*)0);
    return true;
}

// ============================================================================
// getTitleCollection —— ORIG 0x08aa94e1（_Z18getTitleCollectionP15stTitleBookInfoi）
// find(tab)；== end → 返回 0；否则返回 &m_map18[tab]。
// ============================================================================
std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab)
{
    // b28c-l23 根因（P0/P183）：ORIG 无 map 引用局部槽——三处成员访问都是
    // `mov 0x8(%ebp),%eax; lea 0x18(%eax),%eX` 就地折叠寻址（@0x08aa94f1/0x08aa9508/
    // 0x08aa953b）；我方具名 `collection` 引用使 4.4 把基址落槽（-0xc）并在每次
    // 实参装载时回读，槽号整体平移（-0x10→-0x14、-0xc→-0x10）。改为逐点直写表达式。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        book->m_map18.find(tab);
    if (it == book->m_map18.end())
        return 0;
    return &book->m_map18[tab];
}

// ============================================================================
// importRewards —— ORIG 0x08aa8b79
// 报告 _lt_global_gt_/importRewards.md。appendage 命中→stRewardChangeStatus；
// 否则 [give emotion]→stRewardGiveEmotion；[send postal]→stRewardPostal；
// 其余令牌无副作用。恒返回 true（eax 流出）。
// 扫描顺序逐条对齐汇编：change=status 先存 appendage 再 Str/ValueType/Float；
// postal 先两个 int(+0x18/+0x1c) 后三个 string(+0xc/+0x10/+0x14)。
// ============================================================================
// [b28d-solo-ScriptDepsB AE-语义 / N(P197④) 形态封顶 | ORIG 0x08aa8b79]
//  修法：else-if 链 + 单一 return 1（typeText 公共尾块析构）；appendage 异型化
//  （unsigned）翻 store RHS-first（P211）；postal 分支照抄 ORIG -0x20 预清死局部。
//  首条件用 while+goto 复现 ORIG 槽比物化（P225 实证）：4.4 对 `if(槽!=K)` 恒
//  直跳（je 1 条），对 **while 条件**才发 `cmpl $K,槽; setne; test; je` 同形；
//  body 末尾 goto 公共尾（等价 break 后跳尾）⇒ 与 ORIG 0x08aa8ba4/0x08aa8c10 逐条
//  一致（探针 /tmp/sdb_probe/p1.o f_while_goto）。
//  D1 虚调目标（旧 N7）已由 CTitleBook.h 指针算术形修复。
#include "ScriptDepsB/importRewards.inc"
{
    std::string typeText;
    const unsigned int appendage =
        static_cast<unsigned int>(loadAppendageType(token));

    // ORIG 0x08aa8ba4: cmpl $-1,槽; setne %al; test %al,%al; je <else>
    while (appendage != static_cast<unsigned int>(-1))
    {
        stRewardChangeStatus* reward = new stRewardChangeStatus();
        reward->m_appendage = appendage;                    // +0x0c
        ScanStr(&typeText);
        reward->m_valueType = loadValueType(typeText);      // +0x10
        reward->m_value = ScanFloat((bool*)0);              // +0x14（fstps）
        out->push_back(reward);
        goto rewardsTail;                                   // ORIG 0x08aa8c10 jmp 尾
    }
    if (token == "[give emotion]")
    {
        stRewardGiveEmotion* reward = new stRewardGiveEmotion();
        reward->m_emotionId = ScanInt((bool*)0);            // +0x0c
        out->push_back(reward);
    }
    else if (token == "[send postal]")
    {
        int deadPostal = 0;
        stRewardPostal* reward = new stRewardPostal();
        reward->m_itemIdx = ScanInt((bool*)0);              // +0x18
        reward->m_addInfo = ScanInt((bool*)0);              // +0x1c
        ScanStr(&reward->m_sender);                         // +0x0c
        ScanStr(&reward->m_subject);                        // +0x10
        ScanStr(&reward->m_message);                        // +0x14
        out->push_back(reward);
    }
rewardsTail:
    return 1;
}

// ============================================================================
// importTitleSection —— ORIG 0x08aa8d65
// 报告 _lt_global_gt_/importTitleSection.md。三个函数级静态（ORIG 经
// __cxa_guard 初始化）：__lookup 键表 / __init_lock 自旋锁 / __inited 标志。
// 首个令牌触发级联登记（[/section]0x155→[/reward type]0x158→[section name]
// 0x15c→[section icon]0x15f→[frame type]0x166→[lock message]0x169→[name]
// 0x16e→[image path]0x172→[effect path]0x179），置位 __inited 后以当前令牌
// 重新查表派发。查表命中标签则解析对应字段；未命中即奖励行 → importRewards。
// 返回：ScanType 耗尽→true；[/reward type]→true；[/section]→false。
// ============================================================================
// [b28d-solo-ScriptDepsB: switch 驱动级联注册链（P177 static 三件套落循环体内、
//  P163 case 0 撑宽、P192/P315 value_type 直构、P96/P270 const_iterator）；
//  查表段单点共享。ORIG 0x08aa8d65]
#include "ScriptDepsB/importTitleSection.inc"
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    std::string deadToken;          // ORIG -0xec（仅 ctor/dtor，P171 照抄）

    section->m_startIdx = idx * 10;                    // +0x0c
    section->m_endIdx = section->m_startIdx + 10;      // +0x10

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

    dispatch:                       // ORIG +0x72：重派发当前令牌（不重扫）
        static LookupMap __lookup;      // ORIG _ZZ18importTitleSection..8__lookup
        static SpinLock __init_lock;     // ORIG ..11__init_lock
        static bool __inited = false;   // ORIG ..8__inited

        int id;
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                // 双检竞争失败方：解锁后重走查表（当前令牌，不重扫）。
                __init_lock.leave();
                goto dispatch;
            }
            id = 0;                     // 首遍：id=0 驱动级联注册链
        }

        switch (id)
        {
        case 0:
        case 0x155:                                    // [/section]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/section]", 0x155));
            else
                return false;
            // fall through（注册链）
        case 0x158:                                    // [/reward type]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/reward type]", 0x158));
            else
                return true;
            // fall through
        case 0x15c:                                    // [section name]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[section name]", 0x15c));
            else
            {
                ScanStr(&section->m_sectionName);       // +0x04
                break;
            }
            // fall through
        case 0x15f:                                    // [section icon]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[section icon]", 0x15f));
            else
            {
                ScanStr(&section->m_icon);              // +0x24
                section->m_iconId = ScanInt((bool*)0);  // +0x28
                break;
            }
            // fall through
        case 0x166:                                    // [frame type]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[frame type]", 0x166));
            else
            {
                section->m_frameType = ScanInt((bool*)0);  // +0x2c
                break;
            }
            // fall through
        case 0x169:                                    // [lock message]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[lock message]", 0x169));
            else
            {
                ScanStr(&section->m_lockMessage);       // +0x30
                break;
            }
            // fall through
        case 0x16e:                                    // [name]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[name]", 0x16e));
            else
            {
                ScanStr(&section->m_name);              // +0x14
                break;
            }
            // fall through
        case 0x172:                                    // [image path]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[image path]", 0x172));
            else
            {
                ScanStr(&section->m_imagePath);         // +0x18
                section->m_imageParam = ScanInt((bool*)0);  // +0x1c
                break;
            }
            // fall through
        case 0x179:                                    // [effect path]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[effect path]", 0x179));
            else
            {
                ScanStr(&section->m_effectPath);        // +0x20
                break;
            }
            // fall through（注册链尾）
        default:
            break;
        }

        // ORIG +0x6a3 公共尾：注册链上 leave+置位并回循环头重派发；
        // __inited 后每个令牌（含标签）都会进入 importRewards。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto dispatch;
        }

        importRewards(&section->m_rewards, token);
    }
}

// ============================================================================
// importSpPerLevelReferenceTable —— ORIG 0x08910505
// 报告 _lt_global_gt_/importSpPerLevelReferenceTable.md。
// stSpPerLevelTable 在 ORIG 中为空壳派生（W ctor/dtor 与 vector<unsigned> 一致，
// sizeof 0xc）；此处与 CDataManagerMethods.cpp 保持逐 token 相同的局部定义以
// 产生一致的 mangled 形参 R17stSpPerLevelTable。
// 语义：loadRDARScriptFile("", path) 失败→0x2948；ScanType 循环中命中
// "[sp table]" 段则 reserve(200)、先压入 0 号项，随后每行 ScanInt 两枚——
// 仅第二枚入表（首枚为等级位），任一枚失败回到外层令牌循环。成功返回 0。
// ============================================================================
struct stSpPerLevelTable : public std::vector<unsigned int> {};

// [b28d-solo-ScriptDepsB AE: int 局部 + 隐式转换物化右值 push_back(EOj) 对齐
//  ORIG 0x08910505；等级位与数值位均为 int 具名局部（-0x10/-0xc），
//  push_back 传 int 触发 int→unsigned 转换临时（-0x18/-0x14）⇒ 右值重载 EOj]
#include "ScriptDepsB/importSpPerLevelReferenceTable.inc"
{
    bool ok = false;
    std::string token;
    int level;
    int value;

    if (!loadRDARScriptFile("", path))
        return 0x2948;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[sp table]")
        {
            level = 0;
            value = 0;
            table.reserve(200);
            table.push_back(value);                    // 等级 0 占位（int→unsigned 右值）
            for (;;)
            {
                level = ScanInt(&ok);                  // 等级位（丢弃，索引即位）
                if (!ok)
                    break;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                table.push_back(value);                // 仅数值位入表
            }
        }
    }
    return 0;
}

// ============================================================================
// importGenuineUpgradeTable —— ORIG 0x08912336（../RDARScriptEtcParameter.cpp）
// 报告 _lt_global_gt_/importGenuineUpgradeTable.md。节分派：
//   [table]                    m_infos 清空后逐行 {int,int,int,level/10000f,float}
//                              以自增键插入 +0x158；
//   [separate upgrade max]     ScanInt(+0x004)，失败 logFormat(0x2f9e)；
//   [level]                    ScanInt(+0x000)，失败 logFormat(0x2fa5)；
//   [item weights by grade]    {key,int,int} 插入 +0x140；
//   [item weights by rarity]   6×float → +0x008 区（失败 logFormat(0x2fbe) 续扫）；
//   [stat weights by rarity]   6×float → +0x020 区（logFormat(0x2fc9)）；
//   [weapon weights]           11×6 float → +0x038 区（logFormat(0x2fd5)）；
//   [separate upgrade effect]  行循环 {r,g,b}→RGBA 打包入 +0x04，再
//                              {+0xc,+8,+0} 三 int，自增键插入 +0x170。
// loadRDARScriptFile 失败→false；其余路径均 true。
// ============================================================================
// [b28d-solo-ScriptDepsB NEAR(仅 RGBA 寄存器角色) | ORIG 0x08912336]
//  修法：补 ORIG 两枚死 string（-0xb0/-0xb4，P171 照抄）⇒ ctor/dtor 符号族归位；
//  两处 `make_pair(key++, X)` 后置 ++（右值第一参 ⇒ make_pair<int,T&>，P315）；
//  循环上界 `<= 5`/`<= 10` 复现 4.1 `setle;test;jne`（P51）。
//  ① 7 处 `continue` 改 `goto sectionHead`（标签在循环体首）：4.4 对 continue 恒发
//  共享 latch 块（`jmp <尾部>`，与 ORIG 直跳循环头 0x089123a2 形不符），goto 头标签
//  才逐条复现 ORIG 的 10 处 `jmp 0x089123a2`（含入口 3 个 `jmp;nop` 中继岛，
//  0x08912399/0x0891239c/0x0891239f）——探针 /tmp/sdb_probe/p5.o fGoto。
//  ② [separate upgrade effect] 尾 `continue`→`break`（真 bug：ORIG 0x0891285d
//  `je 0x089123a1` 后处理完即 0x089129c4 `jmp 退出块`，不再扫后续令牌）。
//  ③ RGBA 打包用 `(unsigned char)` 逐字节 cast（movzbl），`& 0xffu` 会退化成 and。
//  收口读数：raw 448=448、norm 444=444、p167 55/55 CLEAR；仅余 RGBA 首两操作数
//  寄存器角色（4.1 累加器留 %eax vs 4.4 留 %edx，P321-T5 寄存器角色 ⇒ N）。
//  [E06 2026-09-10 复测] 归一 diff 已收敛到单点：RGBA 打包段
//    ORIG `movzbl %dl,%edx; shl $0x8,%edx; or %edx,%eax`（累加器 %eax）
//    4.4  `movzbl %dl,%edx; shl $0x8,%edx; or %eax,%edx`（累加器 %edx）。
//  探针穷举 15 形（/tmp/e06_rgba/probe.cpp v1..v6 + probe2.cpp v7..v15：
//  原形/具名 unsigned/交换前两项/去 unsigned 提升/右结合/`&0xff` 掩码形/
//  三具名 int/uchar 局部/unsigned long 提升）**全部产 `or %eax,%edx`**；
//  `-fdump-tree-gimple` 证实 GIMPLE 树序未交换（`D = r | (g<<8)`），差异在
//  expand 阶段的可交换操作数 dst 选择 ⇒ 判 N86b（同一算术多 codegen 定论）+
//  P321-T5（寄存器角色不改类型/源码），**无源码旋钮**（探针路径见上）。
#include "ScriptDepsB/importGenuineUpgradeTable.inc"
                              GenuineDamageUpgradeTable* table)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string deadToken1;      // ORIG -0xb0（仅 ctor/dtor，P171 照抄）
    std::string deadToken2;      // ORIG -0xb4
    int level = 0;

    for (;;)
    {
    sectionHead:                 // ORIG 7 处 continue 均为 jmp 0x089123a2（直跳循环头）；
                                 // 4.4 对 continue 恒发共享 latch 块（jmp 尾部），
                                 // goto 头标签才复现直跳形（探针 /tmp/sdb_probe/p5.o fGoto）。
        if (!ScanType(token, true))
            break;

        if (token == "[table]")
        {
            int key = 1;
            table->m_infos.clear();
            for (;;)
            {
                GenuineDamageUpgradeInfo info;
                if (!ScanInt(&info.m_a))
                    break;
                if (!ScanInt(&info.m_b))
                    break;
                if (!ScanInt(&level))
                    break;
                info.m_levelRate =
                    static_cast<float>(level) / 10000.0f;
                if (!ScanFloat(&info.m_goldRate))
                    break;
                table->m_infos.insert(
                    std::make_pair(key++, info));   // 后置 ++ ⇒ make_pair<int,T&>
            }
            goto sectionHead;
        }

        if (token == "[separate upgrade max]")
        {
            if (!ScanInt(&table->m_maxLevel))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2f9e,
                    "importGenuineUpgradeTable() - [separate_upgrade max] false");
            }
            goto sectionHead;
        }

        if (token == "[level]")
        {
            if (!ScanInt(&table->m_divisor))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2fa5,
                    "importGenuineUpgradeTable() - [level] false");
            }
            goto sectionHead;
        }

        if (token == "[item weights by grade]")
        {
            for (;;)
            {
                int key = 0;
                GenuineDamageUpgradeMaterialInfo material;
                if (!ScanInt(&key))
                    break;
                if (!ScanInt(&material.m_materialItemIdx))
                    break;
                if (!ScanInt(&material.m_materialCount))
                    break;
                table->m_materials.insert(
                    std::make_pair(key, material));
            }
            goto sectionHead;
        }

        if (token == "[item weights by rarity]")
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&table->m_itemWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fbe,
                        "importGenuineUpgradeTable() - [item weights by rarity] false");
                }
            }
            goto sectionHead;
        }

        if (token == "[stat weights by rarity]")
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&table->m_statWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fc9,
                        "importGenuineUpgradeTable() - [stat weights by rarity] false");
                }
            }
            goto sectionHead;
        }

        if (token == "[weapon weights]")
        {
            for (int a = 0; a <= 10; ++a)
            {
                for (int b = 0; b <= 5; ++b)
                {
                    if (!ScanFloat(&table->m_weaponWeights[a][b]))
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp",
                            __FUNCTION__, 0x2fd5,
                            "importGenuineUpgradeTable() - [weapon weights] false");
                    }
                }
            }
            goto sectionHead;
        }

        if (token == "[separate upgrade effect]")
        {
            int key = 1;
            for (;;)
            {
                OutLineEFFECTInfoTemp effect;
                int r = 0;
                int g = 0;
                int b = 0;
                if (!ScanInt(&r))
                    break;
                if (!ScanInt(&g))
                    break;
                if (!ScanInt(&b))
                    break;
                // ORIG 0x08912863..0x0891289a：(unsigned char)r | ((unsigned char)g
                //   << 8) | ((unsigned char)b << 16) | 0xff000000 → +0x04
                //   （movzbl 逐字节扩展；`& 0xffu` 形会退化成 and，见 /tmp/sdb_probe/p3.o）
                effect.m_b = static_cast<int>(
                    static_cast<unsigned>(static_cast<unsigned char>(r)) |
                    (static_cast<unsigned>(static_cast<unsigned char>(g)) << 8) |
                    (static_cast<unsigned>(static_cast<unsigned char>(b)) << 16) |
                    0xff000000u);
                if (!ScanInt(&effect.m_d))              // +0x0c
                    break;
                if (!ScanInt(&effect.m_c))              // +0x08
                    break;
                if (!ScanInt(&effect.m_a))              // +0x00
                    break;
                table->m_effects.insert(
                    std::make_pair(key++, effect));     // 后置 ++ ⇒ make_pair<int,T&>
            }
            // ORIG 0x0891285d `je <continue 岛>` + 内层循环尾 0x089129c4 `jmp 退出块`
            // ⇒ 本分支处理完即退出外层令牌循环（真 bug 修正：原写 continue 会继续
            //  扫描后续令牌，与 ORIG 行为不同）。
            break;
        }
    }

    return true;
}
