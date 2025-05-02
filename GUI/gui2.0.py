import time
import socket
import tkinter as tk
import threading
import os
import numpy as np
import matplotlib.pyplot as plt
import re
import queue
from collections import deque
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.lines as mlines
from matplotlib.patches import Circle
from matplotlib.patches import Polygon

curr_keys = set()
handle_flag = True
first_key = None
key_is_pressed = False
key_is_released = False
update_gui = True
window = None
polar_fig = None
polar_ax = None
xy_fig = None
xy_ax = None
polar_canvas = None
xy_canvas = None
zoom_scale = 1.2
drag_start = None

command_queue = queue.Queue()

sensor_scan_raw_data = ""

position = [0.0, 0.0]
heading = 0.0
path = [[0.0], [0.0]]
objects = [[], []]
objects_width = []
objects_color = []

scan_theta = 0.0
update_object_flag = False

bump_flag = False

terminal_output = None
message_log = deque(maxlen=5)
scan_complete = True

def main():
    global window, terminal_output
    window = tk.Tk()
    window.title("CyBot Control Interface")
    window.geometry("1200x700")

    control_frame = tk.Frame(window)
    control_frame.pack(side=tk.TOP, fill=tk.X)

    # Create quit button and center button side-by-side, both aligned to the left
    quit_command_Button = tk.Button(control_frame, text="Press to Quit", command=send_quit)
    quit_command_Button.pack(side=tk.LEFT, padx=5, pady=5)

    # Add Center CyBot Button next to the quit button
    center_button = tk.Button(control_frame, text="Center CyBot", command=center_cybot)
    center_button.pack(side=tk.LEFT, padx=5, pady=5)

    # Add Clear All button
    clear_button = tk.Button(control_frame, text="Clear All", command=clear_all)
    clear_button.pack(side=tk.LEFT, padx=5, pady=5)

    # Terminal Display
    terminal_frame = tk.Frame(window)
    terminal_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=10)

    terminal_label = tk.Label(terminal_frame, text="CyBot Terminal Output (Last 5 Messages):")
    terminal_label.pack(anchor='w')

    terminal_output = tk.Text(terminal_frame, height=5, width=100, state='disabled', bg='black', fg='lime')
    terminal_output.pack(fill=tk.X)

    window.bind("<KeyPress>", key_down)
    window.bind("<KeyRelease>", key_up)

    embed_initial_plot()

    my_thread = threading.Thread(target=socket_thread, daemon=True)
    my_thread.start()

    window.after(100, update_plot_thread)
    window.mainloop()

def center_cybot():
    global position, heading, xy_ax, xy_canvas
    # Set the XY axis limits to center the CyBot position
    xy_ax.set_xlim(position[0] - 200, position[0] + 200)
    xy_ax.set_ylim(position[1] - 200, position[1] + 200)

    # Redraw the canvas after centering
    xy_canvas.draw()

def send_quit():
    global gui_send_message
    global window
    gui_send_message = "quit\n"
    time.sleep(1)
    window.destroy()

def handle_mouse_clicks(event):
    on_press(event)   # For dragging
    on_click(event)   # For deleting objects


def on_click(event):
    global objects, objects_width, xy_canvas, xy_ax, update_gui, objects_color

    if event.inaxes != xy_ax:
        return

    # Convert click to data coordinates
    click_x = event.xdata
    click_y = event.ydata

    if click_x is None or click_y is None:
        return

    tolerance = 5  # Distance tolerance in cm for click radius

    # Iterate over object positions to find if click is close
    for i in range(len(objects[0])):
        obj_x = objects[0][i]
        obj_y = objects[1][i]
        dist = np.hypot(obj_x - click_x, obj_y - click_y)
        if dist <= tolerance:
            # Delete this object
            del objects[0][i]
            del objects[1][i]
            del objects_width[i]
            del objects_color[i]
            update_gui = True
            return  # Delete only one at a time
        
