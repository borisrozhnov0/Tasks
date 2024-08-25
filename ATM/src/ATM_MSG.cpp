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

std::string ATM_MSG::withdrawCash(const std::vector<int> & cash, const std::vector<int> & denom)
{
    std::string s = "withdraw cash:\n";
    for(int i = denom.size() - 1; i!= -1; i--)
    {
        if (!cash[i]) continue;
        s += " - " + std::to_string(denom[i]) 
          + " : " + std::to_string(cash[i]) + ";\n";
    }
    return s;
}