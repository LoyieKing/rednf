// b28d-solo-CHackAnalyzer: AE（299/299，无实质残差；ORIG 0x080f8550）。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
void CHackAnalyzer::AddCheckRateHackTypeList()
{
    // ORIG 0x080f8550：前 18 项压入 m_checkRateHackTypes(+0x7a24)，
    // 后 24 项压入 m_vec7a30(+0x7a30)；均为栈上 int 变量逐值 push_back。
// P0 推定：ORIG 42 个独立栈槽（-0xb0..-0xc 各用一次）= 42 个具名局部；
// 4.4 首声明占最浅槽 ⇒ 声明序倒排（v42..v1）使 v1 落 -0xb0 与 ORIG 同序。
    int v42, v41, v40, v39, v38, v37, v36, v35, v34, v33, v32, v31, v30, v29, v28, v27, v26, v25, v24, v23, v22, v21, v20, v19, v18, v17, v16, v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1;
    v1 = 0x84; m_checkRateHackTypes.push_back(v1);
    v2 = 0xdc; m_checkRateHackTypes.push_back(v2);
    v3 = 0x138; m_checkRateHackTypes.push_back(v3);
    v4 = 0x139; m_checkRateHackTypes.push_back(v4);
    v5 = 0x86; m_checkRateHackTypes.push_back(v5);
    v6 = 0xde; m_checkRateHackTypes.push_back(v6);
    v7 = 0x1a4; m_checkRateHackTypes.push_back(v7);
    v8 = 0x1a5; m_checkRateHackTypes.push_back(v8);
    v9 = 0x1a7; m_checkRateHackTypes.push_back(v9);
    v10 = 0xdf; m_checkRateHackTypes.push_back(v10);
    v11 = 0xe0; m_checkRateHackTypes.push_back(v11);
    v12 = 0xe1; m_checkRateHackTypes.push_back(v12);
    v13 = 0x1a4; m_checkRateHackTypes.push_back(v13);
    v14 = 0x1a5; m_checkRateHackTypes.push_back(v14);
    v15 = 0x1a7; m_checkRateHackTypes.push_back(v15);
    v16 = 0xdf; m_checkRateHackTypes.push_back(v16);
    v17 = 0xe0; m_checkRateHackTypes.push_back(v17);
    v18 = 0xe1; m_checkRateHackTypes.push_back(v18);
    v19 = 0x84; m_vec7a30.push_back(v19);
    v20 = 0xca; m_vec7a30.push_back(v20);
    v21 = 0xcd; m_vec7a30.push_back(v21);
    v22 = 0xce; m_vec7a30.push_back(v22);
    v23 = 0xd0; m_vec7a30.push_back(v23);
    v24 = 0xd1; m_vec7a30.push_back(v24);
    v25 = 0xd7; m_vec7a30.push_back(v25);
    v26 = 0xd8; m_vec7a30.push_back(v26);
    v27 = 0xd9; m_vec7a30.push_back(v27);
    v28 = 0xda; m_vec7a30.push_back(v28);
    v29 = 0xdb; m_vec7a30.push_back(v29);
    v30 = 0xdc; m_vec7a30.push_back(v30);
    v31 = 0x138; m_vec7a30.push_back(v31);
    v32 = 0x139; m_vec7a30.push_back(v32);
    v33 = 0x193; m_vec7a30.push_back(v33);
    v34 = 0x19e; m_vec7a30.push_back(v34);
    v35 = 0x86; m_vec7a30.push_back(v35);
    v36 = 0xde; m_vec7a30.push_back(v36);
    v37 = 0x1a4; m_vec7a30.push_back(v37);
    v38 = 0x1a5; m_vec7a30.push_back(v38);
    v39 = 0x1a7; m_vec7a30.push_back(v39);
    v40 = 0xdf; m_vec7a30.push_back(v40);
    v41 = 0xe0; m_vec7a30.push_back(v41);
    v42 = 0xe1; m_vec7a30.push_back(v42);
}