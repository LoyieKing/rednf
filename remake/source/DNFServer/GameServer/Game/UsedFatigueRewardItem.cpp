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
int UsedFatigueRewardItem::parsing(char* path)
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

    for (int i = 0; i < script.GetRowNum(); ++i)
    {
        Data d;
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
int UsedFatigueRewardItem::find(int low, int high, Data& out)
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

UsedFatigueRewardItem::Data::Data()
{
    m_begin = 0;
    m_end = 0;
    m_id = 0;
    memset(m_str, 0, sizeof(m_str));
    memset(m_name, 0, sizeof(m_name));
}

UsedFatigueRewardItem::Data::~Data()
{
}
