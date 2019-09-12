#mohana palaka
#15mcme01
#a program that divides the quick reduct algorithm into segments that can be processed parallelly

from multiprocessing import Pool
import time
import pandas as pd
import numpy as np

raw = np.array([['case','Temp'     ,'Headache' ,'weak' ,'nausea' ,'flu'],
                [  1    ,'veryhigh'  ,'yes'     ,'yes'  ,'no'     ,'yes'],
                [  2    ,'high'      ,'yes'     ,'no'   ,'yes'    ,'yes'],
                [  3    ,'normal'    ,'no'      ,'no'   ,'no'     ,'no'],
                [  4    ,'normal'    ,'yes'     ,'yes'  ,'yes'    ,'yes'],
                [  5    ,'high'      ,'no'      ,'yes'  ,'no'     ,'yes'],
                [  6    ,'high'      ,'no'      ,'no'   ,'no'     ,'no'],
                [  7    ,'normal'    ,'no'      ,'yes'  ,'no'     ,'no']])
'''
table = pd.DataFrame(data = raw[1: , 1:],
					index = raw[1: , 0],
					columns = raw[0, 1:])
'''

#loading breast cancer dataset...
#table = pd.read_csv("./datasets/breast-cancer.data")	
#print(table.iloc[0:,0])

#loading connect 4 dataset...
#table = pd.read_csv("./datasets/connect-4.data")

#mushroom dataset
table = pd.read_csv("./datasets/agaricus-lepiota.data")


class ParallelQuickReduct:

	def __init__(self, table_name):
		self.table = table_name
		self.U = table_name.index
		self.C = table_name.columns[1:]
		self.D = [table_name.columns[0]]


	def indiscernibility(self, attr):

		u_ind = {}	#an empty dictionary to store the elements of the indiscernibility relation (U/IND({set of attributes}))
		attr_values = []	#an empty list to store the values of the attributes

		for i in (self.table.index):

			attr_values = []
			for j in (attr):

				attr_values.append(self.table.loc[i, j])	#find the value of the table at the corresponding row and the desired attribute and add it to the attr_values list

			#convert the list to a string and check if it is already a key value in the dictionary
			key = ''.join(str(k) for k in (attr_values))

			if(key in u_ind):	#if the key already exists in the dictionary
				u_ind[key].add(i)

			else:	#if the key does not exist in the dictionary yet
				u_ind[key] = set()
				u_ind[key].add(i)

		return list(u_ind.values())



	def lower_approximation(self, R, X):	#We have to try to describe the knowledge in X with respect to the knowledge in R; both are LISTS OS SETS [{},{}]

		l_approx = set()	#change to [] if you want the result to be a list of sets

		for i in range(len(X)):
			for j in range(len(R)):

				if(R[j].issubset(X[i])):
					l_approx.update(R[j])	#change to .append() if you want the result to be a list of sets

		return l_approx



	def parallel_gamma(self, describing_attributes):	#a functuon that takes attributes/features R, X, and the universe of objects

		f_ind = self.indiscernibility(describing_attributes)
		t_ind = self.indiscernibility(self.D)

		f_lapprox = self.lower_approximation(f_ind, t_ind)

		return len(f_lapprox)/len(self.U)

		#return mod_l_approx(l_approx)/len(U)


	def parallel_QR(self):

		C_gamma = self.parallel_gamma(self.C)	#the gamma measure of all the conditional attributes
		int_r_gamma = 0	#gamma measure of the intermediate reduct

		reduct = set()	#list to store the reduct
		
		p = Pool()	#pool of processes; initialized to number to cpu cores in system

		t1 = time.time()

		while(int_r_gamma < C_gamma):

			attr = list()

			#preparing the new attribute list to be passed into map function
			for i in (set(self.C) - reduct):

				a = set()

				if(len(reduct) >= 1):
					a.update(reduct)
				
				a.add(i)
				attr.append(a)
	 
			#using map function and parallelizing gamma computation
			res = p.map(self.parallel_gamma, attr)	#p.map for multiprocessing pool
			
			#print(res)
			max_gamma = max(res)
			max_index = res.index(max_gamma)

			reduct.update(attr[max_index])

			int_r_gamma = self.parallel_gamma(list(reduct))

		p.close()
		p.join()

		print("Parallel took : ", str(time.time() - t1))

		return(reduct)


def main():

	sample_table = ParallelQuickReduct(table)	#an object of the class
	red = sample_table.parallel_QR()
	print(red)


if __name__ == '__main__':

	main()
	