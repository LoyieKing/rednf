void RSA_free(void* rsa_v)
{
    static const char kFile[] = "rsa_lib.c";
    RSA_VIEW* rsa = static_cast<RSA_VIEW*>(rsa_v);
    if (rsa == 0)
        return;
    if (CRYPTO_add_lock(&rsa->references, -1, 9, kFile, 0xdd) > 0)
        return;
    RSA_METH* meth = static_cast<RSA_METH*>(rsa->meth);
    if (meth->finish != 0)
        meth->finish(rsa);
    if (rsa->engine != 0)
        ENGINE_finish(rsa->engine);
    CRYPTO_free_ex_data(6, rsa, &rsa->ex_sk);
    if (rsa->n     != 0) BN_clear_free(rsa->n);
    if (rsa->e     != 0) BN_clear_free(rsa->e);
    if (rsa->d     != 0) BN_clear_free(rsa->d);
    if (rsa->p     != 0) BN_clear_free(rsa->p);
    if (rsa->q     != 0) BN_clear_free(rsa->q);
    if (rsa->dmp1  != 0) BN_clear_free(rsa->dmp1);
    if (rsa->dmq1  != 0) BN_clear_free(rsa->dmq1);
    if (rsa->iqmp  != 0) BN_clear_free(rsa->iqmp);
    if (rsa->blinding    != 0) BN_BLINDING_free(rsa->blinding);
    if (rsa->mt_blinding != 0) BN_BLINDING_free(rsa->mt_blinding);
    if (rsa->lock        != 0) CRYPTO_free_locked(rsa->lock);
    CRYPTO_free(rsa);