def clear_all():
    global objects, objects_width, path, update_gui, objects_color

    # Clear object and path data
    objects = [[], []]
    objects_width = []
    objects_color = []
    path = [[position[0]], [position[1]]]  # Start path at current position

    update_gui = True


def key_down(event):
    global curr_keys, first_key, key_is_pressed, scan_complete
    # Handle one-shot scan ('s') immediately
    if not bump_flag:
        if event.char == 's':
            if scan_complete:
                scan_complete = False
                command_queue.put('s')
                return
        elif event.char not in curr_keys:
            curr_keys.add(event.char)

            #Lock other commands
            if first_key is None and event.char in ['w', 'a', 'd', 'b', 'q']:
                first_key = event.char
                key_is_pressed = True
                command_queue.put(event.char)

def key_up(event):
    global curr_keys, first_key, key_is_released, update_gui
    
    if event.char in curr_keys:
        curr_keys.remove(event.char)
        if event.char == first_key:
            first_key = None
            key_is_released = True
            update_gui = True
            command_queue.put('q')

def embed_initial_plot():
    global polar_fig, polar_ax, xy_fig, xy_ax
    global polar_canvas, xy_canvas

    plot_frame = tk.Frame(window)
    plot_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    polar_fig, polar_ax = plt.subplots(subplot_kw={'projection': 'polar'})
    xy_fig, xy_ax = plt.subplots()

    # Polar setup
    polar_ax.set_title("CyBot Sensor Scan (0-180 Degrees)")
    polar_ax.set_rmax(120)
    polar_ax.grid(True)

    # XY map setup
    xy_ax.set_xlim(-100, 100)
    xy_ax.set_ylim(-100, 100)
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)

    # Embed plots
    polar_canvas = FigureCanvasTkAgg(polar_fig, master=plot_frame)
    polar_canvas.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
    polar_canvas.draw()

    xy_canvas = FigureCanvasTkAgg(xy_fig, master=plot_frame)
    xy_canvas.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
    xy_canvas.mpl_connect("button_press_event", on_click)
    xy_canvas.draw()

    # Mouse bindings
    xy_canvas.mpl_connect("scroll_event", on_scroll)
    xy_canvas.mpl_connect("button_press_event", handle_mouse_clicks)
    xy_canvas.mpl_connect("button_release_event", on_release)
    xy_canvas.mpl_connect("motion_notify_event", on_motion)

def on_scroll(event):
    ax = xy_ax
    if event.inaxes != ax:
        return

    scale = zoom_scale if event.button == 'up' else 1 / zoom_scale

    xlim = ax.get_xlim()
    ylim = ax.get_ylim()
    xdata = event.xdata
    ydata = event.ydata

    new_xlim = [
        xdata - (xdata - xlim[0]) * scale,
        xdata + (xlim[1] - xdata) * scale
    ]
    new_ylim = [
        ydata - (ydata - ylim[0]) * scale,
        ydata + (ylim[1] - ydata) * scale
    ]

    ax.set_xlim(new_xlim)
    ax.set_ylim(new_ylim)
    xy_canvas.draw()


def on_press(event):
    global drag_start
    if event.inaxes != xy_ax:
        return
    drag_start = (event.x, event.y)


def on_release(event):
    global drag_start
    drag_start = None


def on_motion(event):
    global drag_start
    if drag_start is None or event.inaxes != xy_ax:
        return

    dx = event.x - drag_start[0]
    dy = event.y - drag_start[1]
    drag_start = (event.x, event.y)

    ax = xy_ax
    inv = ax.transData.inverted()
    start_data = inv.transform((event.x - dx, event.y - dy))
    end_data = inv.transform((event.x, event.y))

    delta_x = start_data[0] - end_data[0]
    delta_y = start_data[1] - end_data[1]

    xlim = ax.get_xlim()
    ylim = ax.get_ylim()
    ax.set_xlim(xlim[0] + delta_x, xlim[1] + delta_x)
    ax.set_ylim(ylim[0] + delta_y, ylim[1] + delta_y)
    xy_canvas.draw()


