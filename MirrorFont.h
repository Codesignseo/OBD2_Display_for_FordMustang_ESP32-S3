// If we want to display the text on a projective film as a holographic effect on the windshield, then we need to mirror the text

#ifndef _MIRROR_FONT
#define _MIRROR_FONT

uint8_t tempFontBuffer[1024];

// Get the Nth bit in a byte
uint8_t GetNthBit(uint8_t byte, uint8_t n)
{
  return (byte >> n) & 1;
}

// Set the Nth bit in a byte
uint8_t SetNthBit(uint8_t byte, uint8_t n, uint8_t input)
{
  if (input >= 1)
  {
      return byte | (1 << n);
  }
  else
  {
    return byte & ~(1 << n);
  }
}

// Get a pixel in 1-bit font memory. Pixels in the font is 1-bit encoded, i.e. one pixel is one bit. The value returned is a byte, but only represents 0 or 1
uint8_t GetPixel(const uint16_t x, const uint16_t y, uint8_t* pData, const uint8_t width, const uint16_t numPixels, const uint16_t numBytes)
{
  uint16_t pixelPosition = (y * width) + x;
  uint16_t pixelByteOffset = pixelPosition / 8;
  uint8_t pixelBitOffset = pixelPosition % 8;
  uint8_t pixelByte = pData[pixelByteOffset];                       // The byte that contains the pixel at (x,y)
  uint8_t pixelValue = GetNthBit(pixelByte, 7 - pixelBitOffset);    // "7 -" is needed because the most significant bit represents the pixel on the right, not the left
  return pixelValue;
}

// Set pixel in 1-bit font memory. Pixels in the font is 1-bit encoded, i.e. one pixel is one bit. The input value is a byte, but only represents 0 or 1
void SetPixel(const uint16_t x, const uint16_t y, const uint8_t newPixelValue, uint8_t* pData, const uint8_t width, const uint16_t numPixels, const uint16_t numBytes)
{
  uint16_t pixelPosition = (y * width) + x;
  uint16_t pixelByteOffset = pixelPosition / 8;
  uint8_t pixelBitOffset = pixelPosition % 8;
  uint8_t pixelByte = pData[pixelByteOffset];   // The byte that contains the pixel at (x,y)
  pData[pixelByteOffset] = SetNthBit(pixelByte, 7 - pixelBitOffset, newPixelValue);
}

// Mirror the font by mirroring all pixels horizontally
// There's opportunity to optimize this function by processing 8 pixels at a time, but it's already fast enough, ~1ms per character for a 54pt font
void MirrorText(uint8_t* pData, const uint16_t offset, const uint8_t width, const uint8_t height)
{
  // Pixels in the font is 1-bit encoded, i.e. one pixel is one bit
  const uint16_t numPixels = width * height;
  const uint16_t numBytes = (numPixels / 8) + 1;

  // Copy the original data into a temp buffer
  memcpy(tempFontBuffer, pData + offset, numBytes);
  memset(pData + offset, 0, numBytes);

  for (int j = 0; j < height; j++)
  {
    for (int i = 0; i < width; i++)
    {
      int8_t pixelValue = GetPixel(i, j, tempFontBuffer, width, numPixels, numBytes);
      SetPixel(width - 1 - i, j, pixelValue, pData + offset, width, numPixels, numBytes);
    }
  }
}

#endif  // _MIRROR_FONT