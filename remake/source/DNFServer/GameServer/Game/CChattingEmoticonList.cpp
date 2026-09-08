// ============================================================================
// [b28d-38 判词] ImportChattingEmoticon 0x08.. DIFF：p167 CLEAR 42/42（语义完成），
//  byte 层 N 封顶（317=317 助记符序列/槽差）。
// df_game_r 还原 —— CChattingEmoticonList（聊天表情列表，G5 批次）。
// 语义依据 docs/class_func_reports/CChattingEmoticonList/*.md 与
// ChattingEmoticon_{Base,byEvent,byPremium,byLevel}/*.md 反汇编。
// Factory（0x80e53a8）/ Load_list（0x80e54d2）。基类与 CChattingEmoticonList
// 声明见 CDataManager.h；派生类/EmoticonScript 见 CChattingEmoticonList.h。
// ============================================================================

#include "CChattingEmoticonList.h"

#include "CUser.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt
#include <stdio.h>
#include "NpcScript.h"

// class SpinLock / struct less_TCHAR（ImportChattingEmoticon 函数级静态查找表）
#include "CEnvironment.h"
#include <map>
#include <string>
#include <vector>

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_ChattingEmoticonScriptFileList 0x094f6cc0 = 0x18
// STScriptFileList 对象 / g_ChattingEmoticonScriptBaseDirectory
// 0x094f6bc0[0x100]）。由 initChattingEmoticonScriptList
// （InitScriptProducers_5.cpp）填充；Load_list 遍历使用（ORIG 0x80e54f2
// map begin 直取 0x94f6cc0，无空指针检查）。
STScriptFileList g_ChattingEmoticonScriptFileList;
char g_ChattingEmoticonScriptBaseDirectory[0x100];

// ============================================================================
// ImportChattingEmoticon —— ORIG T 0x88e31b4（报告
// docs/class_func_reports/_lt_global_gt_/ImportChattingEmoticon.md）。
// RDAR 双相派发族（P163/P164/P165/P177，与 importCraneMinigameScript
// 同构，见 CraneMinigameManager.cpp）：函数级静态 __lookup
// (map<const char*,int,less_TCHAR>，guard+atexit)/__init_lock(SpinLock)/
// __inited(bool) 三件套位于重派发标签后，每轮穿 guard；注册轮持锁 id=0 →
// case 0 与 case 0x2f 共 leaf（+0x275）→ 贯穿 insert "[type]"0x2f →
// "[ani]"0x40 → "[macro]"0x46（+0x2c1/+0x39e jmp），公共尾 +0x430
// leave+置位+jmp +0x80 以当前 token 重查派发。建表后派发：
// [type]：ScanStr(value) → "EVENT"→m_type=1 / "CASH"→2 / 其它→4，随后
//         ScanInt(&m_param)（失败 return false）；
// [ani]/[macro]：ScanStr 直写 script+0xc / +0x10。
// +0x5b `movl $0x0,-0x20(%ebp)` 为只写死局部，照抄（P171）。
// 收敛口径（§6.2 三件套）：帧 `sub $0x7c` + push edi/esi/ebx 全等；id/死槽
// -0x1c/-0x20 与 ORIG 同槽；P167 调用点差集 42/42 全命中（唯一差集为
// __cxa_atexit @plt 显示）。指令数 322/321，助记符序（剥 nop 后）仅剩
// continue 回跳岛位置差：ORIG 尾部 `je +0x456; nop; jmp +0x62` vs 4.4 把
// 岛前置到 head 前（入口多 1 条 `jmp`、尾少 1 条 `jmp`）。属范式库负例
// 「跨版本：空 else nop;jmp 岛 vs jump-threading」+ N82 槽号族（Crane
// P187 同族封顶实证）⇒ 判语义完成、byte 层 N 封顶，勿再耗轮次。
// ============================================================================
bool ImportChattingEmoticon(EmoticonType::EmoticonScript& script,
                            const char* path)
{
    if (!loadRDARScriptFile(g_ChattingEmoticonScriptBaseDirectory, path))
        return false;                                   // +0x23 ebx=0
    script.m_index =
        g_ChattingEmoticonScriptFileList.FindIndexByFullScan(path);  // +0x34

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;      // ORIG -0x70（+0x45 构造）
    std::string value;      // ORIG -0x74（+0x50 构造）
    int dead20 = 0;         // ORIG -0x20 死写入，照抄

    for (;;)
    {
        if (!ScanType(token, true))    // +0x62（xor$1 物化）
        {
            break;                     // +0x45c：mov $1,%ebx → dtor 岛 → true
        }

    redo_lookup:                       // +0x80：两条重派发边（+0x23c/+0x451）落点
        static LookupMap __lookup;     // guard + __cxa_atexit(map::~map)
        static SpinLock __init_lock;   // guard + __cxa_atexit(SpinLock::~SpinLock)
        static bool __inited = false;  // 零初始化 plain bool（无 guard）

        int id;                        // ORIG -0x1c
        if (__inited)
        {
            const char* key = token.c_str();                           // -0x68
            LookupMap::const_iterator it = __lookup.find(key);         // -0x6c→-0x78
            id = (it != __lookup.end()) ? it->second : -1;             // -0x60/-0x64
        }
        else
        {
            __init_lock.enter();                                       // +0x21f
            if (__inited)
            {
                __init_lock.leave();                                   // +0x236
                goto redo_lookup;                                      // +0x23c
            }
            id = 0;                                // +0x241 注册轮入口
        }

        switch (id)
        {
        case 0:
        case 0x2f:                      // [type]（+0x275 与 case 0 共 leaf）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[type]", 0x2f));
            }
            else
            {
                if (!ScanStr(&value))              // +0x2cc
                    return false;
                if (value == "EVENT")              // +0x2f0
                    script.m_type = 1;
                else if (value == "CASH")          // +0x313
                    script.m_type = 2;
                else
                    script.m_type = 4;
                if (!ScanInt(&script.m_param))     // +0x33b
                    return false;
                break;                             // → 公共尾 +0x430
            }
            /* fall through（+0x2c1 jmp +0x355） */
        case 0x40:                      // [ani]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[ani]", 0x40));
            }
            else
            {
                if (!ScanStr(&script.m_field0c))   // +0x3ac
                    return false;
                break;                             // → 公共尾
            }
            /* fall through（+0x39e jmp +0x3c2） */
        case 0x46:                      // [macro]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[macro]", 0x46));
            }
            else
            {
                if (!ScanStr(&script.m_field10))   // +0x419
                    return false;
                break;                             // → 公共尾
            }
        }

        if (!__inited)                 // 公共尾 +0x430（xor$1 物化）：
        {                              // inited → je 落尾部回跳岛（ORIG +0x456
            __init_lock.leave();                   //   nop;jmp +0x62，nop 剥离
            __inited = true;                       // +0x44e   后为 je;jmp 形）
            goto redo_lookup;                      // +0x451 → guard 重派发
        }
    }
    return true;                                   // +0x4ac ebx=1
}

