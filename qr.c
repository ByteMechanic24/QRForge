/*
 * QR Code Generator
 * 
 * Generates a QR code from text input and outputs it as a PNG file.
 * Uses the qrcodegen library by Project Nayuki for QR code encoding
 * and TinyPngOut for PNG file output.
 *
 * Usage: echo "your text" | ./qr
 *        The output is written to out.png in the current directory.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrcodegen.h"
#include "TinyPngOut.h"

// The hard upper limit of the QR Code standard for UTF-8 text
#define QR_MAX_INPUT_LENGTH 2953

// Scale factor: each QR module becomes SCALE x SCALE pixels in the output PNG
#define SCALE 10

int main(void) {
	// Read input text from stdin
	char inputText[QR_MAX_INPUT_LENGTH];
	memset(inputText, 0, QR_MAX_INPUT_LENGTH);
	fgets(inputText, QR_MAX_INPUT_LENGTH, stdin);

	// Remove trailing newline if present
	size_t len = strlen(inputText);
	if (len > 0 && inputText[len - 1] == '\n') {
		inputText[len - 1] = '\0';
	}

	// Allocate buffers for QR code generation
	uint8_t qr[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

	// Encode the text into a QR code
	bool ok = qrcodegen_encodeText(
		inputText,
		tempBuffer,
		qr,
		qrcodegen_Ecc_LOW,
		qrcodegen_VERSION_MIN,
		qrcodegen_VERSION_MAX,
		qrcodegen_Mask_AUTO,
		true
	);

	if (!ok) {
		fprintf(stderr, "Error: Failed to encode QR code\n");
		return 1;
	}

	// Get the size of the QR code (side length in modules)
	int size = qrcodegen_getSize(qr);

	// Open output file for writing
	FILE *file = fopen("out.png", "wb");
	if (file == NULL) {
		fprintf(stderr, "Error: Couldn't open output file\n");
		return 1;
	}

	// Initialize the PNG writer with scaled dimensions
	struct TinyPngOut writer;
	enum TinyPngOut_Status status = TinyPngOut_init(&writer, (uint32_t)(size * SCALE), (uint32_t)(size * SCALE), file);
	if (status != TINYPNGOUT_OK) {
		fprintf(stderr, "Error: Couldn't initialize TinyPngOut (status %d)\n", status);
		fclose(file);
		return 1;
	}

	// RGB color values for QR code modules
	uint8_t rgbBlack[] = {0, 0, 0};
	uint8_t rgbWhite[] = {255, 255, 255};

	// Write the QR code as pixel data, scaling each module
	for (int y = 0; y < size * SCALE; y++) {
		for (int x = 0; x < size * SCALE; x++) {
			// Map pixel coordinates to QR module coordinates
			bool color = qrcodegen_getModule(qr, x / SCALE, y / SCALE);
			status = TinyPngOut_write(&writer, color ? rgbBlack : rgbWhite, 1);
			if (status != TINYPNGOUT_OK) {
				fprintf(stderr, "Error: Failed to write pixel data\n");
				fclose(file);
				return 1;
			}
		}
	}

	fclose(file);
	fprintf(stderr, "QR code has been generated: out.png (size: %d, scale: %dx)\n", size, SCALE);
	return 0;
}
