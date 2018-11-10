// Midterm.cpp : Defines the entry point for the console application.
//s_r,S_r,s_p FGI ýn leveli kabul ediliyor.
//Zaman girilip istenilen zamana kadar çalýsýr
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <iomanip> //setw için
#include <ctime>   //srand için
#include <random>  //expo için
#include <fstream>
#include <algorithm>

using namespace std;
double Clock;
int iteration;   //iteration of last event
int IMEVT;   /*sýradaki eventin dizideki sýrasý*/
//---------------
float cost_P;    //procurement cost
int Quantity_P; /*procurement quantity at every procuremnt*/
int Current_P;
double LeadTime_Procure;
bool DidProcure;
//---------------
float Cost_CI;
int s_disposal;  //CI level disposal occurs. (sd>= 0)
int Quantity_CI; /*CI level*/
//---------------
bool FactoryBUSY;
float Cost_Reman;
int n;//arrayler kaç elemanlý olacak
int i; //R_leadtime[] ýn iterasyonu
double TotalBUSYtime; //leadtimelarýn toplamý
int Q_OnOrder;
//---------------
float Cost_FGI;
int Quantity_FGI;
//---------------
int s_p; // FGI level procurement is made(sp<= sr)
int s_r; //FGI position remanufacturing begins.
int S_r; //FGI position which we want to try to fix FGI level(Sr>=sr)

int TotalAcceptance;// ne kadarý kabul edilmiþ ona bakmak için
int Q_DemandNotMeet;
int TotalArrival; // ne kadarý kabul edilmiþ ona bakmak için
int DemandMetByOnHand;
double *R_leadtime;
double *ReturnInterArrivals;
double *DemandInterArrivals;
double **SEL;  //sorted event list
int qqq;//SEL e eklerken kacýncý iterasyon olDUÐUNU TUTUYOR
double Return_Lamda;
double Demand_Lamda;
double LT_Lamda;
int Q_madeProc;//kaç defa procurement yapýldý
int Q_Disposal;
int Q_Return;
int Q_Demand;
int Q_Remanufactured;
double CostPerTime;
double cost_disposal;
double cost_demandNotMeet;
double Time_Avg_FGI;
double Time_Avg_CI;
double Time_Avg_NumOfRemanufactured;
double Time_Avg_NUmOfProcurement;
double Time_Avg_disposal;
double Time_Avg_demandNotMeet;
double Time_FGI;
double Time_CI;
double Time_NumOfRemanufactured;
double Time_NUmOfProcurement;
double Time_disposal;
double Time_demandNotMeet;
int ProcurementMethod;
double FinishTime;
ofstream dosya;


void Create_Events()
{
	R_leadtime = new double[n]();
	ReturnInterArrivals = new double[n]();
	DemandInterArrivals = new double[n]();
	SEL = new(double *[4 * n]);
	for (int i = 0; i < 4 * n; i++)
		SEL[i] = new double[2](); //1 incisi time 2inci eventtype 1-REturn 2-demand 3-departure 4-Procurementfinished
}
//void Create_SEL(double lamda)
//{
//	std::random_device rd; // uniformly-distributed integer random number generator
//	std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation
//							//std::default_random_engine generator;
//	std::uniform_real_distribution<double> ZeroOne(0.0, 1.0);
//	std::exponential_distribution<double> dist(lamda);
//	for (int i = 0; i < n; i++)
//	{
//		SEL[i][0] = dist.operator()(rng);
//		//ptr[i] = dist(generator);//exponential bi random üret....
//	}
//	double x;
//	for (int i = 0; i < n; i++)// return mü demand mi burada belirleniyor
//	{
//		x = ZeroOne(rng);
//		if (x <= (Return_Lamda / lamda))
//			SEL[i][1] = 1;
//		else
//			SEL[i][1] = 2;
//	}
//	dosya.open("RandomSayýlar.txt", ios::app);
//	for (int a = 0; a < qqq; a++)
//	{
//		dosya << SEL[a][0] << "-" << SEL[a][1] << "  ";
//	}
//	dosya.close();
//	for (int i = 0; i < n; i++)
//	{
//		SEL[i + 1][0] += SEL[i][0];
//		//ptr[i] = dist(generator);//exponential bi random üret....
//	}
//
//}
void Create_Exp_Random(double *ptr, double lamda)
{
	std::default_random_engine generator;
	std::exponential_distribution<double> dist(lamda);
	for (int i = 0; i < n; i++)
	{
		ptr[i] = dist(generator);//exponential bi random üret....
	}
}

