# Features
based on miniMPL lib,  provide many system explorers and quick excel access without MFC support.

# SystemExplorer
- ProcessExplorer  
  provides windows process inner information.  
- WindowExplorer  
  highlight window control with a red rectangle when your mouse hovers and show the control info (class name, size, X-Y dimension).
- SymbolExplorer  
  given a code address, return its :  
  function name, source file path, line number  
  module-info (module name , module path, size,start-end address).
- exceptionHelper  
  help to capture all exceptions and generate a window application dump file for developer remote analysis. (for windows C++ application).
- executablememoryreadwriter  
  provide runtime instruction code memory modification with the safe method.
- HookHelper  
  provide function hook helper( include E9 and E8 level hook ) and user function hook.  
  current network example only provides an E9 hook  -- it is not enough for the complete hook.  
- PEFileExplorer  
  Help to parse/verify PE file header format.  
### According information printer is also provided.  
  to simply usage.

# Excel
- provide Microsoft Excel access by C++ with **high performance**.
- read/write **big block** Excel data quickly.
- template-based data type parse.
