import tkinter as tk
import requests

# --- CONFIGURACIÓN ---

ESP32_IP = "192.168.100.83"

# --- LÓGICA DE COMUNICACIÓN HTTP ---
def send_request(endpoint, params=None):
    """Envía una petición GET al endpoint especificado en el ESP32."""
    try:
        url = f"{ESP32_IP}/{endpoint}"
        response = requests.get(url, params=params, timeout=3)
        if response.status_code == 200:
            print(f"Éxito: {response.text}")
            update_status(f"Comando '{endpoint}' enviado correctamente.")
        else:
            print(f"Error del servidor: {response.status_code} - {response.text}")
            update_status(f"Error {response.status_code} desde ESP32.")
    except requests.exceptions.RequestException as e:
        print(f"Error de conexión: {e}")
        update_status("Error: No se pudo conectar al ESP32.")

# --- FUNCIONES DE LOS BOTONES DE CONTROL (ahora usan send_request) ---
def start_up():
    send_request('up')

def start_down():
    send_request('down')

def stop_count():
    send_request('stop')

def reset_display():
    send_request('reset')
    calc_entry.delete(0, tk.END)

# --- LÓGICA DE LA CALCULADORA (ahora usa send_request) ---
def calculate():
    try:
        expression = calc_entry.get()
        result = int(eval(expression))
        
        if 0 <= result <= 99:
            # Pasa el resultado como un parámetro en la URL
            # ej: http://192.168.1.123/calculate?value=42
            send_request('calculate', params={'value': result})
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
root.title("Controlador de Display ESP32 (Wi-Fi)")
root.geometry("350x450")
root.resizable(False, False)

main_frame = tk.Frame(root, padx=10, pady=10)
main_frame.pack(fill=tk.BOTH, expand=True)

calc_frame = tk.LabelFrame(main_frame, text="Calculadora", padx=10, pady=10)
calc_frame.pack(pady=10, fill=tk.X)
calc_entry = tk.Entry(calc_frame, font=('Arial', 18), justify='right')
calc_entry.grid(row=0, column=0, columnspan=4, pady=5, sticky="ew")
buttons = ['7', '8', '9', '/', '4', '5', '6', '*', '1', '2', '3', '-', '0', '.', '=', '+']
row_val, col_val = 1, 0
for button in buttons:
    if button == '=':
        btn = tk.Button(calc_frame, text=button, width=5, height=2, command=calculate)
    else:
        btn = tk.Button(calc_frame, text=button, width=5, height=2, command=lambda b=button: on_button_click(b))
    btn.grid(row=row_val, column=col_val, padx=2, pady=2)
    col_val = (col_val + 1) % 4
    if col_val == 0: row_val += 1
clear_btn = tk.Button(calc_frame, text='C', width=5, height=2, command=clear_entry)
clear_btn.grid(row=row_val, column=0, columnspan=4, sticky='ew', pady=2)
def on_button_click(char):
    calc_entry.insert(tk.END, str(char))

control_frame = tk.LabelFrame(main_frame, text="Controles de Conteo", padx=10, pady=10)
control_frame.pack(pady=10, fill=tk.X)
btn_up = tk.Button(control_frame, text="🔼 Iniciar Ascendente", command=start_up)
btn_up.pack(fill=tk.X, pady=2)
btn_down = tk.Button(control_frame, text="🔽 Iniciar Descendente", command=start_down)
btn_down.pack(fill=tk.X, pady=2)
btn_stop = tk.Button(control_frame, text="⏹️ Parar Conteo", command=stop_count)
btn_stop.pack(fill=tk.X, pady=2)
btn_reset = tk.Button(control_frame, text="🔄 Reset (00)", command=reset_display)
btn_reset.pack(fill=tk.X, pady=2)

status_label = tk.Label(main_frame, text="Estado: Esperando comando...", relief=tk.SUNKEN, anchor='w')
status_label.pack(side=tk.BOTTOM, fill=tk.X)

root.mainloop()