void SORT_EVENTS() //sadece ReturnInterArrivals ve DemandInterArrivals sýralanacak. Diðerleri insert edilecek
{

	double *temp;

	for (int a = 1; a < n; a++)
	{
		ReturnInterArrivals[a] += ReturnInterArrivals[a - 1];
	}
	for (int a = 1; a <n; a++)
	{
		DemandInterArrivals[a] += DemandInterArrivals[a - 1];
	}
	for (int a = 0; a <n; a++)
	{
		SEL[a][0] = ReturnInterArrivals[a];
		SEL[a][1] = 1;
	}
	for (int a = n; a <2 * n; a++)
	{
		SEL[a][0] = DemandInterArrivals[a - n];
		SEL[a][1] = 2;
	}
	for (int a = 0; a <2 * n - 1; a++)
	{
		for (int b = 0; b <2 * n - 1; b++)
		{
			if (SEL[b][0] > SEL[b + 1][0])
			{
				temp = SEL[b];
				SEL[b] = SEL[b + 1];
				SEL[b + 1] = temp;
			}
		}

	}
}
void SORTSEL()
{
	double *temp;
	for (int a = iteration; a < qqq - 1; a++)
	{
		for (int b = iteration; b < qqq - 1; b++)
		{
			if (SEL[b][0] > SEL[b + 1][0])
			{
				temp = SEL[b];
				SEL[b] = SEL[b + 1];
				SEL[b + 1] = temp;
			}
		}
	}
}
void DeleteArrays()
{
	delete[] R_leadtime;
	delete[] ReturnInterArrivals;
	delete[] DemandInterArrivals;
	for (int a = 0; a <4 * n; a++)
	{
		delete[] SEL[a];
	}
	delete[] SEL;
}

void Initialize()
{
	Clock = 0;
	iteration = -1;
	/*Return_Lamda = 0.25;
	Demand_Lamda = 0.75;
	LT_Lamda = 0.025;*/
	Q_Disposal = 0;
	cost_P = 5;    //***Ýlerde belirlenekler
	/*Quantity_P = 15; *///***
	Current_P = 0;
	//LeadTime_Procure = Quantity_P * 4;// deterministic
	DidProcure = 0;
	Cost_CI = 2; //***
	//s_disposal = 50;//***
	//Quantity_CI = 25;//Baþlangýçta yarý seviyede core var
	FactoryBUSY = 0;
	Cost_Reman = 1;//***
	Create_Events();
	//Create_SEL(Demand_Lamda + Return_Lamda);
	Create_Exp_Random(R_leadtime, LT_Lamda);
	Create_Exp_Random(ReturnInterArrivals, Return_Lamda);
	Create_Exp_Random(DemandInterArrivals, Demand_Lamda);
	SORT_EVENTS();
	//SORTSEL();
	i = 0;
	TotalBUSYtime = 0;
	Q_OnOrder = 0;
	Cost_FGI = 3;//***
	//Quantity_FGI = 5;//**
	//s_p = 15;//smaller than s_r 
	//s_r = 36; //***
	//S_r = 88; //***

	TotalAcceptance = 0;
	Q_DemandNotMeet = 0;
	TotalArrival = 0;
	DemandMetByOnHand = 0;
	qqq = 2*n;
	Q_madeProc = 0;
	Q_Disposal = 0;
	Q_Return = 0;
	Q_Demand = 0;
	Q_Remanufactured = 0;
}
void TIMEADV()
{
	iteration += 1;
	Clock = SEL[iteration][0];
}
void Report()//satýrý yazdýrracak
{
	
	dosya.open("iterations.xls", ios::app); //deneme.xls isimli bir dosya açtýk.
	dosya << Clock << '\t' << TotalAcceptance << '\t' << Q_Disposal << '\t' << Quantity_CI << '\t' << FactoryBUSY << '\t' << Q_OnOrder << '\t' << Quantity_FGI << '\t' << Q_madeProc << '\t' << DemandMetByOnHand << '\t' << Q_DemandNotMeet << endl; //Dikkat edin burada oluþturduðumuz nesneyi cout gibi kullanýyoruz.
	dosya.close(); //dosyayý kapattýk.
}

