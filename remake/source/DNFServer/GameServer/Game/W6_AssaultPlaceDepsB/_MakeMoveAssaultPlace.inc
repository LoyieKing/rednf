void CAssaultPlace::_MakeMoveAssaultPlace(PacketGuard* p)
{
    p->put_header(0, 0x80);
    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82e8554 直读 +0x104（无 getter call）。
    p->put_int(m_field104);                     // +0x104
    p->put_byte(m_pos.m_x);                     // +0x118 vill（既沿头字段名）
    p->put_byte(m_pos.m_y);                     // +0x11c area
    unsigned short leaderUid0 = 0;
    if (m_checkUser != 0)                                       // +0x134
        leaderUid0 = m_checkUser->get_unique_id();
    p->put_short(leaderUid0);
    unsigned short leaderUid1 = 0;
    if (m_field138 != 0)                                        // +0x138
        leaderUid1 = m_field138->get_unique_id();
    p->put_short(leaderUid1);    // +0x138

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82e8611..0x82e86d0 槽序：
    //   i / count / countIdx；P191 双初始化形（声明=0 + for 初始化各一条）
    //   + P198③ 双槽置 0 两段式；i 为 unsigned（setbe/ja/jbe）。
    // 第一段：场内成员列表
    unsigned int i = 0;
    int count = 0;
    int countIdx = 0;
    countIdx = p->get_index();                  // PacketGuard 局部 idx 补丁槽
    p->put_byte(count);
    for (i = 0; i <= 7; ++i) {                  // cmpl $0x7 … setbe（无符号 ≤7）
        // P223：正极性 guard-continue 复现 ORIG `test %al,%al; jne <nop 岛>`。
        if (m_assaulters[i].Empty())
            continue;
        p->put_byte(i);
        p->put_byte(m_assaulters[i].GetTeam() + 1);       // lea 0x1(%eax)
        // P0：ORIG 无 uid 槽（`call; movzwl %ax,%edx` 直传）——不设具名局部。
        p->put_short(m_assaulters[i].GetUserUniqueId());
        count++;
    }
    p->put_byte(countIdx, count);

    // 第二段：同 vill/area 在线用户 id 列表（上限 10）
    // [b28d-solo-W6_AssaultPlaceDepsB] P0：ORIG 复用 count/countIdx 两槽（P198③
    //   双槽置 0 两段式），不新增 listCount/listIdx；置 0 顺序 countIdx 先于
    //   count（ORIG 0x82e86xx 清零序列 -0x18 → -0xc）。
    countIdx = 0;
    count = 0;
    countIdx = p->get_index();
    p->put_byte(count);
    std::vector<unsigned short> idList;
    G_GameWorld()->get_user_id_list(m_pos.m_x, m_pos.m_y, idList);
    for (i = 0; i < 10 && i < idList.size(); ++i) {
        p->put_short(idList[i]);
        count++;
    }
    p->put_byte(countIdx, count);
    p->finalize(true);
