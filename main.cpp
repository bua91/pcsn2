// Owner: Arunabha Chakraborty

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
  /*char def;
  cout<<"Do you want to use default values[y/n]:"<<"\t";
  cin >> def;
  if (tolower(def) != 'y'){
  	cout<<"Please enter the values of: ph, pl, r2d, r21, r22, mu1, mu2h, mu2l in the order\n";
	cin>> ph >> pl >> r2d >> r21 >> r22 >> mu1 >> mu2h >> mu2l;
  }*/

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
	  double uniform = 0.0;

	  Event* CurrentEvent;

	  uniform = uni_rv();
	  if (uniform <= ph){
	  	Elist.insert(exp_rv(lambda),ARR, QUEUE1, HIGH); // Generate first arrival event to queue 1 for high priority customer
	  }
	  else{
	  	Elist.insert(exp_rv(lambda),ARR, QUEUE1, LOW); // Generate first arrival event to queue 1 for low priority customer
	  }
	  
	  while (!done)
	  {
	  	CurrentEvent = Elist.get();               // Get next Event from list
	    	if (CurrentEvent == NULL){
			uniform = uni_rv();
	    		//IF the eventlist is empty, generate new arrival events.
			if (uniform <= ph)
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
			cout<< " in QUEUE 1 priority = "<<CurrentEvent->priority<<endl;
			switch (CurrentEvent->type) {
		    	case ARR:				// If arrival
				cout<< " in QUEUE 1 ARR\n";
		     		EN += N*(clock-prev);                   //  update system statistics
		      		
				N++;                                    //  update system size
		      		if (CurrentEvent->priority == HIGH)
		      			N1h++;
				else if (CurrentEvent->priority == LOW)
					N1l++;
				
				uniform = uni_rv();
				// Generate next arrival
				if (uniform <= ph)
		      			Elist.insert(clock+exp_rv(lambda),ARR, QUEUE1, HIGH);
				else
					Elist.insert(clock+exp_rv(lambda),ARR, QUEUE1, LOW);

				//If this is the only customer then generate its departure event
		      		if (N==1 && (CurrentEvent->priority == HIGH))
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, HIGH);
				else if (N==1 && (CurrentEvent->priority == LOW))
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);
		      		break;
		    	
			case DEP:                                 // If departure
				cout<< " in QUEUE 1 DEP\n";
		      		EN += N*(clock-prev);                   //  update system statistics
				if (CurrentEvent->priority == HIGH){
					cout << "HIGH\n";
					N1h--;
					N2h++;
					//generate its departure event from queue 2
					Elist.insert(clock+exp_rv(mu2h),DEP, QUEUE2, HIGH);

				}
				else if (CurrentEvent->priority == LOW){
					cout <<"LOW\n";
					N1l--;
					N2l++;
					uniform = uni_rv();
					//generate its departure evenet from queue 2
				 	if (uniform <= r2d)
						Elist.insert(clock+exp_rv(mu2l),DEP, QUEUE2, LOW);
					else if ((uniform > r2d) && (uniform <= (r2d+r21)))
						N1l++;
					else
						N2l += 0.0; //N2l will remain same as the event goes out from queue 2 and returns back to queue 2 again
		      		}

		      		//If customers remain, first generate departure events for all high priority customers.
				//If there is no high priority customers, then only generate departure event for low.
				cout<< " in QUEUE 1::-- N1h="<<N1h<<"\t N1l="<<N1l<<"\t\t N="<<N<<endl;
				if (N1h > 0)
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, HIGH);
				else if (N1l > 0)
					Elist.insert(clock+exp_rv(mu1),DEP, QUEUE1, LOW);

		      		break;
	    	    	}
	    	}
		
		else if (CurrentEvent->queue == QUEUE2){
			cout<< " in QUEUE 2\n";
			switch (CurrentEvent->type) {
			//in queue 2 only departurre events will be there
		    	case DEP:
		      		EN += N*(clock-prev);                   //  update system statistics

		      		N--;                                    //  decrement system size
				if (CurrentEvent->priority == HIGH)
					N2h--;
				else if (CurrentEvent->priority == LOW)
					N2l--;
		      		
				Ndep++;                                 //  increment num. of departures
				cout<<"========="<<Ndep<<endl;
				cout<<"priority=="<<CurrentEvent->priority<<endl;

					cout<< " in QUEUE 2222::-- N2h="<<N2h<<"\t N2l="<<N2l<<"\t\t N="<<N<<endl;
		      		//If customers remain, first generate departure events for all high priority customers.
				//If there is no high priority customers, then only generate departure event for low.
				if (N > 0) {
					cout<<"####################\n";
					if (N2h > 0)
						Elist.insert(clock+exp_rv(mu2h),DEP, QUEUE2, HIGH);
					else if (N2l > 0){
						uniform = uni_rv();
						if (uniform < r2d)
					        	Elist.insert(clock+exp_rv(mu2l),DEP, QUEUE2, LOW);
						else if ((uni_rv() > r2d) && (uni_rv() <= (r2d+r21)))
						        N1l++;
						else
						        N2l += 0.0; //N2l will remain the same as the event goes out from queue 2 and returns back to queue 2 again
					}
		      		} 
		      		break;
	    	    	}
	    	}
	    delete CurrentEvent;
	    //static int test = 0;
	    //cout<<"test ======== "<<test++<<endl;
	    if (Ndep > 500000) done=1;        // End condition
	  }
	  // output simulation results for N, E[N] 
	  //cout << "Current number of customers in system: " << N << endl;
	  //cout << "Expected number of customers (simulation): " << EN/clock << endl;

	  // output derived value for E[N]
  	//double rho = lambda/mu; 
  	//cout << "Expected number of customers (analysis): " << rho/(1-rho) << endl;
	cout <<"lambda:"<<lambda<<endl;
    }
}
