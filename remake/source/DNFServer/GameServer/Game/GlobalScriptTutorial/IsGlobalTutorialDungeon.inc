bool GlobalScriptTutorial::IsGlobalTutorialDungeon(
    register unsigned int dungeon)
{
    if (dungeon == 0xdb2)
        return true;
    return false;
