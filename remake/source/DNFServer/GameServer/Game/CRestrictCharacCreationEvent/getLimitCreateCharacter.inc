// 1 → 限 8；其它 → 0。ORIG 汇编先 test/cmp/jmp 线性分派且 m_limitType 只读
// 一次（mov 0xc(%eax) → test → cmp $1 → jmp else）⇒ switch 决策树形态
// （P46：稀疏 switch 线性 cmp 链，叶块按 case 升序）。
int CRestrictCharacCreationEvent::getLimitCreateCharacter(int characCount) const
{
    switch (m_limitType)
    {
    case 0:
        if (characCount == 0)
        {
            return 7;
        }
        break;
    case 1:
        return 8;
    }
    return 0;
}