// ----------------------------------------------------------------------------
// 构造（[GI 调序 P327] 无参 ctor 定义前移为本 TU 首个强符号 ⇒
// `_GLOBAL__I__ZN27CSharedServerMessageManagerC2Ev`，ORIG 0x086019b4；
// 只移函数定义，不动任何带动态初始化的对象）
// ----------------------------------------------------------------------------
CSharedServerMessageManager::CSharedServerMessageManager()
{
    m_isSet = 0;
    m_event.clear();
}