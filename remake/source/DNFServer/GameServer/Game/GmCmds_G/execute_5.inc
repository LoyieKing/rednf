// 对 6 个样本道具 × 9 个品级 × 1000 次跑分解结果并把六元组刷入日志
// （disjoint_result："rarity;grade;f6;f0;f7;f2\n"，压栈序依 objdump 实测）。
void CTestDisjointCmd::execute()
{
    CItem* samples[6];
    samples[0] = G_CDataManager()->find_item(0x795a);
    samples[1] = G_CDataManager()->find_item(0x795b);
    samples[2] = G_CDataManager()->find_item(0x797a);
    samples[3] = G_CDataManager()->find_item(0x795e);
    samples[4] = G_CDataManager()->find_item(0x7977);
    samples[5] = G_CDataManager()->find_item(620000);
    int runs = 1000;                       // -0x28

    DisJointResult result;                 // -0x68（ctor 清零 0x28）
    for (int s = 0; s <= 5; ++s)           // -0x24
    {
        for (int grade = 0; grade <= 8; ++grade)   // -0x20
        {
            for (int i = 0; i < runs; ++i)         // -0x1c
            {
                expert_job::CDisjointer::get_disjoint_result_(
                    samples[s], (Inven_Item*)0, grade, result, false,
                    (CUser*)0);
                // 参数序（ORIG result@-0x68 下位移实测 +0x14/+0x00/+0x18/+0x04）：
                // f[5] / f[0] / f[6] / f[1]
                OS_API::WriteLog("disjoint_result",
                                 "%d;%d;%d;%d;%d;%d\n",
                                 samples[s]->get_rarity(), grade,
                                 result.m_f[5], result.m_f[0],
                                 result.m_f[6], result.m_f[1]);
            }
        }
    }
}