bool CCreatureMgr::DeleteCreatureItem(int param_1, int param_2)
{
    return UnregisterCreatureItem(param_1);   // ORIG 读 param_1@+0xc（我们误用 param_2@+0x10）
}
