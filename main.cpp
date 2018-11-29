// Jason Jue

#include <iostream>
#include "rv.h"
#include "event.h"

// Simulates an M/M/1 queueing system.  The simulation terminates
// once 100000 customers depart from the system.
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
  	cout<<"Please enter the values of: ph, pl, r2d, r21, r22, mu1, mu2h, mu2l in the order\n";
	cin>> ph >> pl >> r2d >> r21 >> r22 >> mu1 >> mu2h >> mu2l;
  }

  for (lambda = 1; lambda <= 10; lambda++){
	  
	  EventList Elist;                // Create event list
	  enum {ARR,DEP};                 // Define the event types

	  //double lambda1h = ph*lambda;    // High priority customer arrival rate
	  //double lambda1l = pl*lambda;    // Low priority customer arrival rate

	  double clock = 0.0;             // System clock
	  
	  long int N = 0;                 // Number of customers in system
	  long int N1h = 0;		  // Number of high priority customers to be serviced by queue 1
	  long int N1l = 0;		  // Number of low priority customers to be serviced by queue 1
	  long int N2h = 0;		  // Number of high priority customers to be serviced by queue 2
	  long int N2l = 0;		  // Number of low priority customers to be serviced by queue 2
	  
	  long int Ndep = 0;              // Number of departures from system
	  double EN = 0.0;                // For calculating E[N]

	  int done = 0;                   // End condition satisfied?

	  Event* CurrentEvent;
	
	  if (uni_rv() <= ph){
	  	Elist.insert(exp_rv(lambda),ARR, QUEUE1, HIGH); // Generate first arrival event to queue 1 for high priority customer
	  }
	  else{
	  	Elist.insert(exp_rv(lambda),ARR, QUEUE1, LOW); // Generate first arrival event to queue 1 for low priority customer
	  }
	  
	  while (!done)
	  {
	  	CurrentEvent = Elist.get();               // Get next Event from list
	    	if (CurrentEvent == 0){
	    		//IF the eventlist is empty, generate new arrival events.
			if (uni_rv() <= ph)
				Elist.insert(clock+exp_rv(lambda),ARR, QUEUE1, HIGH);
			else 
				Elist.insert(clock+exp_rv(lambda),ARR, QUEUE1, LOW);
			//Refresh the current event
			delete CurrentEvent;
			CurrentEvent = Elist.get();
		}

	    	double prev = clock;                      // Store old clock value
	    	clock=CurrentEvent->time;                 // Update system clock 

	    	if (CurrentEvent->queue == QUEUE1){
			switch (CurrentEvent->type) {
		    	case ARR:                                 // If arrival 
		     		EN += N*(clock-prev);                   //  update system statistics
		      		
				N++;                                    //  update system size
		      		if (CurrentEvent->priority == HIGH)
		      			N1h++;
				else if (CurrentEvent->priority == LOW)
					N1l++;
				
				// Generate next arrival
				if (uni_rv() <= ph){
		      			Elist.insert(clock+exp_rv(lambda),ARR, QUEUE1, HIGH);
				}
				else{
					Elist.insert(clock+exp_rv(lambda),ARR, QUEUE1, LOW);
				}

				//If this is the only customer then generate its departure event
		      		if (N==1 && (CurrentEvent->priority == HIGH)){
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, HIGH);
		      		}
				else if (N==1 && (CurrentEvent->priority == LOW)){
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);
				}
		      		break;
		    	case DEP:                                 // If departure
		      		EN += N*(clock-prev);                   //  update system statistics

		      		N--;                                    //  decrement system size
				if (CurrentEvent->priority == HIGH)
					N1h--;
				else if (CurrentEvent->priority == LOW)
					N1l--;
		      		
				Ndep++;                                 //  increment num. of departures

		      		//If customers remain, first generate departure events for all high priority customers.
				//If there is no high priority customers, then only generate departure event for low.
				if (N > 0) {
					if (N1h > 0)
						Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, HIGH);
					else if (N1l > 0)
						Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);
		      		} 
		      		break;
	    	    	}
	    	}
	    delete CurrentEvent;
	    if (Ndep > 500000) done=1;        // End condition
	  }
	  // output simulation results for N, E[N] 
	  cout << "Current number of customers in system: " << N << endl;
	  cout << "Expected number of customers (simulation): " << EN/clock << endl;

	  // output derived value for E[N]
  	double rho = lambda/mu; 
  	cout << "Expected number of customers (analysis): " << rho/(1-rho) << endl;
  }
}

