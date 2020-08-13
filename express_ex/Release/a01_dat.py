# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
##################################################

##################################################
a=np.fromfile("#1_A01__1.dat",np.double)
b=np.fromfile("out_0.dat",np.double)
##################################################
#delta_t=(1.0/2.0**17)
#N=len(a)
#x=np.arange(N,dtype=float)*(1.0/2.0**17)

plt.plot( a, 'o',       label='origin', markersize=4)
plt.plot( b, 'r', label='fitted')

plt.show()


