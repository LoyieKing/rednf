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
