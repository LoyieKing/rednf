// 0x08651b7a
// ORIG：把 m_characList（vector<_Charac_info> @ +0x796e8）中下标 a 的元素移动到
// 下标 b 处（a<b 时先 insert(begin()+b+1, v[a]) 再 erase(begin()+a)；
// a>b 时 insert(begin()+b, v[a]) 再 erase(begin()+a+1)），随后
// enableSaveCharacView(&m_characterView)。越界（a/b >= size）直接返回。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651b7a | _ZN5CUser16ChangeCharacSlotEjj
void CUser::ChangeCharacSlot(unsigned int a, unsigned int b)
{
    // b28d-GrpD P59/P222：ORIG +0x12..+0x3f 是**单条件取反 + 早期 return**（bool 物化），
    //   非嵌套 if：`call size; cmp 0xc(%ebp),%eax; jbe +0x31`（A 假→mov $1）→
    //   `call size; cmp 0x10(%ebp),%eax; ja +0x38`（B 真→mov $0）→ `test %al,%al;
    //   jne +0x266`（跳函数尾）。⇒ 源码 `if (!(a < size && b < size)) return 0;`
    //   我方旧双层嵌套 if 产两段独立分支、无 bool 物化（ORIG 的两段短路口才落 $1/$0 岛）。
    if (!(a < m_characList.size() && b < m_characList.size()))
        return;
    if (a < b)
    {
        m_characList.insert(m_characList.begin() + b + 1, m_characList[a]);
        m_characList.erase(m_characList.begin() + a);
    }
    else
    {
        m_characList.insert(m_characList.begin() + b, m_characList[a]);
        m_characList.erase(m_characList.begin() + a + 1);
    }
    m_characterView.enableSaveCharacView();
    return;
}