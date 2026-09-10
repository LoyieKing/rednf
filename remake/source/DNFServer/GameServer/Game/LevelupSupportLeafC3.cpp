// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LevelupSupportLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "GameTypes.h"                 // cMyTrace
#include "LevelupSupportEventManger.h" // 四类权威声明

// RDAR 脚本扫描 helper（真实符号，定义在 DNFLexWrapperHelpers.cpp / CDataManager.cpp；
// 此处仅原型。不 include DNFLexWrapper.h：其 DNFLex.h→<iostream> 会引入
// std::ios_base::Init 静态初始化块，ORIG 本域 TU 无该块——经 TU 对比实证）。
#include "LevelupSupportLeafC3/loadRDARScriptFile.inc"
#include "LevelupSupportLeafC3/unloadRDARScriptFile.inc"
#include "LevelupSupportLeafC3/ScanType.inc"
#include "LevelupSupportLeafC3/ScanInt.inc"
#include "LevelupSupportLeafC3/ScanTypeOrInt.inc"
#include "LevelupSupportLeafC3/ScanStr.inc"

// CDataManager.cpp 提供（ORIG T 0x089bbc0f，声明同 EventScriptsCompleteB2.cpp）。
#include "LevelupSupportLeafC3/getCharacterJobToEnumValue.inc"

// ---- 布局断言（头文件注释 + 各 ctor 报告反汇编实证）----
typedef char lvl_manger_size_assert[sizeof(LevelupSupportEventManger) == 0x40 ? 1 : -1];
typedef char lvl_2nd_manger_size_assert[sizeof(LevelupSupport2ndEventManger) == 0x28 ? 1 : -1];
typedef char lvl_sciprt_size_assert[sizeof(LevelupSupportSciprt) == 0x18 ? 1 : -1];
typedef char lvl_2nd_sciprt_size_assert[sizeof(LevelupSupport2ndScript) == 0x24 ? 1 : -1];

// ============================================================================
// LevelupSupportEventManger::Load —— 报告 Load.md @0x08146e3c。
// cMyTrace(pretty,0x18,9,true,false) → 清脚本 map → loadScript()==0 才继续：
// 清 m_stepIndexMap/m_events → 按 map 升序写入 level→档位序号 与
// EventInfo(level 截断 ushort, 0)。迭代器递增为 ORIG 的 operator++(int) 形态。
// [b28d-N82] fresh 复验 110/114：残差 = 迭代器三槽阶梯（ORIG -0x3c/-0x40/-0x44
// 深→浅，4.4 -0x18/-0x2c/-0x44）与 4.4 缺 push %ebx（callee-saved 需求差）
// —— P28/N82 槽方向族；块序/调用点全等，语义等价。
// [M2S1] 旧判词仅剩两处真旋钮（ORIG 有、我方无）：① ORIG 0x08146ede/0x08146ef6
// 在循环外各一次 begin→-0x3c / end→-0x40（我方旧形把 end() 留在条件里每次迭代
// 重算，多 1 call+2 条 / 槽 -0x2c）⇒ 提升为具名 endIt；② ORIG 0x08146f30
// `_ZNSt4pairIKiiEC1IRS0_iEEOT_OT0_`（第二模板参 `i`=int&& 右值）证明原码为
// `value_type(it->first, stepIndex++)` 单次构造，我方旧形 make_pair 中间临时 +
// 转换 ctor（-0x18/-0x20 两槽两 call）⇒ 直接 value_type + 后置++。
// ============================================================================
#include "LevelupSupportLeafC3/Load.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 9, true, false);
    trace("\t- Loading Levelup Support Event script - ");

    m_script.m_stepItems.clear();
    if (!m_script.loadScript())
        goto load_false;
    {
        m_stepIndexMap.clear();
        m_events.clear();

        // ORIG 0x08146ed4 begin→-0x3c 先、0x08146ef6 end→-0x40 后；4.4 按声明序浅→深
        // ⇒ it 先声明（浅）、endIt 后（深），与 ORIG 槽序一致。
        std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator it =
            m_script.m_stepItems.begin();
        std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator endIt =
            m_script.m_stepItems.end();   // 循环外单次求值（非每次迭代重算）
        // [M2FreeNext15A] ORIG 0x08146f9b `movl $0x0,-0xc` 发在 end() 调用**之后** ⇒
        // 原码 stepIndex 的初始化位点晚于两个迭代器（P328⑥ 同族）。
        int stepIndex = 0;   // ORIG -0xc 循环外计数器
        for (; it != endIt; it++)
        {
            // ORIG 0x08146f30 单次 value_type ctor（第二实参 int&& = 后置++临时）
            m_stepIndexMap.insert(std::map<int, int>::value_type(it->first, stepIndex++));
            // ORIG：*pair.first（int 键）截断 ushort 作为达标等级门槛，state=0 默认未领。
            m_events.push_back(EventInfo(static_cast<unsigned short>(it->first), 0));
        }
    }
    // [M2FreeNext15A · 2026-09-11 · 落地] ORIG 尾部布局（0x08146fc7 jmp +0x194 跳过
    // false 块 → 0x08146fc9 false 块 → 0x08146fd0 true 块）⇒ 用 goto 串（否定早退
    // goto load_false + 出口 goto load_true，P223/P325 族）复现；配 P328⑥ 把
    // `int stepIndex=0` 的初始化位点下移到两个迭代器声明之后（ORIG 0x08146f9b）。
    // 核账：show_func_diff orig=110 our=110 **gap=0**、ndiff 7→**2**（唯一残差 =
    // `je <T>` vs `jne <T>` 极性，语义等价，属 P222 对偶）；S1 反演版为 ndiff 7/gap +1。
    goto load_true;
