// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventRewardQuickPartyB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <string>

#include "CDataManager.h"             // eventReward / itemData
#include "CQuickPartyRewardManager.h" // namespace QuickParty / QuickPartyData / import 声明
#include "CEnvironment.h"             // less_TCHAR / SpinLock
#include "DNFLexWrapper.h"            // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "StrikerScript.h"            // ContentsType

// ===========================================================================
// 1) eventReward 解析 helper 族（ORIG 非静态成员；this 不参与逻辑）
// ---------------------------------------------------------------------------
// itemData 真实布局见 itemData.h（+0/+1/+2 三字节 + +0x04/+0x08 int，
// sizeof 0x0c）；ORIG 以 movb/movzbl 访问 +0/+1/+2 字节、movl 访问 +4/+8。
// ===========================================================================


// ORIG T 0x080ef0c0：[sub type] 行名 → 子键类型。
#include "EventRewardQuickPartyB2/getSubKeyType.inc"
{
    if (token == "common")  return 0;
    if (token == "level")   return 1;
    if (token == "fatigue") return 2;
    return 0;
}

// ORIG T 0x080ef128：[/sub key] 行名 → 发放动作（equip=2 / inven=1 / postal=3）。
#include "EventRewardQuickPartyB2/getSubKeyAction.inc"
{
    if (token == "equip")  return 2;
    if (token == "inven")  return 1;
    if (token == "postal") return 3;
    return 0;
}

// ORIG T 0x080ef190：[option] 区选项名 → itemData 字节域写入。
// 返回 false 当且仅当 "upgrade" 的整数扫描失败；未知名返回 true 且不写。
#include "EventRewardQuickPartyB2/getOptionType.inc"
{
    bool ok = true;
    if (token == "unseal")
        out.m_seal = 0;
    else if (token == "seal")
        out.m_seal = 1;
    else if (token == "upgrade")
    {
        // ok 初值 true；ScanInt 自身写失败标志（ORIG 无预置 false）。
        // P0：ORIG `call ScanInt; mov %eax,%edx; mov %dl,(%eax)` 结果无槽往返 ⇒
        // 原码为右值直传，禁具名 int 缓存（缓存多 store/load 两条）。
        out.m_upgrade = ScanInt(&ok);
        if (!ok)
            return false;
    }
    else if (token == "unsealROI")
        out.m_roi = 0;
    else if (token == "sealROI")
        out.m_roi = 1;
    return ok;
}

// ORIG W 0x080ef25c：动作 2（postal）时重置 ROI 域并置 m_count=1；
// 尾部统一保证强化字节非 0（低字节为 0 → 置 1）。
#include "EventRewardQuickPartyB2/defaultOption.inc"
{
    switch (action)                        // ORIG：movsbl 后与 2 比较
    {
    case 2:
        out.m_count = 1;                     // *(int*)(d+8) = 1
        out.m_seal = 0;   // +0x01 = 0
        out.m_roi = 0;    // +0x02 = 0
        break;
    default:
        break;
    }
    if (out.m_upgrade == 0)   // movzbl/test
        out.m_upgrade = 1;    // +0x00 = 1
}

// ===========================================================================
// 2) ContentsType（ORIG 全局类，静态成员）
// ===========================================================================

// ORIG T 0x08ac3434：T(1..12) → 段名字符串；越界/0 → ""（跳转表 ja 0xc 实证）。
#include "EventRewardQuickPartyB2/getString.inc"
{
    switch ((int)type)
    {
    case 1:  return "[normal]";
    case 2:  return "[ancient]";
    case 3:  return "[dimension]";
    case 4:  return "[tower]";
    case 5:  return "[infinite]";
    case 6:  return "[ultimate]";
    case 7:  return "[despair]";
    case 8:  return "[tournament]";
    case 9:  return "[pvp]";
    case 10: return "[fair pvp]";
    case 11: return "[warroom]";
    case 12: return "[assault]";
    default: return "";
    }
}

