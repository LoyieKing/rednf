bool CObjectBringUp::verifyMaterialItemByRule(CUser* user,
                                              unsigned int itemIdx)
{
    // ORIG 0x0810fcfd `test %al,%al; je +0xa7`：flag==0 分支（循环）位于函数尾、
    // flag!=0 分支（idx 计算）直落在前 ⇒ 正向包裹 + 循环后置（P59/P222 块序判据）。
    // idx 预清 `movl $0x0,-0xc(%ebp)` 在 size() 调用前（P198①两句式）。
    if (static_cast<ObjectBringUpScript*>(m_script1c)->m_exchangeRuleFlag != 0)
    {
        // [b28d-W8EventB 实测] idx 用 unsigned int：ORIG 0x810fd65 `lea 0x10(%eax),%edx`
        // 先物化容器基址再载 idx（this-first）；idx 为 int 时 4.4 恒 idx-first（P218）。
        unsigned int idx = 0;
        if (static_cast<ObjectBringUpScript*>(m_script1c)
                ->m_materialItemIdxs.size() <=
            (unsigned int)user->getCurUseBringUpMaterialTotal())
            idx = (unsigned int)static_cast<ObjectBringUpScript*>(m_script1c)
                      ->m_materialItemIdxs.size() - 1;
        else
            idx = (unsigned int)user->getCurUseBringUpMaterialTotal();
        if ((unsigned int)static_cast<ObjectBringUpScript*>(m_script1c)
                ->m_materialItemIdxs[idx] != itemIdx)
            return false;
    }
    else
    {
        for (std::vector<int>::iterator it =
                 static_cast<ObjectBringUpScript*>(m_script1c)
                     ->m_materialItemIdxs.begin();
             it != static_cast<ObjectBringUpScript*>(m_script1c)
                       ->m_materialItemIdxs.end();
             ++it)
        {
            if ((unsigned int)*it == itemIdx)
                return true;
        }
        return false;
    }
    // ORIG 0x810fe0f `mov $0x1,%eax` 位于函数尾并 fall-through 到 epilogue
    // （P191：让目标常量走 fall-through）⇒ flag!=0 命中路径落出 if/else。
    return true;
