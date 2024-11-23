import psutil
import serial
import time
from pynvml import nvmlInit, nvmlDeviceGetHandleByIndex, nvmlDeviceGetTemperature, nvmlShutdown, NVML_TEMPERATURE_GPU, nvmlDeviceGetMemoryInfo, nvmlDeviceGetPowerUsage


def get_gpu_temperature():
    """Récupère la température du GPU NVIDIA si disponible."""
    try:
        nvmlInit()
        handle = nvmlDeviceGetHandleByIndex(0)  # 0 pour le premier GPU
        temp = nvmlDeviceGetTemperature(handle, NVML_TEMPERATURE_GPU)
        power = int(nvmlDeviceGetPowerUsage(handle)/1000)
        nvmlShutdown()
        return temp, power
    except Exception as e:
        print(f"Erreur lors de la récupération de la température GPU : {e}")
        return None

def send_via_serial(port, baudrate, gpu_temp,gpu_power):
    """Envoie les températures via un port série."""
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            message = f"{gpu_temp}${gpu_power}$\n"
            ser.write(message.encode())
    except Exception as e:
        print(f"Erreur lors de l'envoi via le port série : {e}")
        
        

if __name__ == "__main__":
    # Configuration du port COM
    serial_port = "COM12"  # Remplacez par le port COM approprié
    baud_rate = 9600

    # Boucle principale
    while True:
        # Récupérer les températures
        gpu_temp, gpu_power = get_gpu_temperature()
        print(f"{gpu_temp}${gpu_power}$\n")

        # Transmettre via le port série
        send_via_serial(serial_port, baud_rate, gpu_temp,gpu_power)

        # Pause avant la prochaine lecture
        time.sleep(0.5)
