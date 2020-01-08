# Features
based on miniMPL lib,  pack some common feature.

## Excel
   ole excel range access.
- easy access excel document without MFC support.
- batch access excel cells with high performace.
- templated access cell value (benefit from miniMPL::fromString).

## SystemExplorer
   split infomation provider and displayer.
- exception helper -- easily enable/disable excepture capture and generate dump file.
- instruction memory read/writter -- easily use without considering privilege elevate and save/store protect flag.
- hook wrapper -- easily hook export/function , include x86 jmp(E9) and call(E8) instruction.
- stack exploere -- show module/source file/function name/line number by address , print current call stack without exception capture. 
- PE file parser -- easily explorer PE file header.
- process explorer -- easily explorer process info , e.g. module list, threads , memory ,..
- window explorer -- find and mark window by name/pid/point/mouse.
- hover windows explorer -- highlight current window by tracking mouse.
