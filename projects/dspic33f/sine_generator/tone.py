from numpy import *
from matplotlib.pyplot import *

nsamples = 256
amp = (1<<15) - 1

t = arange(0, nsamples)
x = ceil(amp*sin(2*pi/nsamples * t))

plot(t,x)
savetxt('tone.csv', x, newline=',', fmt="%d")
