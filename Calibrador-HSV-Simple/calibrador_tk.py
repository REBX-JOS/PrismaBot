import datetime
import cv2
# My imports.
import iu_calibrador_tk as iu
from procesador import arreglo_a_PIL, procesamiento_base, selectorHSV

class Calibrador:
    def __init__(self, vid):
        self.vid = vid
        iu.ventana.protocol("WM_DELETE_WINDOW", self.terminar_ventana)
        iu.ventana.after(15, self.actualizar_imagen)

    def iniciar(self):
        iu.ventana.mainloop()

    def actualizar_imagen(self):
        capturado, imagen = self.vid.read()
        if not capturado:
            print("ERROR: No se pudo capturar el cuadro de video.")
            self.cerrar_ventana()
            return
        mascara_imagen = selectorHSV(imagen, int(iu.hsv_min[0].get()), int(iu.hsv_min[1].get()), int(iu.hsv_min[2].get()),
                                       int(iu.hsv_max[0].get()), int(iu.hsv_max[1].get()), int(iu.hsv_max[2].get()))
        imagen_procesada = procesamiento_base(imagen)
        imagen_procesada = cv2.bitwise_or(imagen, imagen, mask=mascara_imagen)
        imagen_procesada_tk = arreglo_a_PIL(imagen_procesada) # Convertir imagen a PIL.
        imagen_tk = arreglo_a_PIL(imagen)  # Convertir imagen a PIL.
        #cv2.imshow('Video', imagen)
        iu.img_base_label.anticolector = imagen_tk  # Evitar que la imagen sea recolectada por el recolector de basura de Python.
        iu.img_base_label.config(image=imagen_tk)
        iu.img_procesada_label.anticolector = imagen_procesada_tk  # Evitar que la imagen sea recolectada por el recolector de basura de Python.
        iu.img_procesada_label.config(image=imagen_procesada_tk)

        iu.ventana.after(15, self.actualizar_imagen)

    def cerrar_ventana(self):
        iu.ventana.quit()

    def terminar_ventana(self):
        iu.ventana.destroy()

#cal = Calibrador()
#cal.iniciar()
