import os
from PIL import Image

def resize_images_in_folder(folder_path, output_folder, size):
    """
    Resizes all PNG images in a folder and saves the resized copies to an output folder.

    Parameters:
        folder_path (str): Path to the folder containing the images.
        output_folder (str): Path to the folder where resized images will be saved.
        size (tuple): The target size as (width, height).
    """
    # Create the output folder if it doesn't exist
    os.makedirs(output_folder, exist_ok=True)

    # Iterate through all files in the input folder
    for filename in os.listdir(folder_path):
        if filename.lower().endswith(".png"):
            input_path = os.path.join(folder_path, filename)
            output_path = os.path.join(output_folder, filename)

            try:
                # Open the image
                with Image.open(input_path) as img:
                    # Resize the image
                    img_resized = img.resize(size, Image.ANTIALIAS)

                    # Save the resized image as a copy
                    img_resized.save(output_path)

                    print(f"Resized and saved: {filename}")
            except Exception as e:
                print(f"Failed to process {filename}: {e}")

if __name__ == "__main__":
    # Folder containing the PNG images
    input_folder = "3dmodels"

    # Folder to save resized images
    output_folder = "resized"

    # Target size for resizing (width, height)
    target_size = (8, 8)  # Example size, adjust as needed

    # Call the function to resize images
    resize_images_in_folder(input_folder, output_folder, target_size)
