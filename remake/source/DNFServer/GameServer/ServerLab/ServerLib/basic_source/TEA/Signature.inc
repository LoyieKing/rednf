void CTEA::Signature(char* pcSig)
{
    char acSigData[23] = {0};
    strcat(acSigData, "TEA");
    int iLen = strlen(acSigData);
    memcpy(acSigData + iLen, m_apKey, m_keylength);
    sprintf(acSigData + m_keylength + iLen, "%d%d", m_iMode, m_iPadding);
    CSHA oSHA;
    oSHA.AddData(acSigData, strlen(acSigData));
    oSHA.FinalDigest(pcSig);