void Insert_Events(double LeadTime, double d)//d 3 ya da 4 olacak departure or procurement
{

	double X = Clock + LeadTime;
	SEL[qqq][0] = X;
	SEL[qqq][1] = d;
	qqq += 1;
	SORTSEL();
}

void ProcurementFinished()
{
	Current_P -= Quantity_P;
	Quantity_FGI += Quantity_P;
	DidProcure = 0;
	if (Quantity_FGI + Q_OnOrder > S_r)
	{
		Q_OnOrder -= Quantity_FGI + Q_OnOrder- S_r;
		if (Q_OnOrder< 0)
		{
			Q_OnOrder = 0;
			if(FactoryBUSY = 1)
			{
				Q_OnOrder = 1;
                cout << "()";
			}	
		}
    }
}

void StartProcurement()//LT sabit old. için ne zaman geleceði belli. Eventlerin arasýnda sýrasýna yerlestirilsin
{
	//Quantity_P = S_r - InvPos;
	Q_madeProc += 1;
	DidProcure = 1;
	Insert_Events(LeadTime_Procure, 4);
	Current_P += Quantity_P;
}
bool Remanufacture()
{

	if (Quantity_FGI<S_r && Quantity_CI >= 1)
	{
		if (FactoryBUSY == 0)
		{
			Quantity_CI -= 1;
			FactoryBUSY = 1;
			Insert_Events(R_leadtime[i], 3);
			TotalBUSYtime += R_leadtime[i];
			i += 1;
			return true;
		}
		return false;
	}
	else if(Quantity_FGI>=S_r) //FGI Sr dan büyükse üretimi durdur. 
	{
		if (FactoryBUSY == 1)//Üretim varken durdurulunca core u CI ya ver.
		{
			cout << " ** ";
		}
      return false;
	}
		
}


void ReturnArrival()
{
	//s_d ye bak kabul mü dispose mu belirle.aacept ise Quantity_CI+1
	TotalArrival += 1;
	if (Quantity_CI >= s_disposal)
	{
		Q_Disposal += 1;
	}
	else if (Quantity_CI < s_disposal)
	{
		TotalAcceptance += 1;
		Quantity_CI += 1;
		if (FactoryBUSY == 0 && Q_OnOrder > 0) // CI da yoktu. Kuruk vardý .Sonra return geldi. baslat 
		{
			Remanufacture();
		}
	}
}
void Demand()
{
	if (Quantity_FGI > 0)
	{
		Quantity_FGI -= 1;
		DemandMetByOnHand += 1;

		if (Quantity_FGI <= s_r && (Quantity_FGI + Q_OnOrder + Current_P) < S_r)
		{
			Q_OnOrder += (S_r - (Q_OnOrder + Quantity_FGI + Current_P));
			Remanufacture();
		}
		else if (Quantity_FGI <= s_r && (Quantity_FGI + Q_OnOrder + Current_P) > S_r)
		{
			Q_OnOrder -= Quantity_FGI + Q_OnOrder + Current_P - S_r;
			if (Q_OnOrder < 0)
			{
				Q_OnOrder = 0;
				if (FactoryBUSY = 1)
				{
					Q_OnOrder = 1;
					cout << "()";
				}
			}
		}
	}
	else  //FGI=0        
	{
		Q_DemandNotMeet += 1;
	}

	if (DidProcure == 0 && Quantity_FGI <=s_p)
	{
			Quantity_P = S_r - s_p-FactoryBUSY; //FactoryBUsy demessem Sr ý geçme ihtimali var
			LeadTime_Procure = Quantity_P *((1/LT_Lamda)/2);
			cout << Quantity_P << " " << LeadTime_Procure;
			StartProcurement();
			if (FactoryBUSY == 1)
				Q_OnOrder = 1;
			else
				Q_OnOrder = 0;
	}
}

