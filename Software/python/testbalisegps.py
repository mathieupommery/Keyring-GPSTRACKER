from serial import Serial
import time
import matplotlib.pyplot as plt
import numpy as np
import gpxpy
import gpxpy.gpx
from matplotlib.animation import FuncAnimation

ser = Serial('COM17')
print("connected to: " + ser.portstr)
count=1
array=[]
valuenumber=0
speedarray=[]
vbat=[]
altitude=[]
timearray=[]
a=0
handler=0.0

while True:
    ligne = ser.readline().decode('utf-8').strip()
    data_array = ligne.split(',')
    
    if(len(data_array)==9):
        string=data_array[8]
        data_array = [float(data_array[i]) for i in range (len(data_array)-1)]
        data_array.append(string)
        print(data_array)
        array.append(data_array)
            
    else:
        print("problem")
    if(ligne == "findetrame"):
        print(ligne)
        break
    if not ligne:
        print("Aucune donnée reçue. Fin de la lecture.")
        break
    else:
        print(len(array))
ser.close()
print("Port série fermé.")


for i in range (len(array)):
    speedarray.append(array[i][2])
    vbat.append(array[i][1])
    altitude.append(array[i][7])
    timearray.append(i)
    if(i>0):
        speedarray[i]=(speedarray[i-1]+array[i][2])/2

fig, ax = plt.subplots()
x_data, y_data = [], []
line, = ax.plot([], [], lw=2)


fig, axs = plt.subplots(3, 1, figsize=(8, 12))

axs[0].plot(timearray, speedarray, color="blue")
axs[0].set_title("temps")
axs[0].set_ylabel("speed")

# Deuxième graphique
axs[1].plot(timearray, vbat, color="green")
axs[1].set_title("temps")
axs[1].set_ylabel("vbat")

# Troisième graphique
axs[2].plot(timearray, altitude, color="red")
axs[2].set_title("temps")
axs[2].set_ylabel("altitude")

plt.tight_layout()

plt.show()


coordonnees=[]  



for i in range(len(array)):
    coordonnees.append((array[i][3], array[i][4], array[i][5]))

# Créer un objet GPX
gpx = gpxpy.gpx.GPX()

# Ajouter un tracé au fichier GPX
track = gpxpy.gpx.GPXTrack()
gpx.tracks.append(track)

# Ajouter un segment au tracé
segment = gpxpy.gpx.GPXTrackSegment()
track.segments.append(segment)

# Ajouter les points de tracé
for lat, lon, ele in coordonnees:
    point = gpxpy.gpx.GPXTrackPoint(latitude=lat, longitude=lon, elevation=ele)
    segment.points.append(point)

# Écrire le fichier GPX
with open("train.gpx", "w") as f:
    f.write(gpx.to_xml())

print("Fichier GPX créé avec succès.")
 
# for i in range (len(array)):
#     speedarray.append(array[i][2]*3.6)
#     vbat.append(array[i][1])
#     altitude.append(array[i][5])
#     timearray.append(i)
# Configuration de la figure de Matplotlib
# fig, ax = plt.subplots()
# x_data, y_data = [], []
# line, = ax.plot([], [], lw=2)
# 
# # Limites initiales des axes
# ax.set_xlim(0, 5000)    # Mettre une limite initiale pour x
# ax.set_ylim(0, 200)    # Limites de y en fonction de la plage de données attendues
# ax.set_xlabel("Temps (s)")
# ax.set_ylabel("altitude")
# 
# # Fonction d'initialisation
# def init():
#     line.set_data([], [])
#     return line,
# 
# # Fonction pour mettre à jour les données du graphique
# def update(frame):
#     # Lire une ligne du port série
#     ligne = ser.readline().decode('utf-8').strip()
#     if ligne:
#         # Ajouter des données au graphique si on reçoit une valeur     
#         data_array = ligne.split(',')
#         if(len(data_array)==8):
#             data_array = [float(x) for x in data_array]
#             x_data.append(frame)
#             y_data.append(data_array[2])
#         else:
#             print("problem")
#     ax.set_xlim(0, x_data[-1] + 1)
#     line.set_data(x_data, y_data)
#         
#     return line,
# 
# # Animation en temps réel
# ani = FuncAnimation(fig, update, frames=np.arange(0, 5000), init_func=init, blit=True, interval=10)
# 
# # Afficher le graphique
# plt.show()
# 
# # Fermer le port série quand terminé
# ser.close()
# 
































# fig, axs = plt.subplots(3, 1, figsize=(8, 12))
# 
# axs[0].plot(timearray, speedarray, color="blue")
# axs[0].set_title("temps")
# axs[0].set_ylabel("speed")
# 
# # Deuxième graphique
# axs[1].plot(timearray, vbat, color="green")
# axs[1].set_title("temps")
# axs[1].set_ylabel("vbat")
# 
# # Troisième graphique
# axs[2].plot(timearray, altitude, color="red")
# axs[2].set_title("temps")
# axs[2].set_ylabel("altitude")
# 
# plt.tight_layout()
# 
# plt.show()




