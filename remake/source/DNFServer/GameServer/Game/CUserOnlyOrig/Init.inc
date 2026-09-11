// ORIG T 0x0827b3f4 size 0xe：`mov 0x8(%ebp),%eax; movl $0x0,(%eax); ret`。
// 置为本 TU 第一个强符号（GI 键名载体）。
void cSecuUserAction::Init()
{
    m_state = 0;
}