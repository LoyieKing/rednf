// ============================================================================
// CBattle_Field::convertAPCRoleType2MonsterRoleType(char) const —— ORIG T 0x0830a812。
// 5→0（普通） 6→1（Boss） 7→2（精英） 8→3（特殊），其余原样返回。
// 声明在 CBattle_Field.h:400；本 TU 为该符号唯一定义点。§4.5：删 asm 别名桥，
// 改真实成员定义，调用点自然发射同名 T 符号。
// ============================================================================
char CBattle_Field::convertAPCRoleType2MonsterRoleType(char role) const
{
    switch (role)
    {
    case 5: return 0;
    case 6: return 1;
    case 7: return 2;
    case 8: return 3;
    default: return role;
    }
}