// ORIG T 0x08ac34a4：线性 i=0..12，双构串后 operator==（name 串 vs getString 串），
// 命中返回 i；穷尽未命中返回 0。
// b28d-solo-EventRewardQuickPartyB2 判词：DIFF（N93/N96 族封顶，语义等价）。
// 残差三源均跨版本结构性：①4.1 把 string 单参构造展开的 allocator 临时析构推迟
// 到块尾（槽 -0x15/-0xd），4.4 在 string 构造后立即 ~allocator（显式 allocator
// 局部会新增 ORIG 无的具名槽，P0 禁）；②if(current==expected)return i; 的 CFG：
// ORIG 把 operator== 结果留 %ebx、析构两串后 test %bl；4.4 在测试点即分支并以
// %ebx 作 cleanup 路径标志（多 mov $0/$1,%ebx + 两处 test/reload）；
// ③块级对象槽方向相反（N93）：ORIG expected@-0x1c/current@-0x14，4.4 相反
// （current@-0x18/expected@-0x14），帧 0x30 vs 0x20。
// 实测：continue 形 / 函数级 i / 临时表达式三写法 our_insns 恒 109（ORIG 96）；
// p167 15 vs 16 调用点、0 MISSING（仅 +1 _Unwind_Resume，EH 共享差）。
#include "EventRewardQuickPartyB2/getType.inc"
{
    // [N2-S4 2026-09-11] 4.4 bool 物化消解（109→99，ORIG 96，gap -13→-3）：
    // 把两个 string 的作用域收进内层块、bool 提到块外 ⇒ `if (same)` 落在析构
    // 之后，return 不再跨 cleanup —— 4.4 不再把 operator== 结果物化到
    // callee-saved(%ebx) 并在 landing pad 后重整 0/1（ORIG 0x08ac35a4 仅一条
    // `test %bl,%bl`），也不再需要 `mov $0,%esi; mov %esi,%eax` 的返回值中转。
    // 负结论（同一函数实测 our_insns 不变或变差，故不采用）：声明序交换
    // current/expected(109)、goto 共享出口(109)、while 手写 ++i(109)、
    // 具名 hit+result+break(111)、break+守卫返回 i>12(111)；三元/`==0`/
    // 具名 bool 已由 b28d 判词覆盖（恒 109）。残 -3 为 4.4 的 landing pad
    // 分组（我方 12+12 条 pad + 2 条跳过 jmp vs ORIG 9+14 条）——跨版本结构。
    // [P6S6 2026-09-11] 追加钥匙：`register bool same;`（ORIG 0x08ac35a4
    //   `test %bl,%bl` 直证 same 常驻 callee-saved 字节寄存器）。实测
    //   our_insns 99 不变、gap −3 不变，但 difflib 差异块 14→**12**（消去
    //   `cmpb $0x0,-0x9(%ebp)` 槽物化与 %bl 往返两项）⇒ 按 codegen 轴
    //   「四数不劣 + ndiff 严格改善」保留。
    //   剩余 12 块 = 4.4 landing pad 分组 + string 对象槽深（跨版本结构）。
    for (int i = 0; i <= 12; ++i)
    {
        register bool same;
        {
            std::string expected(getString((ContentsType::T)i));
            std::string current(name);
            same = (current == expected);
        }
        if (same)
            return i;
    }
    return 0;
}

// ===========================================================================
// 3) QuickParty 脚本导入族
// ===========================================================================

