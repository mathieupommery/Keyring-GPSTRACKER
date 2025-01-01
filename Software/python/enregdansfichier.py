from serial import Serial
import time
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import xml.etree.ElementTree as ET
from xml.dom import minidom
ser = Serial('COM10')
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

try:
    # Ouvre le fichier en mode ajout
    with open("ski1.txt", "a") as fichier:
        while True:
            if ser.in_waiting > 0:  # Vérifie s'il y a des données à lire
                ligne = ser.readline().decode('utf-8').strip()  # Lit une ligne et la décode
                fichier.write(ligne + "\n")
                data_array = ligne.split(',')
                if(len(data_array)==9):
                    string=data_array[8]
                    data_array = [float(data_array[i]) for i in range (len(data_array)-1)]
                    data_array.append(string)
                    array.append(data_array)
                else:
                    print("problem")
                if(ligne == "findetrame"):
                    print(ligne)
                    break
except KeyboardInterrupt:
    print("Arrêt par l'utilisateur")
finally:
    ser.close()

print("Port série fermé.")

for i in range (len(array)):
    speedarray.append(array[i][2])
    vbat.append(array[i][1])
    altitude.append(array[i][5])
    timearray.append(i)

fig, ax = plt.subplots()
x_data, y_data = [], []
line, = ax.plot([], [], lw=2)


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
# coordonnees=[]  
# 
# for i in range(len(array)):
#     coordonnees.append((array[i][3], array[i][4], array[i][5]))
# 
# # Créer un objet GPX
# gpx = gpxpy.gpx.GPX()
# 
# # Ajouter un tracé au fichier GPX
# track = gpxpy.gpx.GPXTrack()
# gpx.tracks.append(track)
# 
# # Ajouter un segment au tracé
# segment = gpxpy.gpx.GPXTrackSegment()
# track.segments.append(segment)
# 
# # Ajouter les points de tracé
# for lat, lon, ele in coordonnees:
#     point = gpxpy.gpx.GPXTrackPoint(latitude=lat, longitude=lon, elevation=ele)
#     segment.points.append(point)
# 
# # Écrire le fichier GPX
# with open("mon_tracé2.gpx", "w") as f:
#     f.write(gpx.to_xml())
# 
# print("Fichier GPX créé avec succès.")
# 
#




def create_gpx(data, output_filename="ski1.gpx"):
    # Créer l'élément racine GPX
    gpx = ET.Element("gpx", version="1.1", creator="PythonGPX")
    trk = ET.SubElement(gpx, "trk")
    trkseg = ET.SubElement(trk, "trkseg")

    # Itérer sur chaque entrée dans les données
    for entry in data:
        # Extraire les champs requis
        print(entry)
        timestamp = str(entry[8])
        print(timestamp)# 9e position : temps
        latitude = entry[3]       # 4e position : latitude
        longitude = entry[4]      # 5e position : longitude
        altitude = entry[5]       # 6e position : altitude

        # Créer un point de tracé (trkpt)
        trkpt = ET.SubElement(trkseg, "trkpt", lat=str(latitude), lon=str(longitude))
        ET.SubElement(trkpt, "ele").text = str(altitude)  # Élément pour l'altitude
        ET.SubElement(trkpt, "time").text = timestamp     # Élément pour le temps

    # Convertir en chaîne XML et indenter
    xml_str = ET.tostring(gpx, encoding="utf-8")
    parsed_xml = minidom.parseString(xml_str)
    pretty_xml_str = parsed_xml.toprettyxml(indent="  ")

    # Sauvegarder dans un fichier
    with open(output_filename, "w") as file:
        file.write(pretty_xml_str)

# Exemple d'utilisation avec des données fictives


create_gpx(array)
print("Fichier GPX généré : output.gpx")
