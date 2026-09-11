// verifyMaterialItemByRule（ORIG 0x0810fcea）
// 不缓存 script（P36：ORIG 每次重读 this+0x1c）；flag!=0 分支 ORIG 对
// size()/getCurUseBringUpMaterialTotal 各调用两次（P48 无 CSE），条件先 size
// 后 total（setbe 方向）。
// [b28d-L14 N82附] ORIG 0x0810e7c4；p167 CLEAR（10/10）；残差=bool 汇聚物化+槽深。
// [b28d-W8EventB 本波归零 AE 94/94] 两处旋钮（旧判词「N 封顶 11 条」作废）：
// ①结构：flag!=0 分支只做 `if (idx!=item) return false;`，命中路径落出
//   if/else 到函数尾 `return true;` ⇒ 复现 ORIG 0x810fe0f 尾部 `mov $1,%eax`
//   fall-through 到 epilogue（P191；旧「就地 mov $1;jmp」差异消除）；
// ②idx 类型 int→unsigned int：ORIG 0x810fd65 `lea 0x10(%eax),%edx` 先物化
//   容器基址再载 idx（this-first），int 下标 4.4 恒 idx-first（P218 探针）。
// 语义/调用点全等（p167 CLEAR 10/10）。
// [L14 round2 复核] 95/94：唯一差异 = ORIG 0x0810fcea 段在**循环回边目标前**多 1 个
// 4.1 对齐 nop（orig[37]，紧跟 `call GetInvenSlot; movzbl; mov %eax,-0xc` 之后），
// 4.4 不发同形填充 ⇒ N112 封顶（§6.1 口径）。
bool CObjectBringUp::verifyMaterialItemByRule(CUser* user,
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
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
}