# Tasks
Задание №1
Реализовать клиент-серверное приложение в среде разработки Qt/C++. Приложение должно иметь графический интерфейс, при этом серверная часть отвечает за контроль правильности вводимых пользователем параметров и осуществляет непрерывную передачу данных по протоколу UDP, а клиентская – за отображение принимаемых параметров. Данные должны быть упакованы в пакет, который содержит 4 слова данных, каждое слово данных должно иметь объем 2 байта. Слова данных должны иметь следующую структуру:
Номер бита 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
1 слово    0   0  x  x  x  x  x  x  0  0  x  x  x  x  x  x
2 слово    0   0  x  x  x  x  x  x  x  x  x  x  x  x  x  x
3 слово    x   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x
4 слово  R15 R14 R13 R12 R11 R10 R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
Расшифровка параметров:
Обозначение  Название      Занимаемое кол-во бит  Диапазон значений Тип значений
X            Координата Х  6                      от 0 до 63        Целое
Y            Координата Y
6
от -32 до 31
Целое
V
Скорость
8
от 0 до 255
Целое
М
Режим работы
2
от 0 до 3
Целое
S
Состояние
2
от 0 до 3
Целое
A
Ускорение
8
от -12.7 до 12.8
Дробное
P
Мощность
8
от 0 до 130
Целое
R
Резерв
16
–
–
Пользователь в интерфейсе должен иметь возможность изменять значения параметров X, Y, V, M, S, A, P путем ввода конкретных значений, а параметра R в виде редактирования конкретных бит. В клиентской и серверной частях должна быть реализована возможность настройки сетевого соединения.