// ============================================================================
// ChattingEmoticonBase 派生类（ctor 置 vptr + 调基类；GetStart/End/语义见报告）
// ============================================================================

ChattingEmoticon_byEvent::ChattingEmoticon_byEvent()
    : ChattingEmoticonBase() {}

bool ChattingEmoticon_byEvent::isUsable(const CUser& user) const
{
    for (std::set<int>::const_iterator it = m_set.begin();
         it != m_set.end(); ++it)
    {
        if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)*it))
            return 1;
    }
    return 0;
}

int ChattingEmoticon_byEvent::GetStartArrayIndex() const { return 0; }
int ChattingEmoticon_byEvent::GetEndArrayIndex() const { return 5; }

ChattingEmoticon_byPremium::ChattingEmoticon_byPremium()
    : ChattingEmoticonBase() {}

bool ChattingEmoticon_byPremium::isUsable(const CUser& user) const
{
    for (std::set<int>::const_iterator it = m_set.begin();
         it != m_set.end(); ++it)
    {
        if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)*it))
            return 1;
    }
    return 0;
}

int ChattingEmoticon_byPremium::GetStartArrayIndex() const { return 6; }
int ChattingEmoticon_byPremium::GetEndArrayIndex() const { return 0x11; }

ChattingEmoticon_byLevel::ChattingEmoticon_byLevel(int level)
    : ChattingEmoticonBase(), m_level(level) {}

bool ChattingEmoticon_byLevel::isUsable(const CUser& user) const
{
    // ORIG 0x80e5e4e：call get_charac_level 先行（结果留 %eax），随后重读
    // this->m_level 入 %edx，`cmp %edx,%eax; setge` ⇒ 左操作数为调用结果。
    return user.get_charac_level() >= m_level;
}

int ChattingEmoticon_byLevel::GetStartArrayIndex() const { return 0x12; }
int ChattingEmoticon_byLevel::GetEndArrayIndex() const { return 0x17; }

