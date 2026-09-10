LevelupSupport2ndEventManger::LevelupSupport2ndEventManger()
    : m_script()
{
    // 显式列出基类构造会让 gcc 4.4 在 Singleton 构造后插入 movl $0,(%eax)
    // （ORIG 无此清零）；省略基类（默认构造）以匹配 ORIG 字节。