load_false:
    return false;
load_true:
    return true;
}

// ============================================================================
// LevelupSupportEventManger::Destroy —— 报告 Destroy.md @0x08146fda。
// clearScript（释放堆上 vector）→ 清 m_stepIndexMap → 清 m_events。返回 void
// 见文件头说明。
// ============================================================================
#include "LevelupSupportLeafC3/Destroy.inc"
{
	m_script.clearScript();
	m_stepIndexMap.clear();

	m_events.clear();
	return 1;
}
// ============================================================================
// LevelupSupportEventManger::GetDefaultEventInfo —— 报告 GetDefaultEventInfo.md
// @0x08147012（_ZNK25LevelupSupportEventManger19GetDefaultEventInfoEv，sz 0xb）：
// 仅返回 &m_events（this+0x34）的 const 引用。ORIG 唯一调用点
// Inter_LoadEventLevelupSupport::dispatch_sig @0x84e8d5f 把返回值直接作为
// vector<EventInfo>::operator=(const vector&) 的源实参（0x84f16ae），实证
// 引用返回 ABI；故头声明为 const std::vector<EventInfo>&（本次修正）。
// ============================================================================
const std::vector<LevelupSupportEventManger::EventInfo>&
#include "LevelupSupportLeafC3/GetDefaultEventInfo.inc"
{
    return m_events;
}

// ============================================================================
// LevelupSupport2ndEventManger::Load —— 报告 Load.md @0x081477e4。
// cMyTrace(pretty,0xb7,9,true,false) → 清脚本 map（manager+0x8 =
// m_script.m_stepItems）→ 返回 loadScript()==0。
// [b28d-N82] 同 EventManger::Load（0x08146e3c）：帧 0x?? 与迭代器槽方向差。
// ============================================================================
#include "LevelupSupportLeafC3/Load_1.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xb7, 9, true, false);
    trace("\t- Loading Level Up Support 2nd Event script - ");

    m_script.m_stepItems.clear();
    if (m_script.loadScript())
        return false;
    return true;
}

// ============================================================================
// LevelupSupport2ndEventManger::Destroy —— 报告 Destroy.md @0x08147864。
// 仅 clearScript。
// ============================================================================
#include "LevelupSupportLeafC3/Destroy_1.inc"
{
    m_script.clearScript();
    return 1;
}

