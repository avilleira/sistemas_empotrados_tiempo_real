import matplotlib.pyplot as plot
import pandas as pd

file = "cyclictestURJC.csv"
data_1 = pd.read_csv(file, sep=', ', decimal='.', engine='python')
"""data_2 = pd.read_excel(r'~/Documentos/curso_3/Empotrados/practica_2/src/cyclictestURJCrasp2.xlsx')
data_3 = pd.read_excel(r'/home/orivaj/Documentos/curso_3/Empotrados/practica_2/src/cyclictestURJCrasp3.xlsx')"""

lat_1 = pd.DataFrame(data_1, columns=['LATENCIA'])
"""lat_2 = pd.DataFrame(data_2, columns=['LATENCIA'])
lat_3 = pd.DataFrame(data_3, columns=['LATENCIA'])"""

plot.hist('LATENCIA', data = (lat_1)/1000, bins = 50, alpha=0.5, label='IDLE')
"""plot.hist('LATENCIA', data = (lat_2-1000000)/1000, bins = 500, alpha=0.5, label='hackbench')
plot.hist('LATENCIA', data = (lat_3-1000000)/1000, bins = 500, alpha=0.5, label='bonnie')"""


plot.xlabel('latencia (microsegundos)')
plot.ylabel('Frecuencia')
plot.title("RASP NO-RT")
plot.grid(True)
plot.xlim(0,100)

plot.legend()
plot.show() #dibujamos el histograma