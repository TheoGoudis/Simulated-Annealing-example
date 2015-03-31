//Vlachogkountis Theofilos (Theo Goudis)
//Simulated Annealing probabilistic method
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>


using namespace std;

#define N 10
#define maxOps N-1		//operators (+ - a) (a is not an operator. its a symbol for nothing)
#define maxAttempts 3*N	//max attempts before temperature drop
#define temprDrop 0.999	//rate of temperature drop
#define exitState 9*N	//max total attempts before exit


float tempr=10.0;	//current temperature
int numSeq[N];		//array (representing sequence) of numbers
int ops[N-1];		//0==+ 1==- 2==a (NULL) operators' array
int newops[N-1];	//new operators array
int target;		//goal sum (input by user)
int acsum;		//last accepted sum (sum of numSeq+ops)
int sum;			//sum= number sequence's and operators' numeric result
int newcost;	//cost if new explored state
int cost;		// |target-sum|
int attemptsPerTemp=0;	//till 3N, every 3N this=0 AND change temp
int totalFailedAttempts=0;	//till maxAttempts OR on succes this=0 
int exitFlag=0;	//flag for exit (when maxAttempts are reached)
int powTen[10]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};	//array of powers of ten. used in calcSum

void initialize();
void newRandOps();
int acceptOrNot();
void calcSum(int x);
void finalPrint();

int main(int argc, char** argv){
	cout<<endl<<"Give target number : ";
	cin>>target;
	initialize();
	
	while((exitFlag==0)&&(cost!=0)){		//exploring new points until we reach the maximum allowed number of attempts or until finding the right sum (cost=0)
		newRandOps();		//creating new point
		acceptOrNot();		//acceptance function for new point
	}
	finalPrint();		//print the results
	return 0;
}

void calcSum(int x){		//sum=(numeric expression result)  {IF x=0 then calculate numSeq+ops ELSE IF x=1 calculate numSeq+newops}
	int tsum=0;
	vector <int> tnums;	//LIFO queue (vector) we drop in the numbers last to first (considering NULL operators for creating higher than one digit numbers)
	vector <int> tops;	//LIFO queue (vector) we drop in the operators last to first (ignoring NULL operators)
	//aftre the creation of the vectors we pop the elements from both creating the numerical expression through wich we find the sum
	int z=1;
	
	tsum=numSeq[N-1];
	if(x==1){
		for(int i=N-2;i>-1;i--){
			if(newops[i]!=2){
				z=0;
				tops.push_back(newops[i]);
				tnums.push_back(tsum);
				tsum=0;
			}
			tsum+=numSeq[i]*powTen[z];
			z++;
		}
		tnums.push_back(tsum);
	}
	else{
		for(int i=N-2;i>-1;i--){
			if(ops[i]!=2){
				z=0;
				tops.push_back(ops[i]);
				tnums.push_back(tsum);
				tsum=0;
			}
			tsum+=numSeq[i]*powTen[z];
			z++;
		}
		tnums.push_back(tsum);
	}
	sum=0;
	sum+=tnums.back();
	tnums.pop_back();
	while(!tnums.empty()){
		z=tops.back();
		tops.pop_back();
		if(z==0)
			sum+=tnums.back();
		else
			sum-=tnums.back();
		tnums.pop_back();
	}
}

void initialize(){	//this func initializes the first number array and first operators array
	srand((unsigned)time(NULL));
	cout<<"Data in initial arrays{"<<endl;
	for(int i=0;i<N;i++){
		numSeq[i]=(rand() % 9) + 1;
		cout<<numSeq[i];
	}
	cout<<endl;
	for(int i=0;i<maxOps;i++){
		ops[i]=rand() % 3;
		cout<<ops[i];
	}
	cout<<endl<<"}"<<endl;
	int i=0;
	calcSum(i);
	cost=abs(sum-target);
	acsum=sum;
	cout<<"Generated sequence : ";
	cout<<numSeq[0];
	for(i=1;i<N;i++){
			if(ops[i-1]==0)
				cout<<"+";
			else if(ops[i-1]==1)
				cout<<"-";
			else
				cout<<" ";
			cout<<numSeq[i];
	}
	cout<<" initial sum="<<sum<<endl<<endl;
}

void newRandOps(){	//this func creates a new opperators array. randomly one operator is chosen and replaced by another one. 
	int p=-1;
	int pos=-1;
	int nops=-1;
	pos=rand() % maxOps;
	
	for(int i=0;i<maxOps;i++)
		newops[i]=ops[i];
	p=newops[pos];
	do{
		nops=rand() % 3;
	}while(nops==p);
	newops[pos]=nops;
	p=1;
	calcSum(p);
	newcost=abs(sum-target);
}

int acceptOrNot(){	//this is our acceptance func.
	int deltaE=newcost-cost;	//our De value. calculating the cost difference for moving to another point
	
	if(deltaE<0){		//new point has lower cost so it gets accepted
		for(int i=0;i<maxOps;i++)
			ops[i]=newops[i];
		cost=newcost;
		acsum=sum;
		totalFailedAttempts=0;
	}
	else{
		float probability=rand()/((float) RAND_MAX);
		if(probability<(1/(1+exp(deltaE/tempr)))){	//probability comparison to deltae, temperature expression. if probability is lower we accept the new point.
			for(int i=0;i<maxOps;i++)
				ops[i]=newops[i];
			cost=newcost;
			acsum=sum;
			totalFailedAttempts=0;
		}
		else
			totalFailedAttempts++;		//new point recected
	}
	
	if(attemptsPerTemp==maxAttempts){	//check if maxAttempts per temperature have been reached. if yes the the temperature drops else we try again with the same temperature
		tempr=tempr*temprDrop;
		attemptsPerTemp=0;
	}
	else
		attemptsPerTemp++;
		
	if(totalFailedAttempts==exitState){		//check if total amount of allowed failed attempts have been reached
		cout<<"Max attempts to reach goal reached."<<endl;
		exitFlag=1;
	}
	
}

void finalPrint(){	//this func prints out the results
	cout<<numSeq[0];
	for(int i=1;i<N;i++){
		if(ops[i-1]==0)
			cout<<"+";
		else if(ops[i-1]==1)
			cout<<"-";
		cout<<numSeq[i];
	}
	cout<<" = "<<acsum<<endl;
}
