// ORIG 0x0891aa3a：单循环内逐组「置零 int + clear 向量」，末尾 clear 首向量。
STAncientRareAvatarCube::STAncientRareAvatarCube()
{
    for (int i = 0; i < 10; ++i)
    {
        m_field0c[i] = 0;
        m_vecs[i].clear();
    }
    m_vec.clear();
}