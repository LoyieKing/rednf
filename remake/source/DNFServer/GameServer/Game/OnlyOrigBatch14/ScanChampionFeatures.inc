void ScanChampionFeatures(int count, STAICharacterMapArrangeData& data)
{
    std::string s;
    for (int line = 0; line < count; ++line)
    {
        ScanStr(&s);
        for (int i = 0; i <= 0x24; ++i)
        {
            if (s == g_championFeatureName[g_championFeatureValue[i] & 0xff])
            {
                data.m_championFeatures.push_back(
                    (int)g_championFeatureValue[i]);
                break;
            }
        }
    }
