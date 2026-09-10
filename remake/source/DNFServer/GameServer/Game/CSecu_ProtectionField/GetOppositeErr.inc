int CSecu_ProtectionField::GetOppositeErr(int err)
{
    switch (err)
    {
    case 0x7a: return 0xee;
    case 0x88: return 0xec;
    case 0x7b: return 0x7c;
    case 0x89: return 0x8a;
    case 0x72:
    case 0xe4:
    case 0xe5: return 0x73;
    default: return 1;
    }
