import serial
import gpxpy
import gpxpy.gpx
import matplotlib.pyplot as plt
from dateutil import parser as dateparser

# Modifier le port série ici
PORT = 'COM7'  # ex: '/dev/ttyUSB0' sous Linux
BAUDRATE = 921600

trames = []

# Connexion au port série
with serial.Serial(PORT, BAUDRATE, timeout=1) as ser:
    print("Réception des trames...")
    while True:
        ligne = ser.readline().decode('utf-8').strip()
        if not ligne:
            continue
        if ligne.lower() == "findetrame":
            print("Fin de trames détectée.")
            break
        trames.append(ligne)

# --- Création du GPX ---
gpx = gpxpy.gpx.GPX()
track = gpxpy.gpx.GPXTrack()
gpx.tracks.append(track)
segment = gpxpy.gpx.GPXTrackSegment()
track.segments.append(segment)

# Courbes
temps, temperatures, vitesses, alt_gps, alt_baro = [], [], [], [], []
i=0
for trame in trames:
    try:
        (temp, tension, vitesse, lat, lon, alt_gps_val, sats,
         h_acc, alt_baro_val, timestr) = trame.split(',')

        # Conversion
        lat = float(lat)
        lon = float(lon)
        alt_gps_val = float(alt_gps_val)
        alt_baro_val = float(alt_baro_val)
        vitesse = float(vitesse)
        temp = float(temp)
        dt = dateparser.parse(timestr)

        # GPX Point
        point = gpxpy.gpx.GPXTrackPoint(lat, lon, elevation=alt_gps_val, time=dt)
        segment.points.append(point)

        # Courbes
        temps.append(0.2*i)
        temperatures.append(temp)
        vitesses.append(vitesse)
        alt_gps.append(alt_gps_val)
        alt_baro.append(alt_baro_val)
        i=i+1

    except Exception as e:
        print(f"Erreur sur la trame : {trame} -> {e}")

# Sauvegarde GPX
with open("trace.gpx", "w") as f:
    f.write(gpx.to_xml())
print("Fichier GPX 'trace.gpx' généré.")

# --- Tracé des courbes ---
def tracer_courbe(x, y, titre, ylabel, nom_fichier):
    plt.figure()
    plt.plot(x, y, label=ylabel)
    plt.xlabel('Temps')
    plt.ylabel(ylabel)
    plt.title(titre)
    plt.grid(True)
    plt.legend()
    plt.savefig(nom_fichier)
    plt.close()
    print(f"Courbe '{nom_fichier}' générée.")

tracer_courbe(temps, temperatures, "Température dans le temps", "Température (°C)", "temperature.png")
tracer_courbe(temps, vitesses, "Vitesse dans le temps", "Vitesse (km/h)", "vitesse.png")
tracer_courbe(temps, alt_gps, "Altitude GPS dans le temps", "Altitude GPS (m)", "altitude_gps.png")
tracer_courbe(temps, alt_baro, "Altitude baro dans le temps", "Altitude baro (m)", "altitude_baro.png")
