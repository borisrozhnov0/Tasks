#include "atm.h"
#include "libpq-fe.h"

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
    if(PQstatus(conn) == CONNECTION_OK) is_connect = true;
    else std::cout << PQstatus(conn);
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
        std::string query = "SELECT id FROM card WHERE id = " 
                          + std::to_string(id) + ";";
        
        if(!is_connect) connect();
        res = PQexec(conn, query.c_str());
        bool ret = PQntuples(res) == 1 ? true : false;
        PQclear(res);

        return ret;
    }

    bool ATM_Interface::isCorrectCode(int id, int code)
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

    bool ATM_Interface::isActiveCard(int id)
    {
        std::string query = "SELECT active FROM card WHERE id = " + std::to_string(id) + ";";

        if(!is_connect) connect();

        res = PQexec(conn, query.c_str());
        std::string value = PQgetvalue(res, 0, 0);
        PQclear(res);

        bool ret = value == "t" ? true : false;
        
        return ret; 
    }

    void ATM_Interface::setCash(const std::vector<int> & cash)
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

    int ATM_Interface::getMoney(int id)
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

    std::vector<int> ATM_Interface::getCash(int size)
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

    std::vector<int> ATM_Interface::getDenom(int size)
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
    std::string ATM_Interface::vecToPQarrayStr(const std::vector<int> & v, int size)
    {
        std::string str = "ARRAY[";
        for(int i : v) str += std::to_string(i) + ",";
        for(int i = v.size(); i!= size; i++) str += "0,";
        str.pop_back();
        str += "]";

        return str;
    }