// ============================================================================
// LevelupSupportSciprt::loadScript —— 报告 loadScript.md @0x08ab514c。
// "Event/"+"tw_LevelupSupport.evt"；[step] → [level](ScanInt) → [reward]
// （循环 ScanTypeOrInt 取 item index、ScanInt 取数量，遇 TYPE 退出）→ [/step]
// （find 查重 → duplicate printf+return -1【此路径不 unloadRDARScriptFile，
// 忠实】/ new vector + swap 转移 + insert）。任一节内 ScanType 失败视为正常
// 结束：unloadRDARScriptFile 后返回 0；打开失败直接返回 -1。
// ============================================================================
// [b28d-N86] fresh 复验 210/213：残差 = std::string 局部（-0x44 vs -0x4c）+
// vector<ItemInfo> 构造/析构/swap 的 libstdc++ 外联槽整体平移 8B（P112/N86），
// 解析分支与全部调用点一一对应 ⇒ 语义等价、byte 层封顶。
#include "LevelupSupportLeafC3/loadScript.inc"
{
    if (!loadRDARScriptFile("Event/", "tw_LevelupSupport.evt"))
        return -1;

    std::string token;               // ORIG -0x44
    bool isType = false;             // ORIG -0x45（ScanTypeOrInt 出参）
    bool isInt = false;              // ORIG -0x46
    int curLevel = 0;                // ORIG -0x4c 当前 [level]
    std::vector<ItemInfo> rewards;   // ORIG -0x58 当前 [step] 奖励暂存

    // [M2S1] ORIG 条件在循环**顶部**且带 `xor $1`（0x08ab51b3..0x08ab51bd：
    // call; xor $1; test %al; jne +0x28a）⇒ 原码是 `for (;;) { if (!ScanType(...)) break; }`；
    // 旧 `while (ScanType(...))` 在 4.4 下条件挪到尾部且无 xor（探针 /tmp/probe_brk.o g4 vs g5）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[step]")
        {
            if (!ScanType(token, true))
                break;
            if (token == "[level]")
                curLevel = ScanInt((bool*)0);
            if (!ScanType(token, true))
                break;
            if (token == "[reward]")
            {
                for (;;)
                {
                    // TYPE 终止奖励序列（该 token 由 ScanTypeOrInt 写入 token 缓冲并消费——忠实）。
                    int itemIndex = ScanTypeOrInt(isType, token, isInt, true);
                    if (isType)
                        break;
                    int itemCount = ScanInt((bool*)0);
                    rewards.push_back(ItemInfo(itemIndex, itemCount));
                }
            }
            if (!ScanType(token, true))
                break;
            if (token == "[/step]")
            {
                if (m_stepItems.find(curLevel) != m_stepItems.end())
                {
                    printf("duplicate [level] %d", curLevel);
                    return -1;   // 忠实：重复 [level] 路径不调用 unloadRDARScriptFile
                }
                std::vector<ItemInfo>* levelItems = new std::vector<ItemInfo>();
                levelItems->swap(rewards);   // swap 后 rewards 复位空，供下一 [step] 复用
                // [M2S1] ORIG 0x08ab53b0/0x08ab5c22：只构造一次 value_type
                // （pair ctor IRiRS6_E → insert），无 make_pair 中间临时
                // + 转换 ctor（我方旧形多 1 call + 一个 8B 槽 /tmp/m2probe_pair.o f1 vs f2）。
                m_stepItems.insert(std::map<int, std::vector<ItemInfo>*>::value_type(curLevel, levelItems));
            }
        }
    }

    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// LevelupSupportSciprt::clearScript —— 报告 clearScript.md @0x08ab5442。
// begin/end 遍历 delete 堆上 vector 后 map.clear。注意：ORIG 循环体【无迭代器
// 递增】（汇编 +0x33..+0x55 无 operator++ 调用）——忠实复刻；map 非空时行为与
// ORIG 一致（首元素反复析构路径）。
// ============================================================================
#include "LevelupSupportLeafC3/clearScript.inc"
{
    std::map<int, std::vector<ItemInfo>*>::iterator it = m_stepItems.begin();
    std::map<int, std::vector<ItemInfo>*>::iterator endIt = m_stepItems.end();
    while (it != endIt)
    {
        // [b28c-L29] ORIG 0x08ab5442：ptEv 后 mov 0x4(%eax),%ebx 直接进 delete
        // （无具名 vec 局部槽，P0；delete 自带判空守卫，P141）——不 ++it（忠实）。
        delete it->second;   // ~vector(D1) + operator delete
    }
    m_stepItems.clear();
}

