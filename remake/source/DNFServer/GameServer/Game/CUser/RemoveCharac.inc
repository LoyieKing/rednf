// ORIG T 0x081b441a：按 characNo 线性查找并 erase（ORIG 经 FindIndexByCharacNo，
// 内联等价）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b441a | _ZN20UserMercenaryInfoMgr12RemoveCharacEj
void UserMercenaryInfoMgr::RemoveCharac(unsigned int characNo)
{
    // b28d-solo-CUser p167 修复：ORIG 0x081b441a 实形 =
    //   idx = FindIndexByCharacNo(characNo)；found=(idx!=-1 && idx<size())；
    //   if(found){ try{ erase(begin()+idx); }catch(...){} }
    //   （我方原线性循环缺 FindIndexByCharacNo + __cxa_begin/end_catch）。
    // b28d-solo-CUser 重写（2026-09-09，P226 条件向）：ORIG 无 found 具名局部——条件
    //   全程寄存器物化（`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`），旧
    //   `bool found` 被 4.4 落槽（movb $1,-0x9 / cmpb $0,-0x9）；且 ORIG 第二比较为
    //   **有符号** `jle`（size-idx）⇒ idx/size 两侧取 int。
    int idx = (int)FindIndexByCharacNo(characNo);
    if (({ idx != -1 && idx < (int)m_characs.size(); }))
    {
        try
        {
            m_characs.erase(m_characs.begin() + idx);
        }
        catch (...)
        {
        }
    }
}