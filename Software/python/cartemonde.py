from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import math

# === PARAMÈTRES ===
INPUT_IMAGE = "map2.jpg"  # Ton image source avec les frontières
OUTPUT_HEADER = "world_map.h"
OUTPUT_VAR_NAME = "world_map"
TARGET_WIDTH = 200
TARGET_HEIGHT = 200
THRESHOLD = 200  # seuil de binarisation

# === TRAITEMENT IMAGE ===
img = Image.open(INPUT_IMAGE).convert("L")  # niveaux de gris
img = img.resize((TARGET_WIDTH, TARGET_HEIGHT), Image.Resampling.LANCZOS)
img = img.point(lambda p: 255 if p > THRESHOLD else 0).convert("1")  # binaire
pixels = np.array(img)

# === CONVERSION EN BYTES ===
def image_to_bit_array(pixels):
    height, width = pixels.shape
    assert width % 8 == 0, "Largeur doit être multiple de 8"
    bit_array = []
    for y in range(height):
        for x in range(0, width, 8):
            byte = 0
            for b in range(8):
                bit = 0 if pixels[y, x + b] else 1  # noir = 1
                byte |= (bit << (7 - b))
            bit_array.append(byte)
    return bit_array

bit_array = image_to_bit_array(pixels)

# === GÉNÉRATION DU TABLEAU C ===
def generate_c_array(array, width, height, var_name):
    lines = []
    lines.append(f"// {width}x{height} carte du monde, 1 bit par pixel")
    lines.append(f"const uint8_t {var_name}[] = {{")
    for i in range(0, len(array), 12):
        chunk = array[i:i+12]
        hexes = ", ".join(f"0x{b:02X}" for b in chunk)
        lines.append(f"    {hexes},")
    lines.append("};")
    return "\n".join(lines)

c_code = generate_c_array(bit_array, TARGET_WIDTH, TARGET_HEIGHT, OUTPUT_VAR_NAME)

def lat_to_mercator_y(lat):
    """Convertit latitude en coordonnée y Mercator normalisée"""
    rad = math.radians(lat)
    return math.log(math.tan(math.pi / 4 + rad / 2))

def mercator_to_pixel(lat, lon, 
                      lat1, lon1, x1, y1, 
                      lat2, lon2, x2, y2):
    """
    Calibré à partir de 2 points GPS connus (lat1/lon1 à x1/y1, lat2/lon2 à x2/y2)
    Renvoie (x, y) en pixels correspondant à un (lat, lon) en Mercator.
    """
    # Mercator Y pour les deux points
    y_merc1 = lat_to_mercator_y(lat1)
    y_merc2 = lat_to_mercator_y(lat2)

    # Échelles
    scale_x = (x2 - x1) / (lon2 - lon1)
    scale_y = (y2 - y1) / (y_merc2 - y_merc1)

    # Mercator Y du point à projeter
    y_merc = lat_to_mercator_y(lat)

    # Projection
    x = x1 + (lon - lon1) * scale_x
    y = y1 + (y_merc - y_merc1) * scale_y

    return int(x), int(y)

# === ÉCRITURE DU .H ===
with open(OUTPUT_HEADER, "w") as f:
    f.write("#include <stdint.h>\n\n")
    f.write(c_code)

print(f"✅ Carte convertie et sauvegardée dans {OUTPUT_HEADER}")

# === AFFICHAGE DE L’IMAGE BINAIRE ===
lat_paris = 48.8566
lon_paris = 2.3522
lat_newyork=40.712784
lon_newyork=-74.005941
pointzerox=510
pointzeroy=453
parisx=511
parisy=305

x, y = mercator_to_pixel(
    lat=48.8566, lon=2.3522,
    lat1=0, lon1=0, x1=510, y1=453,
    lat2=lat_newyork, lon2=lon_newyork, x2=291, y2=335
)

x1, y1 = mercator_to_pixel(
    lat=55.757425, lon=37.619183,
    lat1=0, lon1=0, x1=510, y1=453,
    lat2=lat_newyork, lon2=lon_newyork, x2=291, y2=335
)
print(f"paris est à x={x}, y={y}")
print(f"moscou est à x={x1}, y={y1}")
plt.imshow(pixels, cmap='gray')
plt.title("Carte binaire (frontières)")
plt.axis('off')
plt.show()


