#ifndef ATM_H
#define ATM_H

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>

#include "libpq-fe.h"

#ifndef DllExpor
    #define DllExpor __declspec( dllexport )
#endif

#ifndef DllImport
    #define DllImport __declspec( dllimport )
#endif



class DllExpor ATM_MSG
{
    public:
    const std::string HELLO{"INPUT"};
    std::string va_money(const std::vector<int> & cash, const std::vector<int> & denom);
};

class DllExpor ATM_Interface
{
public:
    ATM_Interface()=default;
    ATM_Interface(int id,
                  const char * host, int port,
                  const char * user, const char * pass, const char * db);

    ~ATM_Interface();
    void connect();
    void disconnect();

    bool isExistCard(int id);
    bool isCorrectCode(int id, int code);
    bool isActiveCard(int id);

    void setCash(const std::vector<int> & cash);
    int getMoney(int id);
    std::vector<int> getCash(int size);
    std::vector<int> getDenom(int size);

private:
    std::string vecToPQarrayStr(const std::vector<int> & v, int size);
    bool is_connect;
    int id;
    int AccountId, money;
    std::string conninfo;
    PGconn     *conn;
    PGresult   *res;
};


class DllExpor ATM
{
public:
    ATM();

    void checkCard(int cardId, int cardCode);
    void cashCheck();
    void setMoney(int i);

private:
    std::vector<int> cash, denom;
    ATM_Interface intf;
    ATM_MSG msg;
    int _cardId, max_money;
    bool correct_card;
};


#endif /*ATM_H*/
