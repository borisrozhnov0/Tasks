#include "string_t.h"

/**
 * @short Конструктор структуры string_t.
 * @return struct string_t 
 */ 
string_t init_void(void)
{
    string_t s = { STRING_INIT_SIZE, 0, (char*)malloc( STRING_INIT_SIZE * sizeof(char)) };
    s.val[0] = '\0';
    
    return s;
}

/**
 * @brief Конструктор структуры string_t содержащей копию строки из src.
 * @param[in]   src указатель на строку содержащую символы типа char, 
 *                  оканчивающаяся '\0'.
 * @return string_t
 */
string_t init_str(const char * src)
{
    int sSize, len;
    len = strlen(src);
    sSize = len + 1;
    sSize += (sSize % STRING_INIT_SIZE ? STRING_INIT_SIZE : 0) - sSize % STRING_INIT_SIZE;
    string_t s = { sSize, len, (char*) malloc(sSize * sizeof(char)) };
    memcpy(s.val, src, len);
    
    s.val[len] = '\0';
    return s;
}

/**
 * @brief Деструктор string_t
 * @param[in, out]  s   структура string_t
 */
void del(string_t * s)
{
    free(s->val);
    s->val = NULL;
    s->size = 0;
    s->len = 0;
}

/* Параметры строки */

/**
 * @brief Возвращает наибольшее количество элементов, которые можно хранить 
 *        в строке без увеличения выделения памяти этой строки.
 * @param[in]   s   структура string_t.
 * @return Количество элементов, которые можно хранить в строке без увеличения
 *         выделения памяти этой строки.
 */
inline int capacity(const string_t s) { return s.size - s.len - 1; } 

/**
 * @brief Возвращает текущее количество элементов в строке.
 * @param[in]   s   структура string_t.
 * @return Текущее количество элементов в строке.
 */                                
inline int size(string_t s) { return s.size; } 

/**
 * @brief Проверяет, содержит ли строка символы.
 * @param[in]   s   структура string_t.
 * @return 1 - строка содержит символы;
 *         0 - строка не содержит символы.
 */ 
inline int empty(string_t s) { return s.len ? 1 : 0; }

/**
 * @brief Возвращает текущее количество элементов в строке.
 * @param[in]   s   структура string_t.
 * @return   n - количество символов в строке;
 *           0 если строка не содержит символы.
 */ 
inline int length(string_t s) {return s.len;}

/* Возвращает максимальное количество символов в строке. */

/**
 * @brief Возвращает максимальное количество символов которые может содержаться 
 *        в строке.
 * @param[in]   s   структура string_t.
 * @return n - количество символов в строке.
 */ 
inline int max_size(string_t s) { return s.size - 1; } 

/* Позиция сиволов в тексте */

/**
 * @brief Возвращает ссылку на элемент в заданном положении в строке.
 * @param[in]   s   структура string_t;
 * @param[in]   pos позиция элемента в тексте.
 * @return Указатель на элемент в позиции;
 *         NULL если позиция указана неверно.
 */
inline char * at(const string_t s, int pos)
{
    if(pos < 0) return NULL;
    else if(pos >= s.len) return NULL;
    return s.val + pos;
}

/**
 * @brief Возвращает указатель на последний элемент строки.
 * @param[in]   s   структура string_t;
 * @return Указатель на последний элемент строки.
 * @warning если строка пустая результат неопределен.
 */
inline char * back(const string_t s){ return s.val + s.len - 1; }

/**
 * @brief Возвращает указатель на первый элемент в строке.
 * @param[in]   s   структура string_t;
 * @return Указатель на первый элемент в строке.
 */                                         
inline char * begin(const string_t s){ return s.val; }

/**
 * @brief Возвращает указатель на расположение после последнего элемента 
 *        в строке.
 * @param[in]   s   структура string_t;
 * @return Указатель на расположение после последнего элемента в строке.
 */ 
inline char * end(string_t s){ return s.val + s.len; }

/** 
 * @brief Возвращает указатель на первый элемент в обратной строке. 
 * @param[in]   s   структура string_t;
 * @return Указатель на первый элемент в обратной строке. 
 */
inline char * rbegin(string_t s){ return s.val + s.len - 1; }

/**
 * @brief Возвращает указатель, адресующий расположение после последнего
 *        элемента в обратной строке.
 * @param[in]   s   структура string_t;
 * @return Указатель на расположение после последнего элемента в обратной строке 
 */
inline char * rend(string_t s) { return s.val - 1; }                                                

/* Редактирование строки */

/**
 * @brief   Удаляет диапазон элементов из строке.
 * @details Удаляет диапазон элементов из строки начиная с позиции pos,
 *          в диапазоне size.
 * @note    Если позиция выходит за пределы строки, удаления не происходит.
 * 
 * @param[in]   s    структура string_t;
 * @param[in]   pos  положение первого удоляемого элемента;
 * @param[in]   size размер удаляемой области.
 * 
 * @return структура string_t;
 */