// ============================================================================
// LevelupSupport2ndScript::loadScript —— 报告 loadScript.md @0x08ab55a4。
// 同上状态机，另含 [job type]（ScanStr → getCharacterJobToEnumValue 写 m_jobType）
// 与 [mail title]/[mail content]（ScanStr 直写两个 string 成员）。重复 [level]
// 路径同样不 unloadRDARScriptFile。
// ============================================================================
// [b28d-N86] fresh 复验 264/267（ORIG 0x08ab55a4）：同 LevelupSupportSciprt::
// loadScript（0x08ab514c）——string/vector/map 局部槽整体平移 + swap 形差异。
#include "LevelupSupportLeafC3/loadScript_1.inc"
{
    if (!loadRDARScriptFile("Event/", "tw_LevelupSupport2nd.evt"))
        return -1;

    std::string token;               // ORIG -0x44
    bool isType = false;             // ORIG -0x45
    bool isInt = false;              // ORIG -0x46
    int curLevel = 0;                // ORIG -0x4c
    std::vector<ItemInfo> rewards;   // ORIG -0x58

    // [M2S1] 同 LevelupSupportSciprt::loadScript：ORIG 0x08ab5679..0x08ab5683
    // `call; xor $1; test %al; jne`（顶部条件）⇒ `for(;;) + if(!ScanType(...)) break;`。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[job type]")
        {
            std::string jobName;   // ORIG -0x5c 作用域临时
            ScanStr(&jobName);
            m_jobType = getCharacterJobToEnumValue(jobName);
        }
        else if (token == "[step]")
        {
            if (!ScanType(token, true))
                break;
            if (token == "[level]")
                curLevel = ScanInt((bool*)0);
            if (!ScanType(token, true))
                break;
            if (token == "[reward]")
            {
                for (;;)
                {
                    int itemIndex = ScanTypeOrInt(isType, token, isInt, true);
                    if (isType)
                        break;
                    int itemCount = ScanInt((bool*)0);
                    rewards.push_back(ItemInfo(itemIndex, itemCount));
                }
            }
            if (!ScanType(token, true))
                break;
            if (token == "[/step]")
            {
                if (m_stepItems.find(curLevel) != m_stepItems.end())
                {
                    printf("duplicate [level] %d", curLevel);
                    return -1;   // 忠实：不 unloadRDARScriptFile
                }
                std::vector<ItemInfo>* levelItems = new std::vector<ItemInfo>();
                levelItems->swap(rewards);
                // [M2S1] 同 LevelupSupportSciprt::loadScript：ORIG 0x08ab5xxx 单次
                // value_type ctor（IRiRS6_E ↗）→ insert，去掉 make_pair 中间临时。
                m_stepItems.insert(std::map<int, std::vector<ItemInfo>*>::value_type(curLevel, levelItems));
            }
        }
        else if (token == "[mail title]")
        {
            ScanStr(&m_mailTitle);      // this+0x1c
        }
        else if (token == "[mail content]")
        {
            ScanStr(&m_mailContent);    // this+0x20
        }
    }

    unloadRDARScriptFile();
    return 0;
}

// ============================================================================
// LevelupSupport2ndScript::clearScript —— 报告 clearScript.md @0x08ab5964。
// 与 LevelupSupportSciprt::clearScript 同构（同样无迭代器递增，忠实复刻），
// 目标 map 为 +0x04 的 m_stepItems。
// ============================================================================
// [b28c-31] AE：`delete it->second` 复现 ORIG 0x08ab59c6 无槽往返形（43=43）。
#include "LevelupSupportLeafC3/clearScript_1.inc"
{
    std::map<int, std::vector<ItemInfo>*>::iterator it = m_stepItems.begin();
    std::map<int, std::vector<ItemInfo>*>::iterator endIt = m_stepItems.end();
    while (it != endIt)
    {
        // [b28c-31] ORIG 0x08ab59c6：`call ptEv; mov 0x4(%eax),%ebx; test %ebx,%ebx;
        // je`（无槽往返，P0）⇒ 直接 delete it->second，编译器自带空指针守卫（P141）。
        delete it->second;   // 不 ++it（忠实）
    }
    m_stepItems.clear();
}
