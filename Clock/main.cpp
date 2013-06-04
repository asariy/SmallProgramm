#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <iomanip>

#define INF	0.0005
#define MIN(a, b)	(a) < (b) ? (a) : (b) 

using namespace std;

float ABS(float a){
	return a >= 0 ? a : -a;	
}

class clock
{

private:
	int hour;
	int minute;
	//if kvarc clock type = true
	bool type;

public:
	clock(string str, bool type){

		this->type = type;
		hour = atoi(str.substr(0,2).c_str());
		minute = atoi(str.substr(3,2).c_str());

		if(hour > 11)
			hour += -12;
	}

	int get_hour(){
		return hour;
	}

	int get_minure(){
		return minute;
	}

	float get_angle(){
		float m = minute;
		float h = hour * 5.0;

		if(!type) 
			h += minute / 12.0 ; 

		float angle = MIN(ABS(m - h) , (60.0 - ABS(h - m)));
		angle *= 6.0;

		return angle;
	}

	float get_rad(){
		return M_PI * get_angle() / 180.0;
	}

	string get_dms(){
		float angle = get_angle();
		int d = angle + INF;
		angle -= d;
		angle *= 60.0;
		int m = angle + INF;
		angle -= m;
		angle *= 60.0;
		int s = angle + INF;
		stringstream str;
		str << d << "." << setfill('0') << setw(2) << m << "'" << setw(2) << s << "''";
		return str.str();
	}

};

bool check_data(string str){

	if (str.length() >=5)
		for (int i = 0; i < 5; ++i)
		{
			if (i < 5 && i != 2)
				if (str[i] < '0' || str[i] > '9')
					return false;

			if (i == 2 && str[i] != ':')
				return false;	
		}

	int hour = atoi(str.substr(0,2).c_str());
	int minute = atoi(str.substr(3,2).c_str());



	if (minute < 0 || minute > 59)
		return false;


	if (str.length() == 5){
		if (hour < 0 || hour > 23)
			return false;
		else
			return true;
	} 

// printf("----------------------\n");

	if (str.length() == 8 && (hour < 0 || hour > 11))
		return false;

	if (str.length() == 8 && (str.substr(6,2) == "PM" || str.substr(6,2) == "AM"))
		return true;

	return false;
	
}

int main(int argc, char *argv[])
{

	if(argc == 4){
		string format = argv[2];
		string type_str = argv[3];

		if ((check_data(argv[1]))
			&& (format == "deg" || format == "rad" || format == "dms")
			&& (type_str == "meh" || type_str == "kvc")
		){

			bool type = type_str == "meh" ? false : true;
			clock *my_clock = new clock(argv[1], type);

			if(format == "deg")
				printf("%.1f\n", my_clock->get_angle());
			else if(format == "rad")
				printf("%.4f\n", my_clock->get_rad());
			else if(format == "dms")
				cout << my_clock->get_dms() << endl;
		}

	} else {
		printf("Incorrect input console parameters.\n");
	}

	return 0;
}