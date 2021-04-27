import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import math

def gensig(fa, fb, vol, pan, t):
    fp=19000.0
    A0=0.5*vol*np.sin(2*math.pi*fa*t)
    B0=0.5*vol*np.sin(2*math.pi*fb*t)
    A=A0+pan*B0
    B=B0-pan*A0
    return (0.9*(((A+B)/2.0)+((A-B)/2.0)*np.sin(4.0*math.pi*fp*t))+0.1*np.sin(2.0*math.pi*fp*t))*75000.0

t0 = np.arange(0.0,0.0025,0.0000025)
plt.figure()
plt.plot(t0,gensig(400,800,1.0,0.2,t0))
plt.show()

