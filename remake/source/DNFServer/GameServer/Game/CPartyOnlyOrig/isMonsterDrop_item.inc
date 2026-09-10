bool CParty::isMonsterDrop_item(int monsterIdx)
{
    map_item item;
    // ORIG 0x085a3b44 物化形：found 落 char 槽 -0x9 后 `cmpb $0;je` 直判
    // 需 unsigned char 声明（bool `!`/`==0` 出 movzbl;xor;test，4.1 vs 4.4
    // 指令选择差）；二段 m_count 真值取反 `movzbl;xor$1;test;je`。
    unsigned char found = reinterpret_cast<CBattle_Field*>(&m_battleField)
                              ->get_field_item(monsterIdx, item);
    // ORIG O@0x33..0x4b：found!=0 先 cmpb 直判（跳过 body → 返 1 岛），
    // 体内 m_count 真值取反 movzbl;xor$1;test（两 je 同达返 1 岛）。
    // b28d-solo-CPartyOnlyOrig：P0 判读 —— ORIG 第二判定全程寄存器直传
    // （无槽往返）⇒ 原码无 cnt 局部，取反表达式内联进短路条件。
    if (found != 0 && !item.m_count)
    {
        return false;
    }
    return true;
