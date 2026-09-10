long long CAdvanceAltarLog::makeKey(unsigned short itemType,
                                    unsigned int itemIndex,
                                    unsigned short keyC)
{
    return (static_cast<long long>(
                static_cast<int>((itemType << 16) | keyC))
            << 32) |
           static_cast<long long>(itemIndex);
