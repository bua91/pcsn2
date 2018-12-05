// Owner: Arunabha Chakraborty

#include <iostream>
#include "rv.h"
#include "event.h"
#include <iomanip>
int main()
{
  using namespace std;
  double lambda = 1;
  double ph, pl, r2d, r21, r22, mu1, mu2h, mu2l;
  ph = 2.0/5.0;
  pl = 3.0/5.0;
  r2d = 1.0/4.0;
  r21 = 1.0/4.0;
  r22 = 1.0/2.0;
  mu1 = 20;
  mu2h = 10;
  mu2l = 50;
  
  char def;
  cout<<"Do you want to use default values[y/n]:"<<"\t";
  cin >> def;
  if (tolower(def) != 'y'){
  	cout<<"Please enter the values of: ph, r2d, r21, mu1, mu2h, mu2l in the order\n";
	cin>> ph >> r2d >> r21 >> mu1 >> mu2h >> mu2l;
	pl = 1 - ph;
	r22 = 1 - (r2d+r21);
  }
  for (lambda = 1; lambda <= 10; lambda++){
	  
  	  cout<<"\nFOR lambda = "<<lambda<<"\n=======================================================================================================================\n";
	  EventList Elist;                // Create event list
	  enum {ARR,DEP, TRANS};                 // Define the event types

	  long double clock = 0.0;             // System clock
	  
	  long int N = 0;                 // Number of customers in system
	  long int N1h = 0;		  // Number of high priority customers to be serviced by queue 1
	  long int N1l = 0;		  // Number of low priority customers to be serviced by queue 1
	  long int N2h = 0;		  // Number of high priority customers to be serviced by queue 2
	  long int N2l = 0;		  // Number of low priority customers to be serviced by queue 2
	  
	  // Number of departures from system
	  long int Ndep = 0;              
	  long int Ndep1h = 0;
	  long int Ndep1l = 0;
	  long int Ndep2h = 0;
	  long int Ndep2l = 0;

	  // For calculating E[N] for each priority and queue
	  double EN = 0.0;                
	  double EN1h = 0.0;
	  double EN1l = 0.0;
	  double EN2h = 0.0;
	  double EN2l = 0.0;

	  double prev1h = 0.0;
	  double prev1l = 0.0;
	  double prev2l = 0.0;
	  double prev2h = 0.0;

	  int done = 0;                   // End condition satisfied?
	  double uniform = 0.0;

	  Event* CurrentEvent;

          Elist.insert(exp_rv(lambda*ph),ARR, QUEUE1, HIGH); // Generate first arrival event to queue 1 for high priority customer
	  Elist.insert(exp_rv(lambda*pl),ARR, QUEUE1, LOW); // Generate first arrival event to queue 1 for low priority customer
	  
	  while (!done)
	  {
	  	CurrentEvent = Elist.get();               // Get next Event from list
	    	if (CurrentEvent == NULL){
	    		//IF the eventlist is empty, generate new arrival events.
			Elist.insert(clock+exp_rv(lambda*ph),ARR, QUEUE1, HIGH);
			Elist.insert(clock+exp_rv(lambda*pl),ARR, QUEUE1, LOW);
			
			//Refresh the current event
			delete CurrentEvent;
			CurrentEvent = Elist.get();
		}

	    	//double prev = clock;                      // Store old clock value
	    	clock=CurrentEvent->time;                 // Update system clock 

	    	if (CurrentEvent->queue == QUEUE1){
			switch (CurrentEvent->type) {
		    	case ARR:				// If arrival
		      		if (CurrentEvent->priority == HIGH){
					EN1h += N1h*(clock-prev1h);
					prev1h = clock;
		      			N1h++;
				}
				else if (CurrentEvent->priority == LOW){
					EN1l += N1l*(clock-prev1l);
					prev1l = clock;
					N1l++;
				}				

				// Generate departure and then next arrival based on priority
				if (CurrentEvent->priority == HIGH){
					if ((N1h+N1l) == 1){
						Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, HIGH);
					}
		      			Elist.insert(clock+exp_rv(lambda*ph),ARR, QUEUE1, HIGH);
				}
				else if (CurrentEvent->priority == LOW){
					if ((N1h+N1l) == 1){
						Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);
					}
					Elist.insert(clock+exp_rv(lambda*pl),ARR, QUEUE1, LOW);
				}

		      		break;
		    	
			case DEP:                                 // If departure
				if (CurrentEvent->priority == HIGH){
					EN1h += N1h*(clock-prev1h);
					prev1h = clock;
					EN2h += N2h*(clock-prev2h);
					prev2h = clock;

					N1h--;
					N2h++;
					Ndep1h++;
					//generate its departure event from queue 2
					if ((N2l+N2h) == 1){
						Elist.insert(clock+exp_rv(mu2h),DEP, QUEUE2, HIGH);
					}

				}
				else if (CurrentEvent->priority == LOW){
					EN1l += N1l*(clock-prev1l);
					prev1l = clock;
					EN2l += N2l*(clock-prev2l);
					prev2l = clock;

					N1l--;
					N2l++;
					Ndep1l++;
					if ((N2l+N2h) == 1){
						Elist.insert(clock+exp_rv(mu2l),DEP, QUEUE2, LOW);
					}
				}

		      		//If customers remain, first generate departure events for all high priority customers.
				//If there is no high priority customers, then only generate departure event for low.
				if (N1h > 0)
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, HIGH);
				else if (N1l > 0)
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);

		      		break;
			case TRANS:
				EN1l += N1l*(clock-prev1l);
				N1l++;
				prev1l = clock;
				if ((N1l+N1h) == 1){
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);
				}
	    	    	}
	    	}
		
		else if (CurrentEvent->queue == QUEUE2){
			switch (CurrentEvent->type) {
		    	case DEP:
				if (CurrentEvent->priority == HIGH){
					EN2h += N2h*(clock-prev2h);
					prev2h = clock;
					N2h--;
					if (N2h > 0)
						Elist.insert(clock+exp_rv(mu2h),DEP, QUEUE2, HIGH);
					else if (N2l > 0)
						Elist.insert(clock+exp_rv(mu2l),DEP, QUEUE2, LOW);
					Ndep2h++;
					Ndep++;
				}
				else if (CurrentEvent->priority == LOW){
					EN2l += N2l*(clock-prev2l);
					prev2l = clock;
					N2l--;
					Ndep2l++;
					if (N2h > 0)
						Elist.insert(clock+exp_rv(mu2h),DEP, QUEUE2, HIGH);
					else if (N2l > 0)
						Elist.insert(clock+exp_rv(mu2l),DEP, QUEUE2, LOW);
					uniform = uni_rv();
					if (uniform <= r2d)
						Ndep++;
					else if ((uniform > r2d) && (uniform <= (r2d+r21)))
						Elist.insert(clock,TRANS, QUEUE1, LOW);
					else
						Elist.insert(clock,TRANS, QUEUE2, LOW);
				}		      		
		
		      		break;
			case TRANS:
				EN2l += N2l*(clock-prev2l);
				N2l++;
				prev2l = clock;
				if ((N2l+N2h) == 1)
					Elist.insert(clock+exp_rv(mu2l),DEP, QUEUE2, LOW);

				break;
	    	    	}
	    	}
	    delete CurrentEvent;
	    if (Ndep > 500000) done=1;        // End condition
	  }
	  
	  // RESULTS
	  // simulation and theoretical values for the high-priority throughput and the low-priority throughput of each
	  cout <<"Throughput (analysis)"<<"\n\t"<<"Thrput[1h] = " <<lambda*ph <<"\tThrput[1l] = "
	                           <<(lambda*pl)/(1.0-r22)<<"\tThrput[2h] = "<<lambda*ph<<"\tThrput[2l] = "<<(lambda*pl)/((1.0-r21-r22))<<endl;
	  cout <<"Throughput (simulation)"<<"\n\t"<<"Thrput[1h] = " << Ndep1h/clock <<"\tThrput[1l] = "
	                 <<Ndep1l/clock <<"\tThrput[2h] = "<<Ndep2h/clock<<"\tThrput[2l] = "<<Ndep2l/clock<<endl;

	  // simulation values for the expected number of each priority of customers in each queue
	  cout <<"Expected number of customers (simulation)"<<"\n"<<"\t E[N1h] = " << EN1h/clock <<"\tE[N1l] = "
	       <<EN1l/clock <<"\tE[N2h] = "<<EN2h/clock<<"\tE[N2l] = "<<EN2l/clock<<endl;

	  // simulation values for the average time each priority customer spends in Queue 2
	  cout <<"Avg time each priority customer spends in Queue 2\n"<<"\t E[T2h] = "<< EN2h/Ndep2h
	       <<"\t E[T2l] = "<< EN2l/Ndep2l;
    }
}
