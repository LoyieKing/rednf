// [D08 2026-09-10] DIFF 61/62 → **AE 61/61**（TU 三数 ae 20→21、diff 5→4；p167
//   CLEAR 5/5）。修法 = 配方⑦/P0：删「ORIG 无槽」的具名 `unsigned char over` 局部，
//   整个短路条件（`abs(dx)>0x12c || abs(dy)>0x12c`）内联进 if。ORIG 0x08697d00 末尾为
//   `mov $0x1/%eax; jmp; mov $0x0/%eax; test %al,%al; je`（bool 物化**直测**、全程无
//   栈槽往返）⇒ 原码无该局部；具名局部使 4.4 发 `movb %al,-0x9(%ebp);
//   cmpb $0x0,-0x9(%ebp)` 且帧 `sub $0x24` vs ORIG `sub $0x14`（多 0x10）。
//   内联后逐条全等（帧 0x14 亦归位）。旧 N18a/N96 判词作废。
char CBuffGroup::IsRight(CUser* user)
{
    if (user->get_charac_level() > 0x45)
        return 0;
    if (__builtin_abs((int)(unsigned short)user->get_posX() -
                      (int)(unsigned short)m_user->get_posX()) > 0x12c
        || __builtin_abs((int)(unsigned short)user->get_posY() -
                         (int)(unsigned short)m_user->get_posY()) > 0x12c)
        return 0;
    return 1;
}