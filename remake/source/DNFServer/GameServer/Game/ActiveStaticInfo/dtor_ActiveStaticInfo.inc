ActiveStaticInfo::~ActiveStaticInfo()
{
    --g_ActiveStaticInfoCount;
    g_ActiveStaticInfoSize = g_ActiveStaticInfoCount * 0x2f0;
