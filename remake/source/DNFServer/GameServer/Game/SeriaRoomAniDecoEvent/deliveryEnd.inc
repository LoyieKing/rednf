void OneADayItemShopEvent::deliveryEnd(CDelivery* d)    // 0x081ba19c
{
    // b28c-L19：同 deliveryStart，ORIG 早返形。
    if (d == 0)
        return;
    d->detach(1, this);
    d->detach(2, this);
