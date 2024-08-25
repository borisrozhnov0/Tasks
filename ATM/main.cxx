#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "libpq-fe.h"
#include <iostream>
#include "atm.h"

int
main(int argc, char **argv)
{
    ATM atm;

    atm.checkCard(1, 8080);
    atm.withdrawCash(780);

    return 0;
}