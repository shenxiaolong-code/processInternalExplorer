# Features
based on miniMPL lib,  provide excel access and many system explorer

# SystemExplorer
- ProcessExplorer  
  provide windows process inner information.  
- WindowExplorer  
  highlight window control with a red rectangle when your mouse hovers and show the control info (class name, size, X-Y dimension).
- SymbolExplorer  
  given a code address, return its function name, source file path, line number, module info (module name,size,start-end address).
- exceptionHelper  
  help to capture all exception and generate window application dump file for developer remote analysis. (for windows C++ application).
- executablememoryreadwriter  
  provide runtime instruction code memory modification with safely method.
- HookHelper  
  provde function hook helper( include E9 and E8 level hook ) and user function hook.  
  current network example only provides E9 hook  -- it is not enough for complete hook.  
- PEFileExplorer  
  Help to parse/verify PE file header format.  
### According information printer is also provided.  
  to simply usage.

# Excel
- provide microsoft Excel access by C++ with high performance.
- read/write big block Excel data quickly.
- template-based data type parse.
