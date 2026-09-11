// ============ LevelupSupport2ndEventManger C1/D1（报告转写） ============
// 报告：Singleton base → ScriptInterface base → 写 vptr →
// LevelupSupport2ndScript(+0x04)。D1：写 vptr → 析构脚本对象。
LevelupSupport2ndEventManger::LevelupSupport2ndEventManger()
    : m_script()
{
    // 显式列出基类构造会让 gcc 4.4 在 Singleton 构造后插入 movl $0,(%eax)
    // （ORIG 无此清零）；省略基类（默认构造）以匹配 ORIG 字节。
}