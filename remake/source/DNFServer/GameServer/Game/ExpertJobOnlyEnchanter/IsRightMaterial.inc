// ---------------------------------------------------------------------------
// IsRightMaterial（报告 IsRightMaterial.md，ORIG T 0x849c9b2 sz 0x53）：
// 两槽依次 IsRight()，首个非 0 值即返回；全 0 返 0。
// ---------------------------------------------------------------------------
int CEnchanter::IsRightMaterial()
{
    for (int i = 0; i <= 1; ++i)
    {
        int right = 0;
        if ((right = m_materials[i].IsRight()) != 0)
            return right;
    }
    return 0;
}