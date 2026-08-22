#ifndef WONGWORK_QUESTCLEAR_H
#define WONGWORK_QUESTCLEAR_H

// ============================================================================
// WongWork::CQuestClear / STClearedQuest
//
// 从 GameStubs.cpp 迁移的 G2 桩类。ORIG 符号经 nm -C df_game_r 确认：
//   _ZN8WongWork11CQuestClearC1Ev                       CQuestClear()
//   _ZN8WongWork11CQuestClearD1Ev                       ~CQuestClear()
//   _ZN8WongWork11CQuestClear5clearEv                   clear()
//   _ZNK8WongWork11CQuestClear14isClearedQuestEj        isClearedQuest(unsigned) const
//   _ZNK8WongWork11CQuestClear15getClearedQuestEv       getClearedQuest() const
//   _ZN8WongWork11CQuestClear18resetClearedQuestsEj     resetClearedQuests(unsigned)
//   _ZN8WongWork11CQuestClear15setClearedQuestEPKNS_14STClearedQuestE
//   _ZN8WongWork11CQuestClear10syncScriptEv             static syncScript()
//   _ZN8WongWork11CQuestClear15setClearedQuestEj        setClearedQuest(unsigned)
//
// UserQuest.cpp 以 plain data struct（unsigned char m_cleared[0x7530] 位于 +0x00）
// 使用本类型，并通过 extern "C" asm 标签直调上述符号；header 布局须与之 ABI 一致。
// ============================================================================

namespace WongWork
{
struct STClearedQuest
{
    char m_pad[0x14];
};

class CQuestClear
{
public:
    CQuestClear();
    ~CQuestClear();
    void clear();
    bool isClearedQuest(unsigned int questIdx) const;
    const STClearedQuest* getClearedQuest() const;
    void resetClearedQuests(unsigned int count);
    void setClearedQuest(const STClearedQuest* quest);
    static bool syncScript();
    void setClearedQuest(unsigned int questIdx);

    unsigned char m_cleared[0x7530];   // +0x00（0x7530，与 UserQuest.cpp 布局一致）
};

}  // namespace WongWork

#endif  // WONGWORK_QUESTCLEAR_H