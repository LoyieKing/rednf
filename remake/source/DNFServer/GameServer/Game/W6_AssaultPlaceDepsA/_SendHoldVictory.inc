// ---------------------------------------------------------------------------
// ORIG T 0x82ecaee (size 0x2cd)：_SendHoldVictory(CUser*, CUser*, int)
// 包 0x109：双方连破名单广播（无队用单名，有队 4 成员名）。
// ---------------------------------------------------------------------------
// [L2-S5 2026-09-11 翻正：NEAR → **AE**，201=201] 手法 = **P212 全头部声明 +
//   声明序按 ORIG 槽深→浅**（旧判词「N98 声明序不可翻」被本形推翻）：
//   ① ORIG 槽实录（深→浅）：partyA -0x40 / s1 -0x3c / member1 -0x38 / name1 -0x34 /
//      len1 -0x30 / name1b -0x2c / len1b -0x28 / partyB -0x24 / s2 -0x20 /
//      member2 -0x1c / name2 -0x18 / len2 -0x14 / name2b -0x10 / len2b -0xc
//      （0x082ecb59..0x082ecd30 逐点实证；packet 恒 -0x4c 不动）。
//   ② 4.4 -O0 对**函数头**声明按「先声明 → 更深槽」分配（实验留痕：头部先声明
//      s2/member2 ⇒ s2 落最深 -0x40 反证），块内声明才按首次使用序 ⇒ 全提头部 +
//      按 ① 的深→浅序排列即逐槽归位（每个变量都落回 ORIG 槽址）。
//   ③ 实证读数：改前 s1 -0x38 / member2 -0x3c（-0x3c 被后用的 member2 抢占）；
//      改后逐槽全等、our 201 = orig 201、TU near 2→1 / ae 40→41。
//   ④ 标量（int/指针/size_t）提前声明零语义/零符号影响（P331 禁的是非 POD 类对象
//      提前；packet 保持首位、ctor 发射点不动）。p167 CLEAR。
//   [历史] b28d-62/b28d-solo 判 N98「声明序不可翻」——本形（**全**头部 + ORIG 槽序）
//   是当时未试的组合。ORIG 0x082ecaee
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
void CAssaultPlace::_SendHoldVictory(CUser* userA, CUser* userB, int winTeam)
{
    // [L2-S5 实验-C] P212 全头部声明（块内仅赋值）：声明序 = ORIG 槽**深→浅**
    //   （packet -0x4c → partyA -0x40 → s1 -0x3c → member1 -0x38 → name1 -0x34 →
    //    len1 -0x30 → name1b -0x2c → len1b -0x28 → partyB -0x24 → s2 -0x20 →
    //    member2 -0x1c → name2 -0x18 → len2 -0x14 → name2b -0x10 → len2b -0xc）。
    //   依据 ORIG 全头部槽实录（0x082ecb59/-0x40、0x082ecb92/-0x3c、0x082ecbad/-0x38、
    //   0x082ecbaf/-0x34、0x082ecbbd/-0x30、0x082ecc1a/-0x2c、0x082ecc28/-0x28、
    //   0x082ecc61/-0x24、0x082ecc91/-0x20、0x082ecca3/-0x1c、0x082eccb7/-0x18、
    //   0x082eccc5/-0x14、0x082ecd22/-0x10、0x082ecd30/-0xc）；标量（POD）提前
    //   声明不改语义、不进 mangled、零符号变化（P331 禁的是非 POD 类对象提前）。
    PacketGuard packet;          // -0x4c（类对象保持首位：ctor 发射点不动）
    CParty* partyA;              // -0x40
    int s1;                      // -0x3c
    CUser* member1;              // -0x38
    const char* name1;           // -0x34
    std::size_t len1;            // -0x30
    const char* name1b;          // -0x2c（无队分支）
    std::size_t len1b;           // -0x28
    CParty* partyB;              // -0x24
    int s2;                      // -0x20
    CUser* member2;              // -0x1c
    const char* name2;           // -0x18
    std::size_t len2;            // -0x14
    const char* name2b;          // -0x10（第二无队分支）
    std::size_t len2b;           // -0xc

    packet.put_header(0, 0x109);

    packet.put_byte((int)((CUser*)userA)->getPowerSide());
    packet.put_int(winTeam + 1);

    partyA = (CParty*)((CUser*)userA)->GetParty();
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ecb60 `cmpl $0x0,-0x40; je +0x10e`
    //   ⇒ 非零（有队）走 fall-through 有队分支、零落 +0x10e 无队块（P59 正极性）。
    if (partyA != 0) {
        packet.put_byte(partyA->get_member_count());
        for (s1 = 0; s1 < 4; ++s1) {
            member1 = (CUser*)partyA->get_user(s1);
            if (member1 != 0) {
                name1 = ((CUserCharacInfo*)member1)->getCurCharacName();
                // P0：ORIG strlen 结果落具名槽 -0x30，put_int/put_str 复用同一槽。
                len1 = strlen(name1);
                packet.put_int((int)len1);
                packet.put_str(name1, (int)len1);
            }
        }
    } else {
        packet.put_byte(1);
        name1b = ((CUserCharacInfo*)userA)->getCurCharacName();
        len1b = strlen(name1b);
        packet.put_int((int)len1b);
        packet.put_str(name1b, (int)len1b);
    }

    partyB = (CParty*)((CUser*)userB)->GetParty();
    if (partyB != 0) {
        packet.put_byte(partyB->get_member_count());
        for (s2 = 0; s2 < 4; ++s2) {
            member2 = (CUser*)partyB->get_user(s2);
            if (member2 != 0) {
                name2 = ((CUserCharacInfo*)member2)->getCurCharacName();
                len2 = strlen(name2);
                packet.put_int((int)len2);
                packet.put_str(name2, (int)len2);
            }
        }
    } else {
        packet.put_byte(1);
        name2b = ((CUserCharacInfo*)userB)->getCurCharacName();
        len2b = strlen(name2b);
        packet.put_int((int)len2b);
        packet.put_str(name2b, (int)len2b);
    }

    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}