from serial import Serial
import time
import matplotlib.pyplot as plt
import numpy as np
import gpxpy
import gpxpy.gpx

ser = Serial('COM19')
print("connected to: " + ser.portstr)
count=1
array=[]
valuenumber=0
speedarray=[]
timearray=[]
a=0
handler=0.0
nombrepage=59
for i in range (nombrepage*4):
    ##s = ser.read(256)
    line = ser.readline()
    valuenumber+=1
    if len(line)<90:
        #print("La ligne contient une fin de ligne")
        line = line.decode('utf-8').strip()
        data_array = line.split(',')
        data_array = [float(x) for x in data_array]
        array.append(data_array)
        print(array)
ser.close()

#for i in range (len(array)):
#    speedarray.append(array[i][2]*3.6)
#   timearray.append(i)

#plt.plot(timearray, speedarray)

#plt.show()
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
with open("mon_tracé.gpx", "w") as f:
    f.write(gpx.to_xml())

print("Fichier GPX créé avec succès.")
 




