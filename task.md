# Завдання на виконання роботи

1.  Написати програму, що моделює процес управління пам’яттю (розподілу
пам’яті для процесів), перетворення віртуальної адреси у фізичну, пошук у
пам’яті за запитами процесів, вивільнення пам’яті) при заданому варіантом
способі організації пам’яті (перелік варіантів представлений нижче).
    
    Вхідні дані – розмір пам’яті, що підлягає розподілу, розміри сторінок
(розділів, сегментів тощо), розміри потрібної процесам пам’яті та ін. задаються самостійно та у відповідності до завдання.

2.  Продемонструвати роботу моделі з виконанням основних операцій з
пам’яттю: надання пам’яті потрібного розміру за запитом процесу, перетворення  іртуальної адреси у фізичну‖ при зверненні до комірки пам’яті, здійнення запису або читання, вивільнення пам’яті при завершенні процесу. Завдання операцій можна реалізувати за допомогою меню.

    Якщо потрібно за алгоритмом, додатково продемонструвати процес
вивантаження-завантаження сегментів або сторінок.

    Вихідні дані – наглядна інформація про поточний розподіл пам’яті (карта
пам’яті), що містить адреси ділянок пам’яті, стан (вільно чи зайнято та ким) після кожної операції з пам’яттю.
    
    Окремо показати коректність перетворення віртуальної адреси звернення
до комірки пам’яті за запитом у фізичн адресу і здійнення запису до комірки та читання з неї.

    <em>    Примітка. При моделюванні алгоритмів без використання зовнішньої пам’яті вважати, що вихідні адреси кожного незавантаженого процесу починаються з 0000..00, а розміри кожного із процесів – довільно задані. Кожній адресі незавантаженого процесу за запитом варто ставити відповідність реальну адресу пам’яті, якщо процес завантажується.Завантаження та вивантаження (при необхідності) процесів виконувати згідно з заданою чергою.

    При моделюванні алгоритмів з використанням зовнішньої пам'яті слід задати віртуальний адресний простір кожного процесу. Звернення до пам'яті виконувати за віртуальною адресою, здійснювати при еобхідності завантаження і вивантаження відповідних частин процесів, показуючи начення конкретної фізичної адреси, що відповідає заданій віртуальній. 
    
    При моделюванні алгоритмів роботи кеш пам'яті, адресний простір кеш пам'яті і основної пам'яті може задаватися довільно при співвідношенні їх обсягів не менш ніж 1:10. Пошук, запис і заміщення інформації в кеш  ам'яті повинно виконуватися шляхом завданняшуканих адрес основної пам'яті.</em>

# Варіант 1
<em>Фіксовані розділи</em> (без використання зовнішньої пам’яті). Кількість розділів меньша, ніж кількість процесів. Процеси утворюють загальну чергу до розділів пам’яті. Використовується <em>лінійний</em> адресний простір. Розміри процесів задаються випадково.

