void CDisconnectDetecter::OnDisconnect(CUser* user)
{
    user->OnDisconnect();
}