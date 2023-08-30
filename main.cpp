


//=============================================================================
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <random>
#include <list>

using namespace std;
//=============================================================================



const double PI = acos(-1);

struct Point {
	Point(double X, double Y, double Z = 0) : x(X), y(Y), z(Z) {}
	double x, y, z;
};		// представление точки
struct Vector {
	Vector(double X, double Y, double Z = 0) : x(X), y(Y), z(Z) {}
	double x, y, z;
};		// представление вектора


std::ostream& operator<<(std::ostream& os, const Point& p)
{	// выводим точку в поток
	return os << " P{" << p.x << ", " << p.y << ", " << p.z << "}";
}

std::ostream& operator<<(std::ostream& os, const Vector& v)
{	// выводим вектор в поток
	return os << " V{" << v.x << ", " << v.y << ", " << v.z << "}";
}



// абстракный базовый класс
class Base
{
public:
	Base(const Point& center)
		: c(center)
	{}

	virtual ~Base() {}

	//точка кривой по параметру t
	virtual Point curve_point(double t) const = 0;		//чисто виртуальная  
	//вектор(первая производная) кривой по параметру t
	virtual Vector curve_vector(double t) const = 0;	//чисто виртуальная 
	//информация о классе
	virtual string info() const = 0;	//чисто виртуальная 

protected:
	Point c;
};

//Окружность
class Circle : public Base
{
public:
	Circle(const Point& center, double rad)
		: Base(center), r(rad)
	{}

	double radius() const { return r; }

	//точка кривой по параметру t
	Point curve_point(double t) const override
	{
		return Point(r * cos(t) + c.x, r * sin(t) + c.y, c.z);
	}

	//вектор(первая производная) кривой по параметру t
	Vector curve_vector(double t) const override
	{
		return Vector(-1 * r * sin(t), r * cos(t));
	}

	//рандомная окружность
	static shared_ptr<Base> getCircle()
	{
		uniform_real_distribution<double> dis{ -1, 1 };
		random_device rd;
		return make_shared<Circle>(Point(dis(rd), dis(rd)), abs(dis(rd)));
	}

	//информация о классе
	string info() const override
	{
		return "Circle ";
	}
private:
	double r;
};

// Эллипс
class Elli : public Base
{
public:
	Elli(const Point& center, double rad_x, double rad_y)
		: Base(center),
		r_x(rad_x),
		r_y(rad_y)
	{}

	//точка кривой по параметру t
	Point curve_point(double t) const override
	{
		return Point(r_x * cos(t) + c.x, r_y * sin(t) + c.y, c.z);
	}

	//вектор(первая производная) кривой по параметру t
	Vector curve_vector(double t) const override
	{
		return Vector(-1 * r_x * sin(t), r_y * cos(t));
	}

	//рандомный эллипс
	static shared_ptr<Base> getElli()
	{
		uniform_real_distribution<double> dis{ -1, 1 };
		random_device rd;
		return make_shared<Elli>(Point(dis(rd), dis(rd)), abs(dis(rd)), abs(dis(rd)));
	}

	//информация о классе
	string info() const override
	{
		return "Ellips ";
	}

private:
	double r_x;
	double r_y;
};

// Спираль
class Spiral : public Base
{
public:
	Spiral(const Point& center, double rad, double step)
		: Base(center),
		r(rad),
		s(step)
	{}

	//точка кривой по параметру t
	Point curve_point(double t) const override
	{
		return Point(r * cos(t) + c.x, r * sin(t) + c.y, s * t + c.z);
	}

	//вектор(первая производная) кривой по параметру t
	Vector curve_vector(double t) const override
	{
		return Vector(-1 * r * sin(t), r * cos(t), s);
	}

	//рандомная спираль
	static shared_ptr<Base> getSpiral()
	{
		uniform_real_distribution<double> dis{ -1, 1 };
		random_device rd;
		return make_shared<Spiral>(Point(dis(rd), dis(rd), dis(rd)), abs(dis(rd)), abs(dis(rd)));
	}

	//информация о классе
	string info() const override
	{
		return "Spiral ";
	}

private:
	double r;	//радиус
	double s;	//шаг
};

//генерирует случайный объект кривых
shared_ptr<Base> getRandCurve() {
	enum curve { circle, elli, spiral };
	std::uniform_int_distribution<> dis_num{ circle, spiral };
	random_device rd;

	switch (dis_num(rd))
	{
	case circle:
		return Circle::getCircle();
	case elli:
		return Elli::getElli();
	case spiral:
		return Spiral::getSpiral();
	default:
		;
	}
}

//вывод контейнера
void display(const list<shared_ptr<Base>>& list) {
	for (auto i : list) {
		cout << i->info() << i->curve_point(PI / 4) << i->curve_vector(PI / 4) << endl;
		cout << " count ptr = " << i.use_count() << endl;
	}
	cout << string(79, '-') << endl << endl;
}

void display(const list<shared_ptr<Circle>>& list) {
	for (auto i : list) {
		cout << i->info() << i->curve_point(PI / 4) << i->curve_vector(PI / 4) << " r = " << i->radius() << endl;
		cout << " count ptr = " << i.use_count() << endl;
	}
	cout << string(79, '-') << endl << endl;
}


//*****************************************************************************
int main()
{
	const size_t size = 10;

	list<shared_ptr<Base>> listOne;	//первый контейнер
	list<shared_ptr<Circle>> listTwo;	//Только объекты cirkle
	double sum = 0;

	//заполняем контейнер случайными кривыми
	for (register size_t i = 0; i < size; ++i)
		listOne.emplace_back(getRandCurve());

	//выводим содержимое исходного контейнера
	cout << "List ONE\n" << string(79, '-') << endl;
	display(listOne);


	//разделить объекты circle с другим контейнером + вычисления
	for (auto i : listOne) {
		if (i->info() == "Circle ") {
			listTwo.emplace_back(dynamic_pointer_cast<Circle>(i));
			sum += listTwo.back()->radius();
		}
	}

	cout << "List ONE after cast\n" << string(79, '-') << endl;
	display(listOne);

	cout << "List TWO after cast\n" << string(79, '-') << endl;
	display(listTwo);

	cout << string(79, '-') << " Amount radius listTwo = " << sum << "\n" << string(79, '-') << endl << endl;

	//сортировка
	listTwo.sort([](const shared_ptr<Circle>& l, const shared_ptr<Circle>& r) {
		return l->radius() < r->radius();
		});

	cout << "List TWO after sort\n" << string(79, '-') << endl;
	display(listTwo);

	cout << endl;
	system("pause");
	return 0;
}
//*****************************************************************************

