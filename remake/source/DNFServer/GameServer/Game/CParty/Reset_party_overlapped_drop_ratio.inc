// b28d-solo-CParty: N 封顶 —— N82 帧 0xf8/0xe8 + 槽复用；215 vs 209
void CParty::Reset_party_overlapped_drop_ratio()
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b916e
    m_overlapRare = 0;
    m_overlapUnique = 0;
    int cntA = 0;   // item1 == 0x5e7f
    int cntB = 0;   // item1 0x5e80..0x5e87 区间 / ==0x5ed9
    int cntC = 0;   // item2 0x5693..0x569a 区间 / item1==0x5ed9
    int cntD = 0;   // item3 0x4ecb..0x4ed2 区间 / item1==0x5ed9
    char cntE = 0;  // 累计命中数
    char flagF = 0; // cntE == 3
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        // [P296] 正向 if 包裹（ORIG 0x085b91ee `movzbl; test %al,%al; je <loop 底>` 无
        // xor$1 物化）：原码第二守卫是正向块，非第二个 continue。
        if (m_battleData.m_arr54[i] != 0)
        {
            cntE = 0;
            // [G7-sub] ORIG 0x085b923c / 0x085b9289 / 0x085b92d3：
            //   `mov -0xe1(%ebp),%eax; mov %eax,-0x14(%ebp)`（sret 缓冲 +2 字段 → 具名
            //   int -0x14），-0x10（item2）、-0xc（item3）同形 ⇒ 原码有 3 个具名 int
            //   （P0：有槽往返即有局部）。此后 9 处比较 ORIG 全是 `cmpl $imm,-0x14(%ebp)`
            //   内存直比；旧码直读 sret 缓冲字段 ⇒ 每处多 1 条 load（我方 217 vs 210），
            //   且帧小 0xc（0xe8 vs ORIG 0xf8）。
            unsigned int add1 =
                m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0x13).m_addInfo;
            unsigned int add2 =
                m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0x12).m_addInfo;
            unsigned int add3 =
                m_member[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0x11).m_addInfo;
            if (add1 > 0x5e7f && add1 <= 0x5e87)
            {
                cntB++;
                cntE++;
            }
            else if (add1 == 0x5e7f)
            {
                cntA++;
            }
            else if (add1 == 0x5ed9)
            {
                cntB++;
                cntE++;
            }
            if (add2 > 0x5692 && add2 <= 0x569a)
            {
                cntC++;
                cntE++;
            }
            else if (add1 == 0x5ed9)
            {
                cntC++;
                cntE++;
            }
            if (add3 > 0x4eca && add3 <= 0x4ed2)
            {
                cntD++;
                cntE++;
            }
            else if (add1 == 0x5ed9)
            {
                cntD++;
                cntE++;
            }
            if (cntE == 3)
            {
                flagF = 1;
            }
        }
    }
    if (cntA != 0)
        m_overlapRare += cntA + 2;
    if (cntB != 0)
    {
        m_overlapRare += (cntB + 2) * 2;
        m_overlapUnique += cntB + 2;
    }
    if (cntC != 0)
    {
        m_overlapRare += (cntC + 2) * 2;
        m_overlapUnique += cntC + 2;
    }
    if (cntD != 0)
    {
        m_overlapRare += (cntD + 2) * 2;
        m_overlapUnique += cntD + 2;
    }
    if (flagF != 0)
    {
        m_overlapRare += 5;
        m_overlapUnique += 2;
    }
}