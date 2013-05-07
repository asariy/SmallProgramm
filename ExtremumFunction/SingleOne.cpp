#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

#define E 0.0000001
#define INF 0.000000001

using namespace std;
/* 
	функция для нахождения экстремумов 
	6 + x + 3*x*x;
	x + 3*sin(2*x);
	y=2*sin(3*x)
	на диапозоне значений -10..10
	с точностью 0.0000001
*/

// Возвращает значение функции в точке х
double f(double x){
	double res;
	res = //6 + x + 3*x*x;
	x + 3*sin(2*x);
	//2 * sin(3 * x);
	return res;
}

// Возвращает значение первой производной функции
double df(double x, double h){
	double res;
	res = (f(x + h) - f(x - h)) / (2 * h);
	return res;
}
// Интервал на котором осуществляется поиск
double A = -10, B = 10;

// функция рекурсивно ищет значение на интервале с заданным шагом
double crit(double a, double b, double e){
	double res = df(a, e);
	for (double i = a; i <= b; i+=e)
	{
		// Если значение удовлетворяет погрешность
		if((-E < df(i, e)) && (df(i, e) < E)){
			return i;
		}
		// производная меняет знак с - на +   - ищем минимум на новом интервале с меньшим шагом
		if(res < 0 && df(i, e) > 0){
			return crit(i-e, i+e, e/10);
		}
		// производная меняет знак с + на -   - ищем максимум на новом интервале с меньшим шагом
		if(res > 0 && df(i, e) < 0){
			return crit(i-e, i+e, e/10);
		}
		// следующие значение
		res = df(i, e);
	}
	return INF;
}

int main(int argc, char* argv[])
{
	double res = df(A, 1), ans;
	// Храним все найденые значения в критических точках
	vector<double> min, max;
	for (double i = A; i <= B; i+=1)
	{
		// производная меняет знак с - на +   - ищем минимум на новом интервале с меньшим шагом
		if((res < 0) && (df(i, 1) > 0)){
			ans = crit(i-1, i+1, 0.1);
			// Сохраняем найденное значение
			min.push_back(ans);
		}
		// производная меняет знак с + на -   - ищем максимум на новом интервале с меньшим шагом
		if((res > 0) && (df(i, 1) < 0)){
			ans = crit(i-1, i+1, 0.1);
			// Сохраняем найденное значение
			max.push_back(ans);
		}
		res = df(i, 1);
		
	}	

	double amax, amin;

	// Находим самое большое значение функции из найденных и выводим на экран
	if(max.size() > 0){
		amax = max[0];
		for (unsigned int i = 0; i < max.size(); i++)
		{
			amax = f(amax) < f(max[i]) ? max[i] : amax;
		}
		printf("max function = %.3f in x = %.3f\n", f(amax), amax);
	}
	// Находим самое меньшее значение функции из найденных и выводим на экран
	if(min.size() > 0){
		amin = min[0];
		for (unsigned int i = 0; i < min.size(); i++)
		{
			amin = f(amin) > f(min[i]) ? min[i] : amin;
		}
		printf("min function = %.3f in x = %.3f\n", f(amin), amin);
	}

	// для проверки - выводит значение функции на задоном интервале
	/*
	for (double i = A; i <= B; i+=0.1){
		printf("%.3f\n", f(i));
	}
	*/
	cin.get();
	return 0;
}

