# sconfig_tmct
Ported Intel(ALTERA) SRunner to Raspberry Pi.

This tool is a port of a tool called "SRunner" published by Intel (ALTERA) to the Raspberry Pi.

このツールはIntel(ALTERA)が公開している「SRunner」というツールをRaspberry Piに移植したものです。  
※日本語の解説文は英文解説の後にあります。

---

## What is SRunner?
SRunner is a tool to connect a PC's parallel port to Intel(ALTERA) FPGA configuration ROM(EPCS series) for writing, but it is almost worthless nowadays for the following reasons.

- **It is impossible to get a new PC which has parallel port and works properly.**  
USB-to-parallel converter is also not compatible and useless.
- **Supported OS up to Windows XP.**

...However, the documentation for making a tool to write to the configuration ROM is very scarce, so I've ported it to the Raspberry Pi and published it as "SCONFIG".

The I/O drive part uses the standard Raspberian GPIO driver, but by rewriting this part to just drive I/O, it can easily be ported to general microcontrollers.


## Port Assignment
The port assignment is as follows.

This assignment is defined in bb_gpio.c, so it can be rewritten here to assign it to a different port.

| Pin No. | Pin name | Input/output direction | Signal name | Remarks |
| :-- | :-- | :-- | :-- | :-- |
| 7 | GPIO4;GPIO_GCLK | OUT | EPCS_ASDI | ASDI output ...connect to ASDI of EPCS. |
| 11 | GPIO17;GPIO_GEN0 | OUT | EPCS_nCE | nCE output ...connect to nCE of FPGA |
| 12 | GPIO18;GPIO_GEN1 | OUT | EPCS_nCS | nCS output ... Connect to EPCS nCS. |
| 13 | GPIO27;GPIO_GEN2 | OUT | EPCS_nCONFIG | nCONFIG. Connect to nCONFIG of FPGA. |
| 15 | GPIO22;GPIO_GEN3 | OUT | EPCS_DCLK | DCLK output; Connect to DCLK of EPCS. |
| 16 | GPIO23;GPIO_GEN4 | IN | EPCS_CONFDONE | CONFDONE input; Connect to CONFDONE of FPGA. |
| 18 | GPIO24;GPIO_GEN5 | IN | EPCS_DATAO | DATAO input: Connect to EPCS DATA. |


## How to install
All you need to do is to put the program file itself into an appropriate folder. No special settings are required, but if GPIO resources are being used for other tasks, they cannot be used.

The GPIO control uses the standard Raspberian device driver, so nothing else is needed.


## How to use
This is a command line only tool. Parameters cannot be omitted.

. /mt_sconfig -[command] -[EPCS density] [filename].

| Parameter |  |
| :-- | :-- |
| [command] | **Specifies the command to be executed for EPCS.**<br/>program ... Write the specified RPD file<br/>read ... Save the contents of the EPCS to the specified RPD file<br/>verify ... Verify that the contents of the specified RPD file match the contents of the EPCS<br/>erase ... Erase EPCS (function not available in SRunner) |
| [EPCS density] | **Specify the size of the EPCS.**<br/>4 for EPCS4, 16 for EPCS16. |
| [filename] | **Specifies the RPD file to write target.**<br/>RPD files are not generated by default, so they are generated by converting from SOF files with Quartus' file converter. |


## License
The license terms are governed by the terms of SRunner. You may use it freely for commercial or private use as long as you do not remove the copyright notice, but you do so at your own risk.

You can find more information on the following page. (*Japanese only)

[tmct web-site: SCONFIG](https://ss1.xrea.com/tmct.s1009.xrea.com/doc/ta-ja-7e5g02.html)

---
## SRunnerとは？
SRunnerはパソコンのパラレルポートとIntel(ALTERA)のFPGAコンフィグレーションROM(EPCSシリーズ)を接続して書き込みを行うためのツールですが、下記の理由があり今となってはほぼ利用価値がないツールです。

- **パラレルポートが付いていて正常稼働するパソコンは新品での入手は不可。**  
USB-パラレル変換もソフト的な互換がなく使えない。
- **対応OSがWindows XPまで。**

…とはいえコンフィグレーションROMに書き込むツールを製作するための資料は非常に乏しく、技術資料としての価値は高いので、試しにRaspberry Piに移植してみたものを「SCONFIG」として公開します。

I/O駆動部分はRaspberianのGPIO標準ドライバを利用していますが、この部分をただのI/O駆動に書き換えることで一般的なマイコンなどにも容易にポーティングが可能です。


## ポートアサイン
以下のようなポートアサインになっています。

このアサインは bb_gpio.c に定義しているので、ここを書き換えると別のポートにアサインすることもできます。

| Pin No. | ピン名 | 入出力方向 | 信号名 | 備考 |
| :-- | :-- | :-- | :-- | :-- |
| 7  | GPIO4;GPIO_GCLK  | OUT | EPCS_ASDI     | ASDI出力‥EPCSのASDIに接続 |
| 11 | GPIO17;GPIO_GEN0 | OUT | EPCS_nCE      | nCE出力‥FPGAのnCEに接続 |
| 12 | GPIO18;GPIO_GEN1 | OUT | EPCS_nCS      | nCS出力‥EPCSのnCSに接続 |
| 13 | GPIO27;GPIO_GEN2 | OUT | EPCS_nCONFIG  | nCONFIG‥FPGAのnCONFIGに接続 |
| 15 | GPIO22;GPIO_GEN3 | OUT | EPCS_DCLK     | DCLK出力‥EPCSのDCLKに接続 |
| 16 | GPIO23;GPIO_GEN4 | IN  | EPCS_CONFDONE | CONFDONE入力‥FPGAのCONFDONEに接続 |
| 18 | GPIO24;GPIO_GEN5 | IN  | EPCS_DATAO    | DATAO入力‥EPCSのDATAに接続 |


## インストール方法
適当なフォルダにプログラムファイル本体を入れるだけです。とくに設定などは不要ですが、他のタスクにGPIOリソースを使われている場合は使用できません。

GPIO制御はRaspberian標準のデバイスドライバを使用しており、ほかには何も必要ありません。


## 使い方
コマンドライン専用ツールです。パラメータを省略することはできません。

./mt_sconfig -[command] -[EPCS density] [filename]

| Parameter |  |
| :-- | :-- |
| [command]      | **EPCSに対して実行するコマンドを指定します。**<br/>program ... 指定されたRPDファイルを書き込む<br/>read ... EPCSの内容を指定されたRPDファイルに保存する<br/>verify ... 指定されたRPDファイルの内容とEPCSの内容が一致することを確認する<br/>erase ... EPCSをイレースする(本家SRunnerにはない機能) |
| [EPCS density] | **EPCSのサイズを指定します。**<br/>EPCS4なら4、EPCS16なら16を指定します。 |
| [filename]     | **書き込みターゲットとなるRPDファイルを指定します。**<br/>RPDファイルは標準では生成されないので、SOFファイルからQuartusのファイルコンバータで変換することで生成します。 |


## ライセンス
ライセンス条項はSRunnerの条項に準拠します。著作権表示を消さない限り商用・私用問わず自由に利用して頂いて構いませんが、すべて自己責任にてご利用ください。

下記のページにもう少し詳しく書いてあります。

[tmct web-site: SCONFIG](https://ss1.xrea.com/tmct.s1009.xrea.com/doc/ta-ja-7e5g02.html)

