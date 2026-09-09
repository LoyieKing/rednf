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
// 布局：m_cleared[0x7530] @+0x00（STClearedQuest 元素，1 字节/项）、m_saveFlag @+0x7530，
// sizeof(CQuestClear)=0x7531（ORIG hasEpicQuest 0x086ad266 隐式拷贝 memcpy 长度直证）。
// 构造/析构为头内联定义：ORIG C1Ev 0x08694b92 / D1Ev 0x0822e4cc / STClearedQuest C1Ev
// 0x0848f768 全为 W 弱符号，且被 UserQuest.cpp、Dispatcher_Req_Urgent_Quest.cpp 等消费。
// ============================================================================

namespace WongWork
{
// ORIG STClearedQuest 为 1 字节（C1 0x0848f768 = movb $0,(%eax) 实证；SIG_LOAD_QUEST
// 完成位图数组步长 1）。原 m_pad[0x14] 占位删除。
struct STClearedQuest
{
    bool m_cleared;            // +0x00（b28d-Main：bool 元素直载 movzbl 解释 ORIG 0x0808bae0 体无 setne）

    // ORIG W 0x0848f768（sz 0xb：movb $0,(%eax)）：弱符号 ⇒ 头内联定义
    // （SIG_LOAD_QUEST 等消费 TU 亦实例化；P0：单标量成员无初始化列表差异）。
    STClearedQuest() { m_cleared = 0; }
};

class CQuestClear
{
public:
    // ORIG W 0x08694b92（sz 0x3e）：m_cleared[0x7530] 逐元素构造后调 clear()。
    // 弱符号 ⇒ 头内联定义（UserQuest.cpp / Dispatcher_Req_Urgent_Quest.cpp 亦调用 C1Ev）。
    CQuestClear() { clear(); }
    // ORIG W 0x0822e4cc（D1Ev/D2Ev，sz 5）：空析构，同为头内联弱定义。
    ~CQuestClear() {}
    void clear();
    bool isClearedQuest(unsigned int questIdx) const;  // b28d-Main: P292 判别法证 bool（ORIG 调用点 0x0819968f/0x083550cd/0x085a8320 均 `call; xor $0x1,%eax; test %al,%al; je`）
    const STClearedQuest* getClearedQuest() const;
    bool resetClearedQuests(unsigned int questIdx);   // ORIG 返回 1/0
    void setClearedQuest(const STClearedQuest* quest);
    static bool syncScript();                          // ORIG 恒返回 1
    bool setClearedQuest(unsigned int questIdx);       // ORIG 返回 1/0
    unsigned char checkSaveFlag() const;               // ORIG 返回 +0x7530 save-flag 字节
    int getClearedQuestNum() const;                    // ORIG 统计 m_cleared[1..0x752f] 非零数
    void resetSaveFlags();                             // ORIG 清零 +0x7530 save-flag
    void enumQuestClear(bool (*cb)(unsigned int, char*), ...) const;  // ORIG 遍历 1..0x752f

    STClearedQuest m_cleared[0x7530];  // +0x00（0x7530；元素类型化以复现 ORIG
                                       //   C1 per-element call 循环形，b27 判定）
    unsigned char m_saveFlag;          // +0x7530（ORIG hasEpicQuest 0x086ad266 隐式拷贝
                                       //   构造 memcpy 长度 0x7531 = sizeof 直证；
                                       //   checkSaveFlag/resetSaveFlags 访问该字节）
};

}  // namespace WongWork

static_assert(sizeof(WongWork::STClearedQuest) == 1,
              "STClearedQuest 1-byte bitmap element (ORIG 0x0848f768 步长 1)");
static_assert(sizeof(WongWork::CQuestClear) == 0x7531,
              "CQuestClear ABI size: m_cleared[0x7530] + m_saveFlag@+0x7530");

#endif  // WONGWORK_QUESTCLEAR_H