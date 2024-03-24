"""
Prototype of sepia effect
Halide course - LuxAI
Author: José Gabriel Pereira Tavares (booleangabs@github)

Setup:
    $ pip install opencv-python numpy

Package versions:
    numpy-1.26.4 opencv-python-4.9.0.80

OBS: Run from inside the '00_sepia_effect
' directory.
"""

# site-packages
import cv2
import numpy as np

# native
import time
import os


def sepia(img_input):
    img_input_f32 = img_input.astype("float32")[..., ::-1]
    nr = np.dot(img_input_f32, np.array([0.393, 0.796, 0.189]))
    ng = np.dot(img_input_f32, np.array([0.349, 0.686, 0.164]))
    nb = np.dot(img_input_f32, np.array([0.272, 0.534, 0.131]))
    
    return cv2.merge(
            [np.uint8(np.clip(c, 0, 255)) for c in [nb, ng, nr]]
        )


image_path = "../inputs/monarch.png"
image = cv2.imread(image_path, cv2.IMREAD_COLOR)

times = []
n_runs = 50
n_iterations = 1
for i in range(n_runs):
    t0 = time.perf_counter_ns()
    for _ in range(n_iterations):
        img_output = sepia(image)
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