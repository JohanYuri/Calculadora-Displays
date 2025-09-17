import tkinter as tk
import requests

# --- CONFIGURACIÓN ---

# --- Configuración del Servidor ---
ESP32_IP = "http://192.168.100.83"

# --- LÓGICA DE COMUNICACIÓN HTTP ---
def send_request(endpoint, params=None):
    """Envía una petición GET al endpoint especificado en el ESP32."""
    try:
        url = f"{ESP32_IP}/{endpoint}"
        response = requests.get(url, params=params, timeout=3)
        if response.status_code == 200:
            print(f"Éxito: {response.text}")
            update_status(f"Comando '{endpoint}' recibido.")
        else:
            print(f"Error del servidor: {response.status_code} - {response.text}")
            update_status(f"Error {response.status_code} del ESP32.")
    except requests.exceptions.RequestException as e:
        print(f"Error de conexión: {e}")
        update_status("Error de conexión con ESP32.")

# --- FUNCIONES DE LOS BOTONES DE CONTROL (ahora usan send_request) ---
def start_up():
    send_request('up')

def start_down():
    send_request('down')

def stop_count():
    response_text = send_request('stop')
    if response_text and response_text.isdigit():
        number = int(response_text)
        global current_display_number
        current_display_number = number
        calc_entry.delete(0, tk.END)
        calc_entry.insert(0, str(number))
        update_status(f"Conteo detenido en {number}.")

def reset_display():
    global current_display_number 
    send_request('reset')
    calc_entry.delete(0, tk.END)
    current_display_number = 0

# --- LÓGICA DE LA CALCULADORA (ahora usa send_request) ---
def calculate():
    global current_display_number 
    try:
        expression = calc_entry.get()
        if not expression: return
        result = int(eval(expression))
        
        if 0 <= result <= 99:
            # Pasa el resultado como un parámetro en la URL
            # ej: http://192.168.1.123/calculate?value=42
            send_request('calculate', params={'value': result})
            current_display_number = result
            calc_entry.delete(0, tk.END)
        else:
            update_status("Error: Resultado fuera de rango (0-99)")
            calc_entry.delete(0, tk.END)
            calc_entry.insert(0, "Error Rango")
    except Exception as e:
        update_status(f"Error en la expresión.")
        calc_entry.delete(0, tk.END)
        calc_entry.insert(0, "Error Sintaxis")

def clear_entry():
    calc_entry.delete(0, tk.END)
    update_status("Entrada borrada")

def update_status(message):
    status_label.config(text=f"Estado: {message}")

# --- DISEÑO DE LA INTERFAZ GRÁFICA (GUI) ---
# Esta sección es idéntica a la versión anterior. No necesita cambios.
root = tk.Tk()
root.title("Controlador de Display")
root.configure(bg="#F0F0F0")

# --- Definición de Estilos (TAMAÑOS REDUCIDOS) ---
fuente_grande = tkFont.Font(family="Helvetica", size=14, weight="bold")      # AJUSTE: Reducido de 16 a 14
fuente_display = tkFont.Font(family="Helvetica", size=20, weight="bold")     # AJUSTE: Reducido de 24 a 20
fuente_status = tkFont.Font(family="Helvetica", size=11)                       # AJUSTE: Reducido de 12 a 11

# Paleta de colores
color_fondo = "#F0F0F0"
color_botones_num = "#FFFFFF"
color_botones_op = "#DDEEFF"
color_botones_ctrl = "#D0F0C0"
color_texto = "#000000"

# --- Contenedor Principal ---
main_frame = tk.Frame(root, padx=15, pady=15, bg=color_fondo)
main_frame.pack(fill=tk.BOTH, expand=True)

# --- Frame para la Calculadora ---
calc_frame = tk.LabelFrame(main_frame, text="Calculadora", font=fuente_grande, padx=10, pady=10, bg=color_fondo, relief=tk.GROOVE)
calc_frame.pack(pady=10, fill=tk.X)

# Display de la calculadora
calc_entry = tk.Entry(calc_frame, font=fuente_display, justify='right', relief=tk.FLAT, bg="#E0E0E0", bd=8)
calc_entry.grid(row=0, column=0, columnspan=4, pady=8, ipady=8, sticky="ew")

# Botones de la calculadora
buttons = [
    ('7', color_botones_num), ('8', color_botones_num), ('9', color_botones_num), ('/', color_botones_op),
    ('4', color_botones_num), ('5', color_botones_num), ('6', color_botones_num), ('*', color_botones_op),
    ('1', color_botones_num), ('2', color_botones_num), ('3', color_botones_num), ('-', color_botones_op),
    ('C', color_botones_op), ('0', color_botones_num), ('=', color_botones_op), ('+', color_botones_op)
]

row_val, col_val = 1, 0
for button in buttons:
    if button == '=':
        btn = tk.Button(calc_frame, text=button, width=5, height=2, command=calculate)
    else:
        action = lambda b=text: on_button_click(b)
    
    btn = tk.Button(calc_frame, text=text, font=fuente_grande, command=action,
                    bg=color, fg=color_texto, relief=tk.RAISED, bd=3,
                    height=1, width=4) # AJUSTE: Altura (height) reducida de 2 a 1
    btn.grid(row=row_val, column=col_val, padx=5, pady=5, sticky="nsew")
    col_val += 1
    if col_val > 3:
        col_val = 0
        row_val += 1

for i in range(4):
    calc_frame.grid_columnconfigure(i, weight=1)

# --- Frame para los Controles de Conteo ---
control_frame = tk.LabelFrame(main_frame, text="Controles de Conteo", font=fuente_grande, padx=10, pady=10, bg=color_fondo, relief=tk.GROOVE)
control_frame.pack(pady=15, fill=tk.X, expand=True)

control_buttons = [
    ("🔼 Subir", start_up),
    ("🔽 Bajar", start_down),
    ("⏹️ Parar", stop_count),
    ("🔄 Reiniciar", reset_display)
]

for text, command in control_buttons:
    btn = tk.Button(control_frame, text=text, font=fuente_grande, command=command,
                    bg=color_botones_ctrl, fg=color_texto, relief=tk.RAISED, bd=3,
                    pady=8) # AJUSTE: Espaciado vertical (pady) reducido de 15 a 8
    btn.pack(fill=tk.X, expand=True, padx=5, pady=6)

# --- Etiqueta de Estado ---
status_label = tk.Label(main_frame, text="Bienvenido", font=fuente_status,
                        relief=tk.SUNKEN, anchor='w', padx=10, pady=5,
                        bg="#FFFFCC", fg=color_texto)
status_label.pack(side=tk.BOTTOM, fill=tk.X, pady=(10, 0))

root.mainloop()