import tkinter as tk
import tkinter.ttk as ttk

# Creando una interfaz para el selector HSV.
ventana = tk.Tk()
ventana.title("Selector HSV")
ventana.geometry("1298x895")
ventana.resizable(False, False) # Bloquear la redimensión de la ventana en ambos ejes.
#ventana.configure(background="green")

# Configurando la apariencia básica y los marcos.
tk.Label(ventana, text="Calibrador para el Color", font="Arial 18", pady=15).grid(row=0, column=0, columnspan=2)
marco_arriba = tk.Frame(ventana, width=600, height=500, relief="ridge", background="red", padx=10, pady=5)
marco_arriba.grid(row=1, column=0)
marco_acciones = tk.Frame(marco_arriba, width=600, height=100, padx=5, pady=5)
marco_acciones.grid(row=5, column=0, columnspan=2)
marco_abajo = tk.Frame(ventana, width=600, height=600, relief="solid", background="blue", padx=5, pady=5)
marco_abajo.grid(row=2, column=0)

hsv_min_config = (0, 0, 0)
hsv_max_config = (180, 255, 255)

hsv_min = [tk.StringVar(), tk.StringVar(), tk.StringVar()]
hsv_max = [tk.StringVar(), tk.StringVar(), tk.StringVar()]

# Definir los controles deslizantes y etiquetas.
ttk.Label(marco_arriba, text="HUE Min", borderwidth=15, font="Arial 10 bold", padding=(10, 20)).grid(row=0, column=0)
h_min_scl = tk.Scale(marco_arriba, from_=hsv_min_config[0], to=hsv_max_config[0], variable=hsv_min[0], orient="horizontal", length=510).grid(row=0, column=1)
ttk.Label(marco_arriba, text="HUE Max", borderwidth=15, font="Arial 10 bold", padding=(10, 20)).grid(row=1, column=0)
h_max_scl = tk.Scale(marco_arriba, from_=hsv_min_config[0], to=hsv_max_config[0], variable=hsv_max[0], orient="horizontal", length=510).grid(row=1, column=1)
ttk.Label(marco_arriba, text="SAT Min", borderwidth=15, font="Arial 10 bold", padding=(10, 20)).grid(row=2, column=0)
s_min_scl = tk.Scale(marco_arriba, from_=hsv_min_config[1], to=hsv_max_config[1], variable=hsv_min[1], orient="horizontal", length=510).grid(row=2, column=1)
ttk.Label(marco_arriba, text="SAT Max", borderwidth=15, font="Arial 10 bold", padding=(10, 20)).grid(row=3, column=0)
s_max_scl = tk.Scale(marco_arriba, from_=hsv_min_config[1], to=hsv_max_config[1], variable=hsv_max[1], orient="horizontal", length=510).grid(row=3, column=1)
ttk.Label(marco_arriba, text="VAL Min", borderwidth=15, font="Arial 10 bold", padding=(10, 20)).grid(row=4, column=0)
v_min_scl = tk.Scale(marco_arriba, from_=hsv_min_config[2], to=hsv_max_config[2], variable=hsv_min[2], orient="horizontal", length=510).grid(row=4, column=1)
ttk.Label(marco_arriba, text="VAL Max", borderwidth=15, font="Arial 10 bold", padding=(10, 20)).grid(row=5, column=0)
v_max_scl = tk.Scale(marco_arriba, from_=hsv_min_config[2], to=hsv_max_config[2], variable=hsv_max[2], orient="horizontal", length=510).grid(row=5, column=1)

img_base_label = tk.Label(marco_abajo, text="Imagen base", background="gray")
img_base_label.grid(row=0, column=0, sticky="nesw")

img_procesada_label = tk.Label(marco_abajo, text="Imagen procesada", background="gray")
img_procesada_label.grid(row=0, column=1, sticky="nesw")

#ventana.mainloop() # Test.