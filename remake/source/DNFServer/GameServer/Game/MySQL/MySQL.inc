// [T1-GI · P327] MySQL ctor 定义前移为本 TU 首个「强符号 · 外部链接 · 本 TU 定义」
// 实体 ⇒ GI 键名 `_GLOBAL__I__ZN5MySQLC2Ev`（ORIG 0x83f52ff；4.4 外联 ctor 发 C1+C2，
// C2 先入 assemble 队列）。改前键名 = `_GLOBAL__I__ZN9CDnFTimer11SetLastTimeEv`。
MySQL::MySQL() throw()
{
    m_dbHandle = 0;
    m_pQueryCounter = 0;
    m_bQueryCounterEnable = false;
}