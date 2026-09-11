bool DeleteInvalidItemScript::IsInvalidItem2Delete(int idx)
{
    // ORIG 0x08ac20e0：call operator== ; test %al,%al; je <mov $1> / mov $0;jmp —
    // 0/1 物化 je 岛（test 有 je 消费者，判据③→②直分支 if 形，非 !(x) sete 归一）。
    std::map<int, DeleteInvalidItemData>::iterator it = m_deleteMap.find(idx);
    if (it == m_deleteMap.end())
        return false;
    return true;
}