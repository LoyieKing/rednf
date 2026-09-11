// ORIG 0x082ff988 CheckHellPartyCondition：
//   m_party 为空 → false；rate=(int)party->BattleData.m_hellPartyValueTotal(+0xb0c)；
//   rate >= get_rand_int(100) 且 rate >= stHellParty_t.m_guideLineValue(+0x90) → true。
// b28d-55（P0/P183）：ORIG 无 bd/vals 指针局部、无 ok 槽——party+0xb0c 两次
//   独立读取均单条 `flds 0xb0c(%eax)` 折叠，bool 汇聚在 %eax（g1 形探针
//   /tmp/b28d_l55/probe_bool.cpp）。故去掉中间指针缓存与具名 bool，改嵌套正向
//   if + return（P214①/P169）。残差：ORIG 尾部多一层 bool 归一岛（4.1 寄存器化
//   bool，4.4 具名局部必占槽）——P0 判读为无具名局部，落 N（跨版本 bool 物化）。
bool CBattle_Field::CheckHellPartyCondition()
{
    // b28d-solo-BattleDepsB2：探针 /tmp/b28d_bdb2/probe_cp2.cpp f_fwd_and 实证——ORIG 形
    // =「正向包裹 NULL 检查 + `&&` 短路条件 + 尾部 return false」：
    //   test;je<尾部 mov$0> … jl<内层 mov$0> ×2 ; mov$1;jmp ; mov$0 ;
    //   test %al,%al;je<尾部> ; mov$1 ; mov$0。嵌套 if（f_cur 形）反产
    //   setge/test 物化（+2 条）且 NULL 失败块前置。f_fwd_and 50 条 + ORIG
    //   特有的 G_CDataManager 二次调用 2 条 = ORIG 52 条逐条同形。
    if (m_party != NULL)
    {
        if ((int)m_party->m_battleData.m_hellPartyValueTotal >= get_rand_int(100) &&
            (int)m_party->m_battleData.m_hellPartyValueTotal >=
                static_cast<stHellParty_t*>(
                    G_CDataManager()->get_hellparty_script_values())->m_guideLineValue)
        {
            return true;
        }
    }
    return false;
}