string_t erase(string_t s, int pos, int size)
{
    if(pos > s.len) return s;
    if(pos+size > s.len)
    {
        s.len = pos;
        s.val[pos] = '\0';
    }
    else
    {
        char * end = s.val + pos + size;
        int eSize = strlen(end) + 1;
        memmove(s.val + pos, end, eSize);
        s.len -= size;
    }
    
    return s;
}

/**
 * @brief   Вставляет диапазон элементов типа char на позицию pos.
 * @details Вставляет диапазон элементов типа char, длинной len на позицию pos.
 * @note    Если pos выходит за пределы то данные вставляются в конце/начале
 *          строки.
 * 
 * @param[in]   s    структура string_t;   
 * @param[in]   pos  Позици в которую вставляются элементы; 
 * @param[in]   src  диапазон элементов типа char;
 * @param[in]   len  длинна диапазона элементов типа char.
 * 
 * @return  структура string_t; 
 */
string_t insert(string_t s, int pos, const char * src, int len)
{
    pos = pos < 0 ? pos : pos > s.len ? s.len : pos;
    
    if(s.len + pos >= s.size)
    {
        int new_size = s.len + pos + 1;
        new_size += (new_size % STRING_INIT_SIZE ? STRING_INIT_SIZE : 0) - new_size % STRING_INIT_SIZE;
        s.val = (char*)realloc(s.val, new_size * sizeof(char));
    }
    char * begin = s.val + pos,
         * end = s.val + pos + len;
    size_t sSize = len * sizeof(char);
    
    memmove(end, begin, s.len - pos);
    memcpy(begin, src, sSize);
    
    s.len += len;
    s.val[s.len] = '\0';
    
    return s;
}

string_t push_back(string_t s, char c)
{
    if(s.len == s.size)
    {
        s.size += STRING_INIT_SIZE;
        s.val = (char*)realloc(s.val, s.size * sizeof(char));
    }
    s.val[s.len++] = c;
    s.val[s.len] = '\0';
    
    return s;
}

string_t pop_back(string_t s, int size)
{
    s.len -= size;
    s.val[s.len] = '\0';
    
    return s;
}

string_t append(string_t s, const char * src, int len)
{
    len = len < 0 ? strlen(src) : len;
    
    if(s.len + len >= s.size)
    {
        s.size = s.len + len;
        ALIGN_STR_SIZE(s.size);
    }
    memcpy(s.val + s.len, src, len);
    s.len += len;
    s.val[s.len] = '\0';
    return s;
}

string_t replace(string_t s, int pos, int len, const char * src, int sSize)
{
    if(sSize < 0) sSize = strlen(src);
    if(pos > s.len) pos = s.len;
    if(pos + len > s.len) len = s.len - pos;
    
    if(s.len - len + sSize > s.size)
    {
        s.size = s.len -len + sSize;
        ALIGN_STR_SIZE(s.size);
    }    
    char * _dst, * _src;
    if(len > sSize)
    {
        _dst = s.val + pos + sSize;
        _src = s.val + pos + len;
        memmove(_dst, _src, s.len - pos - sSize);
    }
    else if(len < sSize)
    {
        _dst = s.val + pos + sSize;
        _src = s.val + pos + len;
        memmove(_dst, _src, s.len - pos - len);
    }
    memcpy(s.val+ pos, src, sSize * sizeof(char));
    
    s.len = s.len - len + sSize;
    s.val[s.len] = '\0';
    return s;
}


int find(string_t s, const char * src)
{
    int len = strlen(src);
    if(s.len < len) return -1;
    
    size_t sSize = len * sizeof(char);
    
    char * pt = s.val;
    for(int i = 0, n = s.len - len + 1; i != n; i++)
    {
        if(!memcmp(pt, src, sSize)) return i;
        pt++;
    }
    return -1;
}

int rfind(string_t s, const char * src)
{
    int len = strlen(src);
    if(s.len < len) return -1;
    
    size_t sSize = len * sizeof(char);
    
    char * pt = s.val + s.len - len;
    for(int i = s.len -len, n = -1; i != n; i--)
    {
        if(!memcmp(pt, src, sSize)) return i;
        pt--;
    }
    return -1;
}

int starts_with(const string_t s, const char * pref)
{
    const char * pt = s.val; 
    while(*pref != '\0')
    {
        if(*pref != *pt) return 0;
        pt++;
        pref++;
    }
    return 1;
}

int ends_with(string_t s, const char * suff)
{
    int len = strlen(suff);
    if(s.len < len) return 0;
    return !memcmp(s.val + s.len - len, suff, len * sizeof(char));
}

void print_strinfo(string_t s)
{
    printf("'%s' len: %i, size: %i\n", s.val, s.len, s.size);
}
