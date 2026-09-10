void CDataManager::set_original_dimensionInout(int idx, char value)
{
    if (idx > 5)
        goto end;
    m_originalDimensionInout[idx] = value;
end:
    ;
