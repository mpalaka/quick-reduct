//mohana palaka
//this is a program to read a .csv / .data file and write it into a 2d array
//particularly for iris dataset
//the conditional attributes are stored in a 2d array and the decision attribute(we are considering only one) is stored in a 1d array

#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<iterator>

using namespace std;
typedef map<string, vector<int> > dict; 

class CSVReader{

	//void readFile();
	
	public:
		string filename; //filename; takes in .csv; to be taken in from constructor?
		vector<vector<string> > cond_attr; //a 2d vector of vectors to hold the conditional attributes
		vector<string> dec_attr; // a 1d vector to hold the decision attribute

		void print_cond();
		void print_dec();

		CSVReader(string fname){ //constructor to get name of the file from user
	
			filename = fname;
			
			string line;
			ifstream myfile;
			myfile.open(filename.c_str());
			//vector<vector<string> > cond_attr;

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
			//...assuming that the decision attribute is the last column
			int rows = cond_attr.size();
			
			for(int i = 0; i < rows; ++i){
	
				dec_attr.push_back(cond_attr[i].back());
				cond_attr[i].pop_back();
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

//a function to slice the dataset from m to n and put into another matrix/2d vector
void splitData(vector<vector<string> > input, vector<vector<string> > &output, int m, int n){

	for(int i = 0; i < input.size(); i++){
		
		auto first = input[i].cbegin() + m;
		auto last = input[i].cbegin() + n + 1;

		vector<string> vec(first, last);
		output.push_back(vec);
		
	}

}


int main(){

	//loading data into vectors 
	CSVReader raw_data("mushroom.data");
	
	//number of conditional attributes
	int feat_count = raw_data.cond_attr[0].size();

	vector<int> attr_no;	//vector of ints to store the attribute numbers;not sure if this is necessary
	dict ind_rel;	//dictionary to form the indiscernibility relation

	vector<vector<string> > inp, out;

	vector<string> val;
	val.push_back("a");
	val.push_back("1");
	val.push_back("b");
	val.push_back("2");
	val.push_back("c");
	inp.push_back(val);
	val.clear();
	val.push_back("3");
	val.push_back("d");
	val.push_back("4");
	val.push_back("e");
	val.push_back("5");
	inp.push_back(val);
	val.clear();

	val.push_back("f");
	val.push_back("6");
	val.push_back("g");
	val.push_back("7");
	val.push_back("h");
	inp.push_back(val);
	val.clear();

	splitData(inp, out, 1, 3);

	for(int i = 0; i < out.size(); i++){
		for(int j = 0; j < out[i].size(); j++){
		
			cout<<out[i][j]<<"\t";
		}
		cout<<endl;
	}

	return(0);
}
