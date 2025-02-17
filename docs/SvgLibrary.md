# Библиотека для вывода SVG

Поскольку основное применение этой библиотеки — визуализация «Транспортного справочника», реализовано три типа объектов:

* Круг (Circle). Описывается координатами центра и радиусом;
* Ломаная (Polyline). Описывается координатами вершин;
* Надпись (Text). Описывается текстовым содержимым, координатами опорной точки, смещением относительно опорной точки, размером и названием шрифта.

Все типы объектов имеют следующие свойства:

* цвет линии и цвет заливки;
* толщина линии;
* тип формы конца линии и соединения линий.

Пример:

```cpp
Svg::Document svg;

svg.Add(
    Svg::Polyline{}
    .SetStrokeColor(Svg::Rgb{140, 198, 63})  // soft green
    .SetStrokeWidth(16)
    .SetStrokeLineCap("round")
    .AddPoint({50, 50})
    .AddPoint({250, 250})
);

for (const auto point : {Svg::Point{50, 50}, Svg::Point{250, 250}}) {
  svg.Add(
      Svg::Circle{}
      .SetFillColor("white")
      .SetRadius(6)
      .SetCenter(point)
  );
}

svg.Add(
    Svg::Text{}
    .SetPoint({50, 50})
    .SetOffset({10, -10})
    .SetFontSize(20)
    .SetFontFamily("Verdana")
    .SetFillColor("black")
    .SetData("C")
);
svg.Add(
    Svg::Text{}
    .SetPoint({250, 250})
    .SetOffset({10, -10})
    .SetFontSize(20)
    .SetFontFamily("Verdana")
    .SetFillColor("black")
    .SetData("C++")
);

svg.Render(std::cout);
```

Вывод:

```svg
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
<polyline points="50,50 250,250 " fill="none" stroke="rgb(140,198,63)" 
stroke-width="16" stroke-linecap="round" /><circle cx="50" cy="50" 
r="6" fill="white" stroke="none" stroke-width="1" /><circle cx="250" cy="250" 
r="6" fill="white" stroke="none" stroke-width="1" /><text x="50" y="50" 
dx="10" dy="-10" font-size="20" font-family="Verdana" fill="black" stroke="none" 
stroke-width="1" >C</text><text x="250" y="250" dx="10" dy="-10" font-size="20" 
font-family="Verdana" fill="black" stroke="none" stroke-width="1" >C++</text></svg>
```

## Вспомогательные классы

### Point

Point — структура из двух полей x и y типа double. Можно создать точку с помощью выражения Point{x, y}, а также с помощью конструктора по умолчанию и затем заполнить поля x и y прямым обращением к ним.

### Rgb и Rgba

Rgb — структура из целочисленных полей red, green, blue. Можно создать  объект с помощью выражения Rgb{red, green, blue}, а также с помощью конструктора по умолчанию и затем заполнить поля red, green и blue прямым обращением к ним. Поля заполняются значениями от 0 до 255. В Rgba (RGB с альфа-каналом) также присутствует поле alpha, имеющее тип double.

### Color

Color — тип, который можно проинициализировать одним из трёх способов:

* Конструктором по умолчанию. Такой цвет выводится как none;
* Строкой (std::string). Такой цвет выводится непосредственно как содержимое строки;
* Структурой Rgb. Такой цвет выводится в виде rgb(red,green,blue);
* Структурой Rgba. Такой цвет выводится в виде rgb(red,green,blue,alpha). 

### Document

Document — класс, с помощью которого производится компоновка и отрисовка SVG-документа. Класс поддерживает следующие операции:

* Создание с помощью конструктора по умолчанию: Svg::Document svg;
* Добавление объекта: svg.Add(object), где object имеет тип Circle, Polyline или Text;
* Отрисовка (формирование результирующей строки): svg.Render(out), где out — наследник std::ostream.

## Методы выставления свойств объектов

Классы Circle, Polyline и Text должны поддерживать создание с помощью конструктора по умолчанию, а также поддерживать описанные ниже методы.

Все описанные методы принимают строго одно значение. Упомянутые умолчания — значения свойств в отсутствие вызова соответствующего Set-метода.

### Общие

* SetFillColor(const Color&): задаёт значение свойства fill — цвет заливки. Значение по умолчанию — NoneColor;
* SetStrokeColor(const Color&): задаёт значение свойства stroke — цвет линии. Значение по умолчанию — NoneColor;
* SetStrokeWidth(double): задаёт значение свойства stroke-width — толщину линии. Значение по умолчанию — 1.0;
* SetStrokeLineCap(const string&): задаёт значение свойства stroke-linecap — тип формы конца линии. По умолчанию свойство не выводится;
* SetStrokeLineJoin(const string&): задаёт значение свойства stroke-linejoin — тип формы соединения линий. По умолчанию свойство не выводится.

### Circle

* SetCenter(Point): задаёт значения свойств cx и cy — координаты центра круга. Значения по умолчанию — 0.0;
* SetRadius(double): задаёт значение свойства r — радиус круга. Значение по умолчанию — 1.0.

### Polyline

* AddPoint(Point): добавляет вершину ломаной — элемент свойства points, записываемый в виде x,y и отделяемый пробелами от соседних элементов (см. примеры). Значение свойства по умолчанию — пустая строка.

### Text

* SetPoint(Point): задаёт значения свойств x и y — координаты текста. Значения по умолчанию — 0.0;
* SetOffset(Point): задаёт значения свойств dx и dy — величины отступа текста от координаты. Значения по умолчанию — 0.0;
* SetFontSize(uint32_t): задаёт значение свойства font-size — размер шрифта. Значение по умолчанию — 1;
* SetFontFamily(const string&): задаёт значение свойства font-family — название шрифта. По умолчанию свойство не выводится;
* SetData(const string&): задаёт содержимое тега <text> — непосредственно выводимый текст. По умолчанию текст пуст.

## Отрисовка

Строка, собираемая в методе Svg::Document::Render, должна быть устроена следующим образом:

```svg
<?xml version="1.0" encoding="UTF-8" ?>
 <svg xmlns="http://www.w3.org/2000/svg" version="1.1">
 Объекты, добавленные с помощью Svg::Document::Add, в порядке их добавления.
</svg>
```

Все свойства объектов выводятся в следующем формате: название свойства, символ = и затем значение в кавычках.

Лишние пробельные символы допускаются:

1. Между тегами. (За исключением текста между открывающим и закрывающим тегами <text>.)
1. Между свойствами, перед и после списка свойств, вокруг символа =, разделяющего название свойства и значение.
1. Между координатами, перед и после списка координат в значении свойства points.

### Circle

Круг отображается следующим образом: строка "<circle ", затем через пробел свойства в произвольном порядке, затем строка "/>"

### Polyline

Ломаная отображается следующим образом: строка "<polyline ", затем через пробел свойства в произвольном порядке, затем строка "/>"

### Text

Текст отображается следующим образом: строка "<text ", затем через пробел свойства в произвольном порядке, затем символ ">", содержимое надписи и закрывающий тег "</text>"
