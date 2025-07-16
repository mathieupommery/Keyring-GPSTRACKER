import serial

def send_hex_via_serial(port='COM9', baudrate=38400, data_hex=None):
    if data_hex is None:
        print("Aucune donnée envoyée.")
        return

    # Conversion de l'hex en octets
    data_bytes = bytes.fromhex(data_hex)

    # Ouverture du port Série
    with serial.Serial(port, baudrate, timeout=1) as ser:
        print(f"Port {port} ouvert.")

        # Envoyer les données
        ser.write(data_bytes)
        print(f"Données envoyées : {data_hex}")

        # Lire la réponse
        response = ser.read(100)  # lit jusqu'à 100 octets ou jusqu'au timeout
        print(f"Réponse : {response.hex()}")

if __name__ == "__main__":
    port = "COM16"  # Changez si votre port est différent
    baudrate = 115200
    data_hex = ("B5 62 06 41 10 00 03 00 04 1F 54 5E 79 BF 28 EF 12 05 FD FF FF FF 8F 0D "
                 "B5 62 06 41 1C 00 04 01 A4 10 BD 34 F9 12 28 EF 12 05 05 00 A4 40 00 B0 71 0B 0A 00 A4 40 00 D8 B8 05 DE AE".replace(" ", ""))

    send_hex_via_serial(port, baudrate, data_hex)