# FidelityFX-CAS-CUDA-GUI

A Qt-based application for real-time image sharpening using the Dynamic link library (DLL) implemented by this [Project](https://github.com/kar-dim/FidelityFX-CAS-CUDA). It uses this CAS DLL in order to sharpen the image and show the result in realtime, by leveraging the hardware aceleration that CUDA provides.

## Features

- Load and display images from the local system.
- Allow to parameterize CAS and apply the sharpening, in realtime.
- View the results of processing in real-time.
- Save the processed image back to the system (optional).

## Usage

1. Launch the application.
2. Use the **Open Image** from the File menu to select an image file from the system.
3. Adjust parameters as desired through the user interface.
4. The sharpening is applied in realtime each time a parameter is changed, to allow the user to view the updated image with various configurations.
5. (Optional) Save the processed image using the **Save Image** from the File menu.

## Prerequisites/Dependencies

- **NVIDIA GPU**: An NVIDIA GPU is required in order to use the CAS DLL, with Compute Capability of 5.2 and higher.
- **DLL**: The DLL prerequisites are already present in the ```libs/``` directory (```.lib``` and ```.dll``` file), and the necessary header file in the ```libs/include/``` folder

