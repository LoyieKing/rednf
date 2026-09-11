// ORIG T 0x080e48fc：位展开为索引清单（clear + 双层 setle 循环 + push_back）。
void CCharacInformNotice::getInformNoticeFlagIndex(std::vector<unsigned char>& out)
{
    out.clear();
    for (int byteIdx = 0; byteIdx < InformNotice::kInformNoticeIndex; ++byteIdx)
    {
        unsigned char bits = static_cast<unsigned char>(getInformNoticeFlag(byteIdx));
        for (int bitIdx = 0; bitIdx < InformNotice::kInformNoticeBit; ++bitIdx)
        {
            if (((bits >> bitIdx) & 1) != 0)
            {
                out.push_back(static_cast<unsigned char>(
                    byteIdx * InformNotice::kInformNoticeBit + bitIdx));
            }
        }
    }
}