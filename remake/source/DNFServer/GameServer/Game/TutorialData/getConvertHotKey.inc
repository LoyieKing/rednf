// ORIG _ZN12TutorialData16getConvertHotKeyEi @0x08ab885c
int TutorialData::getConvertHotKey(int hotKey)
{
    if (hotKey < 0 || hotKey > 12) {
        return -1;
    }
    if (hotKey <= 5) {
        return hotKey;
    }
    return hotKey + 0xc0;
}