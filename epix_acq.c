#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tiffio.h>
#include "xcliball.h"

#define DEFAULT_NUM_BUFFERS 3
#define DEFAULT_NUM_FRAMES 100
#define DEFAULT_SAVE_INTERVAL 10
#define DEFAULT_AOI_X 100
#define DEFAULT_AOI_Y 100
#define DEFAULT_AOI_WIDTH 320
#define DEFAULT_AOI_HEIGHT 240

// Global variables for resource management
unsigned short **frame_buffers;  // Buffer to hold 16-bit pixel values
TIFF *tiff;
int num_buffers;
FILE *x_file, *y_file, *z_file, *t_file;  // Timestamp file
double frame_interval_ms;  // Time per frame in milliseconds

// Placeholder for image processing that calculates X, Y, and Z values
void process_image(unsigned short *frame, int width, int height, double *X, double *Y, double *Z) {
    *X = 0;
    *Y = 0;
    *Z = 0;
    for (int i = 0; i < width * height; i++) {
        *X += frame[i];
        *Y += frame[i] * (i % width);      // Example X and Y weighted by position
        *Z += frame[i] * (i / width);      // Example Z weighted by line
    }
    *X /= (width * height);
    *Y /= (width * height);
    *Z /= (width * height);
}

// Save frame as a TIFF page (compatible with Bio-Formats bfopen)
void save_frame_as_tiff(TIFF *tiff, unsigned short *frame, int width, int height) {
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, height);

    TIFFWriteEncodedStrip(tiff, 0, frame, width * height * sizeof(unsigned short));
    TIFFWriteDirectory(tiff);
}

// Setup function
int setup(int aoi_x, int aoi_y, int aoi_width, int aoi_height, int buffer_count, int exposure_time, double acquisition_rate) {
    int status = pxd_PIXCIopen("", "", "");
    if (status < 0) {
        printf("Error initializing PIXCI: %s\n", pxd_mesgErrorCode(status));
        return -1;
    }

    pxd_imageSizeXdim();
    pxd_imageSizeYdim();
    pxd_defineImage(1, aoi_x, aoi_y, aoi_x + aoi_width, aoi_y + aoi_height, "");

    frame_buffers = malloc(buffer_count * sizeof(unsigned short *));
    for (int i = 0; i < buffer_count; i++) {
        frame_buffers[i] = (unsigned short *)malloc(aoi_width * aoi_height * sizeof(unsigned short));
        if (!frame_buffers[i]) {
            printf("Error allocating frame buffer %d\n", i);
            return -1;
        }
    }

    tiff = TIFFOpen("output_frames.tif", "w");
    if (!tiff) {
        fprintf(stderr, "Error opening TIFF file for output.\n");
        return -1;
    }

    // Open binary files for X, Y, Z, and T (timestamps) values
    x_file = fopen("x.bin", "ab");
    y_file = fopen("y.bin", "ab");
    z_file = fopen("z.bin", "ab");
    t_file = fopen("t.bin", "ab");
    if (!x_file || !y_file || !z_file || !t_file) {
        fprintf(stderr, "Error opening binary files for X, Y, Z, or T.\n");
        return -1;
    }

    num_buffers = buffer_count;

    // Set exposure time if provided
    if (exposure_time > 0) {
        status = pxd_setExposureTime(1, exposure_time);  // Hypothetical SDK function
        if (status < 0) {
            printf("Error setting exposure time: %s\n", pxd_mesgErrorCode(status));
            return -1;
        }
        printf("Exposure time set to %d milliseconds\n", exposure_time);
    }

    // Set frame interval based on user-specified acquisition rate, if provided
    if (acquisition_rate > 0) {
        frame_interval_ms = 1000.0 / acquisition_rate;
        printf("Acquisition rate set to %.2f fps, frame interval: %.2f ms\n", acquisition_rate, frame_interval_ms);
    } else {
        // Retrieve acquisition rate from SDK or use default
        int sdk_exposure_time = pxd_getExposureTime();  // Hypothetical SDK function
        if (sdk_exposure_time > 0) {
            frame_interval_ms = sdk_exposure_time / 1000.0;  // Convert microseconds to milliseconds
            printf("Frame interval (from exposure time): %.2f ms\n", frame_interval_ms);
        } else {
            frame_interval_ms = 33.33;  // Default to 30fps if unknown
            printf("Using default frame interval: %.2f ms\n", frame_interval_ms);
        }
    }

    return 0;
}

