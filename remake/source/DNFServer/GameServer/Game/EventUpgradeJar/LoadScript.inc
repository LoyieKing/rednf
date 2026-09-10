int Event_Upgrade_Jar::LoadScript(const char* /*scriptDir*/, const char* path)
{
    return importEventUpgradeJarScript(this, path) ? 1 : 0;
