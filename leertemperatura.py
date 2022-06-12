import string
import matplotlib.pyplot as plt
import numpy as np
import subprocess
Y = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
while(True):

    contenido = subprocess.run(['cat', 'temperaturas.txt'], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    X = np.asarray(contenido.split(','))
    substring = "T"
    if substring in X[0]:
        X[0] = X[0].replace("T", "")
        X = X.astype(float)
        plt.title('Temperatura de clima en 10 días')
        plt.xlabel('Muestra N°')
        plt.ylabel('Temperatura [°C]')
        plt.bar(Y, X)
        plt.draw()
        plt.pause(0.0001)
        plt.clf()

    else:
        X[0] = X[0].replace("C", "")
        X = X.astype(float)
        plt.title('Caudal de un río en 10 días')
        plt.xlabel('Muestra N°')
        plt.ylabel('Caudal [m3/s]')
        plt.bar(Y, X)
        plt.draw()
        plt.pause(0.0001)
        plt.clf()



