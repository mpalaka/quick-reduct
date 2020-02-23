//mohana palaka
//quick reduct using bsp
//the conditional attributes are stored in a 2d array and the decision attribute(we are considering only one) is stored in a 1d array
//TODO:
//figure out how and where to loop to include multiple attributes
//maybe the struct gamma should have vectors of doubles and vectors of ints to hold combinations of attributes
#include "bsp.hpp"
#include<cstdlib>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<iterator>
#include<algorithm>

using namespace std;
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
			//...assuming that the decision attribute is the FIRST column
			int rows = cond_attr.size();
			
			for(int i = 0; i < rows; ++i){
	
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

//a structure to 
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

//function to find the gamma value 
double gamma_pq(dict ind_p, dict ind_q, int U){

	dict::iterator p_it, q_it;
	int pos_count = 0;
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
int main(){

	//loading data into vectors 
	CSVReader raw_data("mushroom.data");
	
	//number of conditional attributes
	int feat_count = raw_data.cond_attr[0].size();


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
	
	bsp_sync();
	//superstep to construct equivalence classes

	vector<int> attr_no;	//vector of ints to store the attribute numbers;not sure if this is necessary
	dict ind_c;	//dictionary to form the indiscernibility relation for conditional attributes
	dict ind_d; //dictionary to form the indeiscernibility relation for decision attribute


	//finding the indiscernibility relation induced by the decision attribute
	for(int i = 1; i < sub_table.size(); i++){
	
		string key = findClass(raw_data.dec_attr[i], raw_data.dec_attr[0]);
		eqClass(ind_d, key, i); //the value is the object index 
	}


	vector<double> loc_gammas; //a vector to store all the local gamma values

	//finding the indiscernibility relation induced by the conditional attributes	
	for(int j = 0; j < nsub_feat; j++){ //iterating over the number of attributes in the processor
		double lmax_g = 0;

		for(int i = 1; i < sub_table.size(); i++){
	
			string key = findClass(sub_table[i][j], sub_table[0][j]);
			eqClass(ind_c, key, i); //the value is the object index 
		}
		
		//find the gamma value of the current feature and place it in its corresponding position in the vector of local gammas
		double g = gamma_pq(ind_c, ind_d, sub_table.size());
		loc_gammas.push_back(g);
		
		ind_c.clear(); //clear the dictionary for the next feature
	}

	//finding the local best gamma value, and it's corresponding index/feature number
	double l_max = *max_element(loc_gammas.begin(), loc_gammas.end());
	int loc_lmax = find(l_max, loc_gammas); //local index
	
	//putting into the struct gamma
	struct gamma lb_gamma;
	lb_gamma.attr_no = (bsp_pid() * def_nfeats) + loc_lmax; //global index
	lb_gamma.gamma_val = l_max;

	/*
	if(bsp_pid() == 1){
	
		for(int i = 0; i < loc_gammas.size(); i++){
		
			cout<<loc_gammas[i]<<"\t";

		}
		cout<<endl;
		cout<<"local max of "<<bsp_pid()<<"is in "<<loc_lmax<<endl;
		cout<<endl<<"lb_gamma attr : "<<lb_gamma.attr_no;		
		cout<<endl<<"lb_gamma val : "<<lb_gamma.gamma_val;

	}
	*/
	
	bsp_sync();
	//create an array/vector of gamma structs to store all the local gammas
	//also register into drma

	struct gamma all_gammas[4]; //4 processors

	for(int i = 0; i < 4; i++){
	
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

	if(bsp_pid() == 0){
	
		for(int i = 0; i < 4; i++){
		
			cout<<"attr no : "<<all_gammas[i].attr_no<<endl;
			cout<<"gamma_val : "<<all_gammas[i].gamma_val<<endl;
		}
	
	}
	
	struct gamma g_max;
	g_max.gamma_val = all_gammas[0].gamma_val;
	g_max.attr_no = all_gammas[0].attr_no;

	for(int i = 1; i < 4; i++){
	
		if(g_max.gamma_val < all_gammas[i].gamma_val){
		
			g_max.gamma_val = all_gammas[i].gamma_val;
			g_max.attr_no = all_gammas[i].attr_no;
		}
	}

	cout<<"max gamma val : "<<g_max.gamma_val<<endl;
	cout<<"max attr no : "<<g_max.attr_no<<endl;
	

	return(0);
}
