# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
##################################################
N=2**21
##################################################
a=np.fromfile("#1_A01__1.dat", np.double,N)
a=np.convolve(a,[-1,1,0],"same")*a
#a=np.fromfile("#1_A01__1.dat", np.double,N)
b=np.fromfile("out_0.dat",np.double,N)
##################################################
NN=min([len(a),len(b)])
b=a[0:NN]-b[0:NN]

##################################################
#delta_t=(1.0/2.0**17)
#N=len(a)
#x=np.arange(N,dtype=float)*(1.0/2.0**17)
size=int(524288)
N=range(int(len(a)/size)+1)
x=[i*size for i in N]
y=[1 for i in N]
#plt.plot( a, 'o', label='origin', markersize=4)
plt.plot( b, 'o', label='fitted')
plt.plot( x,y, 'o', label='fitted')
plt.show()


