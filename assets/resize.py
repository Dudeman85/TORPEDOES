import os
from PIL import Image

input = "3dmodels"
output = "resized"
size = (4, 4)

# Create the output folder if it doesn't exist
os.makedirs(output, exist_ok=True)

# Iterate through all files in the input folder
for filename in os.listdir(input):
    if filename.lower().endswith(".png"):
        input_path = os.path.join(input, filename)
        output_path = os.path.join(output, filename)

        with Image.open(input_path) as img:
            # Resize the image
            img_resized = img.resize(size, Image.NEAREST)

            # Save the resized image as a copy
            img_resized.save(output_path)