namespace QuickParty
{

namespace
{

// importQuickPartyScript 段名 id（ORIG __lookup 表值实证）。
enum
{
    QPSEC_QUICK_PARTY_END    = 0x3e,   // [/quick party]
    QPSEC_UNABLE             = 0x41,   // [unable to quickparty]
    QPSEC_LEVEL_SECTION      = 0x4d,   // [level section]
    QPSEC_BASIC_REWARD       = 0x59,   // [basic reward]
    QPSEC_RANDOM_REWARD      = 0x8f,   // [random reward]
    QPSEC_RANDOM_REWARD_PROB = 0xb8,   // [random reward prob]
    QPSEC_REQUIRE_PARTY_NUM  = 0xce    // [require party num]
};

}  // unnamed namespace

// ORIG T 0x08a6e100：脚本 buff 名 → RandomBuffType；未知 → 11（0xb）。
RandomBuffType getRandomBuffTypeByName(const std::string& name)
{
    RandomBuffType type = (RandomBuffType)0xb;
    if      (name == "[increase exp]")       type = RANDOM_BUFF_INCREASE_EXP;
    else if (name == "[increase stats]")     type = RANDOM_BUFF_INCREASE_STATS;
    else if (name == "[increase speed]")     type = RANDOM_BUFF_INCREASE_SPEED;
    else if (name == "[increase dropprob]")  type = RANDOM_BUFF_INCREASE_DROPPROB;
    else if (name == "[increase allattack]") type = RANDOM_BUFF_INCREASE_ALLATTACK;
    else if (name == "[increase allresist]") type = RANDOM_BUFF_INCREASE_ALLRESIST;
    else if (name == "[increase broken]")    type = RANDOM_BUFF_INCREASE_BROKEN;
    else if (name == "[increase mobhp]")     type = RANDOM_BUFF_INCREASE_MOBHP;
    else if (name == "[increase antidevil]") type = RANDOM_BUFF_INCREASE_ANTIDEVIL;
    else if (name == "[increase coinlimit]") type = RANDOM_BUFF_INCREASE_COINLIMIT;
    else if (name == "[return ticket]")      type = RANDOM_BUFF_RETURN_TICKET;
    return type;
}

// ORIG T 0x08a6e282。双遍结构（ORIG 函数级静态 __lookup/__init_lock/__inited，
// SpinLock 双检建表）：首令牌触发 7 个 (段名,id) 登记后回到派发块重新查表，
// 当前令牌被正常处理（与 importTitleBookEtcScript 同构）。主循环仅以
// [/quick party]（返回 true）或主扫描失败（返回 false）退出；节内子扫描失败
// 一律回到主循环继续取令牌；未知令牌（id==-1）跳过。
//
// 各节写入目标（ORIG 偏移 ↔ QuickPartyData 具名字段，CQuickPartyRewardManager.h）：
//   [level section]        → +0x00 m_vec00（vector<int>）
//   [unable to quickparty] → +0x0c m_dungeonIdxList（vector<int>）
//   [increase gold card]   → +0x18 BasicReward.m_levelDatas（LevelData{率%,档}）
//   [hell ratio]           → +0x30/0x34 m_basicReward.m_field18/m_field1c、
//                            +0x24 BasicReward.m_hellDatas（LevelDataHell）
//   [reward]               → +0x38 m_randomRewards map insert（键=buff 类型）
//   [random reward prob]   → +0x50 m_probs[type][i]（i<7；type≤10 才写）
//   [require party num]    → m_minCounts[dungeonType][diff]
//                            （QuickPartyData +0x1a0 区，行 stride 0x10；
//                            ORIG 寻址 (type*4+diff+0x68)*4 完全一致）
// b28d-solo-EventRewardQuickPartyB2 判词：DIFF（§6.2 三件套达标，byte 层 N 封顶）。
// 已修真缺口：①7 段注册由「循环头统一插入」改 ORIG 的 case 内 fall-through 链
//   （含 case 0 哨兵、goto re_lookup 双检、switch 后 leave+__inited+goto）——
//   消除 pair 值构造/make_pair/临时 ~pair 共 10 类 EXTRA，改 map::value_type
//   引用构造（pair<const char* const,int>::pair<char const(&)[N],int&&>）；
//   ②补 token/value 的 Ss::clear、LevelData entry 提为函数级复用槽、lv 复用
//   -0xf8、ok=ScanStr 两跳槽往返、ScanInt(&lv) 复用；③[reward] 的
//   !=+continue 改 == 包裹（消 operator!= EXTRA / operator== MISSING）；
//   ④while(ScanType) 改 for+if(!ScanType)break（P164 抗循环旋转）。
// 验收：p167 调用点 97=97 CLEAR（0 missing / 0 extra）；栈槽集 42/42 逐项全等；
// gap=2（ORIG 740 / ours 742）。归一化残差 19 行仅剩：LevelData::C1 后 4.4
// EH 边界 jmp+nop（P223/N121 族）、循环内 if(!ok)break 的 je+jmp vs 4.1 jne
// （N122 族）、while 条件块物理位置差（difflib 对齐）。语义与 ORIG 逐块一致。
// 依赖：LevelDataRandomReward::m_field0 int→RandomBuffType（.h 改动，R2 已广播）。
// [P6S6 2026-09-11 追加钥匙] 主循环 `while (true) { if (!ScanType(token,true))
//   break; … }` 改为 `for (; ScanType(token,true); ) { … }`（等价重构，取消 break
//   出口）⇒ 4.4 少发 1 条跳转：ours 742 → **741**（ORIG 740，gap −2→**−1**），
//   四数不变。另一形 `while(true)+goto main_scan_failed` 实测回退到 742 ⇒ 不采用。
//   剩余 3 块：①`xor $0x1,%eax; test; jne` vs 我方 `jmp`（首测块物理位置差）；
//   ②内层 `if (!ok) break;` 的 `jne` vs `je+jmp`（N122 族）；③缺进循环前的
//   前置 `jmp`（4.1 的 while-rotate 形，4.4 恒直落）⇒ 跨版本封顶。
#include "EventRewardQuickPartyB2/importQuickPartyScript.inc"
{
    if (!loadRDARScriptFile("", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    token.clear();                                    // ORIG：C1 后紧跟 Ss::clear
    std::string value;
    value.clear();
    bool ok = false;
    int lv = 0;                                       // ORIG -0xf8：全函数复用
    LevelData entry;                                  // ORIG -0x100：函数级，两节复用

    int id;

    for (; ScanType(token, true); )                   // P6S6: 主扫描失败 → false
    {
    re_lookup:
        static std::map<const char*, int, less_TCHAR> s_lookup;
        static SpinLock s_initLock;
        static bool s_inited = false;

        if (s_inited)
        {
            LookupMap::const_iterator it = s_lookup.find(token.c_str());
            id = (it != s_lookup.end()) ? it->second : -1;
        }
        else
        {
            s_initLock.enter();
            if (s_inited)
            {
                s_initLock.leave();
                goto re_lookup;                       // 双检命中：重查表
            }
            id = 0;                                   // 首遍哨兵 → 注册链
        }

        switch (id)
        {
        case 0:                                       // 首遍注册哨兵（与 0x3e 同块）
        case QPSEC_QUICK_PARTY_END:                   // [/quick party] → 成功
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[/quick party]", (int)QPSEC_QUICK_PARTY_END));
            }
            else
            {
                return true;
            }
            // 首遍：继续登记下一段

        case QPSEC_UNABLE:                            // 单扫描：值即入列元素
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[unable to quickparty]", (int)QPSEC_UNABLE));
            }
            else
            {
                for (;;)
                {
                    lv = ScanInt(&ok);
                    if (!ok)
                        break;
                    data->m_dungeonIdxList.push_back(lv);
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_LEVEL_SECTION:                     // 单扫描：值即入列元素
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[level section]", (int)QPSEC_LEVEL_SECTION));
            }
            else
            {
                for (;;)
                {
                    lv = ScanInt(&ok);
                    if (!ok)
                        break;
                    data->m_vec00.push_back(lv);
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_BASIC_REWARD:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[basic reward]", (int)QPSEC_BASIC_REWARD));
            }
            else
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                        break;
                    if (token == "[/basic reward]")
                        break;

                    if (token == "[increase gold card]")
                    {
                        for (;;)                      // 门扫描弃值，体读「档」
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entry.m_value4 = lv;
                            ok = ScanStr(&value);
                            entry.m_field0 = (ok && value == "%") ? 1 : 0;
                            data->m_basicReward.m_levelDatas.push_back(entry);
                        }
                    }
                    else if (token == "[hell ratio]")
                    {
                        ok = ScanStr(&value);
                        if (!ok)
                            break;                    // ORIG：退出节回主循环
                        lv = ScanInt((bool*)0);
                        data->m_basicReward.m_field18 = lv;   // +0x30
                        lv = ScanInt((bool*)0);
                        data->m_basicReward.m_field1c = lv;   // +0x34
                        LevelDataHell entryHell;
                        for (;;)                      // 门扫描弃值，体读两 int
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entryHell.m_field0 = lv;
                            lv = ScanInt((bool*)0);
                            entryHell.m_value4 = lv;
                            data->m_basicReward.m_hellDatas.push_back(entryHell);
                        }
                    }
                    // 其余令牌：忽略并继续节内扫描
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_RANDOM_REWARD:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[random reward]", (int)QPSEC_RANDOM_REWARD));
            }
            else
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                        break;
                    if (token == "[/random reward]")
                        break;

                    if (token == "[reward]")
                    {
                        ok = ScanStr(&value);
                        if (!ok)
                            break;
                        LevelDataRandomReward lrr;    // ctor：+0=0xb、串/向量清空
                        lrr.m_field0 = getRandomBuffTypeByName(value);
                        ScanStr(&lrr.m_name);         // 返回值被忽略（ORIG 同）

                        for (;;)                      // 门扫描弃值，体读「档」
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entry.m_value4 = lv;
                            ok = ScanStr(&value);
                            entry.m_field0 = (ok && value == "%") ? 1 : 0;
                            lrr.m_levelDatas.push_back(entry);
                        }

                        // ORIG：value_type 引用构造（非 make_pair）
                        data->m_randomRewards.insert(
                            std::map<RandomBuffType, LevelDataRandomReward>::value_type(
                                lrr.m_field0, lrr));
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_RANDOM_REWARD_PROB:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[random reward prob]", (int)QPSEC_RANDOM_REWARD_PROB));
            }
            else
            {
                for (;;)
                {
                    ok = ScanStr(&value);
                    if (!ok)
                        break;
                    const int idx = (int)getRandomBuffTypeByName(value);
                    if (idx > 10)                     // ORIG jle 实证上界
                        continue;
                    for (int i = 0; i < 7; ++i)
                    {
                        lv = ScanInt((bool*)0);
                        data->m_probs[idx][i] = lv;
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_REQUIRE_PARTY_NUM:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[require party num]", (int)QPSEC_REQUIRE_PARTY_NUM));
            }
            else
            {
                while (ScanStr(&value))
                {
                    int dungeonType = -1;
                    if (value == "[normal]")       dungeonType = 0;
                    else if (value == "[ancient]") dungeonType = 1;
                    else break;

                    if (!ScanStr(&value))
                        break;

                    int diff = -1;
                    if      (value == "[easy]")     diff = 0;
                    else if (value == "[medium]")   diff = 1;
                    else if (value == "[hard]")     diff = 2;
                    else if (value == "[ultimate]") diff = 3;
                    else break;

                    if (!ScanInt(&lv))
                        break;

                    // ORIG 双边界检查（js/jg 实证）：type∈{0,1}、diff∈{0..4}
                    if (dungeonType >= 0 && dungeonType <= 1 &&
                        diff >= 0 && diff <= 4)
                    {
                        data->m_minCounts[dungeonType][diff] = lv;
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        default:                                      // 未知令牌（id==-1）：跳过
            break;
        }

        if (!s_inited)
        {
            s_initLock.leave();
            s_inited = true;
            goto re_lookup;                           // 首遍登记完 → 重查表
        }
    }

    return ok;
}

}  // namespace QuickParty
