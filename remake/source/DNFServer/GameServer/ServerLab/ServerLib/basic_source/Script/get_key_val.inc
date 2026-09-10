bool Script::get_key_val(char* line)
{
    tok.get_token(line, strlen(line));
    int n_tok = tok.get_n_token();
    switch (n_tok)
    {
    case 1:
        mParentNames.push_back(std::string(tok.get_context(0)));
        mChildNames.push_back(std::vector<std::string>());
        mChildValues.push_back(std::vector<std::string>());
        tok.free_token(0);
        break;
    case 2:
        mChildNames[mParentNames.size() - 1].push_back(std::string(tok.get_context(0)));
        mChildValues[mParentNames.size() - 1].push_back(std::string(tok.get_context(1)));
        tok.free_token(0);
        tok.free_token(1);
        break;
    case 0:
        break;
    default:
        return false;
    }
    return true;
