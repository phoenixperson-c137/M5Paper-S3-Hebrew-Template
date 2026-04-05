# M5Paper S3 Hebrew Template

This is a barebones PlatformIO starter project demonstrating how to display perfectly anti-aliased TrueType Hebrew fonts on the M5Paper S3. It includes a custom Bidirectional (Bidi) text algorithm for mixing English and Hebrew text flawlessly.

## Features
- **Hardware:** M5Stack M5Paper S3
- **Framework:** Arduino (PlatformIO)
- **Libraries:** `M5Unified` and `OpenFontRender`
- **Typography:** Completely Native Vector Rendering (FreeType/TrueType) using Google's `Assistant-Regular` embedded in flash.
- **Bidi Engine:** A lightweight RTL (Right-to-Left) algorithm ensuring correct ordering of embedded English words, numbers, spacing, and automatic bracket/punctuation mirroring.

## How It Works
Standard graphics libraries like `M5GFX` don't inherently process bidirectional text layout natively, meaning raw Hebrew strings will print backwards.

To solve this, the project parses your raw UTF-8 string through `processHebrewBidi()`. This detects Right-to-Left Hebrew chunks, Left-to-Right Latin/Numeric chunks, and neutral punctuation. It safely reverses the Hebrew orientation while keeping inner English words running left-to-right, then renders everything crisply using `OpenFontRender`.

## Usage
Simply wrap any mixed-content string with `processHebrewBidi()` before drawing:

```cpp
// Optional: Align the text rendering anchor to the Top Right
ofr.setAlignment(Align::TopRight);

// Output Native Hebrew and English natively!
String text = "גם טקסט באנגלית כמו M5Paper נתמך!";
ofr.drawString(processHebrewBidi(text.c_str()).c_str(), 900, 160);
```

## Setup Instructions
1. Clone this repository and open the folder in **VSCode** with the **PlatformIO** extension installed.
2. Connect your M5Paper S3 via USB.
3. Click "Build" and "Upload" in the PlatformIO toolbar. 
4. PlatformIO will automatically fetch `OpenFontRender` and `M5Unified` to get you started immediately!