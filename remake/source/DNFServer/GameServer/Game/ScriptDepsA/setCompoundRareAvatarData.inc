// ORIG setCompoundRareAvatarData T 0x08902e78（报告全文转写）：开列式稀有选项，
// 权重累计入 m_cube.m_field0c[e]，pair 入 m_cube.m_vecs[e]。type==0x16 → ret0。
// [b28d-48] 已闭合（NDIFF=0 全等）：m_field0c 由 int[10] 改 unsigned int[10]
//（CCompoundAvatar.h；双证据：ORIG 0x08902e78 加法段 `mov 0x18(%eax,%ecx,4),%ecx;
// mov -0xc(%ebp),%eax; add %eax,%ecx` 仅字段 unsigned+RHS int 时 4.4 可复现（P274），
// 且 ORIG push_back 的 pair converting ctor 符号 `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`
// 第二参即 unsigned int&）。sizeof/偏移/mangled 不变。
bool setCompoundRareAvatarData(tagCompoundAvatar* avatar, ENUM_EQUIPMENTTYPE equip)
{
    // [b24ab] P48/P0：ORIG 直读 0xc(%ebp)（param_2），无 idx 具名槽；ken禁
    // bool 预清（ScanInt 恒写 ok）。loop 条件式 `(item=ScanInt(&ok)), ok`。
    if (static_cast<int>(equip) == 0x16)
        return false;
    bool ok;
    int item;
    while (true)
    {
        item = ScanInt(&ok);
        if (!ok)
            break;
        const int weight = ScanInt(&ok);
        if (!ok)
            return false;
        // [b28c-lane24] ORIG 0x08902e78 段：写索引(edx)先、读索引(ecx)后；
        // 显式双下标表达式 + 字段 unsigned 化后逐条全等（b28d-48 闭合）。
        avatar->m_cube.m_field0c[static_cast<int>(equip)] =
            avatar->m_cube.m_field0c[static_cast<int>(equip)] + weight;
        // [b24ab] ORIG `_ZNSt4pairIiiEC1IRiRjEEOT_OT0_`（converting ctor,
        // int&/unsigned&）+ push_back(EOS1_) ⇒ 直构 pair 非 make_pair（P192）
        avatar->m_cube.m_vecs[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item,
                                avatar->m_cube.m_field0c[static_cast<int>(equip)]));
    }
    return true;
}