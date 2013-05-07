#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <mpi.h>

#define FUN 6 //����� ������� �� ��������� �������� �� 1 �� 6

//�������� ����������
#define E 0.000001
//��������� �������� ���� ���������
#define INF 0.000000000001
//������������ ��� ���������� �����
#define ADD 0.500001
#define D 100000
//������ �����
#define ABS(a) ((a)>=0?a:-a)

using namespace std;

// �������� �� ������� �������������� �����
double Ax = -100, Bx = 100;
double Ay = -100, By = 100;

int FunNum = FUN;		// ����� �������
int ProcNum;      // ���������� ����������
int ProcRank;     // ����� �������� ���������
//������� ��� �������� ������ ����� ���������
int *pReceiveNum;  
int *pReceiveInd;
//������ ����������� ��� ���������� 
double *pResultMaxX;
double *pResultMaxY;
//���������� ��������� � �������
int sizeMax;
//������� ����������� ��� ���������
double *pResultMinX;
double *pResultMinY;
//���������� ��������� � ������
int sizeMin;

//������� ��� ����������� ���������� �� �������
double *pProcResultX;
double *pProcResultY;

//��������� ����� - ��� �������� �������� � � �
struct point{
	double x;
	double y;
};
//�������� ��������� ��������� �����
bool operator == (point a, point b){
	if(a.x == b.x && a.y == b.y) return true;
	return false;
}

// ���������� �������� ������� � ����� �
// ���������� ���� �� 6 �������
// �������� �� ��������� ����������� � define FUN
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

// ���������� �������� ������ ����������� �������
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

// ������ ��� �������� �������� � ����������� ������
vector<point> Min(0);
vector<point> Max(0);
//������� ���������� �������� � ������ �����������
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
//������� ���������� ��������� � ������ �����������
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

// ������� ���������� ���� �������� �� ��������� � �������� �����
void crit(double ax, double bx, double ay, double by, double e){
	point tmp;
	double resX;
	double resY;
	for (double y = ay; y <= by; y+=e)
		for (double x = ax; x <= bx; x+=e)
		{
			resX = dfx(x-e, y, e);
			resY = dfy(x, y-e, e);

			//���� ��� ��� �� ������� � �������
			if (e < INF) return;

			// ���� �������� ������������� �����������
			if(((-E < dfx(x, y, e)) && (dfx(x, y, e) < E)) &&
				((-E < dfy(x, y, e)) && (dfy(x, y, e) < E))){
					tmp.x = x;
					tmp.y = y;
					// ����������� ������ ���� � - �� +   - �� ����� �������
					if(((resX < 0) && (dfx(x+e, y, e) > 0)) &&
						((resY < 0) && (dfy(x, y+e, e) > 0))){
							addMin(tmp);
							return;
					}
					// ����������� ������ ���� � + �� -   - �� ����� ��������
					if(((resX > 0) && (dfx(x+e, y, e) < 0)) &&
						((resY > 0) && (dfy(x, y+e, e) < 0))){
							addMax(tmp);
							return;
					}
			}
			// ����������� ������ ���� � - �� +   - ���� ������� �� ����� ��������� � ������� �����
			if(((resX < 0) && (dfx(x+e, y, e) > 0)) &&
				((resY < 0) && (dfy(x, y+e, e) > 0))){
				crit(x-e, x+e, y-e, y+e, e/10);
			}
			// ����������� ������ ���� � + �� -   - ���� �������� �� ����� ��������� � ������� �����
			if(((resX > 0) && (dfx(x+e, y, e) < 0)) &&
				((resY > 0) && (dfy(x, y+e, e) < 0))){
				crit(x-e, x+e, y-e, y+e, e/10);
			}
		}
	return;
}
//��������� ������ �������
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
//��������� ������ ������ ������
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
	//��� ������ � ��������
	double Start, Finish, Duration; 
	//�������������� MPI
	MPI_Init(&argc, &argv);   
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);  //���������� ��������
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);	//����� �������� �������
	//getFunction(); // �� �������� �� MPI
	//getRange();	// �� �������� �� MPI
	
	//������ ������ �������� ������ ���������� ����������
	pReceiveNum = new int [ProcNum];   
	pReceiveInd = new int [ProcNum];
	//������������� ��������
	MPI_Barrier(MPI_COMM_WORLD);
	//�������� �����
	Start = MPI_Wtime(); 
	//���������� ������ ����� ���������
	if(ABS(Bx-Ax) > ABS(By-Ay)){
		crit(Ax + ProcRank*(ABS(Bx-Ax)/ProcNum), Ax + (ProcRank+1)*(ABS(Bx-Ax)/ProcNum), Ay, By,1);
	} else {
		crit(Ax, Bx, Ay + ProcRank*(ABS(By-Ay)/ProcNum), Ay + (ProcRank+1)*(ABS(By-Ay)/ProcNum),1);
	}

	//���������� ���������� ��������� ����������
	procRec[0] = Max.size();
	MPI_Allgather(procRec, 1, MPI_INT, pReceiveNum, 1, MPI_INT, MPI_COMM_WORLD); 
	//������������� ��������
	MPI_Barrier(MPI_COMM_WORLD);

	//���������� ��� ����� ������ �� ���� ��������
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
	//������������� ��������
	MPI_Barrier(MPI_COMM_WORLD);
	//�������� ��� ��������� ���������
	MPI_Allgatherv(pProcResultX, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMaxX, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	MPI_Allgatherv(pProcResultY, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMaxY, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	//������������� ��������
	MPI_Barrier(MPI_COMM_WORLD);
	//���������� ��������� ��������� �� �������
	procRec[0] = Min.size();
	MPI_Allgather(procRec, 1, MPI_INT, pReceiveNum, 1, MPI_INT, MPI_COMM_WORLD); 
	//������������� ��������
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
	//������������� ��������
	MPI_Barrier(MPI_COMM_WORLD);
	//�������� ��� ��������� ��������
	MPI_Allgatherv(pProcResultX, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMinX, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	MPI_Allgatherv(pProcResultY, pReceiveNum[ProcRank], MPI_DOUBLE, pResultMinY, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);
	//������������� ��������
	MPI_Barrier(MPI_COMM_WORLD);
	//����������� ������� �����
	Finish = MPI_Wtime(); 
	Duration = Finish-Start;
	//���� ������� ������� ������� ���������
	if (ProcRank == 0) {     
		double aX, aY, tx, ty;
		
		// ������� ����� ������� �������� ������� �� ��������� � ������� �� �����
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
	
		// ������� ����� ������� �������� ������� �� ��������� � ������� �� �����
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
		//������� ����� ������ ���������
		printf("Time of execution = %f\n", Duration); 

	}
 
	//��������� ������ MPI
	MPI_Finalize();

	return 0;
}