def update_plot_thread():
    global update_gui
    if update_gui:
        update_gui = False
        update_plot()
    window.after(100, update_plot_thread)

def update_plot():
    global polar_ax, polar_canvas, xy_ax, xy_canvas

    degree_pattern = r"Degree: (\d+)"
    distance_pattern = r"ADC Distance: (\d+\.\d+)"
    ob_ang_pattern = r"Object at (\d+)"
    ob_r_pattern = r"has a distance of (\d+\.\d+)"
    ob_r_width = r"and width of (\d+\.\d+)"

    # Read sensor data from file
    data = sensor_scan_raw_data

    # Parsing the data
    degrees = np.array([int(deg) for deg in re.findall(degree_pattern, data)])
    distances = np.array([float(dist) for dist in re.findall(distance_pattern, data)])
    angle_radians = np.radians(degrees)

    ob_ang = np.array([int(ob_deg) for ob_deg in re.findall(ob_ang_pattern, data)])
    ob_rad = np.radians(ob_ang)
    ob_r = np.array([float(ob_dist) for ob_dist in re.findall(ob_r_pattern, data)])
    ob_width = np.array([float(ob_diam) for ob_diam in re.findall(ob_r_width, data)])

    # Update the polar plot
    polar_ax.clear()
    polar_ax.plot(angle_radians, distances, color='r', linewidth=4.0)
    if len(ob_ang) > 0:
        polar_ax.scatter(ob_rad, ob_r, s=ob_width**2, c='green')
    polar_ax.set_rmax(120)
    polar_ax.set_rticks([10, 25, 50, 75, 100])
    polar_ax.set_rlabel_position(-22.5)
    polar_ax.set_thetamax(180)
    polar_ax.grid(True)
    polar_ax.set_title("Sensor Scan", size=14)

    # Redraw the polar plot
    polar_canvas.draw()

    # Preserve the current view limits for the XY axis
    xlim = xy_ax.get_xlim()
    ylim = xy_ax.get_ylim()

    # Clear the XY axis content, but preserve limits
    xy_ax.cla()  # This clears content but doesn't affect the figure's properties

    # Restore the view limits
    xy_ax.set_xlim(xlim)
    xy_ax.set_ylim(ylim)

    # Labeling the XY axes and enabling grid
    xy_ax.set_xlabel('X-axis')
    xy_ax.set_ylabel('Y-axis')
    xy_ax.grid(True)

    # Remove previous CyBot marker
    xy_ax.plot(path[0], path[1], color='green', linestyle='-', linewidth=2, label='Path')

    # Add a circle representing the CyBot's 34 cm diameter
    cybot_radius = 17  # Radius = diameter / 2
    cybot_circle = Circle((position[0], position[1]), cybot_radius, color='red', alpha=0.5, label='CyBot')
    xy_ax.add_patch(cybot_circle)

    # Plot the path and current position
    if len(path[0]) > 1:
        xy_ax.plot(path[0][:-1], path[1][:-1], color='green', linestyle='-', linewidth=2, label='Path')


    if update_object_flag:
        update_objects(ob_r, ob_rad, ob_width)

    # Draw objects as Circle patches so size scales with zoom
    for x, y, width, color in zip(objects[0], objects[1], objects_width, objects_color):
        obj_circle = Circle((x, y), width / 2.0, color=color, alpha=0.6)
        xy_ax.add_patch(obj_circle)


    # Triangle parameters
    triangle_radius = 17  # same as the cybot_radius=17
    theta = np.radians(heading)

    # Define triangle vertices relative to the heading
    # Pointing triangle, base 120 degrees apart
    angle_offsets = [0, 140, -140]
    triangle_points = []

    for offset in angle_offsets:
        ang = theta + np.radians(offset)
        x = position[0] + triangle_radius * np.cos(ang)
        y = position[1] + triangle_radius * np.sin(ang)
        triangle_points.append([x, y])

    triangle = Polygon(triangle_points, closed=True, color='blue', alpha=0.8, label='Direction')
    xy_ax.add_patch(triangle)

    # Create a custom legend entry for the annotation (since annotations cannot be directly added to legend)
    arrow_handle = mlines.Line2D([], [], color='blue', marker='^', linestyle='None', markersize=10, label='Direction')

    # Draw legend and update canvas, including the custom arrow legend handle
    xy_ax.legend(handles=[arrow_handle], loc='upper right')
    xy_canvas.draw()

