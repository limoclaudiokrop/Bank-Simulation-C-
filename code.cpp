#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;
struct Customer{
	double arrival;
	double service_time;
	int priority;
}typedef Customer;

struct Teller {
	int idle;
	double end_service;
	int num_customers;
	double idle_time;
} typedef Teller;

struct Node {
	struct Node* next;
	struct Customer c;
}typedef Node;


Customer arr[100]; 
Teller tellers[4];
int num_tellers;
double start_sim;
double end_sim;
double avg_serviceTime;
double avg_waiting;
int max_len_q;
double tot_queueing_time;
double tot_idle_time;

int empty(Node* head){
	if(head->next == nullptr){
		return 1;
	}
	return 0;
}

int lengthQueue(Node* head){
	if(head->next == nullptr){
		return 0;
	}
	Node* curr = head;
	int len = 0;
	while(curr->next != nullptr){
		len ++;
		curr = curr->next;
	}
	return len;
}

Teller serveCustomer(Teller t, Customer c){
	t.num_customers ++;
	t.idle = 0;
	if(c.arrival > t.end_service){
		t.idle_time += c.arrival - t.end_service;
		t.end_service = c.arrival + c.service_time;
	}else{
		avg_waiting += t.end_service - c.arrival;
		t.end_service = t.end_service + c.service_time;
	}
	return t;
}

void addCustomerToQueue(Node* queue, Customer c){
	int added = 0;
	Node* head = queue;
	if(head->next == nullptr){
		head->next = (Node*)malloc(sizeof(Node));
		head->next->next = nullptr;
		head->next->c = c;
		added = 1;
	}else{
		Node* curr = head;

		while(curr->next != nullptr){
			if(c.priority > curr->next->c.priority){
				Node* temp = curr->next;
				curr->next = (Node*)malloc(sizeof(Node));
				curr->next->next = temp;
				curr->next->c = c;
				added = 1;
				break;
			}
			curr = curr->next;
		}
		if(!added){
			curr->next = (Node*)malloc(sizeof(Node));
			curr->next->next = nullptr;
			curr->next->c = c;		
		}
		
	}

	int len = lengthQueue(queue);
	if (len > max_len_q){
		max_len_q = len;
	}
}

Customer take_customer_from_queue(Node* queue){
	if(queue->next == nullptr){
		Customer temp;
		temp.arrival = 0.0;
		temp.service_time = 0.0;
		temp.priority = 0;
		return temp;
	}
	Customer c = queue->next->c;
	queue->next = queue->next->next;
	return c;
}

double minAvailable(){
	double available = tellers[0].end_service;
	for(int i =0; i < num_tellers; i++){
		if(tellers[i].end_service < available){
			available = tellers[i].end_service;
		}
	}
	return available;
}
void serve_customers(Node* queue, Customer newCustomer){

	double arrival = newCustomer.arrival;
	double available = minAvailable();
	while(arrival > available){
		//pop customer from queue and is served
		for(int t_i =0; t_i < num_tellers; t_i++){
			Teller t = tellers[t_i];
			if(arrival > t.end_service){
				if(empty(queue)){
					if(arrival > t.end_service){
						t.idle_time += arrival - t.end_service;
						t.end_service = newCustomer.arrival + newCustomer.service_time;
					}else{
						addCustomerToQueue(queue, newCustomer);
					}
					tellers[t_i] = t;
					return;
				}else{
					Customer c = take_customer_from_queue(queue);
					 
					t.idle = 0;
					t.num_customers ++;
						
					if(c.arrival > t.end_service){
						t.idle_time += c.arrival - t.end_service;
						t.end_service = c.arrival + c.service_time;
					}else{
						avg_waiting += t.end_service - c.arrival;
						t.end_service = t.end_service + c.service_time;
					}
					tellers[t_i] = t;
						
				}
			}
			
		}
		available = minAvailable();

		
		
	}
	addCustomerToQueue(queue, newCustomer);
	
}


