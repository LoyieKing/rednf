int CUser::ChangeCharacSlot(unsigned int a, unsigned int b)
{
    // b28d-GrpD P59/P222：ORIG +0x12..+0x3f 是**单条件取反 + 早期 return**（bool 物化），
    //   非嵌套 if：`call size; cmp 0xc(%ebp),%eax; jbe +0x31`（A 假→mov $1）→
    //   `call size; cmp 0x10(%ebp),%eax; ja +0x38`（B 真→mov $0）→ `test %al,%al;
    //   jne +0x266`（跳函数尾）。⇒ 源码 `if (!(a < size && b < size)) return 0;`
    //   我方旧双层嵌套 if 产两段独立分支、无 bool 物化（ORIG 的两段短路口才落 $1/$0 岛）。
    if (!(a < m_characList.size() && b < m_characList.size()))
        return 0;
    if (a < b)
    {
        // b28d-solo-CUser p167：ORIG 0x08651b7a 的 `begin()+b+1` 是**两次**
        //   operator+（迭代器 +b 再 +1；我方 `begin()+(b+1)` 仅一次 ⇒ MISSING ×2）。
        m_characList.insert(m_characList.begin() + b + 1, m_characList[a]);
        m_characList.erase(m_characList.begin() + a);
    }
    else
    {
        m_characList.insert(m_characList.begin() + b, m_characList[a]);
        m_characList.erase(m_characList.begin() + a + 1);
    }
    m_characterView.enableSaveCharacView();
    return 0;