def update_heading(delta_angle):
    global heading
    heading = (heading + delta_angle) % 360

def update_position(distance):
    global position, heading, path
    rad = np.radians(heading)
    dx = distance * np.cos(rad)
    dy = distance * np.sin(rad)
    position[0] += dx
    position[1] += dy
    path[0].append(position[0])
    path[1].append(position[1])

def update_objects(ob_r, ob_rad, ob_width):
    global update_object_flag
    global objects, objects_width

    update_object_flag = False

    scan_theta_rad = scan_theta * np.pi /180

    # Apply scan angle to object local positions
    ob_x = ob_r * np.cos(ob_rad + scan_theta_rad)
    ob_y = ob_r * np.sin(ob_rad + scan_theta_rad)

    # Shift object positions forward by 10 cm (from scanner to middle of CyBot)
    ob_x += 10 * np.cos(np.radians(heading))
    ob_y += 10 * np.sin(np.radians(heading))

    #Shift object positions forward by the width/2 (We are scanning the front of the object)
    ob_x += ob_width/2.0 * np.cos(np.radians(heading))
    ob_y += ob_width/2.0 * np.sin(np.radians(heading))

    # Transform to global position
    ob_x += position[0]
    ob_y += position[1]


    objects[0].extend(ob_x.tolist())  
    objects[1].extend(ob_y.tolist())
    objects_width.extend([float(np.squeeze(w)) for w in ob_width])  
    objects_color.extend(['green'] * len(ob_width))

def update_terminal(new_message):
    global terminal_output, message_log
    message_log.append(new_message)
    terminal_output.configure(state='normal')
    terminal_output.delete(1.0, tk.END)
    for msg in message_log:
        terminal_output.insert(tk.END, msg + '\n')
    terminal_output.configure(state='disabled')

