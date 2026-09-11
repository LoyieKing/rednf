bool ExceptionItem::find_index(int index)
{
    // ORIG 0x81a210c：find 先于 end（求值序），且尾为 test/je/mov$1/jmp/mov$0
    // 分支归一化（P56）——具名迭代器 + if(cond)return true 复现。
    std::map<int, int>::iterator it = m_exceptionItemIndex.find(index);
    if (it != m_exceptionItemIndex.end())
        return true;
    return false;
}