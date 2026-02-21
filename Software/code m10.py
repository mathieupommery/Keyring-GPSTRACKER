import serial
import time

PORT = "COM20"
BAUDRATE = 115200
TIMEOUT = 1.0


# ---------------------------------------------------------
# Fonction utilitaire pour afficher proprement en hex
# ---------------------------------------------------------
def print_hex(prefix, data: bytes):
    print(prefix, data.hex(" ").upper())


# ---------------------------------------------------------
# Envoi Table 6 + impression de la réponse
# ---------------------------------------------------------
def send_table6_config(ser):
    table6 = bytes.fromhex(
        "B5 62 06 41 10 00 03 00 04 1F 54 5E 79 BF 28 EF 12 05 FD FF FF FF 8F 0D"
        " B5 62 06 41 1C 00 04 01 A4 10 BD 34 F9 12 28 EF 12 05 05 00 A4 40 00"
        " B0 71 0B 0A 00 A4 40 00 D8 B8 05 DE AE"
    )

    print_hex("\n>> Envoi config Table 6 :", table6)
    ser.write(table6)
    ser.flush()

    time.sleep(0.2)
    resp = ser.read(ser.in_waiting or 1)
    print_hex("<< Réponse reçue :", resp)


# ---------------------------------------------------------
# Envoi commande VALGET de vérification + impression
# ---------------------------------------------------------
def send_verification(ser):
    valget = bytes.fromhex(
        "B5 62 06 8B 14 00 00 04 00 00 01 00 A4 40 03 00 A4 40 05 00"
        " A4 40 0A 00 A4 40 4C 15"
    )

    print_hex("\n>> Envoi commande de vérification :", valget)
    ser.write(valget)
    ser.flush()
    
    time.sleep(0.2)
    resp = ser.read(ser.in_waiting or 1)
    print_hex("<< Réponse reçue :", resp)


# ---------------------------------------------------------
# Programme principal
# ---------------------------------------------------------
def main():
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    print(f"Port {PORT} ouvert @ {BAUDRATE} bauds.")

    #send_table6_config(ser)
    
    send_verification(ser)

    ser.close()
    print("\nFini.")


if __name__ == "__main__":
    main()