void Departure()// lead time bitince
{
		Quantity_FGI += 1;
		Q_OnOrder -= 1;
		FactoryBUSY = 0;
		if (Q_OnOrder > 0)
		{
			Remanufacture();
		}
}


int main()
{

	Return_Lamda = 5;
	Demand_Lamda = 8;
	LT_Lamda = 10;
	Quantity_CI = 5;
	Quantity_FGI = 5;
	s_p = 2;//smaller than s_r 
	s_r = 4;
	S_r = 7;
	s_disposal = 10;

	cin >> FinishTime;
	double lamda = std::max(LT_Lamda, Demand_Lamda)+2;//should bigger than demand lamda
	cout << lamda << endl;
	std::default_random_engine generator2;
	std::exponential_distribution<double> dist2(lamda);
	double temp = 0;
	while (temp < FinishTime)
	{
		temp += dist2(generator2);
		n++;
	}
	n += 100;
	cout << n << endl;

	Initialize();
	dosya.open("RandomSayýlar.txt", ios::app);
	dosya << endl << "---------------------------------------------------------------------------------------------------------------------------------";
	dosya << endl << "FOR FinishTime: " << FinishTime << ", retun_lamda: " << Return_Lamda << ", demand_lamda: " << Demand_Lamda << ", LeadTime_lamda: " << LT_Lamda << endl << endl;
	dosya << endl << "SEL:";

	for (int a = 0; a < qqq; a++)
	{
		dosya << SEL[a][0] << "-" << SEL[a][1] << "  ";
	}
	dosya << endl << "1-ReturnArrival 2-Demand 3-Departure 4-ProcurementFinished " << endl << "Lead Times: ";
	for (int t = 0; t < n; t++)
		dosya << R_leadtime[t] << " ";
	dosya.close();
	dosya.open("iterations.xls", ios::app);
	dosya << endl << "---------------------------------------------------------------------------------------------------------------------------------";
	dosya << endl << endl << "FOR FinishTime:"<< '\t' << FinishTime << '\t' << "retun_lamda: " << '\t' << Return_Lamda << '\t' << "demand_lamda: " << '\t' << Demand_Lamda << '\t' << " LeadTime_lamda: " << '\t' << LT_Lamda << endl;
	dosya << endl << "Event" << '\t' << "Clock" << '\t' << "TotalAccept " << '\t' << " TotalDisp" <<  '\t' << "Qnt_CI" << '\t' << "FactoryBUSY " << '\t' << " Q_OnOrder" << '\t' << "Qnt_FGI " << '\t' << " Q_madeProc " << '\t'<<  "Q_DemandMet  " << '\t' << "Q_DemandNotMeet" << endl;
	dosya << 0 << '\t' << Clock << '\t' << TotalAcceptance << '\t' << Q_Disposal << '\t' << Quantity_CI << '\t' << FactoryBUSY << '\t' << Q_OnOrder << '\t' << Quantity_FGI << '\t' << Q_madeProc << '\t' << DemandMetByOnHand << '\t' << Q_DemandNotMeet << endl;
	dosya.close();
	while (Clock < FinishTime)
	{
		cout << ".";
		TIMEADV();

		switch (int(SEL[iteration][1]))
		{
		case 1:
			ReturnArrival();
			Q_Return += 1;
			break;
		case 2:
			Demand();
			Q_Demand += 1;
			break;
		case 3:
			Departure();
			Q_Remanufactured += 1;
			break;
		case 4:
			ProcurementFinished();
			//Q_madeProc += 1;
			break;
		}
		dosya.open("iterations.xls", ios::app);
		dosya << SEL[iteration][1] << '\t';
		dosya.close();
		Report();
	}
	dosya.open("iterations.xls", ios::app);
	dosya << endl << "Q_Return= " << '\t' << Q_Return << '\t' << "Q_Demand= " << '\t' << Q_Demand << '\t' << "Q_Remanufactured= " << '\t' << Q_Remanufactured << '\t' << "Q_madeProc= " << '\t' << Q_madeProc;
	dosya << '\t' << "Q_Disposal= " << '\t' << Q_Disposal << '\t' << "Q_DemandNotMeet= " << '\t' << Q_DemandNotMeet;
	dosya.close();
	DeleteArrays();
    return 0;
}

