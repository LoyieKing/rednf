// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 UsedFatigueRewardItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// UsedFatigueRewardItem::Data —— ORIG W C1 @0x081a40a0 / D1 @0x081a6bf4
// （objdump 逐指令转写；无独立 md 报告，证据见头注释与
//  docs/class_func_reports/UsedFatigueRewardItem/{parsing,find}.md）
// C1：movw $0,+0x00 / movw $0,+0x02 / movl $0,+0x114 →
//     memset(+0x4,0,0xff) → memset(+0x103,0,0x10)（清零宽度实证）。
// D1：push ebp; mov esp,ebp; pop ebp; ret —— 用户定义空析构（ORIG 有 D1/D2 符号）。
#include "UsedFatigueRewardItem.h"

#include <cstring>
#include <string>

#include "Arad_Script.h"
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString

// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：parsing 0x081a25d2
//   214/226 N封顶（P167 EXTRA UsedFatigueRewardItem::Data::~Data ×2，4.4 对局部
//   Data 数组/临时多发析构 + EH 段；MISSING 空 ⇒ 语义完成、byte 层封顶）。

// ============================================================================
// OnlyOrigBatch11 落地：find / parsing（报告 parsing.md / find.md 全文转写）
// ============================================================================

// ORIG T 0x081a25d2：Load(path,"on_off") 失败返 0xa1；GetRowNum()==0 返 0xa4；
// GetString(0,"flag")=="true" → m_enable=1；Load(path,"item_reward") 失败返
// 0xac、GetRowNum()==0 返 0xaf；逐行 GetInt("fatigue"/"item_count"/"item_id")
// + GetString("message"/"title") 填 Data 后 push_back；成功返 0。
#include "UsedFatigueRewardItem/parsing.inc"
{
    Arad_Script script;
    if (!script.Load(path, "on_off"))
        return 0xa1;
    if (script.GetRowNum() == 0)
        return 0xa4;

    m_enable = false;
    std::string flag(script.GetString(0, "flag"),
                     std::allocator<char>());   // ORIG SaIcC1+SsC1(ptr,alloc)
    if (flag == std::string("true"))            // ORIG string==string（构造临时）
        m_enable = true;

    if (!script.Load(path, "item_reward"))
        return 0xac;
    if (script.GetRowNum() == 0)
        return 0xaf;

    // [P3·UsedFatigueRewardItem] ORIG 0x081a2774：Data::Data() 只在循环外构造一次
    // （在 movl $0x0,-0xc(%ebp) 之前），循环体 [0x081a2785,0x081a2883] 内无构造调用，
    // 且全 ELF 无任何 Data::~Data 外部调用点；故对象声明在循环外、每轮复用。
    Data d;
    for (int i = 0; i < script.GetRowNum(); ++i)
    {
        d.m_begin = (short)script.GetInt(i, "fatigue");
        d.m_end = (short)script.GetInt(i, "item_count");
        d.m_id = script.GetInt(i, "item_id");
        strncpy(d.m_str, script.GetString(i, "message"), 0xff);
        strncpy(d.m_name, script.GetString(i, "title"), 0x10);
        m_data.push_back(d);
    }
    return 0;
}

// ORIG T 0x081a28e4：遍历 m_data，命中 m_begin<=low 且 high<m_end（开区间）
// 的项：复制 m_begin/m_id/m_end；m_name 经 findString(4,...) 解析后
// strncpy 0x14；m_str 经 findString(4,...) 解析后 strncpy 0xff；返 1。
// 未命中返 0。
#include "UsedFatigueRewardItem/find.inc"
{
    // [b28c-30] ORIG 0x081a28e4：end() 在循环外求值一次落 -0xc（P13 end 缓存），
    // begin() 落 -0x10；原形每轮重调 m_data.end() 且槽序相反。
    std::vector<Data>::iterator itEnd = m_data.end();
    for (std::vector<Data>::iterator it = m_data.begin(); it != itEnd; ++it)
    {
        if (it->m_begin >= low && it->m_begin < high)   // ORIG 两次 movzwl (%eax)：low/high 均对 m_begin
        {
            out.m_begin = it->m_begin;
            out.m_id = it->m_id;
            out.m_end = it->m_end;
            strncpy(out.m_name,
                    g_scriptStringManager_.findString(4, it->m_name, 0), 0x14);
            strncpy(out.m_str,
                    g_scriptStringManager_.findString(4, it->m_str, 0), 0xff);
            return 1;
        }
    }
    return 0;
}

#include "UsedFatigueRewardItem/Data.inc"
{
    m_begin = 0;
    m_end = 0;
    m_id = 0;
    memset(m_str, 0, sizeof(m_str));
    memset(m_name, 0, sizeof(m_name));
}

// [P3Wave4 2026-09-11] 原 `#include "…/dtor_Data.inc"` + `~Data` 空体定义已删除
// （部件文件同步删除）：
// ORIG 的 Data 是隐式 trivial 析构（直证：ORIG `_ZSt8_Destroy<Data*>` @0x081a7c69
// call 目标 = `_ZNSt12_Destroy_auxILb1EE9__destroyIPN…DataEEEvT_S5_` @0x081a8452，
// ORIG 符号表无 ILb0）；用户定义空析构令 4.4 走 ILb0 分支并使 parsing 多发 1 次
// 析构调用 + 1 个 EH landing pad（11 条残差）。ORIG 的 `Data::~Data` W D1/D2
// @0x081a6bf4 由本 TU `new_allocator<Data>::destroy` 的显式 `p->~Data()` odr-use
// 发射（复编后 nm 已核该符号仍在）。
