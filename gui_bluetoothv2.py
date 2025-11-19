import tkinter as tk
import serial

# === SETTINGS ===
COM_PORT = "COM5"   # HC-05 outgoing port
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
root.title("Robot Control GUI")
#root.geometry("300x300")

icon = tk.PhotoImage(file="dysonlogo.png")
root.iconphoto(True, icon)

# direction frame group together the arrows label
frame_direction = tk.Frame(master=root, borderwidth=1, relief=tk.RAISED)
frame_direction.grid(row=0, column=0)

# roller frame group together the roller related label
frame_roller = tk.Frame(master=root)
frame_roller.grid(row=0, column=1, sticky="n", padx=20)

# level frame for grouping together lv1-3
frame_level = tk.Frame(master=frame_roller)
frame_level.grid(row=2, column=1)

buttons = {}
states = {}

def make_button(master, text, row, col, **kwargs):
    btn = tk.Label(master=master, text=text, font=("Arial", 16), width=4, height=2, bg="lightgray")
    btn.grid(row=row, column=col, padx=5, pady=5)
    return btn

buttons["F"] = make_button(frame_direction, "↑", 0, 1)
buttons["B"] = make_button(frame_direction, "↓", 2, 1)
buttons["S"] = make_button(frame_direction, "■", 1, 1)
buttons["TR"] = make_button(frame_direction, "→", 1, 2)
buttons["TL"] = make_button(frame_direction, "←", 1, 0)

roller_title_lbl = tk.Label(master=frame_roller, text="Roller", font=("Arial", 10))
roller_title_lbl.grid(row=0, column=0, sticky="ew", columnspan=2)
buttons["R"] = roller_title_lbl

roller_status_lbl = tk.Label(master=frame_roller, text="Status")
roller_status_lbl.grid(row=1, column=0)

roller_on_lbl = tk.Label(master=frame_roller, text="OFF", borderwidth=1, relief=tk.SUNKEN, bg="lightcoral", width=10)
roller_on_lbl.grid(row=1, column=1)
states["Roller_ON"] = False

hydration_lbl = tk.Label(master=frame_roller, text="Hydration Level")
hydration_lbl.grid(row=2, column=0)

lv1_lbl = tk.Label(master=frame_level, text="Lv1", borderwidth=1, relief=tk.SUNKEN)
lv1_lbl.grid(row=0, column=0)
lv2_lbl = tk.Label(master=frame_level, text="Lv2", borderwidth=1, relief=tk.SUNKEN)
lv2_lbl.grid(row=0, column=1)
lv3_lbl = tk.Label(master=frame_level, text="Lv3", borderwidth=1, relief=tk.SUNKEN)
lv3_lbl.grid(row=0, column=2)


def send_cmd(cmd):
    if ser:
        try:
            ser.write(cmd.encode())
        except Exception as e:
            print("Write error:", e)
    print("Sent:", cmd)

def update_state(cmd, states_dict):
    if states_dict[cmd] == False:
        states_dict[cmd] = True
    else:
        states_dict[cmd] = False

def highlight(cmd):
    for k, b in buttons.items():
        b.config(bg="lightgray")
    buttons[cmd].config(bg="lightgreen")

    if states["Roller_ON"] == True:
        roller_on_lbl.config(bg="lightgreen", text="ON")
    else:
        roller_on_lbl.config(bg="lightcoral", text="OFF")

def key_press(event):
    key = event.keysym
    if key == "Up":
        send_cmd("F"); highlight("F")
    elif key == "Down":
        send_cmd("B"); highlight("B")
    elif key == "Right":
        send_cmd("TR"); highlight("TR")
    elif key == "Left":
        send_cmd("TL"); highlight("TL")
    elif key == "r":
        send_cmd("R")
        highlight("R")
        update_state("Roller_ON", states) 
        print(states)

def key_release(event):
    send_cmd("S"); highlight("S")

root.bind("<KeyPress>", key_press)
root.bind("<KeyRelease>", key_release)

highlight("S")
root.mainloop()

if ser:
    ser.close()
