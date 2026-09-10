void STSkillScript::Clear()
{
    m_field0 = 0;
    m_str4 = "";
    m_str8 = "";
    m_14 = 0;                            // ORIG 0x83750f2：m_14 在 m_strc 之前
    m_strc = "";
    m_vec18.clear();
    // [b28d-63] ORIG 0x8375128/0x8375138：+0x204/+0x210 的 clear 紧接 +0x18
    // 之后（P167 原报 MISSING vector<int>::clear ×3）。
    m_vec204.clear();
    m_vec210.clear();
    m_vec24.clear();
    m_30 = 1;
    m_34 = 1;
    m_38 = 0;
    m_3c = 1;
    m_40 = 1;
    m_vec44.clear();
    m_vec50.clear();
    m_5c = 0;
    m_60 = 0;
    // ORIG 83751ad/83751b4 双 `movl $0,-0x10`：原码 `int i = 0;` + `for(i=0;…)`
    // 两条初始化都要写（P191 配套）。growType 是独立嵌套 for（4.1 setle 尾测）。
    int i = 0;
    for (i = 0; i <= 5; ++i)
    {
        m_arr64[i] = 0;
        m_arr7c[i] = 0;
    }
    for (i = 0; i <= 5; ++i)
    {
        for (int j = 0; j <= 1; ++j)
        {
            m_growType[i].m_0[j] = 0;
            m_growType[i].m_8[j] = 0;
        }
    }
    m_vecf4.clear();
    // ORIG 8375262..83752bc 单循环：m_f100/m_f108 用 float 视图（mov $0,%edx 中转，
    // P238）、m_dw110/m_arr138 用 int 视图直写。
    for (i = 0; i <= 1; ++i)
    {
        m_f100[i] = 0.f;
        m_f108[i] = 0.f;
        m_dw110[i] = 0;
        m_arr138[i] = 0;
    }
    m_118 = 0;
    m_useItems11c.clear();
    m_vec128.clear();
    m_134 = 1;
    m_vec140.clear();
    m_vec14c.clear();
    m_158 = 1;
    m_15c = 0;
    // ORIG 8375332..8375375 单循环：m_str160[i]="" 后 m_arr168[i]=0。
    for (i = 0; i <= 1; ++i)
    {
        m_str160[i] = "";
        m_arr168[i] = 0;
    }
    m_keyCommand170.clear();
    m_str194 = "";
    m_vec198.clear();
    m_1a4 = -1;
    m_vec1a8.clear();
    m_levelFeatures1b4.clear();
    m_1cc = 0;
    m_1d0 = 0;
    m_1d4 = 0;
    m_1d5 = 0;
    m_1d8 = -1;
    m_1e8 = 0;
    m_1ec = 0;
    m_1f0 = 0;
    m_1f4 = 0;
    m_vec1dc.clear();
    m_vecf4.clear();                    // ORIG 0x8375463：0xf4 二次 clear（P171 照抄）
    m_vec1f8.clear();
    m_str10 = "";                       // ORIG 837547b：+0x10 m_str10 清空
    m_levelFeatures1c0.clear();         // ORIG 837548e：0x1c0 clear 在 0x1f8 之后
    m_21c = 0;
    m_220 = 0;
    m_224 = -1;
    memset(m_228, 0, sizeof(m_228));
    m_228[3] = 1;
    m_23c = 0;
    m_240 = 0;
    m_244 = 0;
    m_248 = 0;
    m_254 = 0;
    m_250 = m_254;
    m_24c = m_250;
    m_258 = -1;
    m_25c = -1;
    m_260 = 0;
    m_passive268.clear();
    m_str274.clear();
    m_str278.clear();
    m_280 = 1;
    m_vec284.clear();
    m_264 = 0;                          // ORIG 83755c5：0x264 在 0x284 clear 之后
    m_290 = 0;                          // [0x837559x] ORIG 在 0x284 之后清 0x290
    m_vec294.clear();                   // [0x8375xxx] ORIG clear 0x294
    m_vec2a0.clear();                   // ORIG clear 0x2a0
