// [round3-L30 修] ORIG 0x08146d30：基类构造后**无** `mov 0x8(%ebp),%eax;
//   movl $0x0,(%eax)` 清零（我方显式列 Singleton/ScriptInterface 会插入该 2 条）
//   ⇒ 按本文件 LevelupSupport2ndEventManger 先例，省略基类（默认构造），
//   只留成员初始化（同 gcc 4.4 对「显式基类构造」的插入行为）。
LevelupSupportEventManger::LevelupSupportEventManger()
    : m_script(),
      m_stepIndexMap(),
      m_events()
{
}