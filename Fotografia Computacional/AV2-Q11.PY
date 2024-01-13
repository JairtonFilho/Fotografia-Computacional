
import cv2
import numpy as np

# Carregar a imagem
image_path = "bald_eagle_portrait.jpg"
image = cv2.imread(image_path)

# Converter a imagem para RGB (OpenCV lê a imagem em formato BGR)
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# Definir a cor amarela
yellow_color = np.array([255, 255, 0])

# Definir a distância de limiar
threshold_distance = np.sqrt(np.sum((np.array([0, 0, 0]) - np.array([100, 100, 100]))**2))

# Calcular as distâncias euclidianas para cada pixel em relação à cor amarela
distances = np.sqrt(np.sum((image_rgb - yellow_color)**2, axis=-1))

# Segmentar pixels dentro da região de limiar
thresholded_mask = distances <= threshold_distance

# Calcular a média das distâncias para os pixels na região de limiar
average_distance = np.mean(distances[thresholded_mask])

print(f"A média dos valores de distância dos pixels para a cor (255, 255, 0) é aproximadamente: {average_distance:.2f}")