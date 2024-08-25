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
        cash = intf.getCash(4);
        denom = intf.getDenom(4);
        max_money = intf.getMoney(cardId);
        std::cout << "Accont money: $" << max_money << ".00\n";
        std::cout << msg.va_money(cash,denom);
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

void ATM::withdrawCash(int total)
{
    if(total > max_money) return;

    auto sum = []( const std::vector<std::pair<int, int>> & src, 
                   int val)
    {  
        std::vector<std::pair<int, int>> v = src;
        for(auto it = v.begin(); it != v.end(); it++)
        {
            it->first += val;
            it->second = val;
        }
        return v;
    };
    auto merge = []( const std::vector<std::pair<int, int>> & first, 
                     const std::vector<std::pair<int, int>> & second, 
                     int val)
    {
        std::vector<std::pair<int, int>> mrg;
        auto it1 = first.begin(),
             it2 = second.begin(),
             end1 = first.end(),
             end2 = second.end();
        while(it1 != end1 || it2 != end2)
            if (it1 == end1)
                while(it2 != end2) 
                    if(it2->first == mrg.back().first) it2++;
                    else mrg.push_back(*it2++);
            else if (it2 == end2)
                while(it1 != end1) 
                    if(it1->first == mrg.back().first) it1++;
                    else mrg.push_back(*it1++);
            else if (it1->first > it2->first) mrg.push_back(*it2++);
            else if (it2->first > it1->first) mrg.push_back(*it1++);
            else /* *it1 == *it2 */
            {
                mrg.push_back(*it1); 
                it2++;
                it1++;
            }
            while(mrg.back().first > val) mrg.pop_back();
        
        return mrg;
    };
    auto count = [&](const std::vector<std::pair<int, int>> & src, int size)
    {
        auto posicion = [](const std::vector<int> & vec, int val)
        {
            int pos = 0;
            for(auto i : vec) 
                if(i == val) return pos;
                else pos++;
            return -1;
        };
        std::vector<int> _cash(size);
        auto rit = src.rbegin();
        int val = rit->first, 
            pos = src.size() - 1;
        while (val != 0)
        {
            _cash[posicion(denom, rit->second)]++;
            val -= rit->second;
            while(val != rit->first) rit++; 
            
        }
        return _cash;
    };

    auto _cash = cash;
    int i = cash.size() - 1;
    std::vector<std::pair<int, int>> buf = {{ 0, 0 }};
    while(i >= 0)
    {
        if(total == buf.back().first) break;
        else if(_cash[i] == 0) i--;
        else
        {
            buf = merge(buf, sum(buf, denom[i]), total);
            _cash[i]--;       
        }
    }
    if(i < 0 )
    {
        std::clog << "Eror: can't withdraw.\n";
        return;
    }
    auto v = count(buf, cash.size());
    std::cout << msg.withdrawCash(v, denom) << "Total: $" << total << ".00\n"; 
    for(int i = 0; i!= cash.size(); i++)
        cash[i] -= v[i]; 
    std::cout << max_money - total;
    intf.setCash(cash);
    intf.setAccountMoney(max_money - total, _cardId);
    intf.disconnect();
}

void ATM::setCash(const std::vector<int> & v)
{
    cash = v;
}

void ATM::setDenomination(const std::vector<int> & v)
{
    denom = v;
}

void ATM::setMaxMoney(int val)
{
    max_money = val;
}