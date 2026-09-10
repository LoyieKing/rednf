void CSkill::reform_ui_group_no(int& uiGroupNo, bool flag, int count)
{
    if (flag)
    {
        uiGroupNo = 3;
        return;
    }
    switch (uiGroupNo)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        if (count > 2)
        {
            uiGroupNo = 0;
        }
        else
        {
            uiGroupNo = 1;
        }
        break;
    case 4:
        uiGroupNo = 2;
        break;
    }
