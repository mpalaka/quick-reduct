//mohana palaka
//quick reduct using bsp
//the conditional attributes are stored in a 2d array and the decision attribute(we are considering only one) is stored in a 1d array
//TODO:
//incorporate new way of finding positive region : key : feature name, value : (value, no.of.instances)
//zeroes

#include "bsp.hpp"
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<iterator>
#include<algorithm>
#include<typeinfo>
#include<chrono>

using namespace std;
using namespace std::chrono;

typedef map<string, vector<int> > dict; 
typedef vector<vector<string> > string_mat;

class CSVReader{

	//void readFile();
	
	public:
		string filename; //filename; takes in .csv; to be taken in from constructor?
		string_mat cond_attr; //a 2d vector of vectors to hold the conditional attributes
		vector<string> dec_attr; // a 1d vector to hold the decision attribute

		void print_cond();
		void print_dec();
		void cutMat(int nrows);

		CSVReader(string fname){ //constructor to get name of the file from user
	
			filename = fname;
			
			string line;
			ifstream myfile;
			myfile.open(filename.c_str());
			//string_mat cond_attr;

			if(!myfile.is_open()){
				
				cout<<"Error opening the file :<";
				exit(0);

			}

			while(getline(myfile, line)){
			
				stringstream s(line);
				vector<string> row;
				string field;

				while(getline(s, field, ',')){
					
					row.push_back(field);
				}
	
				//if the row is not empty, push it into cond_attr
				if(row.size() > 0)	cond_attr.push_back(row);
			}

			myfile.close();
			
			//separating the decision attribute from the conditional attributes and inserting them into another array
			//...assuming that the decision attribute is the LAST column
			int rows = cond_attr.size();
			
			for(int i = 0; i < rows; ++i){
	
				//when the dec. attr is the last element
				/*dec_attr.push_back(cond_attr[i].back());
				cond_attr[i].pop_back();
				*/

				//the following lines of code are for when the decision attribute is the first element
				dec_attr.push_back(cond_attr[i].front());
				cond_attr[i].erase(cond_attr[i].begin());
				
			}

		}
};

void CSVReader::print_cond(){	//a function to print the values in a 2d mattrix

	int i,j = 0;
	int rows = cond_attr.size();
	int cols = cond_attr[0].size();

	for(i = 0; i < rows; ++i){
	
		for(j = 0; j < cols; ++j){
		
			cout<<cond_attr[i][j]<<"\t";
		}
		cout<<endl;
	}
}


void CSVReader::print_dec(){	//a function to print the values in a array of srings

	int i = 0;
	int rows = dec_attr.size();

	for(i = 0; i < rows; i++){
	
		cout<<dec_attr[i]<<endl;
	}
}


void CSVReader::cutMat(int nrows){

	int rem = cond_attr.size() - nrows;

	if(rem <= 0){
	
		cout<<"There's not that many rows! Try another number ... "<<endl;
		
	}

	else{
	
		for(int i = 1; i <= rem; i++){
		
			cond_attr.erase(cond_attr.begin() + nrows+i);
			dec_attr.erase(dec_attr.begin() + nrows+i);
		}
	
	}

}
//a structure to store attribute number and its corresponding gamma value
struct gamma{

	int attr_no;
	double gamma_val;
};

//a function to print the contents of the dictionary with <string, vector>
void printDict(dict ind){

	dict::iterator itr; //iterator for map
	vector<int>::iterator i;

	for(itr = ind.begin(); itr != ind.end(); ++itr){
	
		cout<<'\t'<<itr->first<<'\t';
		for(i = itr->second.begin(); i != itr->second.end(); ++i){
		
			cout<<(*i)<<" ";
		}

		cout<<endl;
	}
}

//a function to form the indiscernibility rel
void eqClass(dict &ind, string key, int value){

	//check the dict for the key. if it exists, update the value vector to include the object no(row no)

	ind[key].push_back(value);
}

//a function to find the name of the equivalence class of the object id by concatenating all the attribute values together "(<attribute_name><attribute_value>)*"
string findClass(vector<string> row, vector<string> attr_names){

	string key = "";
	int i;

	for(i = 0; i < row.size(); i++){
	
		key = key +  attr_names[i] + row[i];
	}

	return key;

}

