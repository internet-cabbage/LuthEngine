import numpy as np
import vispy.scene
from vispy.scene import visuals, transforms, events
from tqdm import tqdm
from moviepy import VideoClip

filePath = "/Users/luthaisb/Code/C++/Galaxy_Simulation_3D_Refactor/DataOutput.bin"

print("Starting to load file.")
with open(filePath, 'rb') as f:
    N, tSteps = np.fromfile(f, np.int32, count=2)
    data = np.fromfile(f, np.float32).reshape(-1, N, 3)
print("Finished loading file.")

print('Initial shape: ', data.shape)

print('N value:', N)
print('tSteps value:', tSteps)




# Make the canvas and add a simple viewer
canvas = vispy.scene.SceneCanvas(keys='interactive', show=True)
view = canvas.central_widget.add_view()

scatter = visuals.Markers(parent=view.scene, scaling="scene",spherical=True)
scatter.set_data(data[0])

maxDist = np.max(data[0])
print("maxDist: ", maxDist)
view.camera = 'fly'
view.camera.aspect = 1
view.camera.set_range(x=(-maxDist, maxDist), y=(-maxDist, maxDist), z=(-maxDist, maxDist))


# Find first NaN

for i in range(data.shape[0]):
    if np.isnan(data[i]).any():
        print("First NaN found at tStep: ", i)
        break



inc = 0
stepsPerFrame = 1
def frame(event):
    global inc
    if (inc * stepsPerFrame) < tSteps:
        scatter.set_data(data[inc * stepsPerFrame],size=25)
        inc += stepsPerFrame
    else:
        inc = 0
        print("Looped")


print("NaN:", np.isnan(data).any(), "  Inf:", np.isinf(data).any())
timer = vispy.app.Timer(interval='auto',connect=frame,start=True)

vispy.app.run()