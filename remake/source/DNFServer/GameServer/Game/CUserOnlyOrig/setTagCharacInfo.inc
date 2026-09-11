// ---------------------------------------------------------------------------
// 13. CUser::setTagCharacInfo @0x08690098
//     m_characList[idx] 元素字段:+0 no / +4 name(strncpy 0x1d) / +0x26 job byte /
//     +0x27 grow ushort / +0x29 扩展 byte / +0x1239 byte / +0x88 起 memcpy 0x52。
//     TAG_CHARAC_DATA 源偏移:+0/+4/+0x21/+0x22(w)/+0x24/+0x25/+0x26(0x52)。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08690098：p167=M0/E0:，gap=-23；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x08690098：P196 有符号界判 + P214① 正极性包裹
// + P30 显式元素取址（strncpy src）三修归零（见函数内注释）。
// [Lane3 spec 2026-09-10] 返回类型 int→bool（CUser.h:3753 已 bool 化）：ORIG 调用点
// 0x084c360b `call; xor $0x1,%eax; test %al,%al; je`（P292/T3）。mangled 不含返回类型
// ⇒ 符号名/ABI 零变化；定义随头（C++ 要求声明与定义返回类型一致）。
bool CUser::setTagCharacInfo(int idx, unsigned int characNo, TAG_CHARAC_DATA* data)
{
    // [b28d-solo-CUserOnlyOrig P196/P310/P214①] ORIG 0x086900c9 `cmp 0xc(%ebp),%eax;
    // jle L_false`（有符号 size<=idx，短路物化 `js L_false` 同一出口）⇒ 正极性
    // 包裹 `if (idx >= 0 && size() > idx) { body } return 0;` 复现 ORIG 块序；
    // 旧 `(size_t)idx >= size()` 生无符号 `jb` + %ebx 缓存 + 反向块序。
    if (idx >= 0 && static_cast<int>(m_characList.size()) > idx)
    {
        if (m_characList[idx].m_characNo != characNo)
            return 0;

        // 2026-09-04 §4.3：dst 视图→_Charac_info 具名字段；src 侧→TAG_CHARAC_DATA
        // 具名 wire 字段（SigTypes_C.h union 字段 by CUserMain）。
        // [b28d-solo-CUserOnlyOrig P167/P157] ORIG 0x086900xx 段命中
        // `vector<_Charac_info>::operator[](unsigned int)` ×6（含入口判等共 7 次），
        // 我方旧 `_Charac_info& ci = m_characList[idx];` 引用缓存只发 1 次 ⇒ 逐字段
        // 重写完整下标表达式（禁引用缓存）。
        m_characList[idx].m_characNo = data->m_characNo;                  // wire +0x00
        // [b28d-solo-CUserOnlyOrig P30] ORIG 0x0869016x `mov 0x14(%ebp),%eax;
        // lea 0x4(%eax),%ebx`（单条 lea 进 callee-saved）vs 数组退化实参
        // `add $0x4,%eax; mov %eax,%ebx` 两段 ⇒ 写显式元素取址。
        strncpy(m_characList[idx].m_name, &data->m_name[0], 0x1d);        // wire +0x04
        m_characList[idx].m_field26 = data->m_field21;                    // wire +0x21 → +0x26 job
        m_characList[idx].m_level = static_cast<short>(data->m_field22);  // wire +0x22 → +0x27
        m_characList[idx].m_field29.m_growthType = data->m_field24;       // wire +0x24 → +0x29
        m_characList[idx].m_field1239 = data->m_field25;                  // wire +0x25 -> +0x1239
        memcpy(&m_characList[idx].m_additioalInfo, &data->m_add, 0x52);   // 具名 _Additioal_info
        return 1;
    }
    return 0;
}