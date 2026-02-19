# Inbyggda-system

## Kunder & deras reklam 
| Kund                             | Betalt   | Vikt | Vad de vill visa              |
| -------------------------------- | -------- | ---- | ----------------------------- |
| **Hederlige Harrys Bilar**       | 5 000 kr | 5    | Slumpa mellan 3 meddelanden   |
| **Farmor Ankas Pajer AB**        | 3 000 kr | 3    | Slumpa mellan 2 meddelanden   |
| **Svarte Petters Svartbyggen**   | 1 500 kr | 1.5  | Växla på jämna/ojämna minuter |
| **Långbens detektivbyrå**        | 4 000 kr | 4    | Växla mellan 2 texter         |
| **IOT:s Reklambyrå (ni själva)** | 1 000 kr | 1    | En text                       |

## Viktiga regler
- Viktad slump - Kunder som betalat mer ska visas oftare
    Harry (5000) visas ~2x oftare än Anka (3000)
- 20 sekunder per visning - Varje reklam visas exakt 20 sek
- Aldrig samma kund 2 gånger i rad - Måste vara olika kunder
- Olika visningsformat:
    Scroll - Text rullar in från höger
    Text - Statisk text
    Blinkande - Text blinkar

## Kopplingsschema: 
LCD Pin    Namn       Kopplas till Arduino
─────────────────────────────────────────
Pin 1      VSS  ─────── GND
Pin 2      VDD  ─────── 5V
Pin 3      VO   ─────── GND (eller potentiometer för kontrast)
Pin 4      RS   ─────── Pin 12
Pin 5      RW   ─────── GND (alltid skrivläge)
Pin 6      E    ─────── Pin 11
Pin 7      D0   ─────── (inte kopplad)
Pin 8      D1   ─────── (inte kopplad)
Pin 9      D2   ─────── (inte kopplad)
Pin 10     D3   ─────── (inte kopplad)
Pin 11     D4   ─────── Pin 5
Pin 12     D5   ─────── Pin 4
Pin 13     D6   ─────── Pin 3
Pin 14     D7   ─────── Pin 2
Pin 15     A    ─────── 5V (bakgrundsbelysning +)
Pin 16     K    ─────── GND (bakgrundsbelysning -)

## Vad varje pin gör: 
| Signal      | Funktion                           |
| ----------- | ---------------------------------- |
| **VSS/VDD** | Ström (+5V och GND)                |
| **VO**      | Kontrast (GND = max kontrast)      |
| **RS**      | Register Select (kommando vs data) |
| **RW**      | Read/Write (GND = alltid skriva)   |
| **E**       | Enable (takt-signal)               |
| **D4-D7**   | Data (4-bitars överföring)         |
| **A/K**     | Bakgrundsbelysning LED             |

