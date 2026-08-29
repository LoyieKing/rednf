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
// 使用本类型，并通过真实 C++ 方法调用（asm 桥已删）；header 布局须与之 ABI 一致，
// 故 sizeof 保持 0x7530，save-flag 字节（+0x7530）以裸偏移访问。
// ============================================================================

namespace WongWork
{
// ORIG STClearedQuest 为 1 字节（C1 0x0848f768 = movb $0,(%eax) 实证；SIG_LOAD_QUEST
// 完成位图数组步长 1）。原 m_pad[0x14] 占位删除。
struct STClearedQuest
{
    unsigned char m_cleared;   // +0x00

    // ORIG W 0x0848f768：C1Ev 仅置首字节 0。
    STClearedQuest();
};

class CQuestClear
{
public:
    CQuestClear();
    ~CQuestClear();
    void clear();
    unsigned char isClearedQuest(unsigned int questIdx) const;
    const STClearedQuest* getClearedQuest() const;
    bool resetClearedQuests(unsigned int questIdx);   // ORIG 返回 1/0
    void setClearedQuest(const STClearedQuest* quest);
    static bool syncScript();                          // ORIG 恒返回 1
    bool setClearedQuest(unsigned int questIdx);       // ORIG 返回 1/0
    unsigned char checkSaveFlag() const;               // ORIG 返回 +0x7530 save-flag 字节
    int getClearedQuestNum() const;                    // ORIG 统计 m_cleared[1..0x752f] 非零数
    void resetSaveFlags();                             // ORIG 清零 +0x7530 save-flag
    void enumQuestClear(bool (*cb)(unsigned int, char*), ...) const;  // ORIG 遍历 1..0x752f

    unsigned char m_cleared[0x7530];   // +0x00（0x7530，与 UserQuest.cpp 布局一致）
};

}  // namespace WongWork

#endif  // WONGWORK_QUESTCLEAR_H