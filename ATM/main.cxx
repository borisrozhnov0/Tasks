#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "libpq-fe.h"
#include <iostream>

class ATM_MSG
{
    friend class ATM;
    const std::string HELLO{"INPUT"};
    std::string va_money(const std::vector<int> & cash, const std::vector<int> & denom)
    {
        std::string ret;
        int sum = 0;
        ret += "Available for withdrawal:\n";
        for(int i = 0; i != cash.size(); i++)
        {
            ret += " - " 
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
};

class ATM_Interface
{
public:
    ATM_Interface()=default;
    ATM_Interface(int id,
                  const char * host, int port,
                  const char * user, const char * pass, const char * db)
                  :id(id)
    {
        conninfo = "host=" + std::string(host) + " ";
        conninfo += "port=" + std::to_string(port) + " ";
        conninfo += "user=" + std::string(user) + " ";
        conninfo += "password=" + std::string(pass) + " ";
        conninfo += "dbname=" + std::string(db);

        is_connect = false;
    }

    ~ATM_Interface()
    {
        if(is_connect) PQfinish(conn);
    }

    void connect()
    {
        conn = PQconnectdb(conninfo.c_str());
        if(PQstatus(conn) == CONNECTION_OK) is_connect = true;
        else std::cout << PQstatus(conn);
    }

    void disconnect()
    {
        if(is_connect)
        {
            is_connect = false;
            PQfinish(conn);
        }
    }

    bool isExistCard(int id)
    {
        std::string query = "SELECT id FROM card WHERE id = " 
                          + std::to_string(id) + ";";
        
        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        bool ret = PQntuples(res) == 1 ? true : false;
        PQclear(res);

        return ret;
    }

    bool isCorrectCode(int id, int code)
    {
        std::string query = "SELECT id FROM card WHERE id = " 
                          + std::to_string(id) + " AND code = "
                          + std::to_string(code) + ";";

        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        bool ret = PQntuples(res) == 1 ? true : false;
        PQclear(res);

        return ret;                  
    }

    bool isActiveCard(int id)
    {
        std::string query = "SELECT active FROM card WHERE id = " + std::to_string(id) + ";";

        if(!is_connect) connect();

        res = PQexec(conn, query.c_str());
        std::string value = PQgetvalue(res, 0, 0);
        PQclear(res);

        bool ret = value == "t" ? true : false;
        
        return ret; 
    }

    void setCash(const std::vector<int> & cash)
    {
        std::string query = "UPDATE ATM SET cash = " 
                          + vecToPQarrayStr(cash, 10) 
                          + " WHERE id = " 
                          + std::to_string(id) 
                          + ";";
    
        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        PQclear(res);
    }

    int getMoney(int id)
    {
        int money = 0;
        std::string query = "SELECT _money FROM Accounts WHERE id = (SELECT AccountId FROM card WHERE id = "
                          + std::to_string(id)
                          + ");",
                    response_str,
                    money_str;

        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        response_str = PQgetvalue(res, 0, 0);
        PQclear(res);

        response_str.pop_back();
        response_str.pop_back();
        response_str.pop_back();

        for(char c : response_str)
            if(c == '$' || c == ',') continue;
            else money_str += c;
        return std::stoi(money_str);
    }

    std::vector<int> getCash(int size)
    {
        std::vector<int> _cash;
        std::string query = {"SELECT cash FROM ATM WHERE id = " + std::to_string(id) + ";"},
                    response_str;
        
        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        response_str = PQgetvalue(res, 0, 0);
        PQclear(res);

        std::string buf;
        for(auto it = response_str.begin()+1; it != response_str.end(); it++)
            if(*it == ',' || *it == '}')
            {
                if(!size--) break;
                _cash.push_back(std::stoi(buf));
                buf.clear();
            }
            else buf+= *it;

        return _cash;
    }

    std::vector<int> getDenom(int size)
    {
        std::vector<int> _denom;
        std::string query = {"SELECT denomination FROM ATM WHERE id = " + std::to_string(id) + ";"},
                    response_str;
        
        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        response_str = PQgetvalue(res, 0, 0);
        PQclear(res);

        std::string buf;
        for(auto it = response_str.begin()+1; it != response_str.end(); it++)
            if(*it == ',' || *it == '}')
            {
                if(!size--) break;
                _denom.push_back(std::stoi(buf));
                buf.clear();
            }
            else buf += *it;

        return _denom;
    }

private:
    std::string vecToPQarrayStr(const std::vector<int> & v, int size)
    {
        std::string str = "ARRAY[";
        for(int i : v) str += std::to_string(i) + ",";
        for(int i = v.size(); i!= size; i++) str += "0,";
        str.pop_back();
        str += "]";

        return str;
    }
    bool is_connect;
    int id;
    int AccountId, money;
    std::string conninfo;
    PGconn     *conn;
    PGresult   *res;
};


class ATM
{
public:
    ATM()
    {
        correct_card = false;
        intf = { 1, "localhost", 5432, "atm_user", "password", "bank" };
    }

    void checkCard(int cardId, int cardCode)
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
    }

    void cashCheck()
    {
        std::cout << msg.va_money(cash, denom);
        max_money = intf.getMoney(_cardId);
        std::cout << "\nAvailable money: " << max_money << "\n";
    }
    void setMoney(int i)
    {

    }
private:
    std::vector<int> cash, denom;
    ATM_Interface intf;
    ATM_MSG msg;
    int _cardId, max_money;
    bool correct_card;
};


int
main(int argc, char **argv)
{
    ATM atm;
    atm.checkCard(1, 8080);
    atm.cashCheck();
    return 0;
}