bool ChattingEmoticon_byLevel::isCommonEmoticon(const CUser& user) const
{
    // [b26N7] 虚调用 callee 寄存器：ORIG(4.1) vfn→%edx，4.4 恒 call *%eax；
    // 探针 /tmp/nl29/n7probe.cpp 实证不可翻，ndiff 12 全由此封顶。
    (void)user;
    return 1;
}

// ORIG W 0x080e5fe0：空体（push ebp/mov esp,ebp/pop ebp/ret）。
void ChattingEmoticon_byLevel::AddPremiumIndex(int /*idx*/)
{
}

// ============================================================================
// Factory（ORIG 0x80e53a8）
// 依 script.m_type 分派构造对应派生类；SetIndex 在**每个 case 内**执行
// （ORIG +0x73/+0xc3/+0x11a 三处独立 call，非共享尾），default 仅保持
// result=0（ORIG cmp 链尾 `cmp $1; jne end` 直落出口，无额外写）。
// ============================================================================
ChattingEmoticonBase* CChattingEmoticonList::Factory(
    const EmoticonType::EmoticonScript& script)
{
    ChattingEmoticonBase* result = 0;
    switch (script.m_type)
    {
    case 1:
        result = new ChattingEmoticon_byEvent();
        result->SetIndex(script.m_index);
        break;
    case 2:
        result = new ChattingEmoticon_byPremium();
        result->SetIndex(script.m_index);
        break;
    case 4:
        result = new ChattingEmoticon_byLevel(script.m_param);
        result->SetIndex(script.m_index);
        break;
    default:
        break;
    }
    return result;
}

// ============================================================================
// Load_list（ORIG 0x80e54d2）
// 遍历 g_ChattingEmoticonScriptFileList.m_fileMap，逐条 ImportChattingEmoticon；
// Factory 成功→ m_map.find(GetIndex()) 判重（重复时虚析构 delete 返回失败）→
// m_map.insert(make_pair(GetIndex()【第二次独立调用，P48】, emo))；
// **m_mapVec 按 script.m_type(+0x4) 入桶**（ORIG +0x187/+0x2ac 传 &script+4，
// 非 GetIndex）；桶缺失时显式 vector 临时+insert+逆序析构，最后统一下标 push_back。
// path==NULL 时 break → return true（+0x310 尾岛 mov $1）。
// 收敛口径：ORIG/ours 245/245，助记符序列全等（NEAR），栈槽逐条一致
// （-0xc/-0x68/-0x64/-0x7c/-0x80/-0x60/-0x58/-0x5c/-0x44/-0x4c/-0x54/
// -0x84/-0x38/-0x90/-0x1c/-0x2c/-0x34 全等）。残余文本差仅两类：
// ① call/数据地址 .o 未重定位显示（链接后 AE 核销）；② 判重 delete 的
// 虚调用 callee 寄存器 %edx↔%eax 互换（范式库 N7，勿再试）。
// ============================================================================
bool CChattingEmoticonList::Load_list()
{
    // [b26L17 判词 N7] 唯一残差：1 处虚调用 callee 装载 %eax↔%edx 互换
    //   （244/244 助记符全等）。
    const char* path = 0;                                  // ORIG -0xc
    std::map<int, std::string>::iterator it =
        g_ChattingEmoticonScriptFileList.m_fileMap.begin();  // ORIG -0x68
    EmoticonType::EmoticonScript script;                   // ORIG -0x7c 环外构造一次

    while (it != g_ChattingEmoticonScriptFileList.m_fileMap.end())
    {
        path = it->second.c_str();          // +0x38：operator->+4 → c_str → -0xc
        if (path == 0)
            break;
        if (!ImportChattingEmoticon(script, path))
        {
            printf("Mission Script Error: %s\n", path);
            return false;
        }
        ChattingEmoticonBase* emo = Factory(script);
        if (emo == 0)
            return false;
        if (m_map.find(emo->GetIndex()) != m_map.end())
        {
            delete emo;
            return false;
        }
        m_map.insert(std::make_pair(emo->GetIndex(), emo));
        std::map<int, std::vector<ChattingEmoticonBase*> >::iterator vit =
            m_mapVec.find(script.m_type);
        if (vit == m_mapVec.end())
        {
            std::vector<ChattingEmoticonBase*> vec;
            m_mapVec.insert(std::make_pair(script.m_type, vec));
        }
        m_mapVec[script.m_type].push_back(emo);
        ++it;
    }
    return true;
}
