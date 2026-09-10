double Arad_Script::GetDouble(int row, char* col)
{
    return GetDouble(row, GetIdx4Col(col));
