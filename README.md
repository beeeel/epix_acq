# EPIX/XCLIB Camera Acquisition and Processing

## Project Overview

This project is designed to acquire and process images from an EPIX/XCLIB camera. The code allows users to specify various parameters for the camera setup, including area of interest (AOI), exposure time, acquisition rate, and more. Images are captured in a 10-bit pixel format and processed to extract valuable metrics, which are then saved for further analysis.

## Features

- Configurable area of interest (AOI) for image acquisition.
- Adjustable exposure time and acquisition rate.
- Processing of acquired images to derive metrics (X, Y, Z).
- Output of captured images in TIFF format, compatible with Bio-Formats.
- Binary output files for processed metrics and timestamps.
- Frame buffer management for efficient image acquisition.

## Role

At the initial commit point of this project, all code was authored by ChatGPT. My role in this project was as a **curator**, overseeing the implementation, ensuring the code meets project requirements, and facilitating the development process. 

## Dependencies

- EPIX/XCLIB SDK
- libtiff
- gcc (GNU Compiler Collection)

## Build Instructions

To compile the project, use the provided `Makefile`. Ensure that the paths to the EPIX/XCLIB SDK are correctly set in the `Makefile`.

1. Clone the repository:
   `git clone <repository_url>
   cd <repository_directory>`

2. Build the project:

    ``make``

## License

This project is free to use and distribute as described by GNU GPL v3.0. See the LICENSE file for details.

## Acknowledgments

Special thanks to OpenAI's ChatGPT for assistance in coding and project development.

