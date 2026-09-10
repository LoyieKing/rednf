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
