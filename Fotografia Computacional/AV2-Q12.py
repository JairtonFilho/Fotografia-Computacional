import cv2
import numpy as np
import matplotlib.pyplot as plt

# Carregar a imagem
image_path = "moon.jpg"
image = cv2.imread(image_path)

# Somar os 3 canais da imagem original
summed_channels = np.sum(image, axis=-1)

# Normalizar o resultado
normalized_result = (summed_channels - summed_channels.min()) / (summed_channels.max() - summed_channels.min())

# Aplicar a aproximação
normalized_result = np.where(normalized_result < 0.25, 0, normalized_result)
normalized_result = np.where((normalized_result >= 0.25) & (normalized_result < 0.5), 0.25, normalized_result)
normalized_result = np.where((normalized_result >= 0.5) & (normalized_result < 0.75), 0.5, normalized_result)
normalized_result = np.where(normalized_result >= 0.75, 1, normalized_result)

# Exibir a imagem resultante
plt.imshow(normalized_result, cmap='gray')
plt.title('Imagem Segmentada')
plt.show()

# Calcular a média dos pixels sobre toda a imagem
average_value = np.mean(normalized_result)
print(f"A média dos pixels sobre toda a imagem é aproximadamente: {average_value:.4f}")