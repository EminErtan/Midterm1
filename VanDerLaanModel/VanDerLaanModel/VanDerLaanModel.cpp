// Midterm.cpp : Defines the entry point for the console application.
//s_r,S_r,s_p FGI ýn leveli kabul ediliyor.
//Zaman girilip istenilen zamana kadar çalýsýr
//Factory 3 e ayrýldý aralarýna buffer eklendi

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
int TotalProcurement;
int Current_P;
double LeadTime_Procure;
bool DidProcure;
//---------------
float Cost_CI;
int s_disposal;  //CI level disposal occurs. (sd>= 0)
int Quantity_CI; /*CI level*/
				 //---------------

float Cost_Reman;
int n;//arrayler kaç elemanlý olacak

//double TotalBUSYtime; //leadtimelarýn toplamý

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
//---FOR DISASSEMBLY-----------------
bool FactoryBusy_Disassembly;
double *Disassembling_Time;
double Disassembly_mean;
double Disassembly_stddev;
int i_disassembly;// Disassembling_Time ýn kacýncý iterasyon olduðunu tutar
int Q_Order_Disassembly;
int Q_Disassembled;
int Q_Disassembled_After_WarmUp;
//---FOR REFURBISHING-----------
bool FactoryBusy_Refurbishing;
double *Refurbishing_Time;
double Refurbishing_mean;
double Refurbishing_stddev;
int i_Refurbishing;
int Q_Order_Refurbishing;
int Q_Refurbished_After_WarmUp;
int Q_Refurbished;
//---FOR REASSEMBLY-----------
bool FactoryBusy_Reassembly;
double *Reassembly_Time;
double Reassembly_mean;
double Reassembly_stddev;
int i_Reassembly;
int Q_Order_Reassembly;
int Q_Reassembled_After_WarmUp;
int Q_Reassembled;
//----------BUFFERS---------
int BufferSize_1;
int BufferSize_2;
int Current_B1;
int Current_B2;
int s_B1_Disassembly;
int s_B2_Refurbishing;
//-----MachineBreakdown--------
double Disassembly_MachineBreakdownRate;
double Refurbishing_MachineBreakdownRate;
double Reassembly_MachineBreakdownRate;
double RepairTime; //Her hata için yeni atanýyor
//------Routing------------------
double RoutingRate;
//-------------------------------------
double *ReturnInterArrivals;
double *DemandInterArrivals;
double **SEL;  //sorted event list
int qqq;//SEL e eklerken kacýncý iterasyon olDUÐUNU TUTUYOR
double Return_Lamda;
double Demand_Lamda;
//double LT_mean;
//double LT_s;
//double LT_lamda;
int Q_madeProc;//kaç defa procurement yapýldý
int Q_Disposal;
int Q_Return;
int TotalProcurement_After_WarmUp;
int Q_Demand;
int Q_Remanufactured;
int Q_R_After_WarmUp;
int Q_Disposal_After_WarmUp;
int Q_DemandNotMeet_After_WarmUp;
double CostPerTime;
double cost_disposal;
double cost_demandNotMeet;
double Time_Avg_FGI; 
double Time_Avg_CI;
double Time_Avg_B1;
double Time_Avg_B2;
double Time_Avg_NumOfRemanufactured;
double Time_Avg_NumOfProcurement;
double Time_Avg_disposal;
double Time_Avg_demandNotMeet;
double Time_Avg_DissambledProduct;
double Time_Avg_RefurnishedProduct;
double Time_Avg_ReassembledProduct;
double Time_Avg_DisassemblyFactoryBusy;
double Time_Avg_RefurbishingFactoryBusy;
double Time_Avg_ReassemblyFactoryBusy;
double FinishTime;
double k_Procurement;
int aaa;
double* warmup;
double Warm_upPeriod;
ofstream dosya;

std::default_random_engine generator123;
std::uniform_real_distribution<double> distribution123(0.0, 1.0);   //for machinebreakdown

std::random_device rd;   //for creating exponential distribution
std::mt19937 rng(rd());

std::random_device rdm; //for creating lognormal distribution
std::mt19937 gen1(rdm());

std::default_random_engine generator1;  
std::uniform_real_distribution<double> distribution1(0.0, 1.0);

std::default_random_engine generator2;
std::uniform_real_distribution<double> distribution2(0.0, 1.0);

