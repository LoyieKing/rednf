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
