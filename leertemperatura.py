import matplotlib.pyplot as plt
import numpy as np
import subprocess
Y = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
plt.title('Resultado de Temperaturas Obtenidas')
plt.xlabel('Muestra N°')
plt.ylabel('Temperatura [°C]')
while(True):
    #contenido = subprocess.run(['cat', 'temperaturas.txt'], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    contenido = subprocess.run(['cat', '/dev/TP_FINAL'], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    X = np.asarray(contenido.split(','), dtype=float)
    plt.bar(Y, X)
    plt.draw()
    plt.pause(0.0001)
    plt.clf()