int readFile(){
	fstream file;
	string word, t, q, filename;
	filename = "sample.txt";
	file.open(filename.c_str());
	int index = 0;
	int arr_ind = 0;
	Customer curr;
	while(file >> word){

		int val = index % 3;
		
		if (val == 0){
			if(index > 0){
				arr[arr_ind] = curr;
				arr_ind++;
			}
			if(atof(word.c_str()) == 0){
				break;
			}
			curr.arrival = atof(word.c_str());
		}
		if(val == 1){
			curr.service_time = atof(word.c_str());
		}
		if(val == 2){
			curr.priority = atoi(word.c_str());
		}
		index ++;
		
	}
	return arr_ind;
}

void runSimulation(Node* queue, int arr_ind){
	avg_serviceTime = 0;
	avg_waiting = 0;
	max_len_q = 0;

	for(int i =0; i < arr_ind; i++){
		avg_serviceTime += arr[i].service_time;
		if(i ==0){
			start_sim = arr[i].arrival;
		}
		if(empty(queue)){
			int served = 0;
			for(int t=0; t < num_tellers; t++){
				if(tellers[t].idle){
					tellers[t] = serveCustomer(tellers[t], arr[i]);
					served = 1;
					break;
				}
			}
			if(!served){
				addCustomerToQueue(queue, arr[i]);
				
			}

		}else{
			serve_customers(queue, arr[i]);
		}

		//cout << tellers[0].end_service << " " << tellers[0].idle << " " << tellers[0].num_customers << '\n';
		// if(i == 4){
		// 	break;
		// }
	}

	// cout << '\n' << "out" <<  '\n';
	// Node* cur = queue->next;
	// while(cur != nullptr){
	// 	cout << cur->c.arrival << " " << cur->c.service_time << " " << cur->c.priority << '\n';
	// 	cur = cur->next;
	// }

	avg_serviceTime = avg_serviceTime/arr_ind;
	tot_queueing_time = avg_waiting;
	avg_waiting = avg_waiting/arr_ind;

	for(int i =0; i < num_tellers; i++){
		if(tellers[i].end_service > end_sim){
			end_sim = tellers[i].end_service;
		}
	}

}

void output_statistics(){
	cout << "Number of tellers: " << num_tellers << '\n';
	cout << "Number of customers served by each teller" << '\n';
	for(int i =0; i < num_tellers; i++){
		cout << "Teller " << i+1 << " : " << tellers[i].num_customers << '\n';
	}
	cout << "\nTotal time of similation: " << end_sim - start_sim << '\n';
	cout << "Average Service time: " << avg_serviceTime << '\n';
	cout << "Average Waiting time: " << avg_waiting << '\n';
	cout << "Maximum length of queue: " << max_len_q << '\n';
	cout << "Avarage length of queue: " << tot_queueing_time/end_sim << '\n';
	cout << "\nIdle rate for each teller" << '\n';
	for(int i =0; i < num_tellers; i++){
		cout << "Teller " << i+1 << " : " << tellers[i].idle_time/(end_sim-start_sim) << '\n';
	}


}

int main(){
	
	cout << "Reading File..." << "\n";
	int arr_ind = readFile();
	num_tellers = 1;
	cout << "Setting up tellers..." << '\n';
	Teller teller;
	teller.idle = 1;
	teller.num_customers = 0;	
	teller.end_service = 0.0;
	tellers[0] = teller;
	cout << "Setting up the queue..." << '\n';
	Node* queue = (Node *) malloc(sizeof(Node));
	queue->next = nullptr;
	Customer nul;
	nul.arrival = 0.0;
	nul.service_time = 0.0;
	nul.priority = 0;
	queue->c = nul;
	cout << "\n Simulation 1\n";
	//Simulation with 1 teller
	//runSimulation(queue, arr_ind);
	//output_statistics();
	num_tellers = 4;
	queue->next = nullptr;
	teller.idle = 1;
	teller.num_customers = 0;	
	teller.end_service = 0.0;
	tellers[0] = teller;
	tellers[1] = teller;
	tellers[2] = teller;
	tellers[3] = teller;

	cout << "\nSimulation 3\n";
	runSimulation(queue, arr_ind);
	output_statistics();
	return 0;
}