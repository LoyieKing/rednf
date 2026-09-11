// ORIG 0x082ffa2e GetHellPartyDifficulty(type)：
//   六字节难度比例取自 stHellParty_t（type==1 → m_field84[6]，否则 m_field7e[6]
//   且非空队伍经 increase_hell_party_ratio 提升），rnd=get_rand_int(100) 落区间
//   [i]=1..5 返回 i，否则 0（下标 0 槽不参与）。
int CBattle_Field::GetHellPartyDifficulty(ENUM_DUNGEON_TYPE type)
{
    // ORIG 0x082ffa2e：ratios `{0}` 单零聚合标量（P202① movl+movw 两写）+
    // get_hellparty_script_values 各分支独立重调（P48）。
    char ratios[6] = { 0 };
    if ((int)type == 1)
    {
        memcpy(ratios, static_cast<stHellParty_t*>(
                   G_CDataManager()->get_hellparty_script_values())->m_field84, 6);
    }
    else
    {
        memcpy(ratios, static_cast<stHellParty_t*>(
                   G_CDataManager()->get_hellparty_script_values())->m_field7e, 6);
        if (m_party != NULL)
            m_party->increase_hell_party_ratio(ratios);
    }

    // ORIG 0x082ffa2e：for(i=1;i<=5;i++) 双入口（进入直接落底部守卫 +0xc6，
    //   setle/test/jne）+ `if(acc>=rnd) return i`（mov acc;cmp rnd;jl continue，
    //   与 ORIG +0xb5/+0xbb 同形）；守卫落空 return 0（P0 具名局部 acc/rnd/i）。
    int acc = 0;
    int rnd = get_rand_int(100);
    int i;
    for (i = 1; i <= 5; i++)
    {
        acc += ratios[i];                        // movsbl 有符号字节累加
        if (acc >= rnd)
            return i;
    }
    return 0;
}