// Acquisition function
void acquisition(int aoi_width, int aoi_height, int num_frames, int save_interval) {
    int status = pxd_goLiveSeq(1, 1, num_buffers, 1);
    if (status < 0) {
        printf("Error starting live capture: %s\n", pxd_mesgErrorCode(status));
        return;
    }

    int frame_index = 0;
    double X_values[save_interval];
    double Y_values[save_interval];
    double Z_values[save_interval];
    double timestamps[save_interval];
    double X, Y, Z;
    int save_count = 0;
    double elapsed_time_ms = 0.0;

    for (int frame_num = 0; frame_num < num_frames; frame_num++) {
        int next_frame = (frame_index + 1) % num_buffers;
        if (pxd_capturedBufferData(next_frame) == NULL) {
            printf("Error: No frame captured\n");
            break;
        }

        // Read frame buffer as 16-bit (10-bit compatible) pixel values
        status = pxd_readushort(1, next_frame, 0, 0, aoi_width, aoi_height, frame_buffers[frame_index], aoi_width * aoi_height, "");
        if (status < 0) {
            printf("Error reading frame buffer %d: %s\n", frame_index, pxd_mesgErrorCode(status));
            break;
        }

        process_image(frame_buffers[frame_index], aoi_width, aoi_height, &X, &Y, &Z);

        // Store X, Y, Z values in arrays
        X_values[save_count] = X;
        Y_values[save_count] = Y;
        Z_values[save_count] = Z;
        
        // Store timestamp based on elapsed time
        timestamps[save_count] = elapsed_time_ms;
        elapsed_time_ms += frame_interval_ms;

        // Save frame to TIFF and write X, Y, Z, and timestamps every save_interval
        if (frame_num % save_interval == 0) {
            save_frame_as_tiff(tiff, frame_buffers[frame_index], aoi_width, aoi_height);
            printf("Saved frame %d to TIFF file\n", frame_num);

            fwrite(X_values, sizeof(double), save_interval, x_file);
            fwrite(Y_values, sizeof(double), save_interval, y_file);
            fwrite(Z_values, sizeof(double), save_interval, z_file);
            fwrite(timestamps, sizeof(double), save_interval, t_file);
            printf("Saved X, Y, Z, and timestamps for %d frames\n", save_interval);

            save_count = 0;
        } else {
            save_count++;
        }

        frame_index = (frame_index + 1) % num_buffers;
    }

    int dropped_frames = pxd_lostFrames();
    if (dropped_frames > 0) {
        printf("Warning: %d frames were dropped during acquisition.\n", dropped_frames);
    }
}

// Cleanup function
void cleanup() {
    TIFFClose(tiff);
    fclose(x_file);
    fclose(y_file);
    fclose(z_file);
    fclose(t_file);

    for (int i = 0; i < num_buffers; i++) {
        free(frame_buffers[i]);
    }
    free(frame_buffers);

    pxd_PIXCIclose();
}

int main(int argc, char *argv[]) {
    // Default parameter values
    int aoi_x = DEFAULT_AOI_X;
    int aoi_y = DEFAULT_AOI_Y;
    int aoi_width = DEFAULT_AOI_WIDTH;
    int aoi_height = DEFAULT_AOI_HEIGHT;
    int num_buffers = DEFAULT_NUM_BUFFERS;
    int num_frames = DEFAULT_NUM_FRAMES;
    int save_interval = DEFAULT_SAVE_INTERVAL;
    int exposure_time = 0;  // 0 indicates no user-specified exposure time
    double acquisition_rate = 0.0;  // 0.0 indicates no user-specified acquisition rate

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "x:y:w:h:b:n:s:e:r:")) != -1) {
        switch (opt) {
            case 'x': aoi_x = atoi(optarg); break;
            case 'y': aoi_y = atoi(optarg); break;
            case 'w': aoi_width = atoi(optarg); break;
            case 'h': aoi_height = atoi(optarg); break;
            case 'b': num_buffers = atoi(optarg); break;
            case 'n': num_frames = atoi(optarg); break;
            case 's': save_interval = atoi(optarg); break;
            case 'e': exposure_time = atoi(optarg); break;  // Exposure time in microseconds
            case 'r': acquisition_rate = atof(optarg); break;  // Acquisition rate in frames per second
            default:
                fprintf(stderr, "Usage: %s [-x AOI_X] [-y AOI_Y] [-w AOI_WIDTH] [-h AOI_HEIGHT] "
                                "[-b NUM_BUFFERS] [-n NUM_FRAMES] [-s SAVE_INTERVAL] "
                                "[-e EXPOSURE_TIME] [-r ACQUISITION_RATE]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Setup acquisition environment
    if (setup(aoi_x, aoi_y, aoi_width, aoi_height, num_buffers, exposure_time, acquisition_rate) != 0) {
        fprintf(stderr, "Error during setup. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    // Begin acquisition
    acquisition(aoi_width, aoi_height, num_frames, save_interval);

    // Cleanup resources
    cleanup();

    // Report any dropped frames (hypothetical SDK function)
    int dropped_frames = pxd_lostFrames();  // Hypothetical function to get dropped frame count
    if (dropped_frames > 0) {
        printf("Acquisition completed with %d dropped frames.\n", dropped_frames);
    } else {
        printf("Acquisition completed with no dropped frames.\n");
    }

    return 0;
}

