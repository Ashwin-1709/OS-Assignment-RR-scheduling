import numpy
import sys

matrix = numpy.genfromtxt(sys.argv[1])

transpose_matrix = numpy.array(matrix , dtype=int)
transpose_matrix = transpose_matrix.T

numpy.savetxt('transpose.txt' , transpose_matrix , fmt = "%d")
