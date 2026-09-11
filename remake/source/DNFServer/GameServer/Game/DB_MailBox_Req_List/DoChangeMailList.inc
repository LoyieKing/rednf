// ============================================================================
// DoChangeMailList —— ORIG 0x0841d470（void）：清 mailIdx 表头；postal 数为 0 时
//   +0x79c 直接置 param_3；否则 +0x79c=0 并逐项取 max(postal_id) 回写 +0x79c，
//   mailIdx 表收 postal 的 +0x2e add_info。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=AE（check_tu_routed，56=56）。修法：
// ①SIG_MAILBOX_LIST 权威头新增 m_box 具名视图（SigTypes_D.h，R2 广播）——单 disp
//   count/0x79c + `mov (%edx,%eax,1)` postal_id + `add $0x20; mov 0xe` add_info；
// ②if/else 极性反写复现 `test; je <count==0 块>` 块序；③max 目标地址不再被 4.4
//   跨 call 缓存（cast 视图形恒缓存于 %ebx，本型视图形不缓存，探针 /tmp/p2/w.cpp
//   w1 vs /tmp/p2/rm2.cpp）。
void DoChangeMailList(SIG_MAILBOX_LIST* list, unsigned int* mailIdx, int count)
{
    mailIdx[0] = 0;
    // [b28d-solo-DB_MailBox_Req_List] 已按 ORIG 布局改写：①SIG_MAILBOX_LIST 权威头
    // 新增 m_box 具名视图（SigTypes_D.h，R2 已广播）后成员访问复现单 disp
    // （count/0x79c）与 `add $0x20; mov 0xe` 两级尾段、且装载序为 ORIG 的
    // 「下标先、基址后」（cast 视图形恒 base 先，P235）；②if/else 极性反写，复现
    // ORIG `test; je <count==0 块>`（then 块落尾）的块序。
    // 残余：`std::max` 赋值目标地址被 4.4 跨 call 缓存于 %ebx（ORIG 4.1 在 call 后
    // `mov 0x8(%ebp),%eax` 重算），四形探针（视图/宏/混合/别名视图）均恒缓存 ⇒
    // 跨版本代码生成差，记 N（+push/pop %ebx）。
    if (list->m_box.m_count != 0)
    {
        list->m_box.m_maxPostalId = 0;
        for (int i = 0; i < list->m_box.m_count; ++i)
        {
            unsigned int id = (unsigned int)list->m_box.m_postals[i].m_postalId;
            list->m_box.m_maxPostalId = std::max(list->m_box.m_maxPostalId, id);
            mailIdx[0] = mailIdx[0] + 1;
            mailIdx[mailIdx[0]] = list->m_box.m_postals[i].m_tail.m_addInfo;
        }
    }
    else
    {
        list->m_box.m_maxPostalId = (unsigned int)count;
    }
}