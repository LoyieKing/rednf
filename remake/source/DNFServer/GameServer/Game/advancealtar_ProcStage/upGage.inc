void CharacData::upGage()
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;
    // ORIG 求值序：先 aux[level]（→edx）后 *m_strPtr（→eax）再 imul ⇒ 交换书写序；
    //   索引位 `static_cast<unsigned int>(m_gageLevel)` 复现 ORIG 两段装载
    //   `mov 0x8(%eax),%eax; mov %eax,%edx`（P319；裸 int 索引 4.4 恒折单段）。
    int maxGage = cfg->m_gageUpAux[static_cast<unsigned int>(m_gageLevel)] *
                  *reinterpret_cast<int*>(cfg->m_strPtr);
    if (m_gage >= maxGage)
    {
        m_gage = maxGage;
        sendTestCheckGage();
    }
    else
    {
        float ratio = (float)cfg->m_gageUpRate[static_cast<unsigned int>(m_gageLevel)] /
                      (float)*reinterpret_cast<int*>(cfg->m_strPtr);
        int inc = (int)((float)cfg->m_gageUpRate[0] * ratio);
        m_gage = m_gage + inc;
        sendTestCheckGage();
    }
