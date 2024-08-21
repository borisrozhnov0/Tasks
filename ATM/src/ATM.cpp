#include "atm.h"

ATM::ATM()
{
    correct_card = false;
    intf = { 1, "localhost", 5432, "atm_user", "password", "bank" };
}

void  ATM::checkCard(int cardId, int cardCode)
{
    correct_card = true;
    if(!intf.isExistCard(cardId))
    {
        correct_card = false;
        std::cout << "Error: card not found";
        return;
    }
    if(!intf.isCorrectCode(cardId, cardCode))
    {
        correct_card = false;
        std::cout << "Error: wrong code";
        return;
    }
    if(!intf.isActiveCard(cardId))
    {
        correct_card = false;
        std::cout << "Error: card blocked";
        return;
    }
    if(correct_card)
    {
        std::cout << "Input money\n";
        cash = intf.getCash(4);
        denom = intf.getDenom(4);
    }
    intf.disconnect();
    _cardId = cardId;
}

void ATM::cashCheck()
{
    std::cout << msg.va_money(cash, denom);
    max_money = intf.getMoney(_cardId);
    std::cout << "\nAvailable money: " << max_money << "\n";
}

void ATM::setMoney(int i)
{

}