std::default_random_engine generator3;
std::uniform_real_distribution<double> distribution3(0.0, 1.0);
void Create_Events()
{
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
void Create_Exp_Random(double *ptr, double lamda,int x)
{
	/*std::random_device rd;
	std::mt19937 rng(rd());*/
	std::exponential_distribution<double> dist(lamda);
	for (int i = 0; i < x; i++)
	{
		ptr[i] = dist.operator()(rng);//exponential bi random üret....

	}
}
void Create_Lognormal_Random(double *ptr, double mean,double standardDeviation,int aaa)
{
	double Log_mean = 2 * log(mean) - 0.5*log(pow(standardDeviation, 2) + pow(mean, 2));
	double log_standardDeviation = sqrt(-2 * log(mean) + log(pow(standardDeviation, 2) + pow(mean, 2)));
  //std::random_device rdm;
  //std::mt19937 gen1(rdm());
  std::lognormal_distribution<> d(Log_mean,log_standardDeviation);
  for (int i = 0; i <aaa; i++)
  {
	  ptr[i] = d.operator()(gen1);//lognormal bi random üret....
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
	for (int a = 0; a <2 * n-1; a++)
	{
		for (int b = 0; b <2 * n-1; b++)
		{
			if (SEL[b][0] > SEL[b + 1][0])
			{
				temp = SEL[b];
				SEL[b] = SEL[b + 1];
				SEL[b + 1] = temp;
			}
		}

	}
	for (int a = 0; a < 2 * n; a++)
	{
		if (SEL[a][0] >= FinishTime)
		{
			aaa = a + 1;
			cout <<"aaa="<<aaa<<endl;
			if (2 * aaa > n)
				cout << "......SEL array inin kapasitesi yetmedi....";
			break;
		}
	}
	for (int a = 0; a < 2 * n; a++)
	{
		if (SEL[a][0] >= Warm_upPeriod)
		{
			warmup = &SEL[a][0];
			break;
		}
	}
}
void Create_RemanufacturingEvents()
{
	Refurbishing_Time = new double[aaa]();
	Disassembling_Time = new double[aaa]();
	Reassembly_Time = new double[aaa]();
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
	delete[] Refurbishing_Time;
	delete[] Disassembling_Time;
	delete[] Reassembly_Time;
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
	Q_Disposal = 0;
	Current_P = 0;
	DidProcure = 0;
	
	Create_Events();
	Create_Exp_Random(ReturnInterArrivals, Return_Lamda,n);
	Create_Exp_Random(DemandInterArrivals, Demand_Lamda,n);
	SORT_EVENTS();
	Create_RemanufacturingEvents();
	
	Create_Lognormal_Random(Disassembling_Time, Disassembly_mean, Disassembly_stddev,aaa);
	Create_Lognormal_Random(Refurbishing_Time, Refurbishing_mean, Refurbishing_stddev, aaa);
	Create_Lognormal_Random(Reassembly_Time, Reassembly_mean, Reassembly_stddev, aaa);
	//---FOR DISASSEMLBY-----------
	FactoryBusy_Disassembly = 0;
	i_disassembly=0;
    Q_Order_Disassembly=0;
	Q_Disassembled=0;
	Q_Disassembled_After_WarmUp=0;
	//---FOR REFURBISHING-----------
	FactoryBusy_Refurbishing=0;
	i_Refurbishing=0;
	Q_Order_Refurbishing=0;
	Q_Refurbished_After_WarmUp=0;
	Q_Refurbished=0;
	//---FOR REASSEMBLY-----------
	FactoryBusy_Reassembly=0;
	i_Reassembly=0;
	Q_Order_Reassembly=0;
	Q_Reassembled_After_WarmUp = 0;
	Q_Reassembled = 0;
	TotalProcurement = 0;
	//TotalBUSYtime = 0;

	TotalAcceptance = 0;
	Q_DemandNotMeet = 0;
	TotalArrival = 0;
	DemandMetByOnHand = 0;
	qqq = aaa;
	Q_madeProc = 0;
	Q_Disposal = 0;
	Q_Return = 0;
	Q_Demand = 0;
	Q_Remanufactured = 0;
	Q_R_After_WarmUp = 0;
	Q_DemandNotMeet_After_WarmUp = 0;
	Q_Disposal_After_WarmUp = 0;
	TotalProcurement_After_WarmUp = 0;
	if(Warm_upPeriod==0)
	{ 
		Time_Avg_CI = Quantity_CI*SEL[0][0]; // clock 0 ile ilk event zamaný arasýný hesaplýyor
	    Time_Avg_FGI = Quantity_FGI*SEL[0][0];
		Time_Avg_B1 = Current_B1*SEL[0][0];
		Time_Avg_B2 = Current_B2*SEL[0][0];
		Time_Avg_DisassemblyFactoryBusy =FactoryBusy_Disassembly*SEL[0][0];
		Time_Avg_RefurbishingFactoryBusy = FactoryBusy_Refurbishing*SEL[0][0];
		Time_Avg_ReassemblyFactoryBusy=FactoryBusy_Reassembly*SEL[0][0];
	}
	else
	{
		Time_Avg_CI = 0;
		Time_Avg_FGI = 0;
		Time_Avg_B1 = 0;
		Time_Avg_B2 = 0;
		Time_Avg_DisassemblyFactoryBusy = 0;
		Time_Avg_RefurbishingFactoryBusy = 0;
		Time_Avg_ReassemblyFactoryBusy = 0;
	}
	
}
void TIMEADV()
{
	iteration += 1;
	Clock = SEL[iteration][0];
}
void Report()//satýrý yazdýrracak
{

	dosya.open("iterations.xls", ios::app); //deneme.xls isimli bir dosya açtýk.
	dosya << Clock << '\t' << TotalAcceptance << '\t' << Q_Disposal << '\t' << Quantity_CI << '\t' << FactoryBusy_Disassembly << '\t' << Q_Order_Disassembly << '\t' << Current_B1;
	dosya << '\t' << FactoryBusy_Refurbishing << '\t' << Q_Order_Refurbishing << '\t' << Current_B2 << '\t' << FactoryBusy_Reassembly << '\t' << Q_Order_Reassembly;
	dosya << '\t' << Quantity_FGI << '\t' << Q_madeProc << '\t' << DemandMetByOnHand << '\t' << Q_DemandNotMeet << endl;
	dosya.close(); //dosyayý kapattýk.
}
void MachineBreakdown(int MachineType)
{
	/*std::default_random_engine generator123;
	std::uniform_real_distribution<double> distribution123(0.0, 1.0);*/
	//std::exponential_distribution<double> dist(2);
	RepairTime = distribution123(generator123);
	for (int i =iteration+1; i <= qqq; i++)//en yakýn remanufacturing time ý bul
	{
 	 if (SEL[i][1] == MachineType)//Arýza yapan makinenin en yakýn bitirmesini bulduk.
	   {         
		 dosya.open("MachineBreakdowns.xls", ios::app);
		 dosya << Clock << '\t' << MachineType << '\t' << SEL[i][0] << '\t' << RepairTime ;
		 dosya.close();
		 SEL[i][0] +=RepairTime ;// Tamir süresi eklenecek for lognormal distribution
		 dosya.open("MachineBreakdowns.xls", ios::app);
		 dosya << '\t' << SEL[i][0] << endl;
		 dosya.close();
		 SORTSEL();
		 cout << MachineType << "-" << RepairTime;
		break;
	   }
	}

}
void Insert_Events(double LeadTime, int d);
void ROUTING(double *ptr,int &i, int EventType)
{

		Insert_Events(ptr[i], EventType);
		i++;
	
	dosya.open("Routings.xls", ios::app);
	dosya << Clock << '\t' << EventType<< '\t' << SEL[iteration][0] << '\t' << ptr[i] <<'\t' << SEL[iteration][0] +ptr[i] << endl;
	dosya.close();
}


void Insert_Events(double LeadTime, int d)//d 3 ya da 4 olacak departure or procurement
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
	Q_Order_Reassembly += (S_r - Quantity_FGI - Q_Order_Reassembly);
	if (Quantity_FGI >= S_r) //Fabrikayý durdur
	{
		Q_Order_Reassembly = 0;
		if (FactoryBusy_Reassembly == 1)
		{
			Q_Order_Reassembly = 1;
		}
		//Burada fabrikada üretilen sey durdurulup geri depoya yollanýr ve lead time SEL den cýkarýlýr.
		//if (FactoryBusy_Reassembly == 1)
		//{
		//	FactoryBusy_Reassembly = 0;
		//	for (int i = 0; i <= qqq; i++)//en yakýn remanufacturing time ý bul
		//	{
		//		if (SEL[iteration + i][1] == 3)
		//		{//SEL time ý finishtime+100 yap sýrala qqq yu bir düsür
		//			SEL[iteration + i][0] = FinishTime + 100;
		//			SORTSEL();
		//			qqq--;
		//			break;
		//		}
		//	}
		//	i_Reassembly--;//LT ýn iterasyonunu bir düsür
		//	Current_B2++;//B2 ý bir artýr eðer factory Busy ise
		//}
	}
}

void StartProcurement()//LT sabit old. için ne zaman geleceði belli. Eventlerin arasýnda sýrasýna yerlestirilsin
{
	//Quantity_P = S_r - InvPos;
	Q_madeProc += 1;
	DidProcure = 1;
	Insert_Events(LeadTime_Procure, 4);
	Current_P += Quantity_P;
	TotalProcurement += Quantity_P;
	if (Clock >= Warm_upPeriod)
		TotalProcurement_After_WarmUp++;
}
bool Disassemble()
{
	if (Quantity_CI >= 1 && FactoryBusy_Disassembly == 0)
	{
		Quantity_CI--;
		FactoryBusy_Disassembly = 1;
		Insert_Events(Disassembling_Time[i_disassembly], 31);
		//std::default_random_engine generator1;
		//std::uniform_real_distribution<double> distribution1(0.0, 1.0);
		double x = distribution1(generator1);
		if(x<=Disassembly_MachineBreakdownRate)
		{
			MachineBreakdown(31);
			//cout << "(31)ARIZA";
		}
		i_disassembly++;
		return true;
	}
	else
		return false;

}
bool Refurbish();
void DisassemblingFinished()
{
	double y = distribution1(generator1);
	if (y <= RoutingRate)
	{
		ROUTING(Disassembling_Time,i_disassembly,31);
		return;
    }
	Current_B1++;
	Q_Order_Disassembly--;
	FactoryBusy_Disassembly = 0;
	if (Q_Order_Disassembly > 0)
	{
		Disassemble();
	}
	if (FactoryBusy_Refurbishing == 0 && Q_Order_Refurbishing > 0) // B1 da yoktu. Kuruk vardý .Sonra geldi. baslat 
	{
		Refurbish(); 
	}
}
bool Refurbish()
{
	if (Current_B1 >= 1 && FactoryBusy_Refurbishing == 0)
	{
		Current_B1--;
		if (Current_B1 <= s_B1_Disassembly)
		{
			Q_Order_Disassembly += BufferSize_1 - Current_B1 - Q_Order_Disassembly;
			Disassemble();
		}
		FactoryBusy_Refurbishing = 1;
		Insert_Events(Refurbishing_Time[i_Refurbishing], 32);
		/*std::default_random_engine generator2;
		std::uniform_real_distribution<double> distribution2(0.0, 1.0);*/
		double x = distribution2(generator2);
		if (x <= Refurbishing_MachineBreakdownRate)
		{
			MachineBreakdown(32);
			//cout << "(32)ARIZA";
		}
		i_Refurbishing++;
		return true;
	}
	else
		return false;

}
bool Reassemble();
void RefurbishingFinished()
{
	Current_B2 ++;
	Q_Order_Refurbishing--;
	FactoryBusy_Refurbishing = 0;
	if (Q_Order_Refurbishing > 0)
	{
		Refurbish();
	}
	if (FactoryBusy_Reassembly == 0 && Q_Order_Reassembly > 0) // B2 da yoktu. Kuruk vardý .Sonra geldi. baslat 
	{
		Reassemble(); 
	}
}

bool Reassemble()
{
	if (Current_B2 >= 1 && FactoryBusy_Reassembly == 0)
	{
		
			Current_B2 --;
			if (Current_B2 <= s_B2_Refurbishing)
			{
				Q_Order_Refurbishing += BufferSize_2 - Current_B2 - Q_Order_Refurbishing;
				Refurbish();
			}
			FactoryBusy_Reassembly = 1;
			Insert_Events(Reassembly_Time[i_Reassembly], 33);
			/*std::default_random_engine generator3;
			std::uniform_real_distribution<double> distribution3(0.0, 1.0);*/
			double x = distribution3(generator3);
			if (x <= Reassembly_MachineBreakdownRate)
			{
				MachineBreakdown(33);
				//cout << "(33)ARIZA";
			}
			i_Reassembly ++;
			return true;
	}
	else
		return false;
	
}

void ReturnArrival()
{
	//s_d ye bak kabul mü dispose mu belirle.aacept ise Quantity_CI+1
	TotalArrival += 1;
	if (Quantity_CI >= s_disposal)
	{
		Q_Disposal += 1;
		if (Clock >= Warm_upPeriod)
			Q_Disposal_After_WarmUp++;
	}
	else if (Quantity_CI < s_disposal)
	{
		TotalAcceptance += 1;
		Quantity_CI += 1;
		if (FactoryBusy_Disassembly == 0 && Q_Order_Disassembly > 0) // CI da yoktu. Kuruk vardý .Sonra return geldi. baslat 
		{
			Disassemble(); //Dissaembly olacak
		}
	}
}
void Demand()
{
	if (Quantity_FGI > 0)
	{
		Quantity_FGI -= 1;
		DemandMetByOnHand += 1;

		if (Quantity_FGI <= s_r)
		{
			Q_Order_Reassembly += (S_r - Quantity_FGI- Q_Order_Reassembly);
			Reassemble();
		}

	}
	else  //FGI=0        
	{
		Q_DemandNotMeet += 1;
		if (Clock >= Warm_upPeriod)
			Q_DemandNotMeet_After_WarmUp++;
	}

	if (Quantity_FGI <= s_p)
	{
		//Quantity_P = S_r - s_p - FactoryBUSY; //FactoryBusy demessem Sr ý geçme ihtimali var
		LeadTime_Procure = Quantity_P *k_Procurement;//*((LT_mean))
		//cout << Quantity_P;
		StartProcurement();
		/*if (FactoryBUSY == 1)
			Q_OnOrder = 1;
		else
			Q_OnOrder = 0;*/
	}
}

void ReassemblingFinished()// lead time bitince
{
	Quantity_FGI += 1;
	Q_Order_Reassembly --;
	FactoryBusy_Reassembly = 0;
	if (Q_Order_Reassembly > 0)
	{
		Reassemble();
	}
}
void QueuLengthWÝthoutDividing()// Kuyruk uzunluklarýný hesaplamak için zaman*adet (bölü finish time yok)
{
	if (SEL[iteration][0]<=FinishTime && SEL[iteration][0] >= *warmup)
	{
		Time_Avg_CI += Quantity_CI*(SEL[iteration + 1][0] - SEL[iteration][0]) ;
		Time_Avg_FGI += Quantity_FGI*(SEL[iteration + 1][0] - SEL[iteration][0]);
		Time_Avg_B1+=Current_B1*(SEL[iteration + 1][0] - SEL[iteration][0]);
		Time_Avg_B2 += Current_B2*(SEL[iteration + 1][0] - SEL[iteration][0]);
		Time_Avg_DisassemblyFactoryBusy+= FactoryBusy_Disassembly*(SEL[iteration + 1][0] - SEL[iteration][0]);
		Time_Avg_RefurbishingFactoryBusy+= FactoryBusy_Refurbishing*(SEL[iteration + 1][0] - SEL[iteration][0]);
		Time_Avg_ReassemblyFactoryBusy += FactoryBusy_Reassembly*(SEL[iteration + 1][0] - SEL[iteration][0]);
	}
}
void CostCalculation() // bölü finish time burada yapýlýyor    CI ve FGI haric yanlýs warmuptan sonrakileri almak lazým
{
	Time_Avg_CI = Time_Avg_CI / (SEL[iteration][0] -*warmup);
	Time_Avg_FGI= Time_Avg_FGI / (SEL[iteration][0] - *warmup);
	Time_Avg_B1= Time_Avg_B1 / (SEL[iteration][0] - *warmup);
	Time_Avg_B2 = Time_Avg_B2 / (SEL[iteration][0] - *warmup);
	Time_Avg_NumOfRemanufactured=Q_R_After_WarmUp/ (SEL[iteration][0] - *warmup);
	Time_Avg_NumOfProcurement= TotalProcurement_After_WarmUp / (SEL[iteration][0] - *warmup);
    Time_Avg_disposal= Q_Disposal_After_WarmUp / (SEL[iteration][0] - *warmup);
	Time_Avg_demandNotMeet= Q_DemandNotMeet_After_WarmUp / (SEL[iteration][0] - *warmup);
	Time_Avg_DissambledProduct=Q_Disassembled_After_WarmUp / (SEL[iteration][0] - *warmup);
	Time_Avg_RefurnishedProduct = Q_Refurbished_After_WarmUp / (SEL[iteration][0] - *warmup);
	Time_Avg_ReassembledProduct = Q_Reassembled_After_WarmUp / (SEL[iteration][0] - *warmup);
	Time_Avg_DisassemblyFactoryBusy= Time_Avg_DisassemblyFactoryBusy/ (SEL[iteration][0] - *warmup);
	Time_Avg_RefurbishingFactoryBusy= Time_Avg_RefurbishingFactoryBusy / (SEL[iteration][0] - *warmup);
	Time_Avg_ReassemblyFactoryBusy = Time_Avg_ReassemblyFactoryBusy / (SEL[iteration][0] - *warmup);
	CostPerTime = Time_Avg_CI*Cost_CI + Time_Avg_FGI*Cost_FGI + Time_Avg_NumOfRemanufactured*Cost_Reman + Time_Avg_NumOfProcurement*cost_P + Time_Avg_disposal*cost_disposal + Time_Avg_demandNotMeet*cost_demandNotMeet;
}
int main()
{
	//-------------LAMDA-------------------
	Return_Lamda = 0.60;
	Demand_Lamda =1;

	Disassembly_mean=1;
	Refurbishing_mean =0.8;
	Reassembly_mean =0.66;
	Disassembly_stddev = 0.5;
	Refurbishing_stddev = 0.5;
	Reassembly_stddev = 0.5;

	k_Procurement = 0.5;// =LT_Procurement/Quantity_Procurement
	//-----------BEGINING LEVEL------------
	Quantity_CI = 4;
	Quantity_FGI = 5;// s_r dan büyük baþlat yoksa ilk adýmda remanufacturing yapmýyor
	Quantity_P = 1;
	BufferSize_1=5;
	BufferSize_2=5;
	Current_B1 = 3;
	Current_B2 = 3;
	//--------MachineBreakdownRate-----
	Disassembly_MachineBreakdownRate=0.05;
	Refurbishing_MachineBreakdownRate=0.07;
	Reassembly_MachineBreakdownRate=0.1;
	//----------Routing--------------
	RoutingRate = 0.05;
	//------------S sayýlarý--------------
	s_p = 3;//smaller than s_r 
	s_r =4;
	S_r =5;
	s_disposal = 5;
	s_B1_Disassembly=2;
	s_B2_Refurbishing=2;
	//----------WARMUP PERIOD---------
	Warm_upPeriod = 50;
	//-------COSTS-----------
	cost_P=10;
	Cost_CI=0.5;
	cost_demandNotMeet = 50;
	cost_disposal=0;
	Cost_Reman=5;
	Cost_FGI = 1;
    //----------------------------
	cout << "Enter Finish Time of Simulation: " << endl;
	cin >> FinishTime;
	double lamda = std::max(Return_Lamda, Demand_Lamda) + 6;//should bigger than demand lamda

	std::default_random_engine generator2;
	std::exponential_distribution<double> dist2(lamda);
	double temp = 0;
	while (temp < FinishTime)
	{
		temp += dist2(generator2);
		n++;
	}
	n += 150;
	cout << n << "  Return and Demand arrivals were created.." << endl;

	Initialize();
	dosya.open("RandomSayýlar.xls", ios::app);
	dosya << endl << "---------------------------------------------------------------------------------------------------------------------------------";
	dosya << endl << "FOR FinishTime: " << '\t' << FinishTime << '\t' << "retun_lamda: " << '\t' << Return_Lamda << '\t' << "demand_lamda: " << '\t' << Demand_Lamda << '\t' << " Disassembling_mean: " << '\t' << Disassembly_mean << '\t' << " Disassembling_stddev: " << '\t' << Disassembly_stddev << '\t' << " Refurnishing_mean: " << '\t' << Refurbishing_mean << '\t' << " Refurnishing_stddev: " << '\t' << Refurbishing_stddev << '\t' << " Reassembling_mean: " << '\t' << Reassembly_mean<<'\t' << " Reassembling_stddev: " << '\t' << Reassembly_stddev << endl;
	dosya << endl << "SEL:" << '\t';

	for (int a = 0; a< qqq; a++)
	{
		dosya << SEL[a][0] << "-" << SEL[a][1] <<'\t';
	}
	dosya << endl << "1-ReturnArrival 2-Demand 31-DisasemblyFinished  32-RefurbishingFinished 33-ReassemblyFinished 4-ProcurementFinished " ;
	dosya << endl <<  "Disassembling Times: " << '\t';
	for (int t = 0; t < aaa; t++)
		dosya << Disassembling_Time[t] <<'\t';
	dosya << endl << "Refurbishing Times: " << '\t';
	for (int t = 0; t < aaa; t++)
		dosya << Refurbishing_Time[t] << '\t';
	dosya << endl << "Reassembling Times: " << '\t';
	for (int t = 0; t < aaa; t++)
		dosya << Reassembly_Time[t] << '\t';
	dosya.close();
	if (FinishTime<=Warm_upPeriod)
	{
		cout <<" WarmUP period is so long";
		for(int i=0;i<10000000000;i++){}
		return 0;
	}
	if (Quantity_FGI <= s_r)
	{
		iteration = 0;
		Q_Order_Reassembly+= (S_r - (Q_Order_Reassembly + Quantity_FGI));
		Reassemble();
		iteration = -1;
	}
	if (Current_B2 <= s_B2_Refurbishing)
	{
		iteration = 0;
		Q_Order_Refurbishing += (S_r - (Q_Order_Refurbishing + Current_B2));
		Refurbish();
		iteration = -1;
	}
	if (Current_B1 <= s_B1_Disassembly)
	{
		iteration = 0;
		Q_Order_Disassembly += (S_r - (Q_Order_Disassembly + Current_B1));
		Disassemble();
		iteration = -1;
	}
	dosya.open("MachineBreakdowns.xls", ios::app);
	dosya << "Clock" << '\t' << "Machine Type" << '\t' << "Old Finish Time" << '\t' << "Repair Time" << '\t' << "New Finish Time"<<  endl;
	dosya.close();
	dosya.open("Routings.xls", ios::app);
	dosya << "Clock" << '\t' << "EventType" << '\t' << "RoutingStartTime" << '\t' << "Routing Time" << '\t' << "RoutingFinishTÝme" << endl;
	dosya.close();
	dosya.open("iterations.xls", ios::app);
	dosya << endl << "---------------------------------------------------------------------------------------------------------------------------------";
	dosya << endl << endl << "FOR FinishTime:" << '\t' << FinishTime << '\t' << "retun_lamda: " << '\t' << Return_Lamda << '\t' << "demand_lamda: " << '\t' << Demand_Lamda << '\t' << " Disassembling_mean: " << '\t' << Disassembly_mean << '\t' << " Disassembling_stddev: " << '\t' << Disassembly_stddev << '\t' << " Refurnishing_mean: " << '\t' << Refurbishing_mean << '\t' << " Refurnishing_stddev: " << '\t' << Refurbishing_stddev << '\t' << " Reassembling_mean: " << '\t' << Reassembly_mean << '\t' << " Reassembling_stddev: " << '\t' << Reassembly_stddev << endl;
	dosya << endl << "Event" << '\t' << "Clock" << '\t' << "TotalAccept " << '\t' << " TotalDisp" << '\t' << "Qnt_CI" << '\t' << "FactoryBUSY_Disassembly " << '\t' << " Q_Order_Disassembly" << '\t' << "Buffer1";
	dosya << '\t' << "FactoryBUSY_Refurbishing " << '\t' << " Q_Order_Refurbishing" << '\t' << "Buffer2" << '\t' << "FactoryBUSY_Reassembly " << '\t' << " Q_Order_Reassembly";
	dosya << '\t' << "Qnt_FGI " << '\t' << " Q_madeProc " << '\t' << "Q_DemandMet  " << '\t' << "Q_DemandUnMet" << endl;
	dosya << 0 << '\t' << Clock << '\t' << TotalAcceptance << '\t' << Q_Disposal << '\t' << Quantity_CI << '\t' << FactoryBusy_Disassembly << '\t' << Q_Order_Disassembly << '\t' << Current_B1;
	dosya << '\t' << FactoryBusy_Refurbishing << '\t' << Q_Order_Refurbishing << '\t' << Current_B2 << '\t' << FactoryBusy_Reassembly << '\t' << Q_Order_Reassembly;
	dosya<< '\t' << Quantity_FGI << '\t' << Q_madeProc << '\t' << DemandMetByOnHand << '\t' << Q_DemandNotMeet << endl;
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
		case 31:
			DisassemblingFinished();
			Q_Disassembled += 1;
			if (Clock >= Warm_upPeriod)
				Q_Disassembled_After_WarmUp++;
			break;
		case 32:
			RefurbishingFinished();
			Q_Refurbished += 1;
			if (Clock >= Warm_upPeriod)
				Q_Refurbished_After_WarmUp++;
			break;
		case 33:
			ReassemblingFinished();
			Q_Remanufactured++;
			Q_Reassembled ++;
			
			if (Clock >= Warm_upPeriod)
			{
				Q_R_After_WarmUp++;
				Q_Reassembled_After_WarmUp++;
			}
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
		QueuLengthWÝthoutDividing();
	}
	CostCalculation();
	//cout << *warmup << " " << warmup;
	dosya.open("iterations.xls", ios::app);
	dosya << endl << "Q_Return= " << '\t' << "Q_Demand= " << '\t' << "Q_Disassembled" << '\t' << "Q_Refurbished" << '\t' << "Q_Reassembled" << '\t' << "Q_madeProc= " << '\t' << "Total Procurement= " << '\t' << "Q_Disposal= " << '\t' << "Q_DemandUnMet= ";
	dosya << endl << Q_Return << '\t' << Q_Demand << '\t' << Q_Disassembled << '\t' <<Q_Refurbished<< '\t'<< Q_Reassembled << '\t' << Q_madeProc << '\t' << TotalProcurement << '\t' << Q_Disposal << '\t' << Q_DemandNotMeet;
	dosya << endl << "S_r:" << '\t' << "s_r:" << '\t' << "s_p:" << '\t' << "s_d:" << '\t' << "retun_lamda: " << '\t' << "demand_lamda: ";
	dosya << '\t' << " Disassembling_mean:" << '\t' << " Disassembling_stddev: " << '\t'<<" Refurnishing_mean: " << '\t' << " Refurnishing_stddev: " << '\t' << " Reassembling_mean: " << '\t' << " Reassembling_stddev: " << '\t' << " LT_Procurment ";
	dosya << endl << S_r << '\t' << s_r << '\t' << s_p << '\t' << s_disposal << '\t' << Return_Lamda << '\t' << Demand_Lamda;
	dosya << '\t' << Disassembly_mean << '\t' << Disassembly_stddev << '\t'<< Refurbishing_mean << '\t' << Refurbishing_stddev << '\t' << Reassembly_mean << '\t' << Reassembly_stddev << '\t' << "Q_P*" << k_Procurement << endl;
	dosya << endl << "Time_Avg_CI" << '\t' << Time_Avg_CI << '\t' << "Time_Avg_FGI" << '\t' << Time_Avg_FGI << '\t' <<"Time_Avg_B1"<< '\t' << Time_Avg_B1<<'\t'<< "Time_Avg_B2" << '\t' << Time_Avg_B2 << '\t' << "Time_Avg_NumOfRemanufactured" << '\t' << Time_Avg_NumOfRemanufactured;
	dosya<< '\t' << "Time_Avg_NumOfProcurement"<< '\t' << Time_Avg_NumOfProcurement << '\t' << "Time_Avg_disposal"<< '\t' << Time_Avg_disposal << '\t' << "Time_Avg_demandNotMeet" << '\t' << Time_Avg_demandNotMeet;
	dosya << endl << "Time_Avg_DisassemblyFactoryBusy" << '\t' << Time_Avg_DisassemblyFactoryBusy << '\t' << "Time_Avg_RefurbishingFactoryBusy" << '\t' << Time_Avg_RefurbishingFactoryBusy << '\t' << "Time_Avg_ReassemblyFactoryBusy " << '\t' << Time_Avg_ReassemblyFactoryBusy;
	dosya << endl << "COST per time=" << '\t' << CostPerTime;
	dosya.close();
	dosya.open("RandomSayýlar.xls", ios::app);
	dosya << endl << "Last version of SEL:" << '\t';
	for (int a = 0; a< qqq; a++)
	{
		dosya << SEL[a][0] << "-" << SEL[a][1] << '\t';
	}
	dosya.close();
	DeleteArrays();
	if (i_disassembly > aaa + 1 || i_Reassembly > aaa + 1 || i_Refurbishing > aaa + 1) //Ürettiðim zamanlar yetersiz mi?
		cout << "Üretim zamanlarý yetmedi........";
	return 0;
}
 
 
