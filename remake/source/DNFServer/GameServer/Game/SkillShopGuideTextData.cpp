// ============================================================================
// df_game_r 还原 —— SkillShopGuideTextData 域（独占 TU）
//
// 报告：docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md（已完整阅读
// 汇编 + 反编译 C 两段）。
// 类方法（W 符号）布局证据：objdump 0x0891cb4c/0x08932804/0x089328e6；
//   分配尺寸 0x10（new_allocator::construct movl $0x10），C1/D1 仅 string 成员，
//   拷贝 ctor/op= 逐字段 int,int,string,byte,byte,byte。
//   [判据㉓ 本轮] 四件套定义已移入 SkillShopGuideTextData.h 类内（ORIG nm 全 W：
//   C1/C2 0x0891cb4c、D1/D2 0x0891cb62、C1ERKS_/C2ERKS_ 0x08932804、aSERKS_ 0x089328e6；
//   我方原 .cpp 外联定义发射 T）⇒ nm 绑定 342 共有符号 bind-diff=0；C1ERKS_ 逐条全等
//   （33=32+对齐 nop）；sizeof/偏移/mangled 零变化；四数不回退。
//   [P327/P338 本轮] 断 DNFLexWrapper.h 链：其 DNFLex.h:16 `#include <iostream>` 使本 TU
//   发射 std::ios_base::Init __ioinit + `_GLOBAL__I_<本 TU 首强符号>`（ORIG 全 509 个 GI
//   无 SkillShop 键名 ⇒ 仅 NEW 符号）⇒ 改局部权威声明四个 DNFLex 函数（mangled 与 ORIG
//   实证一致）。nm 确认 GI/ioinit/static_init 全消，四数 diff 3→2、missing_in_game 62→61。
//   [Main 排查 2026-09-10] 脚本循环 `goto/break` 目标逐点核：goto loop → ORIG jmp
//   08914553（guard 段回边）一致；ScanType 失败 break → ORIG jne 08914d40（函数出口）一致；
//   Tree 内层 break → ORIG jmp 089143e5（外层 continue）一致。全 TU jmp/jcc 目标方向
//   （F/B）逐位核对 239 符号仅 2 处差异（P164 两跳/单跳、岛中转 vs 直跳），无回边→出口反转。
// importSkillShopGuideText：ORIG T 0x08914483，无调用者（全量 objdump 无 call 命中），
//   语义：装载 SkillShopGuideText.etc，段键经 static map 查找（首次 SpinLock 保护
//   注册键值），"[character job]"+growType 为键、"[text]/[color]/[level range]"
//   填充数据向量后 insert 进 out map。
// ============================================================================

#include "SkillShopGuideTextData.h"
#include "CEnvironment.h"    // class SpinLock / less_TCHAR
#include "LogManager.h"
#include "Quest.h"           // enum ENUM_CHARACTERJOB
#include "SyncScriptDeps.h"  // int getCharacterJob(const char*)

#include <map>
#include <vector>
#include <utility>

// DNFLexWrapper.h 在本 TU 只提供 loadRDARScriptFile/ScanType/ScanInt/ScanStr 四个
// 函数声明，但其 include 链 DNFLex.h:16 `#include <iostream>` 会带出
// std::ios_base::Init __ioinit 与 `_GLOBAL__I_<本 TU 首强符号>`（ORIG 本 TU 无 GI，
// P327/P338；属「仅 NEW」符号）⇒ 改用局部权威声明断链。签名由 ORIG mangled 实证：
// _Z18loadRDARScriptFilePKcS0_ / _Z8ScanTypeRSsb / _Z7ScanStrPSs / _Z7ScanIntPb；
// 返回类型由调用点判据定（P292/P168a：`xor $1;test;je` ⇒ bool；ScanInt 单字节/32 位
// 两处使用 ⇒ int）。
bool loadRDARScriptFile(const char* path, const char* fileName);
bool ScanType(std::string& token, bool skip);
bool ScanStr(std::string* out);
int ScanInt(bool* ok);

// ORIG T 0x088be63d（定义于 InitScriptProducers_6.cpp，本 TU 仅引用）
int getGrowTypeIndex(const char* name);

