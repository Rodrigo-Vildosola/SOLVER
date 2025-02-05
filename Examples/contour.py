import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from solver import Solver  # Replace with your module name

# Create an instance of your solver
solver = Solver()

# Parameters
FPS = 60  # Set FPS to 30 or 60
DURATION = 10  # Animation duration in seconds
TOTAL_FRAMES = FPS * DURATION  # Total frames based on FPS

# Generate animation data for "sin(x)" with fixed number of frames
x_vals, y_vals = solver.generate_animation_data("sin(x) + cos(x ^ 2)", "x", 0, 3 * np.pi, TOTAL_FRAMES)

# Set up the figure
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)
ax.set_xlim(min(x_vals), max(x_vals))
ax.set_ylim(min(y_vals), max(y_vals))

# Animation initialization
def init():
    line.set_data([], [])
    return (line,)

# Animation update function
def animate(i):
    line.set_data(x_vals[:i], y_vals[:i])
    return (line,)

# Set interval to control FPS
ani = animation.FuncAnimation(fig, animate, frames=TOTAL_FRAMES, init_func=init, interval=1000 / FPS, blit=False)

plt.show()

# Similarly, you could generate contour data:
x_vals, y_vals, z_vals = solver.generate_contour_data("sin(x) * y ^ 3", "x", "y", 0, 100, 500, 0, 100, 500)

# Convert lists to NumPy arrays
X, Y = np.meshgrid(x_vals, y_vals)
Z = np.array(z_vals)

fig = plt.figure(figsize=(8,6))
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(X, Y, Z, cmap="viridis")

ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
ax.set_title("3D Surface Plot of sin(x) * cos(y)")

def rotate(i):
    ax.view_init(elev=30, azim=i * (360 / TOTAL_FRAMES))

ani = animation.FuncAnimation(fig, rotate, frames=TOTAL_FRAMES, interval=1000 / FPS)
plt.show()