//overriding for single columns
string findClass(string col, string attr_name){

	string key = "";
	int i;

	key = key +  attr_name + col;

	return key;

}

//a function to slice the dataset from m to n and put into another matrix/2d vector
void splitData(string_mat input, string_mat &output, int m, int n){

	for(int i = 0; i < input.size(); i++){
		
		auto first = input[i].cbegin() + m;
		auto last = input[i].cbegin() + n + 1;

		vector<string> vec(first, last);
		output.push_back(vec);
		
	}

}

//function to output 2d vector
void printMat(string_mat out){

	for(int i = 0; i < out.size(); i++){
		for(int j = 0; j < out[i].size(); j++){
		
			cout<<out[i][j]<<"\t";
		}
		cout<<endl;
	}

}


//function to check whether all the elements in a exist in b; i.e, if a is a subset of b 
bool in(vector<int> a, vector<int> b){

	bool flag = 1; 

	for(int i = 0; i < a.size(); i++){
	
		if(find(b.begin(), b.end(), a[i]) == b.end()) return 0;
	}

	return flag;

}
//overloading for finding an int in a vector
bool in(int a, vector<int> b){

	bool flag = 1; 

	
	if(find(b.begin(), b.end(), a) == b.end()) return 0;

	return flag;

}



//function to find the gamma value 
double gamma_pq(dict ind_p, dict ind_q, int U, int prev_pos){

	dict::iterator p_it, q_it;
	int pos_count = prev_pos; //this is for cases of postive region removal
	double gamma_val = 0.0;

	for(p_it = ind_p.begin(); p_it != ind_p.end(); ++p_it){
	
		vector<int> p_vec = p_it->second;

		for(q_it = ind_q.begin(); q_it != ind_q.end(); ++q_it){
		
			vector<int> q_vec = q_it->second;
			if(in(p_vec, q_vec)){ //check if p_vec belongs to current q_vec
			
				pos_count = pos_count + p_vec.size();
				break;//if you already found a superset of p_vec, then theres no way that another superset
			}
		}
	}

	gamma_val = double(pos_count) / U;
	
	return gamma_val;	
}

int find(double val, vector<double> vec){ //this function finds the index/position of the given value in the vector

	int i;

	for(i = 0; i < vec.size(); i++){
		
		if(vec[i] == val) break;
	}

	i++; //don't want zero valued indexing

	return i;
}

//function to remove the positive region of a granular structure and return the number of positive objects; can be clubbed with gamma_pq?
int remove_pos(dict &ind_p, dict ind_q){

	dict::iterator p_it, q_it;

	bool pos_flag = 0; //a flag to make 1 if we find a positive granule
	int pos_count = 0; //counter to count the number of positive objects 

	for(p_it = ind_p.begin(); p_it != ind_p.end(); ++p_it){
	
		vector<int> p_vec = p_it->second;
		pos_flag = 0;

		for(q_it = ind_q.begin(); q_it != ind_q.end(); ++q_it){
		
			vector<int> q_vec = q_it->second;

			//just printing q
			//cout<<"q_vec is ";
			//for(int i = 0; i < q_vec.size(); i++) cout<<q_vec[i]<<"\t";
			if(in(p_vec, q_vec)){ //check if p_vec belongs to current q_vec
				
				pos_flag = 1;

				//cout<<"p_vec is ";
				//for(int i = 0; i < p_vec.size(); i++) cout<<p_vec[i]<<"\t";

				pos_count = pos_count + p_vec.size();
				break;//if you already found a superset of p_vec, then theres no way that another superset
			}
		}
		
		if(pos_flag == 1) ind_p.erase(p_it); //remove granule
	
	}

	return pos_count;
}
/*
int findPos(ind_c, ind_d){ //to find the positive region of ind_c based on ind_d

	dict pos;
	int pos_count = 0;




}
*/

//a function to find the number of digits in an integer
int numDigits(int n){

	int digits = 0;

	while(n){
		
		n /= 10;
		digits++;
	
	}


	return digits;
}

int percentSamp(int p, int U){

	double  n_samples = (double)p / 100.0 * (double)U;
	
	return (int)n_samples;

}

