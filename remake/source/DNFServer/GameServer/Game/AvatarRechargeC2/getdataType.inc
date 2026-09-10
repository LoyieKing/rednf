int advancealtar::AdvanceAltarShopParameter::getdataType(FieldType::T ft)
{
    if (static_cast<unsigned int>(ft) < 8)
    {
        register unsigned int bit = 1u;
        bit <<= ft;
        if ((bit & 0xc8u) != 0)
            return 0;
    }
    return 1;
