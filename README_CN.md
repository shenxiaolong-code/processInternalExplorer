# 功能特性

基于 miniMPL 库，提供多种系统内部探索工具和高性能 Excel 访问能力，无需 MFC 支持。

## SystemExplorer（系统探索器）

- **SymbolExplorer（符号探索器）**  
  给定代码地址，返回：函数名、源码文件路径、行号、模块信息（模块名、模块路径、大小、起止地址）。

- **exceptionHelper（异常助手）**  
  帮助捕获所有异常，并为开发者生成 Windows 应用程序的 dump 文件，便于远程分析（适用于 Windows C++ 应用）。

- **executablememoryreadwriter（可执行内存读写器）**  
  提供安全的运行时指令代码内存修改方法。

- **HookHelper（钩子助手）**  
  提供函数钩子辅助（支持 E9 和 E8 级别钩子）及用户自定义函数钩子。  
  当前网络上的示例多为 E9 钩子，这远不能满足完整钩子需求。

- **ProcessExplorer（进程探索器）**  
  提供 Windows 进程内部信息浏览能力。

- **WindowExplorer（窗口探索器）**  
  当鼠标悬停时，用红色矩形高亮窗口控件，并显示控件信息（类名、尺寸、坐标等）。

- **PEFileExplorer（PE 文件探索器）**  
  帮助解析和验证 PE 文件头格式。

- **信息打印辅助**  
  提供便捷的信息打印接口，简化使用。

---

## Excel

- 提供 C++ 下无需 MFC 环境的高性能 Microsoft Excel 访问能力。
- 可快速读写大块 Excel 数据。
- 基于模板的数据类型解析。

---

## 使用示例

你可以在以下目录查看用例源码：  
https://github.com/shenxiaolong-code/processInternalExplorer/tree/master/sources/UnitTest
