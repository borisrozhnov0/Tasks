/**
 * @file    string_t.h
 * @brief   Функции для работы с динамичной строкой.
 * @anchor  Рожнов Б. И.
 */
#ifndef STRING_T_H
#define STRING_T_H

#include<string.h> /* memmove, memcpy, memcmp */
#include<stdlib.h> /* malloc, realloc */

#define STRING_INIT_SIZE 16                                                     /* Минимальный размер выделяемой памяти под строку. */


#define ALIGN_STR_SIZE(size)\
(size = size \
      + (size % STRING_INIT_SIZE ? STRING_INIT_SIZE : 0)\
      - size % STRING_INIT_SIZE)                                                /* Макрос выравнивающий значение кратное STRING_INIT_SIZE */

typedef struct string_t
{
    int size,
        len;    
    char * val;
}string_t;

/***** Конструктор деструктор *****/

string_t init_void();                                                           /* Инициализирует структуру string_t */
string_t init_str(const char * src);                                            /* Инициализирует структуру string_t содержащую строку символов*/
inline void del(string_t * s);

/* Изменение объема памяти выделенное под строку */

inline void __resize_string(string_t * s, int new_size);                        /* Изменяет выделенную память под строку */
//inline void shrink_to_fit(string_t * s);	                                    /* Удаляет излишнюю вместимость строки. */
//inline void reserve(string_t * s, int size);	                                /* Устанавливает вместимость строки не меньше указанного числа. */

/***** Параметры строки *****/

inline int capacity(const string_t s); 	                                        /* Возвращает наибольшее количество элементов, которые можно хранить в строке без увеличения выделения памяти этой строки.*/
inline int size(string_t s);	                                                /* Возвращает текущее количество элементов в строке. */
inline int empty(string_t s);	                                                /* Проверяет, содержит ли строка символы. */
inline int length(string_t s);	                                                /* Возвращает текущее количество элементов в строке. */
inline int max_size(string_t s);	                                            /* Возвращает максимальное количество символов в строке. */

/* Позиция сиволов в тексте */
inline char * at(const string_t s, int pos);                                    /* Возвращает ссылку на элемент в заданном положении в строке.*/
inline char * back(const string_t s);                                           /* Ссылка на последний элемент строки, которая не должна быть пустой.*/
inline char * begin(const string_t s);	                                        /* Возвращает ссылку на первый элемент в строке.*/
inline char * end(string_t s);	                                                /* Возвращает указатель адресующий расположение после последнего элемента в строке.*/
inline char * rbegin(string_t s);	                                            /* Возвращает указатель на первый элемент в обратной строке. */
inline char * rend(string_t s);	                                                /* Возвращает указатель, адресующий расположение после последнего элемента в обратной строке.*/

/* Редактирование строки */
string_t append(string_t s, const char * src, int len);                         /* Добавляет символы в конец строки.*/
string_t erase(string_t s, int begin, int end);	                                /* Удаляет элемент или диапазон элементов с указанного положения в строке. */
string_t pop_back(string_t s, int N);	                                        /* Удаляет последний элемент строки. */
string_t push_back(string_t s, char c);	                                        /* Добавляет элемент в конец строки. */
string_t insert(string_t s, int pos, const char *src, int len);                 /* Вставляет элемент, несколько элементов или диапазон элементов в строку в указанной позиции. */
string_t replace(string_t s, int pos, int len, const char *src, int sSize);	    /* Заменяет элементы в указанном положении в строке на указанные символ. */
//string_t resize(string_t s, int size);                                        /* Указывает новый размер строки, добавляя или удаляя элементы по мере необходимости.*/
//string_t substr(string_t s, int pos, int size);	                            /* Копирует из указанного положения в строке подстроку, содержащую по крайней мере несколько символов.*/
//void swap(string_t * first, string_t * second);	                            /* Меняет местами содержимое двух строк. */
//void clear(const string_t s);	                                                /* Удаляет все элементы строки. */

/* Поиск подстроки в строке */
int find(string_t s, const char * src);                                         /* Поиск по строке в прямом направлении до первого вхождения подстроки, совпадающей с заданной последовательностью символов.*/
//char * find_first_not_of(const string_t s, const char * src);	                /* Выполняет поиск по строке первого символа, который не является элементом указанной строки.*/
//char * find_first_of(const string_t s, const char * src);	                    /* Выполняет поиск первого символа в строке, совпадающего с любым элементом заданной строки.*/
//char * find_last_not_of(const string_t s, const char * src);	                /* Выполняет поиск по строке последнего символа, который не является элементом указанной строки.*/
//char * find_last_of(const string_t s, const char * src);	                    /* Выполняет поиск последнего символа в строке, совпадающего с любым элементом заданной строки. */
int rfind(string_t s, const char * src);                                        /* Поиск по строке в обратном направлении до первого вхождения подстроки, совпадающей с заданной последовательностью символов.*/
int ends_with(string_t s, const char * src);	                                /* Проверяет, заканчивается ли строка указанным суффиксом.*/
int starts_with(string_t s, const char * src);	                                /* Проверяет, начинается ли строка с указанным префиксом. */

/* Сравнение строк */
//int compare(const string_t first, const string_t second);	                    /* Сравнивает строку с указанной строкой, чтобы определить их равенство или установить, что одна из строк лексикографически меньше второй.*/

/* private */
#endif /*STRING_T_H*/