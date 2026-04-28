# 🔡 Braille Writer - Joystick → MAX7219 Matrix → I2C LCD

An Arduino-based Braille input device where you navigate a 2×3 Braille dot grid using a joystick, toggle dots ON/OFF, and confirm letters to an I2C LCD display. Supports the full 26-letter Grade 1 Braille alphabet.

---
## Preview

![Demo](https://github.com/Obedientononline/Braille_writer_arduino/raw/250a5418fee1b55c68fd382cf47e90ee38a9f8b7/Images/project_photo.jpeg)
![Demo](https://github.com/Obedientononline/Braille_writer_arduino/raw/250a5418fee1b55c68fd382cf47e90ee38a9f8b7/Images/project_photo2.jpeg)

---
## ✨ Features

- **2×3 Braille zone** rendered inside a visible border on the 8×8 LED matrix
- **Joystick navigation** across 6 dot positions (dot1 top-left → dot6 bottom-right)
- **Short press** = toggle current dot ON/OFF
- **Long press (1 second)** = decode pattern → send letter to LCD
- **Cursor blink** (300 ms) for clear dot-position feedback
- **Flash confirmation** (3× blink) before letter is committed
- **LCD word wrap** across both rows (16 chars per row)
- Full 26-letter Braille alphabet lookup table

---

## 🧰 Components

| Component | Quantity | Notes |
|---|---|---|
| Arduino Uno | 1 | Any 5V Arduino works |
| MAX7219 8×8 LED Matrix | 1 | Parola hardware type |
| I2C LCD 16×2 | 1 | Default address `0x27` (try `0x3F` if blank) |
| Analog Joystick Module | 1 | VRx, VRy, SW pins used |
| Breadboard + Jumper Wires | — | — |

## Images

### Joystick
![Demo](https://github.com/Obedientononline/Braille_writer_arduino/raw/250a5418fee1b55c68fd382cf47e90ee38a9f8b7/Images/Components/HW-504-JOYSTICK.jpg)

### LCD Display
![Demo](https://github.com/Obedientononline/Braille_writer_arduino/raw/250a5418fee1b55c68fd382cf47e90ee38a9f8b7/Images/Components/I2C-LCD-DISPLAY.jpg)

### 8x8 Dot Matrix
![Demo](https://github.com/Obedientononline/Braille_writer_arduino/raw/250a5418fee1b55c68fd382cf47e90ee38a9f8b7/Images/Components/MAX7219-8x8-Dot-Matrix.jpg)

---

## 🔌 Wiring

| Signal | Arduino Pin |
|---|---|
| Joystick VRx | A0 |
| Joystick VRy | A1 |
| Joystick SW (button) | D6 |
| MAX7219 DIN | D11 |
| MAX7219 CLK | D13 |
| MAX7219 CS | D10 |
| I2C LCD SDA | A4 |
| I2C LCD SCL | A5 |

---

## 📟 Circuit Connections

![Demo](https://github.com/Obedientononline/Braille_writer_arduino/raw/250a5418fee1b55c68fd382cf47e90ee38a9f8b7/Images/braille_circuit_diagram.svg)

## 📐 Braille Zone Layout (on 8×8 Matrix)

```
col:  0    1    2    3
row 0:[B] [B]  [B]  [B]    B = Border (always ON)
row 1:[B] [D1] [D4] [B]    D1-D6 = Braille dot positions
row 2:[B] [D2] [D5] [B]
row 3:[B] [D3] [D6] [B]
row 4:[B] [B]  [B]  [B]
```

The cursor blinks over the active dot. Short-press toggles it ON/OFF; hold 1 second to decode and confirm the letter to the LCD.

---

## 📚 Libraries Required

Install via **Arduino IDE → Tools → Manage Libraries**:

| Library | Author | Version Tested |
|---|---|---|
| `MD_MAX72XX` | MajicDesigns | 3.5.1 |
| `LiquidCrystal_I2C` | Frank de Brabander | 1.1.2 |

> `Wire.h` and `SPI.h` are built-in Arduino libraries — no installation needed.

---

## 🚀 How to Upload & Run

1. Clone this repository:
   ```bash
   git clone https://github.com/Obedientononline/Braille_writer_arduino.git
   ```
2. Open `braille_writer/braille_writer.ino` in Arduino IDE
3. Install the required libraries listed above
4. Select your board: **Tools → Board → Arduino Uno**
5. Select the correct port: **Tools → Port → COMX**
6. Click **Upload** ⬆️
7. Open **Serial Monitor** at **9600 baud** to see dot state and decoded letters in real time

---

## 🕹️ How to Use

1. On boot, the LCD shows a 2-screen tutorial (2.5 seconds each)
2. Cursor starts at **dot 1** (top-left of Braille zone), blinking
3. Use the joystick to move between dots:
   - **Left / Right** → switch columns (dot1–3 ↔ dot4–6)
   - **Up / Down** → move within a column
4. **Short press** the joystick button to toggle the current dot ON or OFF
5. **Hold 1 second** to confirm the letter — matrix flashes 3×, letter decoded, sent to LCD
6. Dots reset to all-OFF after each confirmed letter; cursor returns to dot1

---

## 📖 Braille Reference (Grade 1, A–Z)

| Letter | Dots Active | Letter | Dots Active |
|---|---|---|---|
| A | 1 | N | 1, 3, 4, 5 |
| B | 1, 2 | O | 1, 3, 5 |
| C | 1, 4 | P | 1, 2, 3, 4 |
| D | 1, 4, 5 | Q | 1, 2, 3, 4, 5 |
| E | 1, 5 | R | 1, 2, 3, 5 |
| F | 1, 2, 4 | S | 2, 3, 4 |
| G | 1, 2, 4, 5 | T | 2, 3, 4, 5 |
| H | 1, 2, 5 | U | 1, 3, 6 |
| I | 2, 4 | V | 1, 2, 3, 6 |
| J | 2, 4, 5 | W | 2, 4, 5, 6 |
| K | 1, 3 | X | 1, 3, 4, 6 |
| L | 1, 2, 3 | Y | 1, 3, 4, 5, 6 |
| M | 1, 3, 4 | Z | 1, 3, 5, 6 |

---

## 🗂️ Project Structure

```
Braille_writer_arduino/
├── braille_writer/
│   └── braille_writer.ino      ← Main Arduino sketch
├── images/
│   └── project_photo.jpg       ← Project hardware photo
├── .gitignore                  ← Excludes build artifacts
└── README.md                   ← This file
```

---

## 🔧 Troubleshooting

| Problem | Fix |
|---|---|
| LCD is blank | Change I2C address from `0x27` to `0x3F` in code |
| Matrix shows random pixels on boot | Check `mx.clear()` is called in `setup()` before `drawMatrix()` |
| Joystick not responding | Verify A0/A1 wiring and adjust `JOY_THRESHOLD` (default: 300) |
| Button registers double press | Increase `MOVE_DELAY` or add software debounce |
| Letter shows `?` on LCD | The dot pattern doesn't match any Braille letter — try again |

---

## 📄 License

MIT License - free to use, modify, and share with attribution.
