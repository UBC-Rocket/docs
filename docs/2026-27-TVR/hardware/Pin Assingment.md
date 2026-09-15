# Board to Board Connector Pin Assignment

## List of Interfaces / PWR needed between boards: (Pin Budget)

| # | Signal Group          | Count | Total Pins |
|---|-----------------------|-------|------|
| 1 | +3V3                  | 2×    | 2    |
| 2 | +5V                   | 2×    | 2    |
| 3 | PWM                   | 2×    | 2    |
| 4 | Gate Driver SPI       | 2×    | 8    |
| 5 | ESC MCU UART          | 1×    | 2    |
| 6 | Gate Driver EN        | 2×    | 2    |
| 7 | Gate Driver nFault INT| 2×    | 2    |
| 8 | ESC Temp Sensor I2C   | 2×    | 4    |
| 9 | Cell Voltages         | 6x     | 6    |
| 10 | Fuel Gauge I2C        | 1×    | 2    |
| 12 | Cell Monitoring I2C   | 1×    | 2    |
| 13 | Camera I2C            | 1×    | 2    |
| 14 | Alternating GND       | 15x     | 15   |
|   | **TOTAL**             |       | **51** |

## Choosen Connector:


## Pin Assignment:

| Net          | Pin |                | Pin | Net          |
|-------------:|:---:|:--------------:|:---:|:-------------|
|     V_CELL6         | 1   | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 2   | +3V3              |
|     V_CELL5       | 3   | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 4   | GND             |
|     GND         | 5   | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 6   | +5V             |
|     V_CELL4         | 7   | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 8   | GND             |
|     V_CELL3         | 9   | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 10  |   ESC1 Temp Sensor I2C_SDA           |
|     GND         | 11  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 12  |       ESC1 Temp Sensor I2C_SCL       |
|     V_CELL2         | 13  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 14  |   GND           |
|     V_CELL1         | 15  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 16  |   Cell Monitoring I2C_SDA           |
|     GND         | 17  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 18  |  Cell Monitoring I2C_SCL            |
|     PWM1         | 19  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 20  |     GND         |
|     GND         | 21  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 22  |     Gate Driver_1 EN      |
|     PWM2         | 23  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 24  |   Gate Driver_2 EN           |
|     GND         | 25  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 26  | +3V3             |
|     Fuel Gauge I2C_SDA         | 27  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 28  | GND             |
|     Fuel Gauge I2C_SCL         | 29  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 30  | +5v             |
|     GND         | 31  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 32  | GND             |
|     Gate Driver_1 SPI_CS         | 33  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 34  | Gate Driver_1 nFault INT             |
|     Gate Driver_1 SPI_SCLK         | 35  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 36  | Gate Driver_2 nFault INT             |
|     GND         | 37  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 38  |  GND            |
|     Gate Driver_1 SPI_MISO         | 39  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 40  |              |
|     Gate Driver_1 SPI_MOSI         | 41  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 42  |              |
|     GND         | 43  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 44  |              |
|              | 45  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 46  |              |
|              | 47  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 48  |              |
|              | 49  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 50  |              |
|              | 51  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 52  |              |
|              | 53  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 54  | +3V3             |
|              | 55  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 56  | GND             |
|              | 57  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 58  | +5V             |
|              | 59  | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 60  | GND             |


