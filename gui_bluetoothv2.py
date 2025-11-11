import tkinter as tk
import serial

# === SETTINGS ===
COM_PORT = "COM6"   # HC-05 outgoing port
BAUD_RATE = 9600

# === CONNECT TO SERIAL ===
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=0.1)
    print(f"Connected to {COM_PORT}")
except Exception as e:
    print("Failed to connect:", e)
    ser = None

# === GUI SETUP ===
root = tk.Tk()
root.title("Motor Control")
root.geometry("200x200")

buttons = {}

def make_button(text, row, col):
    btn = tk.Label(root, text=text, font=("Arial", 16), width=4, height=2, bg="lightgray")
    btn.grid(row=row, column=col, padx=5, pady=5)
    return btn

buttons["F"] = make_button("↑", 0, 1)
buttons["B"] = make_button("↓", 2, 1)
buttons["S"] = make_button("■", 1, 1)

def send_cmd(cmd):
    if ser:
        try:
            ser.write(cmd.encode())
        except Exception as e:
            print("Write error:", e)
    print("Sent:", cmd)

def highlight(cmd):
    for k, b in buttons.items():
        b.config(bg="lightgray")
    buttons[cmd].config(bg="lightgreen")

def key_press(event):
    key = event.keysym
    if key == "Up":
        send_cmd("F"); highlight("F")
    elif key == "Down":
        send_cmd("B"); highlight("B")

def key_release(event):
    send_cmd("S"); highlight("S")

root.bind("<KeyPress>", key_press)
root.bind("<KeyRelease>", key_release)

highlight("S")
root.mainloop()

if ser:
    ser.close()
