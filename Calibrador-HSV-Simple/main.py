import cv2
import calibrador_tk

def main():
    vid = cv2.VideoCapture(0)

    if not vid.isOpened():
        print("ERROR:La camara no se pudo abrir")
        return -1

    # Calibrar visión.
    calibrador = calibrador_tk.Calibrador(vid)
    calibrador.iniciar()
    print("Calibración terminada.")
    calibrador.terminar_ventana()

if __name__ == "__main__":
    main()