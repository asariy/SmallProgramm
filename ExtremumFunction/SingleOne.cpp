#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

#define E 0.0000001
#define INF 0.000000001

using namespace std;
/* 
	������� ��� ���������� ����������� 
	6 + x + 3*x*x;
	x + 3*sin(2*x);
	y=2*sin(3*x)
	�� ��������� �������� -10..10
	� ��������� 0.0000001
*/

// ���������� �������� ������� � ����� �
double f(double x){
	double res;
	res = //6 + x + 3*x*x;
	x + 3*sin(2*x);
	//2 * sin(3 * x);
	return res;
}

// ���������� �������� ������ ����������� �������
double df(double x, double h){
	double res;
	res = (f(x + h) - f(x - h)) / (2 * h);
	return res;
}
// �������� �� ������� �������������� �����
double A = -10, B = 10;

// ������� ���������� ���� �������� �� ��������� � �������� �����
double crit(double a, double b, double e){
	double res = df(a, e);
	for (double i = a; i <= b; i+=e)
	{
		// ���� �������� ������������� �����������
		if((-E < df(i, e)) && (df(i, e) < E)){
			return i;
		}
		// ����������� ������ ���� � - �� +   - ���� ������� �� ����� ��������� � ������� �����
		if(res < 0 && df(i, e) > 0){
			return crit(i-e, i+e, e/10);
		}
		// ����������� ������ ���� � + �� -   - ���� �������� �� ����� ��������� � ������� �����
		if(res > 0 && df(i, e) < 0){
			return crit(i-e, i+e, e/10);
		}
		// ��������� ��������
		res = df(i, e);
	}
	return INF;
}

int main(int argc, char* argv[])
{
	double res = df(A, 1), ans;
	// ������ ��� �������� �������� � ����������� ������
	vector<double> min, max;
	for (double i = A; i <= B; i+=1)
	{
		// ����������� ������ ���� � - �� +   - ���� ������� �� ����� ��������� � ������� �����
		if((res < 0) && (df(i, 1) > 0)){
			ans = crit(i-1, i+1, 0.1);
			// ��������� ��������� ��������
			min.push_back(ans);
		}
		// ����������� ������ ���� � + �� -   - ���� �������� �� ����� ��������� � ������� �����
		if((res > 0) && (df(i, 1) < 0)){
			ans = crit(i-1, i+1, 0.1);
			// ��������� ��������� ��������
			max.push_back(ans);
		}
		res = df(i, 1);
		
	}	

	double amax, amin;

	// ������� ����� ������� �������� ������� �� ��������� � ������� �� �����
	if(max.size() > 0){
		amax = max[0];
		for (unsigned int i = 0; i < max.size(); i++)
		{
			amax = f(amax) < f(max[i]) ? max[i] : amax;
		}
		printf("max function = %.3f in x = %.3f\n", f(amax), amax);
	}
	// ������� ����� ������� �������� ������� �� ��������� � ������� �� �����
	if(min.size() > 0){
		amin = min[0];
		for (unsigned int i = 0; i < min.size(); i++)
		{
			amin = f(amin) > f(min[i]) ? min[i] : amin;
		}
		printf("min function = %.3f in x = %.3f\n", f(amin), amin);
	}

	// ��� �������� - ������� �������� ������� �� ������� ���������
	/*
	for (double i = A; i <= B; i+=0.1){
		printf("%.3f\n", f(i));
	}
	*/
	cin.get();
	return 0;
}

