#include <Arduino.h>
#include <M5Unified.h>
#include <OpenFontRender.h>
#include "Assistant_ttf.h" // Byte array of Assistant TTF

OpenFontRender ofr;

#include <vector>

bool isRTL(uint32_t c) { return (c >= 0x0590 && c <= 0x05FF); }
bool isLTR(uint32_t c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'); }
bool isInternalLTR(uint32_t c) {
    return c == ' ' || c == '.' || c == ',' || c == ':' || c == '-' || 
           c == '/' || c == '_' || c == '\'';
}
uint32_t swapMirrored(uint32_t c) {
    switch(c) {
        case '(': return ')'; case ')': return '(';
        case '[': return ']'; case ']': return '[';
        case '{': return '}'; case '}': return '{';
        case '<': return '>'; case '>': return '<';
        default: return c;
    }
}

String processHebrewBidi(const char* str) {
    std::vector<uint32_t> vec;
    int len = strlen(str);
    for (int i = 0; i < len; ) {
        uint32_t cp = 0;
        if ((str[i] & 0x80) == 0) {
            cp = str[i];
            i++;
        } else if ((str[i] & 0xE0) == 0xC0) {
            cp = ((str[i] & 0x1F) << 6) | (str[i+1] & 0x3F);
            i+=2;
        } else if ((str[i] & 0xF0) == 0xE0) {
            cp = ((str[i] & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
            i+=3;
        } else if ((str[i] & 0xF8) == 0xF0) {
            cp = ((str[i] & 0x07) << 18) | ((str[i+1] & 0x3F) << 12) | ((str[i+2] & 0x3F) << 6) | (str[i+3] & 0x3F);
            i+=4;
        } else {
            i++;
        }
        vec.push_back(cp);
    }

    std::reverse(vec.begin(), vec.end());
    for (size_t i = 0; i < vec.size(); i++) {
        vec[i] = swapMirrored(vec[i]);
    }

    for (size_t i = 0; i < vec.size(); ) {
        if (isLTR(vec[i])) {
            size_t start = i;
            size_t end = i;
            size_t j = i + 1;
            while (j < vec.size()) {
                if (isLTR(vec[j])) {
                    end = j;
                } else if (isInternalLTR(vec[j])) {
                    // Allowed internal character, continue checking
                } else {
                    break;
                }
                j++;
            }
            std::reverse(vec.begin() + start, vec.begin() + end + 1);
            for (size_t k = start; k <= end; k++) {
                vec[k] = swapMirrored(vec[k]);
            }
            i = end + 1;
        } else {
            i++;
        }
    }

    String out = "";
    for (uint32_t cp : vec) {
        if (cp <= 0x7F) {
            out += (char)cp;
        } else if (cp <= 0x7FF) {
            out += (char)(0xC0 | ((cp >> 6) & 0x1F));
            out += (char)(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            out += (char)(0xE0 | ((cp >> 12) & 0x0F));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        } else {
            out += (char)(0xF0 | ((cp >> 18) & 0x07));
            out += (char)(0x80 | ((cp >> 12) & 0x3F));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        }
    }
    return out;
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Clear the screen for e-ink
    M5.Display.fillScreen(TFT_WHITE);
    
    // Initialize OpenFontRender
    ofr.setDrawer(M5.Display);
    
    // Load the TTF byte array
    if (ofr.loadFont(Assistant_ttf, sizeof(Assistant_ttf)) != 0) {
        M5.Display.println("Font load failed!");
        return;
    }
    
    ofr.setFontColor(TFT_BLACK);
    ofr.setFontSize(30); 
    ofr.setAlignment(Align::TopLeft);
    
    int leftX = 50;
    int rightX = M5.Display.width() - 50;
    
    // Both English and Punctuation will work natively with this font
    ofr.drawString("Example: English with punctuation!", leftX, 100);
    
    ofr.setAlignment(Align::TopRight);
    String hebrewText = "שלום עולם!"; 
    ofr.drawString(processHebrewBidi(hebrewText.c_str()).c_str(), rightX, 160);

    ofr.drawString(processHebrewBidi("גם פיסוק (!?) עובד מצוין :)").c_str(), rightX, 220);

    ofr.drawString(processHebrewBidi("גם טקסט באנגלית כמו M5Paper נתמך!").c_str(), rightX, 280); 

    M5.Display.waitDisplay();
}

void loop() {
    M5.update();
    delay(10);
}
