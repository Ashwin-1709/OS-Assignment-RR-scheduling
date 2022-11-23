import os
import numpy

matrix = numpy.genfromtxt('in2.txt')

transpose_matrix = numpy.array(matrix , dtype=int)
transpose_matrix = transpose_matrix.T

numpy.savetxt('transpose.txt' , transpose_matrix , fmt = "%d")
