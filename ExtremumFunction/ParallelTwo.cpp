#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <mpi.h>

#define FUN 6 //Выбор функции по умолчанию значения от 1 до 6

//Точность вычислений
#define E 0.000001
//Граничное значения шага измерений
#define INF 0.000000000001
//Используется для округления числа
#define ADD 0.500001
#define D 100000
//модуль числа
#define ABS(a) ((a)>=0?a:-a)

using namespace std;

// Интервал на котором осуществляется поиск
double Ax = -100, Bx = 100;
double Ay = -100, By = 100;

int FunNum = FUN;		// номер функции
int ProcNum;      // количество процесоров
int ProcRank;     // номер текущего процесора
//Массивы для передачи данных между процесами
int *pReceiveNum;  
int *pReceiveInd;
//Масывы результатов для максимумов 
double *pResultMaxX;
double *pResultMaxY;
//Количество максимумв в массивк
int sizeMax;
//Массивы результатов для минимумов
double *pResultMinX;
double *pResultMinY;
//Количество минимумов в масиве
int sizeMin;

//Массивы для результатов вычеслений на процесе
double *pProcResultX;
double *pProcResultY;

//структура точки - для хранения значения х и у
struct point{
	double x;
	double y;
};
//оператор сравнения структуры точки
bool operator == (point a, point b){
	if(a.x == b.x && a.y == b.y) return true;
	return false;
}

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
	if (ProcRank == 0) {
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
		MPI_Bcast(&FunNum, 1, MPI_INT, 0, MPI_COMM_WORLD); 
	}
}
//Интерфейс выбора границ поиска
void getRange(){
	if(ProcRank == 0){
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
		MPI_Bcast(&Ax, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
		MPI_Bcast(&Bx, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
		MPI_Bcast(&Ay, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
		MPI_Bcast(&By, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
	}
}

int main(int argc, char* argv[])
{
	int procRec[1];
	//Для работы с таймером
	double Start, Finish, Duration; 
	//Инициализируем MPI
	MPI_Init(&argc, &argv);   
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);  //количество процесов
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);	//номер текущего процеса
	//getFunction(); // не работает на MPI
	//getRange();	// не работает на MPI
	
	//Задаем размер массивов равный количеству процесоров
	pReceiveNum = new int [ProcNum];   
	pReceiveInd = new int [ProcNum];
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);
	//Засекаем время
	Start = MPI_Wtime(); 
	//Разделение данных между процесами
	if(ABS(Bx-Ax) > ABS(By-Ay)){
		crit(Ax + ProcRank*(ABS(Bx-Ax)/ProcNum), Ax + (ProcRank+1)*(ABS(Bx-Ax)/ProcNum), Ay, By,1);
	} else {
		crit(Ax, Bx, Ay + ProcRank*(ABS(By-Ay)/ProcNum), Ay + (ProcRank+1)*(ABS(By-Ay)/ProcNum),1);
	}

	//Сохранение количества найденных максимумов
	procRec[0] = Max.size();
	MPI_Allgather(procRec, 1, MPI_INT, pReceiveNum, 1, MPI_INT, MPI_COMM_WORLD); 
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);

	//Подготовка для сбора данных со всех процесов
	pReceiveInd[0] = 0;
	for (int i = 1; i < ProcNum; i++)
	{
		pReceiveInd[i] = pReceiveInd[i-1] + pReceiveNum[i-1];
	}
	sizeMax = pReceiveInd[ProcNum-1] + pReceiveNum[ProcNum-1];
	pResultMaxX = new double[sizeMax];
	pResultMaxY = new double[sizeMax];
	pProcResultX = new double[Max.size()];
	pProcResultY = new double[Max.size()];
	for (int i = 0; i < Max.size(); i++)
	{
		pProcResultX[i] = Max[i].x;
		pProcResultY[i] = Max[i].y;
	}
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);
	//Собираем все найденные максимумы
	MPI_Allgatherv(pProcResultX, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMaxX, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	MPI_Allgatherv(pProcResultY, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMaxY, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);
	//Количество найденных минимумов на процесе
	procRec[0] = Min.size();
	MPI_Allgather(procRec, 1, MPI_INT, pReceiveNum, 1, MPI_INT, MPI_COMM_WORLD); 
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);
	pReceiveInd[0] = 0;
	for (int i = 1; i < ProcNum; i++)
	{
		pReceiveInd[i] = pReceiveInd[i-1] + pReceiveNum[i-1];
	}
	sizeMin = pReceiveInd[ProcNum-1] + pReceiveNum[ProcNum-1];
	pResultMinX = new double[sizeMin];
	pResultMinY = new double[sizeMin];
	pProcResultX = new double[Min.size()];
	pProcResultY = new double[Min.size()];
	for (int i = 0; i < Min.size(); i++)
	{
		pProcResultX[i] = Min[i].x;
		pProcResultY[i] = Min[i].y;
	}
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);
	//Собираем все найденные минимумы
	MPI_Allgatherv(pProcResultX, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMinX, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	MPI_Allgatherv(pProcResultY, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMinY, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	//Синхронизация процесов
	MPI_Barrier(MPI_COMM_WORLD);
	//Заканчиваем считать время
	Finish = MPI_Wtime(); 
	Duration = Finish-Start;
	//Если нулевой процесс выводим результат
	if (ProcRank == 0) {     
		double aX, aY, tx, ty;
		
		// Находим самое меньшее значение функции из найденных и выводим на экран
		if(sizeMin > 0){
			aX = pResultMinX[0];
			aY = pResultMinY[0];
			for (unsigned int i = 0; i < sizeMin; i++)
			{
				tx = f(aX,aY) > f(pResultMinX[i], pResultMinY[i]) ? pResultMinX[i] : aX;
				ty = f(aX,aY) > f(pResultMinX[i], pResultMinY[i]) ? pResultMinY[i] : aY;
				aX = tx;
				aY = ty;
			}
			printf("min function = %.3f \tin \tx = %.3f \ty = %.3f\n", f(aX, aY), aX, aY);
		}else
		{
			printf("NO Min\n");
		}
	
		// Находим самое большое значение функции из найденных и выводим на экран
		if(sizeMax > 0){
			aX = pResultMaxX[0];
			aY = pResultMaxY[0];
			for (unsigned int i = 0; i < sizeMax; i++)
			{
				tx = f(aX,aY) > f(pResultMaxX[i], pResultMaxY[i]) ? pResultMaxX[i] : aX;
				ty = f(aX,aY) > f(pResultMaxX[i], pResultMaxY[i]) ? pResultMaxY[i] : aY;
				aX = tx;
				aY = ty;
			}
			printf("max function = %.3f \tin \tx = %.3f \ty = %.3f\n", f(aX, aY), aX, aY);
		}else
		{
			printf("NO Max\n");
		}
		//Выводим время работы программы
		printf("Time of execution = %f\n", Duration); 

	}
 
	//Завершаем работу MPI
	MPI_Finalize();

	return 0;
}

