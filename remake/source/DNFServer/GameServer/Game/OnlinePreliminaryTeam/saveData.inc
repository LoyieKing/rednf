// ORIG W 0x08588d60 saveData【推断：objdump】＝顺序调用 _saveTeam + _saveTeamMatchList。
void COnlinePreliminaryTeam::saveData()
{
    _saveTeam();
    _saveTeamMatchList();
}