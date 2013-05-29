#!/usr/bin/python
"""
Plot data from datalogger.
Takes name of csv file to plot as argument.
"""
from numpy import *
from pylab import *
from matplotlib import *
import sys

data = csv2rec(sys.argv[1], delimiter=',')

pyplot.figure("Temperature")
plot(numpy.arange(len(data))+1, data[('temperature')])
title('Temperature')
ylabel('Temperature')
xlabel('Time')
#show()

pyplot.figure("Humidity")
plot(numpy.arange(len(data))+1, data[('humidity')])
title('Humidity')
ylabel('Humidity')
xlabel('Time')
#show()

pyplot.figure("X-Axis Acceleration")
plot(numpy.arange(len(data))+1, data[('accx')])
title('x-axis acceleration')
ylabel('Acceleration')
xlabel('Time')
#show()

pyplot.figure("Y-Axis Acceleration")
plot(numpy.arange(len(data))+1, data[('accy')])
title('y-axis acceleration')
ylabel('Acceleration')
xlabel('Time')
#show()

pyplot.figure("Z-Axis Acceleration")
plot(numpy.arange(len(data))+1, data[('accz')])
title('z-axis acceleration')
ylabel('Acceleration')
xlabel('Time')
show()
