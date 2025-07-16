import serial

def send_hex_via_serial(port='COM9', baudrate=38400, data_hex=None, expected_response=None):
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
        response = ser.read(54)  # lit jusqu'à 100 octets ou jusqu'au timeout
        print(f"Réponse : {response.hex()}")

        if expected_response is not None:
            if response.hex() == expected_response:
                print("La réponse est attendue.")
            else:
                print(f"La réponse est INATTENDUE : {response.hex()}")

if __name__ == "__main__":
    port = "COM16"  # Changez si votre port est différent
    baudrate = 115200
    data_hex = ("B5 62 06 8B 14 00 00 04 00 00 01 00 A4 40 03 00 A4 40 05 00 A4 40 0A 00 A4 40 4C 15".replace(" ", ""))
    expected_response = [
        0xB5, 0x62, 0x06, 0x8B, 0x24, 0x00, 0x01, 0x04,
        0x00, 0x00, 0x01, 0x00, 0xA4, 0x40, 0x00, 0xB0,
        0x71, 0x0B, 0x03, 0x00, 0xA4, 0x40, 0x00, 0xB0,
        0x71, 0x0B, 0x05, 0x00, 0xA4, 0x40, 0x00, 0xB0,
        0x71, 0x0B, 0x0A, 0x00, 0xA4, 0x40, 0x00, 0xD8,
        0xB8, 0x05, 0x76, 0x81, 0xB5, 0x62, 0x05, 0x01,
        0x02, 0x00, 0x06, 0x8B, 0x99, 0xC2
    ]

    send_hex_via_serial(port, baudrate, data_hex, expected_response)

