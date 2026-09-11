// b28d-20: ORIG 0x08ab937a。修法：①while(true)+`if(!ScanInt×3)break;`（P133）；
// ②实参顺序修正为 addEquipmentIndex(type,index,slot) 且声明序 slot,type,index ——
// ORIG 压栈 arg1=第二扫描值/arg2=第三/arg3=第一（addEquipmentIndex 报告：p1=key,p2=value,
// p3=map 索引且 cmpl $3,p3 越界保护），原实现把第一扫描值当 key 属真语义错位。
// 结果 IDENTICAL_AE（51=51）。
void GlobalScriptTutorial::LoadEquipmentTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, type, index;
    while (true)
    {
        if (!ScanInt(&slot) || !ScanInt(&type) || !ScanInt(&index))
            break;
        m_data[static_cast<int>(job)].addEquipmentIndex(type, index, slot);
    }
}