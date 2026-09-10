void TraceLog::setMask()
{
    set_mask(0, G_Script()->findCharValue(0, 0xe));
    set_mask(1, G_Script()->findCharValue(0, 0xf));
    set_mask(2, G_Script()->findCharValue(0, 0x10));
    set_mask(3, G_Script()->findCharValue(0, 0x11));
    set_mask(4, G_Script()->findCharValue(0, 0x12));
    set_mask(5, G_Script()->findCharValue(0, 0x13));
    set_mask(6, G_Script()->findCharValue(0, 0x14));
    set_mask(7, G_Script()->findCharValue(0, 0x15));
    set_mask(8, G_Script()->findCharValue(0, 0x16));
