#a program to find the lower approximation of a feature/ set of features
#mohana palaka
#2019

import pandas as pd
import numpy as np
import time

example = np.array([['U', 'Headache', 'Temp', 'Flu'],
					[1, 'Yes', 	   'Normal','No'],
					[2, 'Yes',       'High'  ,'Yes'],
					[3, 'Yes',       'VeryHigh','Yes'],
					[4, 'No',        'Normal',  'No'],
					[5, 'No',        'High',    'No'],
					[6, 'No',        'VeryHigh','Yes'],
					[7, 'No',        'High',    'Yes'],
					[8, 'No',        'VeryHigh','No']])

example_table = pd.DataFrame(data = example[1:, 1:],
							index = example[1: , 0],
							columns = example[0, 1:])

raw = np.array([['case','Temp'     ,'Headache' ,'weak' ,'nausea' ,'flu'],
                [  1    ,'veryhigh'  ,'yes'     ,'yes'  ,'no'     ,'yes'],
                [  2    ,'high'      ,'yes'     ,'no'   ,'yes'    ,'yes'],
                [  3    ,'normal'    ,'no'      ,'no'   ,'no'     ,'no'],
                [  4    ,'normal'    ,'yes'     ,'yes'  ,'yes'    ,'yes'],
                [  5    ,'high'      ,'no'      ,'yes'  ,'no'     ,'yes'],
                [  6    ,'high'      ,'no'      ,'no'   ,'no'     ,'no'],
                [  7    ,'normal'    ,'no'      ,'yes'  ,'no'     ,'no']])

my_table = pd.DataFrame(data = raw[1: , 1:],
					index = raw[1: , 0],
					columns = raw[0, 1:])


#loading breast cancer dataset...
breast_cancer = pd.read_csv("./datasets/breast-cancer.data")	


#loading connect 4 dataset
connect4 = pd.read_csv("./datasets/connect-4.data")
#print(connect4)

#loading balance scale dataset
balloons = pd.read_csv("./datasets/balance-scale.data")
#print(balloons)

#loading audiology datasets .... not good.. one of the features is unique
audio = pd.read_csv("./datasets/audiology.standardized.data", header = None)
#print(audio)

#mushroom dataset...
mushroom = pd.read_csv("./datasets/agaricus-lepiota.data")
print(mushroom)

def indiscernibility(attr, table):

	u_ind = {}	#an empty dictionary to store the elements of the indiscernibility relation (U/IND({set of attributes}))
	attr_values = []	#an empty list to tore the values of the attributes

	for i in (table.index):

		attr_values = []
		for j in (attr):

			attr_values.append(table.loc[i, j])	#find the value of the table at the corresponding row and the desired attribute and add it to the attr_values list

		#convert the list to a string and check if it is already a key value in the dictionary
		key = ''.join(str(k) for k in (attr_values))

		if(key in u_ind):	#if the key already exists in the dictionary
			u_ind[key].add(i)

		else:	#if the key does not exist in the dictionary yet
			u_ind[key] = set()
			u_ind[key].add(i)

	return list(u_ind.values())



def lower_approximation(R, X):	#We have to try to describe the knowledge in X with respect to the knowledge in R; both are LISTS OS SETS [{},{}]

	l_approx = set()	#change to [] if you want the result to be a list of sets

	#print("X : " + str(len(X)))
	#print("R : " + str(len(R)))

	for i in range(len(X)):
		for j in range(len(R)):

			if(R[j].issubset(X[i])):
				l_approx.update(R[j])	#change to .append() if you want the result to be a list of sets

	return l_approx



def gamma_measure(describing_attributes, attributes_to_be_described, U, table):	#a functuon that takes attributes/features R, X, and the universe of objects

	f_ind = indiscernibility(describing_attributes, table)
	t_ind = indiscernibility(attributes_to_be_described, table)

	f_lapprox = lower_approximation(f_ind, t_ind)

	return len(f_lapprox)/len(U)

	#return mod_l_approx(l_approx)/len(U)


def quick_reduct(C, D, table):	#C is the set of all conditional attributes; D is the set of decision attributes

	reduct = set()

	gamma_C = gamma_measure(C, D, table.index, table)
	print(gamma_C)
	gamma_R = 0

	while(gamma_R < gamma_C):

		T = reduct

		for x in (set(C) - reduct):

			feature = set()	#creating a new set to hold the currently selected feature
			feature.add(x)

			#print(feature)

			new_red = reduct.union(feature)	#directly unioning x separates the alphabets of the feature...

			gamma_new_red = gamma_measure(new_red, D, table.index, table)
			gamma_T = gamma_measure(T, D, table.index, table)

			if(gamma_new_red > gamma_T):

				T = reduct.union(feature)
				#print("added")

		reduct = T

		#finding the new gamma measure of the reduct

		gamma_R = gamma_measure(reduct, D, table.index, table)
		print(gamma_R)

	return reduct


t1 = time.time()

#final_reduct = quick_reduct(audio.columns[0:-1], [audio.columns[-1]], audio)
final_reduct = quick_reduct(mushroom.columns[1:], [mushroom.columns[0]], mushroom)

print("Serial took : ", str(time.time() - t1))
print(final_reduct)
'''
w_ind = indiscernibility(['weak'], my_table)
d_ind = indiscernibility(['flu'], my_table)

w_gamma = gamma_measure(['weak'], ['flu'], my_table.index)
print(w_gamma)
'''     
