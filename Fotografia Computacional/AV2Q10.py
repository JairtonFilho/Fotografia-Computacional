import cv2
import numpy as np
import matplotlib.pyplot as plt

# Carregando a imagem
image_path = "baboon.jpg"
image = cv2.imread(image_path)

# Convertendo de BGR para RGB (OpenCV lê a imagem em formato BGR)
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# Decomposição nos canais RGB
r_channel, g_channel, b_channel = cv2.split(image_rgb)

# Decomposição nos canais CMY
c_channel = 255 - r_channel
m_channel = 255 - g_channel
y_channel = 255 - b_channel

# Calculando as intensidades médias
mean_intensity_r = np.mean(r_channel)
mean_intensity_g = np.mean(g_channel)
mean_intensity_b = np.mean(b_channel)

mean_intensity_c = np.mean(c_channel)
mean_intensity_m = np.mean(m_channel)
mean_intensity_y = np.mean(y_channel)

# Exibindo as imagens originais e decompostas
plt.figure(figsize=(12, 6))

plt.subplot(2, 4, 1)
plt.imshow(image_rgb)
plt.title('Original')

plt.subplot(2, 4, 2)
plt.imshow(r_channel, cmap='gray')
plt.title('R Channel')

plt.subplot(2, 4, 3)
plt.imshow(g_channel, cmap='gray')
plt.title('G Channel')

plt.subplot(2, 4, 4)
plt.imshow(b_channel, cmap='gray')
plt.title('B Channel')

plt.subplot(2, 4, 6)
plt.imshow(c_channel, cmap='gray')
plt.title('C Channel')

plt.subplot(2, 4, 7)
plt.imshow(m_channel, cmap='gray')
plt.title('M Channel')

plt.subplot(2, 4, 8)
plt.imshow(y_channel, cmap='gray')
plt.title('Y Channel')

plt.tight_layout()
plt.show()

# Exibindo as intensidades médias
print(f"Intensidade média do canal R: {mean_intensity_r:.2f}")
print(f"Intensidade média do canal G: {mean_intensity_g:.2f}")
print(f"Intensidade média do canal B: {mean_intensity_b:.2f}")
print(f"Intensidade média do canal C: {mean_intensity_c:.2f}")
print(f"Intensidade média do canal M: {mean_intensity_m:.2f}")
print(f"Intensidade média do canal Y: {mean_intensity_y:.2f}")