#ifndef GAME_CCHATTINGEMOTICONLIST_H_
#define GAME_CCHATTINGEMOTICONLIST_H_

// df_game_r 聊天表情脚本类型（G5 批次，2026-08-23）。
// 布局/语义依据 docs/class_func_reports/CChattingEmoticonList/*.md 与
// ChattingEmoticon_{Base,byEvent,byPremium,byLevel}/*.md 反汇编。
// ChattingEmoticonBase / CChattingEmoticonList 权威声明见 CDataManager.h；
// 本头补充 EmoticonScript 与三个派生表情类（各带独立 vtable 0x8b30680 族）。

#include <set>

#include "CDataManager.h"

// ---- 表情脚本（EmoticonScript）----
// 布局依据 ImportChattingEmoticon（0x88e31b4）/ Factory（0x80e53a8）：
//   +0x00 int m_index（FindIndexByFullScan 写回）
//   +0x04 int m_type（1=event / 2=premium / 4=level，Factory 分派键）
//   +0x08 int m_param（byLevel 用的等级阈值）
namespace EmoticonType
{

struct EmoticonScript
{
    int m_index;   // +0x00
    int m_type;    // +0x04
    int m_param;   // +0x08
};

}  // namespace EmoticonType

// ---- 表情派生类 ----
// byEvent（type=1）：GetStartArrayIndex 返回 0，GetEndArrayIndex 返回 5；
//   isUsable 遍历 m_set，命中 isAffectedPremium 返回 true。
// byPremium（type=2）：GetStartArrayIndex 返回 6，GetEndArrayIndex 返回 0x11。
// byLevel（type=4）：GetStartArrayIndex 返回 0x12，GetEndArrayIndex 返回 0x17。

class ChattingEmoticon_byEvent : public ChattingEmoticonBase
{
public:
    ChattingEmoticon_byEvent();
    virtual bool isUsable(const CUser& user) const;   // ORIG 0x80e5ca6
    virtual int GetStartArrayIndex() const;           // ORIG 0x80e5d2e（0）
    virtual int GetEndArrayIndex() const;             // ORIG 0x80e5d38（5）
};

class ChattingEmoticon_byPremium : public ChattingEmoticonBase
{
public:
    ChattingEmoticon_byPremium();
    virtual bool isUsable(const CUser& user) const;   // ORIG 0x80e5d42
    virtual int GetStartArrayIndex() const;           // ORIG 0x80e5dca（6）
    virtual int GetEndArrayIndex() const;             // ORIG 0x80e5dd4（0x11）
};

class ChattingEmoticon_byLevel : public ChattingEmoticonBase
{
public:
    ChattingEmoticon_byLevel(int level);
    virtual bool isUsable(const CUser& user) const;   // ORIG 0x80e5e4e
    virtual int GetStartArrayIndex() const;           // ORIG 0x80e5e6c（0x12）
    virtual int GetEndArrayIndex() const;             // ORIG 0x80e5e76（0x17）
    virtual bool isCommonEmoticon(const CUser& user) const;  // ORIG 0x80e5e80

private:
    int m_level;   // +0x20（byLevel 等级阈值；+0x20 = base(0x18 尾部) 后首个字段）
};

// ORIG 全局（BSS）：表情脚本列表与基础目录（g_ChattingEmoticonScriptFileList
// B 0x094f6cc0 = 0x18 STScriptFileList 对象 / 基础目录 B 0x094f6bc0[0x100]）。
// 由 initChattingEmoticonScriptList（InitScriptProducers_5.cpp）填充；
// Load_list 遍历使用。定义与本地同布局 STScriptFileList 见
// CChattingEmoticonList.cpp。

// ORIG _Z22ImportChattingEmoticonRN12EmoticonType14EmoticonScriptEPKc（0x88e31b4）。
// 数据域 TU 提供；本头仅声明。
bool ImportChattingEmoticon(EmoticonType::EmoticonScript& script, const char* path);

#endif  // GAME_CCHATTINGEMOTICONLIST_H_
