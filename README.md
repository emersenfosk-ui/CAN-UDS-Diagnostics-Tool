# STM32 UDS 诊断系统（H7 ECU + CANable 诊断仪）

## 项目简介
基于 STM32H743 实现 UDS（ISO 14229）诊断服务，使用经典 CAN 2.0（250kbps，采样点 87.5%），通过 TJA1044GT 收发器与 CANable（Cangaroo）进行总线交互。使用逻辑分析仪抓取 RXD/TXD 波形，验证请求-响应全过程。

## 硬件架构
- **ECU**：STM32H743VIT6 + TJA1044GT
- **诊断仪**：CANable + Cangaroo（PC 端）
- **总线**：经典 CAN 2.0，波特率 250kbps
- **调试工具**：Saleae Logic 2 逻辑分析仪

## 已实现 UDS 服务
| 服务 ID | 功能 | 请求（0x7E0） | 响应（0x7E8） |
| :--- | :--- | :--- | :--- |
| 0x10 | 会话控制 | `02 10 03` | `05 50 10 03 00 32` |
| 0x22 | 读数据（VIN） | `03 22 F1 90` | `07 62 F1 90 4C 58 42 37` |
| 0x19 | 读 DTC | `02 19 02` | `02 59 02` |
| 0x3E | 保持会话 | `02 3E 00` | `02 7E 00` |
| 0x14 | 清除 DTC | `04 14 FF FF FF` | `01 54` |

## 关键代码
- `uds.c`：UDS 请求解析与响应构造
- `bsp_can.c`：FDCAN 过滤器配置、接收中断、UDS 分发

## 实验截图
### Cangaroo 抓包
![0x10 会话控制](docs/cangaroo_uds_0x10.png)
![0x22 读VIN](docs/cangaroo_uds_0x22.png)
![0x19 读DTC](docs/cangaroo_uds_0x19.png)
![0x3E 保持会话](docs/cangaroo_uds_0x3E.png)
![0x14 清除DTC](docs/cangaroo_uds_0x14.png)

### 逻辑分析仪波形
![RXD接收请求](docs/logic_analyzer_rxd.png)
![TXD发送响应](docs/logic_analyzer_txd.png)

## 踩坑记录（重点！面试加分项）
1. **FDCAN 时钟源被 CubeMX 覆盖**：必须在 `SystemClock_Config()` 末尾手动强制切换 FDCAN 时钟源到 PLL1Q，否则实际波特率错误，无法通信。
2. **ISO-TP 单帧首字节**：必须正确计算有效数据长度（如 `05 50 10 03 00 32` 中的 `05`），否则接收方无法解析。
3. **采样点匹配**：H7 与 CANable 必须统一使用 250kbps + 87.5% 采样点。
4. **过滤器配置**：H7 的 FDCAN 过滤器必须精确匹配 `0x7E0`，否则请求帧会被硬件直接丢弃。

## 环境要求
- Keil MDK v5.38 及以上
- STM32CubeMX
- CANable + Cangaroo
- Saleae Logic 2（可选，用于抓波形）

## 项目结构
- `H7_UDS_ECU/Core/Src/`：H7 端主要代码
- `main.c`：主循环与初始化
- `bsp_can.c`：FDCAN 配置、过滤器、接收中断、UDS 分发
- `uds.c`：UDS 服务解析与响应构造
- `can_app.c`：应用层任务调度
