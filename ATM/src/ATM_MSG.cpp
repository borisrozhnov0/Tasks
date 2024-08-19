#include "atm.h"



std::string ATM_MSG::va_money(const std::vector<int> & cash, const std::vector<int> & denom)
{
    std::string ret;
    int sum = 0;
    ret += "Available for withdrawal:\n";
    for(int i = 0; i != cash.size(); i++)
    {
       ret  += " - " 
            + std::to_string(cash[i]) 
            + " notes, denomination " 
            + std::to_string(denom[i])
            + "\n";
            sum += cash[i] * denom[i];
    }
    ret += "Total: " 
        + std::to_string(sum);
    return ret;
}