def read_cybot(cybot):
    global update_gui
    global previous_distance  # Tracks last known total distance
    global previous_angle
    global scan_theta, update_object_flag
    global scan_complete
    global bump_flag

    previous_distance = 0.0
    previous_angle = 0.0

    while True:
        try:
            rx_message = cybot.readline()
            decoded = rx_message.decode().strip()

            if not decoded:
                continue

            update_terminal(decoded)

            # Sensor data handling
            if decoded[0] == 's':
                scan_data_lines = []  # collect scan data
                scan_theta = heading - 90
                update_object_flag = True

                while decoded != "END" and decoded != "Scan stopped.":
                    rx_message = cybot.readline()
                    decoded = rx_message.decode().strip()
                    update_terminal(decoded)
                    
                    scan_data_lines.append(decoded)

                # Join all lines and store in global variable
                global sensor_scan_raw_data
                sensor_scan_raw_data = "\n".join(scan_data_lines)

                update_gui = True
                scan_complete = True  # Mark scan as finished


            # Movement data handling
            elif decoded[0] in ('w', 'a', 'd'):
                while decoded != "END":
                    
                    # Match movement updates
                    rx_message = cybot.readline()
                    decoded_line = rx_message.decode().strip()
                    update_terminal(decoded_line)
                    # Movement parsing
                    forward_match = re.search(r"Drove Forward: (\d+\.\d+)", decoded_line)
                    left_match = re.search(r"Turned Left: (\d+\.\d+)", decoded_line)
                    right_match = re.search(r"Turned Right: (\d+\.\d+)", decoded_line)
                    back_match = re.search(r"Drove Back (\d+)cm", decoded_line)
                    event_match = re.match(r"(OB|Hole).*", decoded_line)  # Match OB/Hole detections
                    bump_match = re.match(r"Hit object on (left|right)", decoded_line)  # Match bump events

                    if forward_match:
                        current_distance = float(forward_match.group(1))
                        delta = current_distance - previous_distance
                        previous_distance = current_distance
                        update_position(delta)
                        update_gui = True

                    elif left_match:
                        current_angle = float(left_match.group(1))
                        delta = current_angle - previous_angle
                        previous_angle = current_angle
                        update_heading(delta)
                        update_gui = True

                    elif right_match:
                        current_angle = float(right_match.group(1))
                        delta = current_angle - previous_angle
                        previous_angle = current_angle
                        update_heading(-delta)
                        update_gui = True

                    elif event_match:
                        event_type = event_match.group(1)  # Either "OB" or "Hole"
                        bump_flag = True
                        color = 'blue' if event_type == 'OB' else 'yellow'
                        direction_match = re.search(r"(Front Left|Front Right|Rear Left|Rear Right|Front|Rear|Left|Right)", decoded_line)
                        if direction_match:
                            direction = direction_match.group(0)
                            angle_offset = {
                                "Front": 0, "Rear": 180,
                                "Left": 90, "Right": -90,
                                "Front Left": 45, "Front Right": -45,
                                "Rear Left": 135, "Rear Right": -135
                            }.get(direction, 0)
                            hazard_dx = 17 * np.cos(np.radians(heading + angle_offset))
                            hazard_dy = 17 * np.sin(np.radians(heading + angle_offset))
                            hazard_x = position[0] + hazard_dx
                            hazard_y = position[1] + hazard_dy
                            objects_width.append(10.0)
                            objects_color.append(color)
                            objects[0].append(hazard_x)
                            objects[1].append(hazard_y)

                    elif bump_match:
                        bump_flag = True
                        side = bump_match.group(1)  # "left" or "right"
                        bump_angle = heading + (90 if side == "left" else -90)
                        bump_dx = 17 * np.cos(np.radians(bump_angle))
                        bump_dy = 17 * np.sin(np.radians(bump_angle))
                        bump_x = position[0] + bump_dx
                        bump_y = position[1] + bump_dy
                        objects_width.append(10.0)
                        objects_color.append("grey")
                        objects[0].append(bump_x)
                        objects[1].append(bump_y)

                    elif back_match:
                        backward_distance = float(back_match.group(1))
                        update_position(-backward_distance)
                        update_gui = True
                        bump_flag = False
                    
                    decoded = decoded_line

                previous_distance = 0.0
                previous_angle = 0.0

        except Exception as e:
            print(f"Error reading from cybot: {e}")
            break


def socket_thread():
    global curr_keys
    global first_key
    global key_is_pressed
    global key_is_released
    global update_gui

    HOST = "192.168.1.1"  # Use this if using the actual Cybot
    #HOST = "127.0.0.1"  # Use this if using the MOCK Cybot
    PORT = 288          # The port used by the server
    cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cybot_socket.connect((HOST, PORT))

    cybot = cybot_socket.makefile("rbw", buffering=0)

    read_thread = threading.Thread(target=read_cybot, args=(cybot,), daemon=True)
    read_thread.start()

    send_message = "Connection Initialized"
    print(send_message)

    last_command = None

    while send_message != 'quit\n':
        try:
            command = command_queue.get(timeout=0.1)

            if command != last_command or command == 'q' or command == 's':
                cybot.write(command.encode())
                last_command = command
                if command == 'quit\n':
                    break

            command_queue.task_done()

        except queue.Empty:
            pass

    print("Client exiting, and closing file descriptor, and/or network socket\n")
    time.sleep(2)
    cybot.close()
    cybot_socket.close()

main()