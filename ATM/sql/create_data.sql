INSERT INTO Clients VALUES
(1, 'Tom L. Lemon'),
(2, 'Sam S. Nolan'),
(3, 'Raychel R. Allan');

INSERT INTO Accounts VALUES
(1, 1, 1000),
(2, 2, 1000),
(3, 2, 1000),
(4, 3, 500),
(5, 3, 6000),
(6, 3, 100);

INSERT INTO card VALUES
(1, 1, 8080, true),
(2, 2, 1234, true),
(3, 3, 0000, false);

INSERT INTO ATM VALUES
(1, 4, 
ARRAY[10, 10, 10, 10, 0, 0, 0, 0, 0, 0],
ARRAY[10, 20, 50, 100, 0, 0, 0, 0, 0, 0],
'455 Larkspur Dr. California Springs, CA 92926 USA');

