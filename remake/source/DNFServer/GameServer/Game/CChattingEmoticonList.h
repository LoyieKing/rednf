#ifndef GAME_CCHATTINGEMOTICONLIST_H_
#define GAME_CCHATTINGEMOTICONLIST_H_

// df_game_r 聊天表情脚本类型（G5 批次，2026-08-23）。
// 布局/语义依据 docs/class_func_reports/CChattingEmoticonList/*.md 与
// ChattingEmoticon_{Base,byEvent,byPremium,byLevel}/*.md 反汇编。
// ChattingEmoticonBase / CChattingEmoticonList 权威声明见 CDataManager.h；
// 本头补充 EmoticonScript 与三个派生表情类（各带独立 vtable 0x8b30680 族）。

#include <string>

#include "CDataManager.h"

// ---- 表情脚本（EmoticonScript）----
// 布局依据 ImportChattingEmoticon（0x88e31b4）/ Factory（0x80e53a8）：
//   +0x00 int m_index（FindIndexByFullScan 写回）
//   +0x04 int m_type（1=event / 2=premium / 4=level，Factory 分派键）
//   +0x08 int m_param（byLevel 用的等级阈值）
// ORIG C1 W 0x080e602e：+0x0c/+0x10 两 std::string 默认构造（sizeof 0x14）；
// ORIG D1 W 0x080e607a：析构 +0x10 再 +0x0c（逆序）。
namespace EmoticonType
{

// ORIG 域常量（nm：_ZN12EmoticonTypeL20kInvalidChatEmoticonE，r 段 1 字节；
// 无效表情索引 = 0；m_nCount==0 空行语义 / m_index 初始值）
static const unsigned char kInvalidChatEmoticon = 0;
// ORIG 域计数常量（r 段 4 字节，值实测 0x08cd16b0..）：cash=12 / event=6 / level=6
static const int kEmoticonCashCnt = 12;
static const int kEmoticonEventCnt = 6;
static const int kEmoticonLevelCnt = 6;

struct EmoticonScript
{
    EmoticonScript() : m_field0c(), m_field10() {}   // ORIG C1 W 0x080e602e
    ~EmoticonScript() {}                             // ORIG D1 W 0x080e607a

    int m_index;        // +0x00
    int m_type;         // +0x04
    int m_param;        // +0x08
    std::string m_field0c;   // +0x0c（ORIG C1/D1 实证；语义无直接写入证据）
    std::string m_field10;   // +0x10
};  // sizeof 0x14

}  // namespace EmoticonType

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
    void AddPremiumIndex(int idx);                    // ORIG W 0x080e5fe0（空体）

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
