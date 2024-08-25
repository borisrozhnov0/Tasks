#include "atm.h"
#include "libpq-fe.h"
#include <stdarg.h>

 ATM_Interface::ATM_Interface(int id,
                  const char * host, int port,
                  const char * user, const char * pass, const char * db)
                  :id(id)
{
    conninfo =  "host=" + std::string(host) + " ";
    conninfo += "port=" + std::to_string(port) + " ";
    conninfo += "user=" + std::string(user) + " ";
    conninfo += "password=" + std::string(pass) + " ";
    conninfo += "dbname=" + std::string(db);

    is_connect = false;
}

ATM_Interface::~ATM_Interface()
{
    if(is_connect) PQfinish(conn);
}

void ATM_Interface::connect()
{
    conn = PQconnectdb(conninfo.c_str());
    if(PQstatus(conn) == CONNECTION_OK) 
        is_connect = true;
    else 
        throw ATM_error::disconnect;
}

void ATM_Interface::disconnect()
{
    if(is_connect)
    {
        is_connect = false;
        PQfinish(conn);
    }
}

bool ATM_Interface::isExistCard(int id)
{
    std::string query = format("SELECT id FROM card WHERE id = {i};", (int)id);
        
    if(!is_connect) connect();
    res = PQexec(conn, query.c_str());
    bool ret = PQntuples(res) ? true : false;
    PQclear(res);

    return ret;
}

    bool ATM_Interface::isCorrectCode(int id, int code)
    {
        std::string query = format("SELECT id FROM card WHERE id = {i} AND code = {i};", (int)id, (int)code);
        /* "SELECT id FROM card WHERE id = " + std::to_string(id) + " AND code = " + std::to_string(code) + ";";*/

        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        bool ret = PQntuples(res) ? true : false;
        PQclear(res);

        return ret;                  
    }

    bool ATM_Interface::isActiveCard(int id)
    {
        std::string query = format("SELECT active FROM card WHERE id = {i};", (int)id);
        /*"SELECT active FROM card WHERE id = " + std::to_string(id) + ";";*/

        if(!is_connect) connect();

        res = PQexec(conn, query.c_str());
        std::string value = PQgetvalue(res, 0, 0);
        PQclear(res);

        bool ret = value == "t" ? true : false;
        
        return ret; 
    }

    void ATM_Interface::setCash(const std::vector<int> & cash)
    {
        std::string query = format("UPDATE ATM SET cash = {s} WHERE id = {i};", vecToPQarrayStr(cash, 10).c_str(), (int)id );
        /*"UPDATE ATM SET cash = " + vecToPQarrayStr(cash, 10) + " WHERE id = " + std::to_string(id)  + ";";*/
        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        PQclear(res);
    }

    int ATM_Interface::getMoney(int id)
    {
        int money = 0;
        std::string query = format("SELECT _money FROM Accounts WHERE id = (SELECT AccountId FROM card WHERE id = {i} );", (int)id),
        /*"SELECT _money FROM Accounts WHERE id = (SELECT AccountId FROM card WHERE id = "+ std::to_string(id)+ ");",*/
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

    std::vector<int> ATM_Interface::getCash(int size)
    {
        std::vector<int> _cash;
        std::string query = format("SELECT cash FROM ATM WHERE id = {i}", (int)id),
        /*{"SELECT cash FROM ATM WHERE id = " + std::to_string(id) + ";"},*/
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
            else buf += *it;

        return _cash;
    }

    std::vector<int> ATM_Interface::getDenom(int size)
    {
        std::vector<int> _denom;
        std::string query = format("SELECT denomination FROM ATM WHERE id = {i};", id),
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

std::string ATM_Interface::vecToPQarrayStr(const std::vector<int> & v, int size)
{
    std::string str = "ARRAY[";
    for(int i : v) str += std::to_string(i) + ",";
    for(int i = v.size(); i!= size; i++) str += "0,";
    str.pop_back();
    str += "]";

    return str;
}

void ATM_Interface::setAccountMoney(int val, int cardId)
{
    std::string query = format("UPDATE Accounts SET _money = {i} WHERE id = (SELECT AccountId FROM card WHERE id = {i} );", val, cardId);
    if(!is_connect) connect();
    res = PQexec(conn, query.c_str());
    PQclear(res);
}

std::string ATM_Interface::format(const char * src, ...)
{
    va_list arg;
    va_start(arg, src);
    
    std::string ret;
    while(*src != '\0')
        if(*src == '{')
        {
            src++;
            if(*src == 'i')
            {
                src += 2;
                ret += std::to_string(va_arg(arg, int));
            }
            else if(*src == 's')
            {
                src += 2;
                ret += std::string(va_arg(arg, char*));
            }
            else
            {
                src += 2;
                ret += std::string(va_arg(arg, char*));
            }
        }
        else
        ret += *src++;

    va_end(arg);

    return ret;
}