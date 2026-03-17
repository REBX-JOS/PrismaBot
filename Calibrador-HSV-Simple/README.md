⚠️ ***ESTE README FUE ESCRITO CON IA.***
⚠️ ___THIS README WAS WRITTEN WITH AI.___
# Calibrador-HSV-Simple

[![Python](https://img.shields.io/badge/Python-3.x-blue?logo=python)](https://www.python.org/)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green?logo=opencv)](https://opencv.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## 🇪🇸 Español

### ¿Qué es este proyecto?

**Calibrador-HSV-Simple** es una herramienta de escritorio que facilita la selección interactiva de rangos de color en el espacio **HSV** (Hue / Matiz, Saturation / Saturación, Value / Valor) para su uso con **OpenCV**.

El programa captura video en tiempo real desde la cámara web y muestra, de forma simultánea:

- La imagen original capturada por la cámara.
- La imagen filtrada con la máscara generada a partir de los rangos HSV seleccionados.

Esto permite encontrar de manera rápida y visual los valores mínimos y máximos de HSV necesarios para segmentar un color específico en proyectos de visión por computadora.

### Requisitos previos

- Python 3.x
- pip

### Instalación

1. Clona el repositorio:

   ```bash
   git clone https://github.com/Isaias-Is/Calibrador-HSV-Simple.git
   cd Calibrador-HSV-Simple
   ```

2. Instala las dependencias:

   ```bash
   pip install opencv-python numpy pillow
   ```

> **Nota:** `tkinter` está incluido en la mayoría de las distribuciones estándar de Python. Si no está disponible, instálalo con el gestor de paquetes de tu sistema operativo (p. ej., `sudo apt install python3-tk` en distribuciones basadas en Debian/Ubuntu).

### Uso

1. Conecta una cámara web a tu equipo.
2. Ejecuta el programa:

   ```bash
   python main.py
   ```

3. Se abrirá una ventana con:
   - **Sliders** para ajustar los valores mínimos y máximos de **Hue (H)**, **Saturation (S)** y **Value (V)**.
   - El **panel izquierdo** muestra la imagen original de la cámara.
   - El **panel derecho** muestra la imagen filtrada con la máscara HSV aplicada.

4. Mueve los sliders hasta aislar el color que deseas segmentar. Los valores que aparecen en los sliders son los rangos HSV que puedes usar directamente en tu proyecto de OpenCV:

   ```python
   import cv2
   import numpy as np

   hsv_min = (H_min, S_min, V_min)
   hsv_max = (H_max, S_max, V_max)

   imagen_hsv = cv2.cvtColor(imagen_bgr, cv2.COLOR_BGR2HSV)
   mascara = cv2.inRange(imagen_hsv, hsv_min, hsv_max)
   ```

5. Cierra la ventana cuando hayas terminado la calibración.

### Estructura del proyecto

```
Calibrador-HSV-Simple/
├── main.py                 # Punto de entrada: inicializa la cámara y el calibrador
├── calibrador_tk.py        # Lógica del calibrador (captura de video y procesamiento)
├── iu_calibrador_tk.py     # Interfaz gráfica con Tkinter (ventana y sliders)
└── procesador.py           # Funciones de procesamiento de imagen (HSV, PIL, ecualización)
```

---

## 🇺🇸 English

### What is this project?

**Calibrador-HSV-Simple** is a desktop tool that makes it easy to interactively select color ranges in the **HSV** (Hue, Saturation, Value) color space for use with **OpenCV**.

The program captures real-time video from the webcam and simultaneously displays:

- The original image captured by the camera.
- The filtered image with the mask generated from the selected HSV ranges.

This allows you to quickly and visually find the minimum and maximum HSV values needed to segment a specific color in computer vision projects.

### Prerequisites

- Python 3.x
- pip

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/Isaias-Is/Calibrador-HSV-Simple.git
   cd Calibrador-HSV-Simple
   ```

2. Install the dependencies:

   ```bash
   pip install opencv-python numpy pillow
   ```

> **Note:** `tkinter` ships with most standard Python distributions. If it is not available, install it via your OS package manager (e.g., `sudo apt install python3-tk` on Debian/Ubuntu-based distros).

### Usage

1. Connect a webcam to your computer.
2. Run the program:

   ```bash
   python main.py
   ```

3. A window will open with:
   - **Sliders** to adjust the minimum and maximum values of **Hue (H)**, **Saturation (S)**, and **Value (V)**.
   - The **left panel** shows the original camera image.
   - The **right panel** shows the filtered image with the HSV mask applied.

4. Move the sliders until you isolate the color you want to segment. The values shown in the sliders are the HSV ranges you can use directly in your OpenCV project:

   ```python
   import cv2
   import numpy as np

   hsv_min = (H_min, S_min, V_min)
   hsv_max = (H_max, S_max, V_max)

   image_hsv = cv2.cvtColor(image_bgr, cv2.COLOR_BGR2HSV)
   mask = cv2.inRange(image_hsv, hsv_min, hsv_max)
   ```

5. Close the window when you have finished calibrating.

### Project structure

```
Calibrador-HSV-Simple/
├── main.py                 # Entry point: initializes the camera and the calibrator
├── calibrador_tk.py        # Calibrator logic (video capture and processing)
├── iu_calibrador_tk.py     # Tkinter GUI (window and sliders)
└── procesador.py           # Image processing utilities (HSV masking, PIL conversion, equalization)
```
