// ============================================================================
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

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_ChattingEmoticonScriptFileList 0x094f6cc0 = 0x18
// STScriptFileList 对象 / g_ChattingEmoticonScriptBaseDirectory
// 0x094f6bc0[0x100]）。由 initChattingEmoticonScriptList
// （InitScriptProducers_5.cpp）填充；Load_list 遍历使用（ORIG 0x80e54f2
// map begin 直取 0x94f6cc0，无空指针检查）。
STScriptFileList g_ChattingEmoticonScriptFileList;
char g_ChattingEmoticonScriptBaseDirectory[0x100];

bool ImportChattingEmoticon(EmoticonType::EmoticonScript& script,
                            const char* path)
{
    // ORIG 0x88e31b4：loadRDARScriptFile 后 FindIndexByFullScan 写 index；
    // 再依 [type]/[level] 等键解析。本 TU 先提供符号；待数据域 TU 细化。
    // [依赖] ImportChattingEmoticon 完整逻辑属数据域 TU（可依 _lt_global_gt_ 报告）。
    if (!loadRDARScriptFile(g_ChattingEmoticonScriptBaseDirectory, path))
        return false;
    script.m_index = EmoticonType::kInvalidChatEmoticon;
    script.m_type = 0;
    script.m_param = 0;
    bool ok = false;
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[type]")
            script.m_type = ScanInt(&ok);
        else if (token == "[level]")
            script.m_param = ScanInt(&ok);
    }
    return true;
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
    return m_level <= user.get_charac_level();
}

int ChattingEmoticon_byLevel::GetStartArrayIndex() const { return 0x12; }
int ChattingEmoticon_byLevel::GetEndArrayIndex() const { return 0x17; }

bool ChattingEmoticon_byLevel::isCommonEmoticon(const CUser& user) const
{
    (void)user;
    return 1;
}

// ORIG W 0x080e5fe0：空体（push ebp/mov esp,ebp/pop ebp/ret）。
void ChattingEmoticon_byLevel::AddPremiumIndex(int /*idx*/)
{
}

// ============================================================================
// Factory（ORIG 0x80e53a8）
// 依 script.m_type 分派构造对应派生类，并 SetIndex(script.m_index)。
// ============================================================================
ChattingEmoticonBase* CChattingEmoticonList::Factory(
    const EmoticonType::EmoticonScript& script)
{
    ChattingEmoticonBase* result = 0;
    switch (script.m_type)
    {
    case 1:
        result = new ChattingEmoticon_byEvent();
        break;
    case 2:
        result = new ChattingEmoticon_byPremium();
        break;
    case 4:
        result = new ChattingEmoticon_byLevel(script.m_param);
        break;
    default:
        result = 0;
        break;
    }
    if (result != 0)
        result->SetIndex(script.m_index);
    return result;
}

// ============================================================================
// Load_list（ORIG 0x80e54d2）
// 遍历 g_ChattingEmoticonScriptFileList->m_fileMap，逐条 ImportChattingEmoticon，
// Factory 成功则按 index 插入 m_map；重复 index 时 delete 返回失败；
// 0x18 mapVec 按 index 入桶。
// ============================================================================
bool CChattingEmoticonList::Load_list()
{
    // ORIG 0x80e54d2：无空指针检查，直接遍历对象 m_fileMap。
    EmoticonType::EmoticonScript script;
    for (std::map<int, std::string>::const_iterator it =
             g_ChattingEmoticonScriptFileList.m_fileMap.begin();
         it != g_ChattingEmoticonScriptFileList.m_fileMap.end(); ++it)
    {
        const char* path = it->second.c_str();
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
        int idx = emo->GetIndex();
        if (m_map.find(idx) != m_map.end())
        {
            delete emo;
            return false;
        }
        m_map[idx] = emo;
        m_mapVec[idx].push_back(emo);
    }
    return true;
}
