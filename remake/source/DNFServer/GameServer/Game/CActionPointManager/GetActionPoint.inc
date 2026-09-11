// ORIG 0x8694bfc（W，内联）：mov 0x8(%ebp),%eax; add $0x8,%eax; ret。
// 返回嵌入的 _SIG_LOAD_ACTION_POINT 首地址；具体返回类型仍按现有 ABI
// 以字节指针暴露，避免把 packed 包误当成普通对齐对象。
unsigned char* CActionPointManager::GetActionPoint() const
{
    return reinterpret_cast<unsigned char*>(
        const_cast<_SIG_LOAD_ACTION_POINT*>(&m_sig));
}