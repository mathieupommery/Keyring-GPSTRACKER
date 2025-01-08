import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
from collections import deque

# Configuration du port série
PORT = 'COM5'
BAUDRATE = 115200
TIMEOUT = 1  # Timeout en secondes

# Configuration du graphique
BUFFER_SIZE = 100  # Nombre de points à afficher
REFRESH_INTERVAL = 1000  # Intervalle de mise à jour en ms (1 seconde)

# Ouverture du port série
ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)

# Définir une file circulaire pour stocker les données
data_buffer = deque([0.0] * BUFFER_SIZE, maxlen=BUFFER_SIZE)

# Fonction pour lire les données du port série
def read_serial_data():
    values = []
    try:
        while ser.in_waiting > 0:  # Tant que des données sont disponibles
            line = ser.readline().decode('utf-8').strip()  # Lire et nettoyer la ligne
            try:
                value = float(line)  # Convertir en float
                if 0.0 <= value <= 3.3:  # Vérifier la plage des tensions
                    values.append(value)
            except ValueError:
                pass  # Ignorer les erreurs de conversion
        return values
    except Exception as e:
        print(f"Erreur lors de la lecture des données : {e}")
    return []

# Fonction pour mettre à jour les données du graphique
def update(frame):
    # Lire les 10 nouvelles valeurs
    new_values = read_serial_data()
    if new_values:
        data_buffer.extend(new_values)  # Ajouter les nouvelles valeurs au buffer

    # Mettre à jour le graphique
    ax.clear()  # Effacer le graphique précédent
    ax.plot(range(len(data_buffer)), data_buffer, label="Tension (V)", color="blue")
    ax.set_ylim(0, 3.3)  # Fixer les limites de l'axe Y
    ax.set_xlabel("Échantillons")
    ax.set_ylabel("Tension (V)")
    ax.set_title("Graphique de tension en temps réel (port COM5)")
    ax.legend()
    ax.grid()

# Configuration du graphique
fig, ax = plt.subplots()
ani = FuncAnimation(fig, update, interval=REFRESH_INTERVAL)

# Afficher le graphique
plt.show()

# Fermer le port série à la fin
ser.close()
