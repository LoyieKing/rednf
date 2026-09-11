// ---------------------------------------------------------------------------
// ORIG T 0x82e8b66 (size 0xf3)：_MakeUserInfo(PacketGuard*, ENUM_USERINFO)
// 包 0x02：byte(type)+short(count)+逐人 basic_info。
// ---------------------------------------------------------------------------
// [b28d-62] AE：删 P171 多余空指针守卫 + Empty() 正极性 continue + P134 user
//   免槽内联；71=71，p167 CLEAR。ORIG 0x082e8b66
void CAssaultPlace::_MakeUserInfo(PacketGuard* p, ENUM_USERINFO type)
{
    // [b28d-62] P171：ORIG 0x082e8b6e 无空指针守卫，删多余 `if (p==0) return;`。
    p->put_header(0, 2);
    p->put_byte((int)type);
    int indexSlot = 0;
    int count = 0;
    indexSlot = p->get_index();
    p->put_short(count);
    for (int i = 0; i < 8; ++i) {
        // [b28d-62] ORIG 0x082e8be3 `test %al,%al; jne <循环尾>` ⇒ `if (Empty()) continue;`
        //   （正极性直测）；user 免槽直用（P134，ORIG 无 -0xc 指针槽）。
        if (m_assaulters[i].Empty())
            continue;
        m_assaulters[i].GetUser()->make_basic_info((char*)p, (char)type);
        count = count + 1;
    }
    p->put_short(indexSlot, count);
    p->finalize(true);
}