int main(){

	//loading data into vectors 
	CSVReader raw_data("connect-4.data");
	int sample_size = percentSamp(15, raw_data.cond_attr.size());
	cout<<"sample size is "<<sample_size;
	raw_data.cutMat(sample_size);
	
	auto start_time = high_resolution_clock::now();

	//number of conditional attributes
	int feat_count = raw_data.cond_attr[0].size();	
	int obj_count = raw_data.cond_attr.size() - 1; // number of objects in the dataset; -1 because the first row is attribute names
	//cout<<"n features : "<<feat_count;
	int MAX_OBJ = (numDigits(obj_count) * obj_count) + (obj_count - 1);

	cout<<obj_count<<"\t"<<MAX_OBJ<<endl;
	//start the bsp computation
	bsp_begin(bsp_nprocs());

	int n_procs = bsp_nprocs();
	int def_nfeats = feat_count / n_procs; //default number of subfeatures
	int nsub_feat = feat_count / n_procs;

	if(bsp_pid() == bsp_nprocs() - 1){ //last processor gets the remaining columns
	
		nsub_feat = nsub_feat + (feat_count % n_procs);
	}

	bsp_sync();
	//this superstep is to create the subtables in local memory

	string_mat sub_table; //this is where the local sub-table is stored
	int start = bsp_pid() * def_nfeats; //starting index to be copied
	int end = start + nsub_feat -1; //last index to be copied

	splitData(raw_data.cond_attr, sub_table, start, end);

	//cout<<"feature count is "<<feat_count;
	//if(bsp_pid() == 3) cout<<"I am proc "<<bsp_pid()<<"and i have "<<start<<"to"<<end<<endl;

	/*
	cout<<"subtable of "<<bsp_pid()<<endl;
	for(int i = 0; i < sub_table.size(); i++){
		for(int j = 0 ; j < sub_table[0].size(); j++){	
			cout<<sub_table[i][j]<<"\t";
		}
		cout<<endl;
	}
	*/
	bsp_sync();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//this is from where we need to loop; 
//everything before this was dataloading and datasplitting
	
	//superstep to construct equivalence classes

	vector<int> attr_no;	//vector of ints to store the attribute numbers;not sure if this is necessary
	//dict ind_c;	//dictionary to form the indiscernibility relation for conditional attributes
	dict ind_d; //dictionary to form the indeiscernibility relation for decision attribute
	double tot_gamma = 0; //this will store the gamma value of all the best attributes so far; consistency throughout the processors should be maintained
	vector<int> reduct;
	string granule = "";
	char granule_arr[MAX_OBJ]; //should be set to the max number of characters for this dataset
	int pos_count = 0;
	int it_count = 0;

	//finding the indiscernibility relation induced by the decision attribute
	for(int i = 1; i < sub_table.size(); i++){
	
		string key = findClass(raw_data.dec_attr[i], raw_data.dec_attr[0]);
		eqClass(ind_d, key, i); //the value is the object index 
	}

	if(bsp_pid() == 3){
		
		cout<<"number of objects "<<obj_count<<endl;
		cout<<"number of attributes "<<feat_count<<endl;
	}
	bsp_sync();

	//this is the main loop that will continue until the stopping criteria is met
	while((tot_gamma < 1) && (reduct.size() <= feat_count) && (pos_count < obj_count)){

		it_count++;
		if(bsp_pid() == 0) {
			cout<<"****************************************"<<endl;
			cout<<"we are on iteration"<<it_count<<endl;
			cout<<"gamma is :"<<tot_gamma<<endl;
			cout<<"pos count is "<<pos_count<<endl;
			cout<<"current reduct is \t"<<endl;
			for(int i = 0; i < reduct.size(); i++) cout<<reduct[i]<<"\t";
			cout<<endl;
			//cout<<"size is "<<reduct.size();
		}
		vector<double> loc_gammas(nsub_feat, -1.0); //a vector to store all the local gamma values; initialize to the number of features
		dict ind_c; //dictionary to store ind relation for conditional attributes

		//finding the indiscernibility relation induced by the conditional attributes	
		for(int j = 0; j < nsub_feat; j++){ //iterating over the number of attributes in the processor
			//check to make sure the attribute is NOT one of the global max attribute
			
					
				//double lmax_g = 0;
				if(it_count == 1){ //in the first iteration, where we need to iterate through all the objects
					for(int i = 1; i < sub_table.size(); i++){
			
						string key = findClass(sub_table[i][j], sub_table[0][j]);
						eqClass(ind_c, key, i); //the value is the object index 
					}	
					//find the gamma value of the current feature and place it in its corresponding position in the vector of local gammas
					double g = gamma_pq(ind_c, ind_d, sub_table.size(), pos_count);
					loc_gammas[j] = g;
						
					ind_c.clear(); //clear the dictionary for the next feature

				}

				else{
					//cout<<"@@@@@granule is "<<granule_arr<<endl;
					int glob_attrno = (bsp_pid() * def_nfeats) + j + 1; //converting the local attribute number into global for checking				
					if(!in(glob_attrno, reduct)){
								
						//cout<<"j is :"<<j<<endl;
						//cout<<"glob attr is :"<<glob_attrno<<endl;
						//cout<<granule_arr<<endl;
						char grarr_copy[MAX_OBJ];
						strcpy(grarr_copy, granule_arr);
						char* r1 = granule_arr;
						char* token = strtok_r(grarr_copy, ";", &r1);
						int gr_no = 1; //granule number for unique identification
						//cout<<token;
						while(token != NULL){//iterating through the granules
							//cout<<token<<endl;

							string temp(token);
							char substr[MAX_OBJ] = "";
							strcpy(substr, temp.c_str());
							char* r2 = substr;
					
							char* obj_id = strtok_r(substr, ",", &r2);
							
							while(obj_id != NULL){
								//cout<<obj_id<<endl;
								string s_obj_id(obj_id);//converting it into a string
								int i_obj_id = stoi(s_obj_id);//converting the string to a int
								//if(bsp_pid() == 0) cout<<i_obj_id<<endl;
								string key = to_string(gr_no) + findClass(sub_table[i_obj_id][j], sub_table[0][j]); //we append the gr_no so we know that all these belong to the same previous granule
								eqClass(ind_c, key, i_obj_id);

								obj_id = strtok_r(NULL, ",", &r2);//goto next object id
							}


							token = strtok_r(NULL, ";", &r1); //goto next substring/token
							gr_no++; //next granule id
						}
						if(bsp_pid() == 2) {
							printDict(ind_c);
							//cout<<"#####granule is "<<granule_arr<<endl;
						}
						double g = gamma_pq(ind_c, ind_d, sub_table.size(), pos_count);
						loc_gammas[j] = g;
						ind_c.clear();
												
					}
					
					//cout<<"dict for attr"<<j<<" of proc "<<bsp_pid()<<endl;
					//if(bsp_pid() == 2) printDict(ind_c);
					//find the gamma value of the current feature and place it in its corresponding position in the vector of local gammas
				
					//ind_c.clear(); //clear the dictionary for the next feature

				}
				/*
				double g = gamma_pq(ind_c, ind_d, sub_table.size(), pos_count);
				loc_gammas.push_back(g);
				ind_c.clear();
				*/
		}

		bsp_sync();


		if(bsp_pid() == 3){
		
			cout<<"local gamas size "<<loc_gammas.size();
			//for(int i = 0; i < loc_gammas.size(); i++) cout<<loc_gammas[i]<<"\t";
			cout<<endl;
		}
		//finding the local best gamma value, and it's corresponding index/feature number
		double l_max = *max_element(loc_gammas.begin(), loc_gammas.end());
		int loc_lmax = find(l_max, loc_gammas); //local index
		
		//putting into the struct local best gamma
		struct gamma lb_gamma; 
		lb_gamma.attr_no = (bsp_pid() * def_nfeats) + loc_lmax; //global index
		lb_gamma.gamma_val = l_max;

			
		//if(bsp_pid() == 0){
		/*
		for(int i = 0; i < loc_gammas.size(); i++){
		
			cout<<loc_gammas[i]<<"\t";

		}
		cout<<endl;
		cout<<"local max of "<<bsp_pid()<<"is in "<<loc_lmax<<endl;
		cout<<endl<<"lb_gamma attr : "<<lb_gamma.attr_no<<endl;		
		cout<<endl<<"lb_gamma val : "<<lb_gamma.gamma_val;
		*/
		//}
		
		
		//bsp_sync(); //maybe unecessary because we are just creating a new local memory and preparing it for drma

		//create an array/vector of gamma structs to store all the local gammas
		//also register into drma

		struct gamma all_gammas[bsp_nprocs()]; //4 processors

		for(int i = 0; i < bsp_nprocs(); i++){
		
			bsp_push_reg(&all_gammas[i].attr_no, sizeof(string));
			bsp_push_reg(&all_gammas[i].gamma_val, sizeof(double));
		}

		
		bsp_sync();
		//put your local max gammas in corresponding place in all_gammas of all processors / broadcast to all 

		for(int i = 0; i < bsp_nprocs(); i++){
		
			bsp_put(i, &lb_gamma.attr_no, &all_gammas[bsp_pid()].attr_no, 0, sizeof(lb_gamma.attr_no));
			bsp_put(i, &lb_gamma.gamma_val, &all_gammas[bsp_pid()].gamma_val, 0, sizeof(lb_gamma.gamma_val));
		}

		bsp_sync();
		//superstep to find the global max gamma value

		//deregistering to close drma
		for(int i = 0; i < 4; i++){
		
			bsp_pop_reg(&all_gammas[i].attr_no);
			bsp_pop_reg(&all_gammas[i].gamma_val);
		}
		
		//outputting the global gamma values
		
		if(bsp_pid() == 3){
			cout<<"best gammas : "<<endl;

			for(int i = 0; i < 4; i++){
			
			cout<<"attr no : "<<all_gammas[i].attr_no<<endl;
			cout<<"gamma_val : "<<all_gammas[i].gamma_val<<endl;
			}
		
		}
		

		//finding global max gamma and storing it in another gamma struct
		struct gamma g_max;
		g_max.gamma_val = all_gammas[0].gamma_val;
		g_max.attr_no = all_gammas[0].attr_no;

		for(int i = 1; i < 4; i++){
		
			if(g_max.gamma_val < all_gammas[i].gamma_val){
			
				g_max.gamma_val = all_gammas[i].gamma_val;
				g_max.attr_no = all_gammas[i].attr_no;
			}
		}


		if(!in(g_max.attr_no, reduct)){ //to make sure that attribute has not already been added

			//putting the value of gmax in tot_gamma
			tot_gamma = g_max.gamma_val;
			//adding the attribute number to the reduct vector
			reduct.push_back(g_max.attr_no);
		}

		else break;

		bsp_sync();

		int gmax_pid = -1;
		
		if(g_max.attr_no >= start+1 && g_max.attr_no <= end+1){ //if this processor has the gbest attribute
		
			gmax_pid = bsp_pid();

		}

		//else break;
		
		//register the area to get it ready to recieve the gmax_pid
		bsp_push_reg(&gmax_pid, sizeof(int));


		bsp_sync();
		//broadcast the pid to everyone

		if(gmax_pid >= 0 ){
			
			int lpid = bsp_pid();
		
			for(int i = 0; i < bsp_nprocs(); i++){
				
				if(i != bsp_pid()) bsp_put(i, &lpid, &gmax_pid, 0, sizeof(lpid));
			}
		}

		bsp_sync();

		//deregistering gmax_pid
		bsp_pop_reg(&gmax_pid);

		//cout<<"GMAX PID IS"<<gmax_pid<<endl;

		//dict ind_gb;
		//vector<string> granule_array;
		//int n_grans = 0;
		//granule_arr[0] = '\0'; //this is currently the only thing that works; must find a maximum number of charaters
				       //im resetting the array
		//granule = ""; //this is just reset, the variable is declared in the beginning
		//vector<int> test;
		string new_gran = "";
		int gran_size = 0;

		//cout<<"gmax_pid is "<<gmax_pid<<endl;
		
		//if the index belongs to the processor
		if(bsp_pid() == gmax_pid){

			dict ind_gb;//to store the granules of gbest feature

			cout<<"Processor "<<bsp_pid()<<" has the global max"<<endl;
			int local_id = g_max.attr_no - (bsp_pid() * def_nfeats) - 1;  

			if(it_count == 1){ //first iteration
				for(int i = 1; i < sub_table.size(); i++){
			
					string key = findClass(sub_table[i][local_id], sub_table[0][local_id]);
					eqClass(ind_gb, key, i); //the value is the object index 
				}
			}
			else{

				//cout<<"((((granule is "<<granule_arr<<endl;
				char* r1;
				char* token = strtok_r(granule_arr, ";", &r1);
				int gr_no = 1; //granule number for unique identification
				//cout<<token;
				while(token != NULL){//iterating through the granules
					
					string temp(token);
					char substr[MAX_OBJ] = "";
					strcpy(substr, temp.c_str());
					char* r2;
				
					char* obj_id = strtok_r(substr, ",", &r2);

					while(obj_id != NULL){
						//cout<<obj_id<<endl;
						string s_obj_id(obj_id);//converting it into a string
						int i_obj_id = stoi(s_obj_id);//converting the string to a int

						string key = to_string(gr_no) + findClass(sub_table[i_obj_id][local_id], sub_table[0][local_id]); //we append the gr_no so we know that all these belong to the same previous granule
						eqClass(ind_gb, key, i_obj_id);

						obj_id = strtok_r(NULL, ",", &r2);//goto next object id
					}


					token = strtok_r(NULL, ";", &r1); //goto next substring/token
					gr_no++; //next granule id
				}
			}
			//int all_grans = ind_gb.size();
			//cout<<"The size of the dictionary is : "<<all_grans;
			printDict(ind_gb);
			int c_pos = remove_pos(ind_gb, ind_d);
			cout<<"current pos count is "<<c_pos<<endl;
			pos_count = pos_count + c_pos; //the current number of objects in the positive region
			cout<<"Final Pos count is "<<pos_count<<endl;	
			dict::iterator i; //to iterate through the keys
			vector<int>::iterator j; //to iterate through the values of a key
			granule = "";
			granule_arr[0] = '\0';

			for(i = ind_gb.begin(); i != ind_gb.end(); ++i){
			
				//string granule = "";

				for(j = i->second.begin(); j != i->second.end(); ++j){
				
					granule = granule + to_string(*j) + ",";
					//test.push_back(*j);
				}

				granule = granule + ";";
			}

			granule.pop_back();
			granule.pop_back();
			gran_size = granule.length() + 1;
			strcpy(granule_arr, granule.data());
		}
		else{
		
			granule = "";
			granule_arr[0] = '\0';
		
		}
		//bsp_push_reg(&gran_size, sizeof(int));
		//bsp_sync();
		cout<<"hello1!"<<endl;	
		bsp_push_reg(&granule_arr, MAX_OBJ);
		bsp_push_reg(&pos_count, sizeof(int));//also pass the number of postive granules

		bsp_sync();
		
		cout<<"hello2!"<<endl;
		if(bsp_pid() == gmax_pid){

			for(int i = 0; i < bsp_nprocs(); i++){
			
				if(i != gmax_pid){
					
					bsp_put(i, &granule_arr, &granule_arr, 0, MAX_OBJ);
					bsp_put(i, &pos_count, &pos_count, 0, sizeof(int));
					
				}
			}

		}
		
		cout<<"hello3!"<<endl;
		bsp_sync();
		//deregistering granule_arr
		bsp_pop_reg(&granule_arr);
		bsp_pop_reg(&pos_count);
		cout<<"hello4"<<endl;
		//cout<<"pos count is "<<bsp_pid()<<pos_count<<endl;
		granule = granule_arr; //converting the character array back into a string
		if(bsp_pid() == 3) {
			cout<<"for "<<bsp_pid()<<" best attribute "<<g_max.attr_no<<" has neg granules : ";
			cout<<"\t"<<granule<<endl;
		} 

		bsp_sync();

	}
	bsp_sync();
	/*
	cout<<endl<<"##############################################################################"<<endl;
	cout<<"Reduct has been found"<<endl<<"Reduct consists of the following attribute numbers "<<endl;

	for(int i = 0; i < reduct.size(); i++) cout<<reduct[i]<<"\t";
	*/

	tot_gamma = pos_count/obj_count;

	bsp_end();

	auto stop_time = high_resolution_clock::now();
	auto time_dur = duration_cast<milliseconds>(stop_time - start_time);
	
	cout<<endl<<"##############################################################################"<<endl;
	cout<<"Reduct has been found"<<endl<<"Reduct consists of the following attribute numbers "<<endl;

	cout<<"Time Taken : "<<time_dur.count()<<endl;

	for(int i = 0; i < reduct.size(); i++) cout<<reduct[i]<<"\t";
	cout<<endl<<"Final Gamma Value is : "<<tot_gamma;	
	cout<<endl;

	return(0);
	
}
