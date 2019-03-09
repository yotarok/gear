import sys
import numpy as np
import pylab as pl


data = np.loadtxt(sys.argv[1], delimiter=',')
data = data.T

pl.imshow(data, origin='lower', aspect='auto', interpolation='nearest')
pl.colorbar()
pl.show()


