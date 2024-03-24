"""
Prototype of gradient magnitude computation
Halide course - LuxAI
Author: José Gabriel Pereira Tavares (booleangabs@github)

Setup:
    $ pip install opencv-python numpy

Package versions:
    numpy-1.26.4 opencv-python-4.9.0.80

OBS: Run from inside the '00_gradient' directory.
"""

# site-packages
import cv2
import numpy as np

# native
import time
import os


def gradient(img_input, threshold):
    img_gray = np.dot(np.int32(img_input), np.array([0.114, 0.587, 0.299]))

    kernel_1 = np.int32([[1, 0, -1]])
    kernel_2 = np.int32([[1, 2, 1]])

    kernel_vert = kernel_1.T @ kernel_2
    kernel_horiz = kernel_2.T @ kernel_1

    vert_gradient = cv2.filter2D(img_gray, -1, kernel_vert, borderType=cv2.BORDER_CONSTANT)
    horiz_gradient = cv2.filter2D(img_gray, -1, kernel_horiz, borderType=cv2.BORDER_CONSTANT)
    
    magnitude = vert_gradient**2 + horiz_gradient**2

    # Ao invés de normalizar os kernels -> multiplicar threshold por 4 (duas vezes)
    # Ao invés de tirar a raiz da magnitude -> elever threshold
    thresh = 4 * 4 * (threshold**2)
    return 255 * np.uint8(magnitude < thresh)


image_path = "../inputs/monarch.png"
image = cv2.imread(image_path, cv2.IMREAD_COLOR)

times = []
n_runs = 50
n_iterations = 1
for i in range(n_runs):
    t0 = time.perf_counter_ns()
    for _ in range(n_iterations):
        img_output = gradient(image, threshold=50)
    t1 = time.perf_counter_ns()
    times.append((t1 - t0) / 1e6)
mean = np.mean(times)
best = np.min(times)
worst = np.max(times)

print(f"Best time: {best} [ms]")
print(f"Mean time: {mean} [ms]")
print(f"Worst time: {worst} [ms]")

cv2.imshow(f"Mean time: {mean:.5f} [ms]", img_output)
cv2.waitKey(0)
cv2.destroyAllWindows()

os.system("mkdir -p outputs")
cv2.imwrite("outputs/python_result.png", img_output)