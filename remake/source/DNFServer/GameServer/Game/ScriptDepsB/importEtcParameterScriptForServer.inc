// ============================================================================
// importEtcParameterScriptForServer —— ORIG 0x080966fa
// 报告 _lt_global_gt_/importEtcParameterScriptForServer.md：
//   loadRDARScriptFile("", "Etc/EtcParameter.etc") 失败→false；
//   do { ScanType(token,false); } while (token != "[damage fomula constant]");
//   随后按序 ScanFloat 写入 26 个 g_*Const_ 全局 → true。
//   （ORIG 另构造一个未使用局部 string local_20，无语义，略。）
// ============================================================================
// [b28d-solo-ScriptDepsB AE: 补 ORIG 两枚 string + int/float/bool 死局部（P232/P171
//  照抄）：ctor 序 -0x18→-0x1c、清零序 -0x14(int 单段)/-0x10(float 两段 eax 中转)/-0x9(bool)，
//  目标串以立即数压参。ORIG 0x080966fa]
bool importEtcParameterScriptForServer(void)
{
    if (!loadRDARScriptFile("", "Etc/EtcParameter.etc"))
        return false;

    std::string token;
    std::string unused;
    int deadInt = 0;
    float deadFloat = 0.0f;
    bool deadBool = false;

    for (;;)
    {
        ScanType(token, false);
        if (token == "[damage fomula constant]")
            break;
    }

    g_physicalDamageRateConst_            = ScanFloat((bool*)0);
    g_physicalDamageAddConst_             = ScanFloat((bool*)0);
    g_physicalReduceRateConst_            = ScanFloat((bool*)0);
    g_physicalReduceAddConst_             = ScanFloat((bool*)0);
    g_physicalRandomBaseConst_            = ScanFloat((bool*)0);
    g_physicalRandomRateConst_            = ScanFloat((bool*)0);
    g_magicalDamageRateConst_             = ScanFloat((bool*)0);
    g_magicalDamageAddConst_              = ScanFloat((bool*)0);
    g_magicalReduceRateConst_             = ScanFloat((bool*)0);
    g_magicalReduceAddConst_              = ScanFloat((bool*)0);
    g_magicalRandomBaseConst_             = ScanFloat((bool*)0);
    g_magicalRandomRateConst_             = ScanFloat((bool*)0);
    g_commonElementBaseConst_             = ScanFloat((bool*)0);
    g_commonElementRateConst_             = ScanFloat((bool*)0);
    g_commonElementRateConstInPVP_        = ScanFloat((bool*)0);
    g_commonElementRateConstInLadderPVP_  = ScanFloat((bool*)0);
    g_hpIncreaseRateConst_                = ScanFloat((bool*)0);
    g_hpRegenRateConst_                   = ScanFloat((bool*)0);
    g_mpIncreaseRateConst_                = ScanFloat((bool*)0);
    g_mpRegenRateConst_                   = ScanFloat((bool*)0);
    g_activeStatusDamageRateConst_        = ScanFloat((bool*)0);
    g_activeStatusDefenseRateConst_       = ScanFloat((bool*)0);
    g_physicalDefenseAddConst_            = ScanFloat((bool*)0);
    g_magicalDefenseAddConst_             = ScanFloat((bool*)0);
    g_hpRegenRateAddRateConst_            = ScanFloat((bool*)0);
    g_mpRegenRateAddRateConst_            = ScanFloat((bool*)0);
    return true;
}