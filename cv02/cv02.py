# -*- code: utf-8 -*-

#http://mrl.cs.vsb.cz/people/fabian/vd/cv02.py

from numpy import fft
import math
import numpy as np 
import matplotlib.pyplot as plt

import csv

def column(matrix, i):
    return [float(row[i]) for row in matrix]

with open('data.txt', 'r') as csvfile:
  spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
  x = list(spamreader)

# exp(-j*2*math.pi*m*n) basis function
# exp(j*x) = cos(x) + j*sin(x) Euler's formula

def basis(m, n): # m-th basis for forward transform
  phi = 2*math.pi*m*n/N 
  return math.cos(phi) - 1j*math.sin(phi)
  
def ibasis(m, n): # m-th basis for inverse transform
  phi = 2*math.pi*m*n/N 
  return math.cos(phi) + 1j*math.sin(phi)
  
def F(m, f): # forward transform
  tmp = 0
  for n in range(N):
    tmp += f[n]*basis(m, n)     
  return tmp/N
  
def f(n, F): # inverse transform
  tmp = 0
  for m in range(N):
    tmp += F[m]*ibasis(m, n)     
  return tmp
  
tau = 1.0 # time (s)
  
def p(F): # power spectrum (Hz, power)
  return [(m / (tau*N) * N, F[m].real**2 + F[m].imag**2) for m in range(int(N/2))]

def print2fl(lst):
  print(lst)

def print2fc(lst):  
  print(["{0:.2f}{1}{2:.2f}j".format(x.real, "+" if x.imag>=0 else "", x.imag) for x in lst])  

def print2f(lst):    
  print([float("{0:.2f}".format(x)) for x in lst])

N = 10
samples = column(x, 3)[:3650]
# samples = [math.sin(2*math.pi*x/N) for x in range(N)] # sampled data
# samples = [1 for x in range(10)] # sampled data

N = len(samples) # number of samples
D = range(N)
y = [2*x for x in D]

print("Samples")
print2f(samples)
fsamples = [F(m, samples) for m in range(N)] # samples in frequency domain
print("\nSamples in frequency domain")
print2fc(fsamples)
rsamples = [f(n, fsamples) for n in range(N)] # reconstructed samples
print("\nReconstructed samples back in spatial domain")
print2fc(rsamples)
print("\nPower spectrum")
print2fl(p(fsamples))

"""
y = [basis(1, x) for x in D]

plt.plot(D, y, 'ro')
plt.show()
"""