// SkillShopGuideTextData 四件套（ORIG W 0x0891cb4c / 0x0891cb62 / 0x08932804 /
//   0x089328e6）已按判据㉓ 移入 SkillShopGuideTextData.h 类内 inline 定义
//   （ORIG 全 W ⇒ 头内定义由 ODR 使用点发射；.cpp 外联定义发射 T 是绑定缺口）。
// ============================================================================
// importSkillShopGuideText —— ORIG T 0x08914483
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md
// 返回 int：0=失败（文件加载失败/段内 ScanStr 失败），1=成功。
// 静态局部：__lookup（map<const char*,int,less_TCHAR>，惰性 guard+atexit）、
//   __init_lock（SpinLock，惰性 guard+atexit）、__inited（bool，SpinLock 保护）。
// 控制流（汇编 LAB_08914553 do 循环）：__inited==0 时持锁注册全部段键后置 1，
//   回 do 头以 find 路径重处理当前 token；__inited==1 直接 find 分派。
// ORIG 三个 static 的 guard 检查位于 do 头（08914553/089145cf/0891464b，每次
//   迭代经 guard 跳过初始化），故声明须在循环体内。
// 注册链按 ORIG fall-through：case 按数值升序，__inited==0 时逐段注册后落入
// 下一 case；段键注册用直接 pair 构造（ORIG pair<char_const(&)[N],int> 模板实例）。
// ---------------------------------------------------------------------------
// b28d-solo-SkillShopGuideTextData 判词：语义完成（p167 CLEAR 76=76；帧 0x14c、
//   槽位 key/-0x104、data/-0x114、vec/-0x120、token/-0x124、type/-0x1c、
//   jobStr/-0x12c、growStr/-0x130 与 ORIG 全等；修真语义 bug：case "[/text info]"
//   原缺 vec.push_back(data)；case 0x3334 的 ok 走寄存器消 4B 槽，帧/槽全等）。
// b28d-solo-SkillShopGuideTextData 二轮（本轮）：
//   ① 循环尾 `if(__inited) continue;` → 负极性 `if(!__inited){leave;inited=true;goto loop;}`
//      （ORIG 0x08914d14 `movzbl __inited; xor $0x1,%eax; test %al,%al; je`；P168a/P194）：
//      gap 10→9，jcc 序列不变。
//   ② case 0x3334 的 ScanStr 检查块试过负极性形 `if(!ScanStr(...)){ok=0;return 0;}` +
//      ok 裸声明 + jobStr/growStr 嵌套作用域（三种 ok 类型组合）：可复现 ORIG 0x08914911
//      的 `call; xor $0x1,%eax; test %al,%al; je` 条件形，但 4.4 为 `ok=0/1` 额外引入
//      %esi/%edi 双寄存器 bool 归一化（净 delta +9→−7，代价是多出 1 个 `jne`：jcc 39≠38）
//      ⇒ 按「jcc 逐位一致优先」回退原形（保 jcc 38=38）。
//   读数（fresh check_tu_routed）：identical/ae/near/diff = 94/143/0/3（3 = 本件 +
//      importSkillShopTree + N123 static_init 撞名），missing_in_game=62 与基线一致；
//      gap：orig 551 / our 542（+9；残余 = ScanStr 失败块 ORIG `mov $0,%ebx`/
//      `mov $0,%esi`/`jmp` 物化 ×2 组共 6 条 + guard catch `mov %esi,%eax` 2 条 +
//      循环尾 `xor` 后 4.4 折掉 `test %al,%al` 1 条 + D 段 2 条，减循环入口/尾
//      `jmp+nop` 2 条）。
//   byte 层 N 封顶（N7 跨版本寄存器分配 / N18a 单段 regmove / P164 循环旋转 /
//   N122 `xor` 后 test 折叠 / 对齐 nop 岛）；源码形不可择，不再耗轮次。
// ============================================================================
int importSkillShopGuideText(
    const char* path,
    std::map<std::pair<ENUM_CHARACTERJOB, int>,
             std::vector<SkillShopGuideTextData, std::allocator<SkillShopGuideTextData> >,
             std::less<std::pair<ENUM_CHARACTERJOB, int> >,
             std::allocator<std::pair<std::pair<ENUM_CHARACTERJOB, int> const,
             std::vector<SkillShopGuideTextData, std::allocator<SkillShopGuideTextData> > > > >& out)
{
    typedef std::map<std::pair<ENUM_CHARACTERJOB, int>,
                     std::vector<SkillShopGuideTextData,
                                 std::allocator<SkillShopGuideTextData> >,
                     std::less<std::pair<ENUM_CHARACTERJOB, int> >,
                     std::allocator<std::pair<std::pair<ENUM_CHARACTERJOB, int> const,
                     std::vector<SkillShopGuideTextData,
                                 std::allocator<SkillShopGuideTextData> > > > > OutMap;
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3318,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
        return 0;
    }

    std::pair<ENUM_CHARACTERJOB, int> key;
    key.first = (ENUM_CHARACTERJOB)0xb;
    key.second = -1;
    SkillShopGuideTextData data;
    std::vector<SkillShopGuideTextData> vec;
    std::string token;

    // ORIG 0x08914532：循环条件在顶部（入口直落 ScanType，失败 jne 出口）；
    // 4.4 对 while 恒旋转 ⇒ 用 for(;;)+break 复现（P164）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        // ORIG 0x08914d35 的 goto 回跳目标是 0x08914553（__lookup guard 检查）⇒
        // loop 标签在三个 static 声明之前，回跳时重新过 guard 检查（P177 同族）。
    loop:
        ;
        typedef std::map<const char*, int, less_TCHAR> LookupMap;
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int type;

        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(token.c_str());
            // ORIG 0x08914704/08914709：两臂经 %eax 汇聚后统一 mov %eax,-0x1c ⇒ 三目形（P47）。
            type = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto loop;
            }
            type = 0;
        }

        switch (type)
        {
        case 0:
        case 0x332c:   // "[guide]"
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[guide]", 0x332c));
            else
                break;
            // fall through
        case 0x3330:   // "[/guide]"
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/guide]", 0x3330));
            else
                break;
            // fall through
        case 0x3334:   // "[character job]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[character job]", 0x3334));
            }
            else
            {
                std::string jobStr;
                // ORIG 0x0891490c/0891495a 的 ok 值走 %esi（无栈槽）⇒ 纯 register
                // 存储类提示（P231；非 asm 绑定，P0 合规）。
                // ORIG 0x089149b6 用 32 位 test %esi,%esi ⇒ ok 取 int 宽度（纯 register
                // 存储类提示，P231；非 asm 绑定，P0 合规）。
                register int ok = ScanStr(&jobStr);
                if (ok)
                {
                    key.first = (ENUM_CHARACTERJOB)getCharacterJob(jobStr.c_str());
                    std::string growStr;
                    ok = ScanStr(&growStr);
                    if (ok)
                        key.second = getGrowTypeIndex(growStr.c_str());
                }
                if (!ok)
                    return 0;
                break;
            }
            // fall through
        case 0x3342:   // "[/character job]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/character job]", 0x3342));
            }
            else
            {
                // ORIG 0x08914a7b：直接就地构造 out 的 value_type
                // （_ZNSt4pairIKS_I17ENUM_CHARACTERJOBiESt6vectorI22SkillShopGuideTextData
                //   SaIS4_EEEC1IRS1_RS6_EEOT_OT0_ = pair<const pair<JOB,int>,vector>
                //   的 pair<pair<JOB,int>&, vector&> 模板构造），随后 map::insert；
                // make_pair 会多出 pair<pair<JOB,int>,vector> → value_type 的转换临时。
                out.insert(OutMap::value_type(key, vec));
                key.first = (ENUM_CHARACTERJOB)0xb;
                key.second = -1;
                vec.clear();
                break;
            }
            // fall through
        case 0x334c:   // "[text info]"
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[text info]", 0x334c));
            else
                break;
            // fall through
        case 0x3350:   // "[/text info]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/text info]", 0x3350));
            }
            else
            {
                // ORIG 0x08914b92：__inited==1 路径把当前 data 推入 vec
                // （_ZNSt6vectorI22SkillShopGuideTextDataSaIS0_EE9push_backERKS0_，
                //   p167 差集曾 MISSING×1）。
                vec.push_back(data);
                break;
            }
            // fall through
        case 0x3355:   // "[text]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[text]", 0x3355));
            }
            else
            {
                ScanStr(&data.m_text);
                break;
            }
            // fall through
        case 0x335a:   // "[color]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[color]", 0x335a));
            }
            else
            {
                data.m_colorR = (unsigned char)ScanInt((bool*)0);
                data.m_colorG = (unsigned char)ScanInt((bool*)0);
                data.m_colorB = (unsigned char)ScanInt((bool*)0);
                break;
            }
            // fall through
        case 0x3361:   // "[level range]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[level range]", 0x3361));
            }
            else
            {
                data.m_levelMin = ScanInt((bool*)0);
                data.m_levelMax = ScanInt((bool*)0);
                break;
            }
            break;
        default:
            break;
        }

        // ORIG 0x08914d14：`movzbl __inited; xor $0x1,%eax; test %al,%al; je <跳过>`
        // ⇒ 原码是负极性正向包裹（P168a/P194）；写 `if(__inited) continue;` 4.4 产
        // `test;jne` 反极性且少一条 xor。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto loop;
        }
    }

    return 1;
}
// ============================================================================
// importSkillShopTree —— ORIG T 0x0891427f（范围 [0x0891427f, 0x08914482]）
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopTree.md
// 返回 int：0=文件加载失败，1=成功。与 importSkillShopGuideText 同属
//   RDARScriptEtcParameter.cpp 日志域；装载 SkillShopTree.etc 后 clear out，
//   逐 token："[skill tree]" 段内循环 ScanStr 职业名 → getCharacterJob 转枚举、
//   ScanStr 树名，pair 构造后 insert 进 out map；ScanStr 失败跳出内层循环回到
//   外层 do；遇到 "[/skill tree]" 或 EOF 结束返回 1。
// ---------------------------------------------------------------------------
// b28d-solo-SkillShopGuideTextData 判词：语义完成（p167 CLEAR 24=24；帧 0x50、
//   槽位 token/-0x20、bool/-0x9、job/-0x24、treeStr/-0x28、jobStr/-0x2c、
//   pair/-0x14、insert 返回/-0x1c 与 ORIG 全等）。byte 层 N 封顶：内层 ScanStr
//   检查 ORIG 为「xor $1;test;je body;jmp outer」两跳形，4.4 恒产
//   「xor $1;test;jne outer」单跳 + 中继 nop 岛（P164 循环旋转族）；外层/析构段
//   另有 N7 %esi↔%ebx 角色差。源码形不可择，不再耗轮次。
// ============================================================================
int importSkillShopTree(
    const char* path,
    std::map<ENUM_CHARACTERJOB, std::string, std::less<ENUM_CHARACTERJOB>,
             std::allocator<std::pair<ENUM_CHARACTERJOB const, std::string> > >& out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x32eb,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
        return 0;
    }
    // 声明序/clear 位置按 ORIG 0x089142de..0x08914309：token(-0x20)、treeStr(-0x28)、
    // jobStr(-0x2c) 三个 string 构造完成后才调 out.clear()。
    std::string token;
    // ORIG 0x089142e9 有 movb $0x0,-0x9(%ebp)（token 构造后的一次性 bool 清零，
    // 全函数不再读写）= 原码具名 bool 局部死初始化，照抄（P171/P232）。
    bool parseOk = false;
    (void)parseOk;
    // ORIG 0x08914374 用 mov %eax,-0x24(%ebp) 写该值 ⇒ 原码 job 声明在函数头
    // （槽 -0x24 夹在 token 与 treeStr 之间），循环内仅赋值。
    ENUM_CHARACTERJOB job;
    std::string treeStr;
    std::string jobStr;
    out.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[skill tree]")
        {
            for (;;)
            {
                // 4.4 对本形产「xor $1;test;jne 共享出口」；ORIG 为
                // 「xor $1;test;je body;jmp outer」两跳——P164 旋转族不可择
                // （X1/裸内联/语句表达式/goto 尾标签四形实测均不闭合）。
                if (!ScanStr(&jobStr))
                {
                    break;
                }
                else
                {
                    job = (ENUM_CHARACTERJOB)getCharacterJob(jobStr.c_str());
                    if (!ScanStr(&treeStr))
                    {
                        break;
                    }
                    else
                    {
                        // ORIG 0x0891439f：直接就地构造 value_type
                        // （_ZNSt4pairIK17ENUM_CHARACTERJOBSsEC1IRS0_RSsEEOT_OT0_ =
                        //   pair<const ENUM_CHARACTERJOB,string> 的 pair<JOB&,string&> 构造）
                        // 后 map::insert(value_type const&)；make_pair 会多出转换临时。
                        out.insert(
                            std::map<ENUM_CHARACTERJOB, std::string,
                                     std::less<ENUM_CHARACTERJOB>,
                                     std::allocator<std::pair<ENUM_CHARACTERJOB const,
                                     std::string> > >::value_type(job, treeStr));
                    }
                }
            }
            continue;
        }
        if (token == "[/skill tree]")
            break;
    }
    return 1;
}

// ============================================================================
// getSkillMaxLevelOnSteal —— ORIG T 0x08a9af4f（0x53 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillMaxLevelOnSteal.md
// 与 getSkillSpendSPOnSteal（SkillSlot.cpp）共用 .rodata 常量 0x8e2a180 = 100.0f：
//   f = penalty / 100.0f；r = (int)(sp * f)（fistpl 控制字改 0x0c → RC=11 截断
//   向零，与 C 浮点转整一致）；返回 std::max(1, r)。
// ============================================================================
int getSkillMaxLevelOnSteal(int sp, int penalty)
{
    float f = (float)penalty / 100.0f;
    int r = (int)((float)sp * f);
    return std::max(1, r);
}

// ============================================================================
// getSkillNeedLevelOnSteal —— ORIG T 0x08a9aff5（0xe 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillNeedLevelOnSteal.md
// 汇编仅 lea (%edx,%eax,1) + ret：直接返回 param_1 + param_2。
// ============================================================================
int getSkillNeedLevelOnSteal(int level, int add)
{
    return level + add;
}
