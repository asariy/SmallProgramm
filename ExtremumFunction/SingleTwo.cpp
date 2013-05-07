#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>

#define FUN 6 //Выбор функции по умолчанию значения от 1 до 6

//Точность вычислений
#define E 0.000001
//Граничное значения шага измерений
#define INF 0.000000000001
//Используется для округления числа
#define ADD 0.500001
#define D 100000

using namespace std;
//структура точки - для хранения значения х и у
struct point{
	double x;
	double y;
	double e;
};
//оператор сравнения структуры точки
bool operator == (point a, point b){
	if(a.x == b.x && a.y == b.y) return true;
	return false;
}
int FunNum = FUN;	// номер функции
// Возвращает значение функции в точке х
// Выбирается одна из 6 функций
// Значение по умолчанию указывается в define FUN
double f(double x, double y){
	double res;
	switch (FunNum)
	{
	case 1:	res = x*x + y*y;
		break;
	case 2:	res = 3*x*x*y - x*x*x - y*y*y*y;
		break;
	case 3:	res = 3*x*x*x + y*y +4*x*y - x + 2;
		break;
	case 4:	res = -x*x+7*y-y*y;
		break;
	case 5:	res = x*x*x + 8*y*y*y - 6*x*y + 5;
		break;
	case 6:	res = 2*x*x*x + x*y*y + 5*x*x + y*y - 1;
		break;
	default: res = 2*x*x*x + x*y*y + 5*x*x + y*y - 1;
		break;
	}
	    //x*x + y*y;
		//3*x*x*y - x*x*x - y*y*y*y;
		//3*x*x*x + y*y +4*x*y - x + 2;
		//-x*x+7*y-y*y;
		//x*x*x + 8*y*y*y - 6*x*y + 5;
		//2*x*x*x + x*y*y + 5*x*x + y*y - 1;
	return res;
}

// Возвращает значение первой производной функции
double dfx(double x, double y, double h){
	double res;
	res = (f(x + E, y) - f(x - E, y)) / (2 * E);
	return res;
}
double dfy(double x, double y, double h){
	double res;
	res = (f(x, y + h) - f(x, y - h)) / (2 * h);
	return res;
}
// Интервал на котором осуществляется поиск
double Ax = -100, Bx = 100;
double Ay = -100, By = 100;

// Храним все найденые значения в критических точках
vector<point> Min(0);
vector<point> Max(0);
//функция добавления минимума с учетом погрешности
void addMin(point a){
	double x = floor(a.x*D + ADD)/D;
	double y = floor(a.y*D + ADD)/D;
	point tmp;
	tmp.x = x;
	tmp.y = y;
	tmp.e = E;
	for (int i = 0; i < Min.size(); i++)
	{
		if(Min[i] == tmp) return;
	}
	Min.push_back(tmp);
}
//функция добавления максимума с учетом погрешности
void addMax(point a){
	double x = floor(a.x*D + ADD)/D;
	double y = floor(a.y*D + ADD)/D;
	point tmp;
	tmp.x = x;
	tmp.y = y;
	tmp.e = E;
	for (int i = 0; i < Max.size(); i++)
	{
		if(Max[i] == tmp) return;
	}
	Max.push_back(tmp);
}

// функция рекурсивно ищет значение на интервале с заданным шагом
void crit(double ax, double bx, double ay, double by, double e){
	point tmp;
	double resX;
	double resY;
	for (double y = ay; y <= by; y+=e)
		for (double x = ax; x <= bx; x+=e)
		{
			resX = dfx(x-e, y, e);
			resY = dfy(x, y-e, e);

			//если шаг мал то выходим с функции
			if (e < INF) return;

			// Если значение удовлетворяет погрешность
			if(((-E < dfx(x, y, e)) && (dfx(x, y, e) < E)) &&
				((-E < dfy(x, y, e)) && (dfy(x, y, e) < E))){
					tmp.x = x;
					tmp.y = y;
					tmp.e = e;
					// производная меняет знак с - на +   - мы нашли минимум
					if(((resX < 0) && (dfx(x+e, y, e) > 0)) &&
						((resY < 0) && (dfy(x, y+e, e) > 0))){
							addMin(tmp);
							return;
					}
					// производная меняет знак с + на -   - мы нашли максимум
					if(((resX > 0) && (dfx(x+e, y, e) < 0)) &&
						((resY > 0) && (dfy(x, y+e, e) < 0))){
							addMax(tmp);
							return;
					}
			}
			// производная меняет знак с - на +   - ищем минимум на новом интервале с меньшим шагом
			if(((resX < 0) && (dfx(x+e, y, e) > 0)) &&
				((resY < 0) && (dfy(x, y+e, e) > 0))){
				crit(x-e, x+e, y-e, y+e, e/10);
			}
			// производная меняет знак с + на -   - ищем максимум на новом интервале с меньшим шагом
			if(((resX > 0) && (dfx(x+e, y, e) < 0)) &&
				((resY > 0) && (dfy(x, y+e, e) < 0))){
				crit(x-e, x+e, y-e, y+e, e/10);
			}
		}
	return;
}



//Интерфейс выбора функции
void getFunction(){
	bool flag = true;
	string str;
	do{
				
		printf("1: x*x + y*y\n");
		printf("2: 3*x*x*y - x*x*x - y*y*y*y\n");
		printf("3: 3*x*x*x + y*y +4*x*y - x + 2\n");
		printf("4: -x*x+7*y-y*y\n");
		printf("5: x*x*x + 8*y*y*y - 6*x*y + 5\n");
		printf("6: 2*x*x*x + x*y*y + 5*x*x + y*y - 1\n");
		printf("Select number of function [6]:");
			
		getline(cin, str);
		if(str.size() == 1 && str[0] >= '1' && str[0] <= '6'){
			FunNum = atoi(str.substr(0,1).c_str());
			flag = false;
		}
		if(str == ""){
			FunNum = 6;
			flag = false;
		}

		printf("\n");
	} while(flag);
}
//Интерфейс выбора границ поиска
void getRange(){
	printf("Enter range of X and Y.\n");
	printf("Enter begin of X:");
	cin >> Ax;
	printf("Enter end of X:");
	cin >> Bx;
	printf("Enter begin of Y:");
	cin >> Ay;
	printf("Enter end of Y:");
	cin >> By;
	printf("\n");
}


int main(int argc, char* argv[])
{
	getFunction();
	getRange();

	crit(Ax, Bx, Ay, By, 1);
	
	point amax, amin;

	// Находим самое меньшее значение функции из найденных и выводим на экран
	if(Min.size() > 0){
		amin = Min[0];
		for (unsigned int i = 0; i < Min.size(); i++)
		{
			amin = f(amin.x,amin.y) > f(Min[i].x,Min[i].y) ? Min[i] : amin;
		}
		printf("min function = %.3f \tin \tx = %.3f \ty = %.3f\n", f(amin.x, amin.y), amin.x, amin.y);
	}else
	{
		printf("NO Min\n");
	}
	
	// Находим самое большое значение функции из найденных и выводим на экран
	if(Max.size() > 0){
		amax = Max[0];
		for (unsigned int i = 0; i < Max.size(); i++)
		{
			amax = f(amax.x,amax.y) > f(Max[i].x,Max[i].y) ? Max[i] : amax;
		}
		printf("max function = %.3f \tin \tx = %.3f \ty = %.3f\n", f(amax.x, amax.y), amax.x, amax.y);
	}else
	{
		printf("NO Max\n");
	}
	
	char r;
	cin >> r;

	return 0;
}

