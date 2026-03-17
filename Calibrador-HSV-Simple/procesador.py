import cv2
import numpy as np
from PIL import ImageTk, Image


# Implementando una función propia para seleccionar los valores HSV.
def selectorHSV(imagen_BGR, h1, s1, v1, h2, s2, v2) -> np.ndarray:
    imagen_HSV = cv2.cvtColor(imagen_BGR, cv2.COLOR_BGR2HSV)
    return cv2.inRange(imagen_HSV, (h1, s1, v1), (h2, s2, v2))

# Función que transforma una imagen de OpenCV a PIL.
def arreglo_a_PIL(imagen_cv:np.ndarray):
    # Convertir la imagen de BGR a RGB y luego a PIL.
    return ImageTk.PhotoImage(Image.fromarray(cv2.cvtColor(imagen_cv, cv2.COLOR_BGR2RGB)))

# Procesamiento base.
def procesamiento_base(imagen_cv:np.ndarray) -> np.ndarray:
    imagen_grises = cv2.cvtColor(imagen_cv, cv2.COLOR_BGR2GRAY)
    imagen_procesada = cv2.equalizeHist(imagen_grises)
    return imagen_procesada