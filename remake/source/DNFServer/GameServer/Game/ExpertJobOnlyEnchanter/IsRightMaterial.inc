int CEnchanter::IsRightMaterial()
{
    for (int i = 0; i <= 1; ++i)
    {
        int right = 0;
        if ((right = m_materials[i].IsRight()) != 0)
            return right;
    }
    return 0;
