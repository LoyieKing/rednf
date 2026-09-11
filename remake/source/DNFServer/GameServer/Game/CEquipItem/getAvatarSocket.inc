// ORIG 0x8150f36：填充 emblem 的小孔类型。
// 1) 先将 m_vec180（vector<ushort>）逐项写入 emblem.stEmblemSocket[i].emblemSocketType_
// 2) 依 slot 选择 m_avatarSelect 项并写入其 socketType 数组（index 续接）
// [b28d-solo-CEquipItem] 逐条还原（ORIG 0x8150f36）：
//   ① 第一循环 ORIG 无 index<5 上限（P140：ORIG 无防御就照抄直访）且 end() 循环前求值一次
//      （P13 缓存 end）；
//   ② slot 范围检查与 empty() 检查是**两个独立 if**（ORIG 各出一段 bool 物化，
//      P133 独立守卫不可合并）；
//   ③ slot==-1 分支同样缓存 end()；
//   ④ 残余：ORIG `add $0x8,%ecx; movzwl 0x4(%eax,%ecx,2)` 是 4.1 对 +0x14 数组下标的
//      add+disp 拆形，4.4 恒折单 disp 0x14(%eax,%edx,2)（P30/N4 结构性伪影，封顶）。
void CEquipItem::getAvatarSocket(int slot, stAvatarEmblemInfo_t& emblem) const
{
    size_t index = 0;
    std::vector<unsigned short>::const_iterator it = m_vec180.begin();
    std::vector<unsigned short>::const_iterator e = m_vec180.end();
    for (; it != e; ++it)
    {
        emblem.stEmblemSocket[index].emblemSocketType_ = *it;
        index++;
    }

    if (slot < -1 || slot >= (int)m_avatarSelect.size())
        return;
    if (m_avatarSelect.empty())
        return;

    int selIndex = slot;
    if (slot == -1)
    {
        // 找第一个 m_field10 > 0 的选项
        std::vector<stAvatarTypeSelect_t>::const_iterator it2 =
            m_avatarSelect.begin();
        std::vector<stAvatarTypeSelect_t>::const_iterator e2 =
            m_avatarSelect.end();
        while (it2 != e2)
        {
            ++selIndex;
            if (it2->m_field10 > 0)
                break;
            ++it2;
        }
        if (it2 == e2)
            return;
    }

    const stAvatarTypeSelect_t* p = &m_avatarSelect.at(selIndex);
    if (index + (size_t)p->m_field10 > 4)
        return;

    // ORIG 增量段序：先 index++ 再 i++（`addl $0x1,-0x18` 在 `addl $0x1,-0xc` 之前）。
    for (int i = 0; i < p->m_field10; ++index, ++i)
        emblem.stEmblemSocket[index].emblemSocketType_ = p->m_socketType[i];
}