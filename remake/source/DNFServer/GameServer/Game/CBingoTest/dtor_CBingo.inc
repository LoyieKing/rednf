// CBingo::~CBingo @ 0x080ca8f0（docs/class_func_reports/CBingo/~CBingo.md 全文已读）：
//   try { ~shared_ptr<CMTRand>(this+0x10) } catch { ~vector<int>(this); rethrow }
//   → ~vector<int>(this)。空体即编译器按逆声明序析构成员：
//   m_today(int)→m_rand(shared_ptr dtor)→m_fieldC(int)→m_boards(vector dtor)，
//   shared_ptr 先于 vector，与 ORIG 调用序一致。
CBingo::~CBingo()
{
}