## Бинаризация ##

Выше представлена реализация алгоритма бинаризации с нахождением среза по методу Оцу.

Результаты неоптимизированного метода Оцу можно посмотреть в директории `results`.
Как видно, метод плохо справляется с бинаризацие изображения, когда часть его покрыта
тенями или просто затемнена.

Для лучшего результата была реализована оптимизация, заключающаяся
в том, чтобы разбить изображение на части и применять метод Оцу локально для этих частей.
Как можно увидеть (результаты оптимизированного метода представлены в директории `results2`),
алгоритм стал лучше справляться с затемненными участками и стал отделеять в них текст от фона,
однако появились проблемы с бинаризацией частей изображения, приблизительно монотонных (в основном
относящихся к фону), там стали появляться различного рода шумы.

Чтобы побороть эту проблему, была применена вторая оптимизация (результаты в директории `results3`),
заключающаяся в том, чтобы для каждого учаска изображения считать общее отклонение интенсивности пикселей от
среднего значения интенсивности на конкретном участке, и, в случае, если это отклонение довольно мало, в
ручную задавать значение среза (в данном случа это 128, т.е. половина значений интенсивности будет помечена белым,
другая половина - черным). Это позволило уменьшить количество шумов, однако полностью избавиться от них не удалось,
и возможно дальнейшее улучшение алгортма, направленное на избавления от этих самых шумов.

Логи и время работы алгоритма на каждом изображении лежат в файле `logs`.

Чтобы запустить бинаризацию тестовых изображений, выполнить
```
make
./a.out
```